/* 
 * File:   GameWorld.cpp
 * Author: jgdo
 * 
 * Created on February 5, 2016, 1:40 PM
 */

#include "GameWorld.h"

#include "FullBody.h"
#include "Lift.h"
#include "PlayerBody.h"

#include <iostream>

GameWorld::GameWorld() :
_controlEngine(&_resourceManager), _lastStateQueue(std::make_shared<EventQueue>()), _activeStateQueue(std::make_shared<EventQueue>()) {
}

GameWorld::~GameWorld() {
}

void GameWorld::initBulletWorld(const std::vector<float> &terrainData, int terrainDataSideLength,
								float terrainWorldSize) {
	_broadphase.reset(new btDbvtBroadphase());

	_collisionConfiguration.reset(new btDefaultCollisionConfiguration());
	_collisionDispatcher.reset(new btCollisionDispatcher(_collisionConfiguration.get()));

	_solver.reset(new btSequentialImpulseConstraintSolver);

	_dynamicsWorld.reset(new btDiscreteDynamicsWorld(_collisionDispatcher.get(), _broadphase.get(), _solver.get(), _collisionConfiguration.get()));

	_dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
	_dynamicsWorld->getSolverInfo().m_numIterations = 100;
	_terrainHeightData = terrainData;

	_groundShape.reset(new btHeightfieldTerrainShape(terrainDataSideLength, terrainDataSideLength, _terrainHeightData.data(), 1, -25.5, 25.5, 1, PHY_FLOAT, false)); // FIXME min/max height
	float metersBetweenVertices = terrainWorldSize / (terrainDataSideLength - 1);
	std::cout << "----- metersBetweenVertices = " << metersBetweenVertices << std::endl;
	_groundShape->setLocalScaling(btVector3(metersBetweenVertices,1,metersBetweenVertices));
	// groundShape->setUseDiamondSubdivision(true);
	_groundShape->setUseZigzagSubdivision(true);

	_groundMotionState.reset(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))));
	btRigidBody::btRigidBodyConstructionInfo
			groundRigidBodyCI(0, _groundMotionState.get(), _groundShape.get(), btVector3(0, 0, 0));
	groundRigidBodyCI.m_restitution = 0.00;
	groundRigidBodyCI.m_friction = 3;
	_groundRigidBody.reset(new btRigidBody(groundRigidBodyCI));
	_dynamicsWorld->addRigidBody(_groundRigidBody.get());
}

FullBody* GameWorld::addNewFullBody(BlockID blockID, const btVector3& worldPos, const btQuaternion& worldOorientation, btQuaternion const& localBlockOrientation) {
	FullBody* body = _resourceManager.create<FullBody>(this);
	_allBodies.emplace(body);

	if(_listener)
		_listener->onFullBodyWasAdded(this, body); // notify such that the listener can install new listeners on the body which can notice the new bodypart add below

	body->addNewBodyPart(blockID, btVector3(0, 0, 0), btQuaternion(0, 0, 0, 1), localBlockOrientation, nullptr);
	body->resetPosition(worldPos, worldOorientation);
	
	return body;
}


Lift* GameWorld::addLift(btVector3 const& position, float height)
{
	Lift* lift = _resourceManager.create<Lift>(this);
	_lifts.emplace(lift);

	lift->deploy(position, height);

	if (_listener)
		_listener->onLiftWasAdded(this, lift);

	return lift;
}

void GameWorld::removeAndDeleteLift(Lift* lift)
{
	auto iter = _lifts.find(lift);
	if (iter != _lifts.end()) {
		if (_listener)
			_listener->onLiftIsBeeingRemoved(this, lift);

		_lifts.erase(iter);
		_resourceManager.destroy(lift);
	}

	// FIXME: what to do if lift not present
}

void GameWorld::removeAndDeleteFullBody(FullBody* fullBody)
{
	auto iter = _allBodies.find(fullBody);
	if (iter != _allBodies.end()) {
		if (_listener)
			_listener->onFullBodyIsBeeingRemoved(this, fullBody);

		_allBodies.erase(iter);
		_resourceManager.destroy(fullBody->resourceID);
	}

	// FIXME: what to do if body not present
}

PlayerBody *GameWorld::addPlayerBody(btVector3 const &position) {
	PlayerBody* player = _resourceManager.create<PlayerBody>(this, position);
	_players.emplace(player);

	if(_listener)
		_listener->onPlayerBodyWasAdded(this, player); // notify such that the listener can install new listeners on the body which can notice the new bodypart add below

	return player;
}

void GameWorld::removeAndDeletePlayerBody(PlayerBody *player) {
	auto iter = _players.find(player);
	if (iter != _players.end()) {
		if (_listener)
			_listener->onPlayerBodyIsBeeingRemoved(this, player);

		_players.erase(iter);
		_resourceManager.destroy(player);
	}

	// FIXME: what to do if player not present
}

void GameWorld::activate()
{
	for (auto& e : _allBodies)
		e->activate();
}

void GameWorld::makeStep(float timestep)
{
	// pre-step actions
	for (Lift* lift : _lifts)
	{
		lift->performRaise(timestep);
	}

	// make step
	_controlEngine.step();
	_dynamicsWorld->stepSimulation(timestep, 10);

	// post-step actions
	// collect data

	// FIXME: update only position of moved bodies
	for(auto& fullBody: _allBodies)
	{
		fullBody->foreachPart([&](const RigidBodyPart* part)
		{
			_activeStateQueue->addState(ObjectStatePtr(new BodyPartState(part->resourceID, part->getCurrentWorldTransform())));
		});

		for(Joint* joint: fullBody->getJoints())
		{
			_activeStateQueue->addState(ObjectStatePtr(new JointState(joint->resourceID, joint->getCurrentWorldTransform())));
		}

		for(Spring* spring: fullBody->getSprings())
		{
			btTransform tans = spring->getCenterTransform();
			_activeStateQueue->addState(ObjectStatePtr(new SpringState(spring->resourceID, tans.getOrigin(), tans.getRotation(), spring->getSpringElongation())));
		}
	}

	for (Lift* lift : _lifts)
	{
		_activeStateQueue->addState(ObjectStatePtr(new LiftState(lift->resourceID, lift->getPositionOnGround(), lift->getHeight(), lift->getPlatformHalfSize())));
	}

	for(PlayerBody* player: _players) {
		_activeStateQueue->addState(ObjectStatePtr(new PlayerBodyState(player->resourceID, player->getHeadPosition(), player->getHeadOrientation())));
	}

	// merge previous states into new ones (and not the other way around) 
	// since the states in _activeStateQueue have to be kept
	_queueMutex.lock();
	_activeStateQueue->mergeFromOther(std::move(*_lastStateQueue));
	_lastStateQueue = std::make_shared<EventQueue>();
	_lastStateQueue.swap(_activeStateQueue);
	_queueMutex.unlock();
}

EventQueueConstPtr GameWorld::getLastEventQueue()
{
	// return the previous old _lastStateQueue and replace it with an empty one
	// std::lock_guard<std::recursive_mutex> lock(_queueMutex);
	_queueMutex.lock();
	EventQueuePtr queue = std::make_shared<EventQueue>();
	queue.swap(_lastStateQueue);
	_queueMutex.unlock();
	return queue;
}
