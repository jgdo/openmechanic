#include "FullBody.h"
#include "GameWorld.h"
#include <iostream>

FullBody::FullBody(RID rid, GameWorld* world) :
	PhysicalObject(rid), _gameWorld(world) {
}

FullBody::~FullBody() {
}

RigidBodyPart* FullBody::addNewBodyPart(BlockID blockId, btVector3 const& worldPosition, btQuaternion const& worldOrientation, btQuaternion const& localBlockOrientation, RigidBodyPart* reference) {	
	RigidBodyPart* bodyPart = _gameWorld->getResoureManager()->create<RigidBodyPart>(this, _gameWorld);
	
	btTransform worldTransform(worldOrientation, worldPosition);
		
	btTransform refTransform = worldTransform;
	
	if(reference) {
		auto iter = _allBodyParts.find(reference);
		if(iter != _allBodyParts.end()) {
			refTransform = iter->second->_centerOffset * iter->second->_bodyPart->getCurrentWorldTransform().inverse() * worldTransform;
		}
	}

	_allBodyParts[bodyPart] = PartEntryPtr(new PartEntry(bodyPart, refTransform)); // new for aligned allocation
	bodyPart->setFreezed(_freezed);

	//_allBodyParts.emplace(std::piecewise_construct, std::make_tuple(bodyPart.get()), std::make_tuple(bodyPart, refTransform));
	// FIXME: above works on GCC
	if (_listener)
		_listener->onPartWasAdded(this, bodyPart);


	bodyPart->addBlock(blockId, BlockIndex(0, 0, 0), localBlockOrientation);
	bodyPart->setWorldTransform(worldTransform);

	return bodyPart;
}

void FullBody::removeAndDeleteBodyPart(RigidBodyPart* bodyPart) {
	auto iter = _allBodyParts.find(bodyPart);
	if(iter != _allBodyParts.end()) {
		if (_listener)
				_listener->onPartIsBeingRemoved(this, bodyPart);

			// detach all joints from bodypart and delete floating joints
			std::vector<Joint*> attachedJoints = bodyPart->removeAllJoints();
			for (auto joint : attachedJoints) {
				if (joint->isFloating()) {
					removeAndDeleteJoint(joint);
				}
			}

			// detach all springs from bodypart and delete floating springs
			std::vector<Spring*> attachedSprings = bodyPart->removeAllSprings();
			for (auto spring : attachedSprings) {
				if (true|| spring->isFloating()) { // FIXME: for now
					removeAndDeleteSpring(spring);
				}
			}
			
			_allBodyParts.erase(iter);
			_gameWorld->getResoureManager()->destroy(bodyPart);
	}
	
	// FIXME: what to do if joint not present
}

Joint* FullBody::addJoint(RigidBodyPart * bodyPart, BlockIndex const& position, BlockIndex const& direction) {
	// create joint attached only to one part for now
	Joint* joint = _gameWorld->getResoureManager()->create<Joint>(_gameWorld, bodyPart, position, direction);
	
	_allJoints.push_back(joint);

	if (_listener)
		_listener->onJointWasAdded(this, joint);

	return joint;
}

void FullBody::removeAndDeleteJoint(Joint* joint) {
	// TODO better than linear search
	for (auto iter = _allJoints.begin(); iter != _allJoints.end(); ++iter) {
		if (*iter == joint) {
			if (_listener)
				_listener->onJointIsBeingRemoved(this, joint);

			_allJoints.erase(iter);
			_gameWorld->getResoureManager()->destroy(joint);
			return;
		}
	}

	// FIXME: what to do if joint not present
}

void FullBody::resetPosition(const btVector3& worldPos, const btQuaternion& worldOorientation) {
	btTransform worldTrans(worldOorientation, worldPos);
	
	std::cout << "resetting position for full body" << std::endl;
	
	for(auto& entry: _allBodyParts) {
		entry.second->_bodyPart->setWorldTransform(worldTrans*entry.second->_centerOffset);
	}
}

void FullBody::activate() {
	for(auto& e: _allBodyParts)
		e.second->_bodyPart->getBtBody()->activate(true);
}

void FullBody::setFreezed(bool freezed)
{
	_freezed = freezed;
	for (auto& e : _allBodyParts)
		e.second->_bodyPart->setFreezed(freezed);
}

btDiscreteDynamicsWorld* FullBody::getDynmicsWorld() {
	return _gameWorld->getBtDynWorld();
}

Spring *FullBody::addSpring(RigidBodyPart *bodyPart, const BlockIndex &position, const BlockIndex &direction) {
	// create spring attached only to one part for now
	Spring* spring = _gameWorld->getResoureManager()->create<Spring>(_gameWorld, bodyPart, position, direction);

	_allSprings.push_back(spring);

	if (_listener)
		_listener->onSpringWasAdded(this, spring);

	return spring;
}

void FullBody::removeAndDeleteSpring(Spring *spring) {
	// TODO better than linear search
	for (auto iter = _allSprings.begin(); iter != _allSprings.end(); ++iter) {
		if (*iter == spring) {
			if (_listener)
				_listener->onSpringIsBeingRemoved(this, spring);

			_allSprings.erase(iter);
			_gameWorld->getResoureManager()->destroy(spring);
			return;
		}
	}

	// FIXME: what to do if spring not present
}
