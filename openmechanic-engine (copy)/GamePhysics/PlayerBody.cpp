//
// Created by jgdo on 14.05.16.
//

#include "PlayerBody.h"

#include "GameWorld.h"

#include "Control/DriverSeatControl.h"

#include <iostream>

const float PlayerBody::BODY_HEIGHT = 1.6, PlayerBody::BODY_RADIUS = 0.35;

PlayerBody::PlayerBody(RID rid, GameWorld* gameWorld, const btVector3& position):
		PhysicalObject(rid), _gameWorld(gameWorld){

	_collisionShape = new btCapsuleShape(BODY_RADIUS, BODY_HEIGHT - 2*BODY_RADIUS);

	btMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), position));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1, motionState, _collisionShape);
	rigidBodyCI.m_friction = 0;
	rigidBodyCI.m_rollingFriction = 0;
	rigidBodyCI.m_restitution = 0;
	_body = new btRigidBody(rigidBodyCI);
	_body->forceActivationState(DISABLE_DEACTIVATION);
	_gameWorld->getBtDynWorld()->addRigidBody(_body);
}

PlayerBody::~PlayerBody() {
	_gameWorld->getBtDynWorld()->removeCollisionObject(_body);
	delete _body->getMotionState();
	delete _body;
	delete _collisionShape;
}

btVector3 PlayerBody::getHeadPosition() {
	if(_attachedDSC)
		return _attachedDSC->getObjectPosition() + btVector3(0,1,0) * (BODY_HEIGHT*0.7) +  btMatrix3x3(btQuaternion(_camYaw, 0, _camPitch)) *  btVector3(-2, 0., 0.);
	else
		return _body->getCenterOfMassPosition() + btVector3(0,1,0) * (BODY_HEIGHT / 2 - 0.3);
}

btQuaternion PlayerBody::getHeadOrientation() {
	if(_attachedDSC)
		return btQuaternion(_camYaw, _camPitch, 0);
	else
		return _body->getOrientation() * btQuaternion(0, _camPitch, 0);
}

void PlayerBody::attachDriverSeat(DriverSeatControl* dsc) {
	if(_attachedDSC)
		detachDriverSeat();

	_attachedDSC = dsc;
	_attachedDSC->attachPlayerBody(this);

	_body->forceActivationState(DISABLE_SIMULATION);
}

void PlayerBody::detachDriverSeat(bool informDSC) {
	if(_attachedDSC) {
		if (informDSC) {
			_attachedDSC->detachPlayerBody();
		}

		// make player jump on top of the driver seat on exit
		auto trans = _body->getWorldTransform();
		trans.setOrigin(_attachedDSC->getObjectPosition() + btVector3(0,1,0) * (BODY_HEIGHT / 2));
		_body->setWorldTransform(trans);

		_attachedDSC = nullptr;

		_body->forceActivationState(DISABLE_DEACTIVATION);
		_body->activate(true);
	}
}

void PlayerBody::insertClientMotion(const ClientMotion& motion) {
	_camYaw = motion.yaw;
	_camPitch = motion.pitch;

	static const float maxSpeed = 2.5;
	static const float accelFactor = 0.1;
	static const float jumpSpeed = 4;

	if(_attachedDSC)
		_attachedDSC->setControlValues(motion.forward, motion.sidewards);
	else {
		// first rotate the body according to yaw
		auto trans = _body->getWorldTransform();
		trans.setRotation(btQuaternion(motion.yaw, 0, 0));
		_body->setWorldTransform(trans);

		// now calculate the forward and sidewards directions
		btVector3 sideDir = trans.getBasis()*btVector3(0,0,1);
		// y should be 0, but reset it anyway
		sideDir.setY(0);
		sideDir.normalize();
		// calc forward
		btVector3 fwdDir = -sideDir.cross(btVector3(0,1,0));

		//std::cout << "motion: " <<motion.forward << " " << motion.sidewards << std::endl;
		//std::cout << "pos: " << trans.getOrigin().x() << " " << trans.getOrigin().z() << std::endl;

		// compute and apply target veocity
		auto targetVel = ((fwdDir * motion.forward) + (sideDir * motion.sidewards))  * maxSpeed;
		auto currentVel = _body->getLinearVelocity();
		auto setVel = currentVel + (targetVel - currentVel) * accelFactor;
		setVel.setY(currentVel.getY() + (motion.jump? jumpSpeed : 0)); // reset y to previous value to simulate free fall

		//std::cout << "setvel: " << setVel.x() << " " << setVel.z() << std::endl;
		_body->setLinearVelocity(setVel);
	}
}


