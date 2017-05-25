#include "FullBody.h"
#include "GameWorld.h"
#include <iostream>

FullBody::FullBody(OID rid, GameWorld* world) : 
	GameObject(rid), _gameWorld(world), myIndex(_gameWorld->myIndex.container.newBin<FullBodyData>(rid, *this)) {
}

FullBody::~FullBody() {
}

RigidBodyPart* FullBody::addNewBodyPart(BlockID blockId, btVector3 const& worldPosition, btQuaternion const& worldOrientation, btQuaternion const& localBlockOrientation, RigidBodyPart* reference) {	
	std::set<RigidBodyPart>::iterator res = _allBodyParts.emplace(_gameWorld->getIdGenerator().generateNewID(), this, _gameWorld).first;
	RigidBodyPart* bodyPart = const_cast<RigidBodyPart*>( &*res); // FIXME why const_cast needed???
	
	btTransform worldTransform(worldOrientation, worldPosition);
		
	btTransform refTransform = worldTransform;
	
	if(reference) {
		refTransform = reference->_centerOffset * reference->getCurrentWorldTransform().inverse() * worldTransform;
	}
	
	bodyPart->_centerOffset = refTransform;
	
	bodyPart->setFreezed(_freezed);

	//_allBodyParts.emplace(std::piecewise_construct, std::make_tuple(bodyPart.get()), std::make_tuple(bodyPart, refTransform));
	// FIXME: above works on GCC


	bodyPart->addBlock(bodyPart->myIndex, blockId, BlockIndex(0, 0, 0), BlockIndex(0, 0, 0), localBlockOrientation);
	bodyPart->setWorldTransform(worldTransform);

	return bodyPart;
}

void FullBody::removeAndDeleteBodyPart(RigidBodyPart* bodyPart) {
	auto iter = _allBodyParts.find(*bodyPart);
	if(iter != _allBodyParts.end()) {
		
#if 0
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
			
#endif 
			_allBodyParts.erase(iter);
	}
	
	// FIXME: what to do if joint not present
}

#if 0
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
#endif 

void FullBody::resetPosition(const btVector3& worldPos, const btQuaternion& worldOorientation) {
	btTransform worldTrans(worldOorientation, worldPos);
	
	std::cout << "resetting position for full body" << std::endl;
	
	for(const RigidBodyPart& bodyPart: _allBodyParts) {
		const_cast<RigidBodyPart&>(bodyPart).setWorldTransform(worldTrans*bodyPart._centerOffset);
	}
}

void FullBody::activate() {
	for(auto& e: _allBodyParts)
		const_cast<RigidBodyPart&>(e).getBtBody()->activate(true);
}

void FullBody::setFreezed(bool freezed)
{
	_freezed = freezed;
	for (auto& e : _allBodyParts)
		const_cast<RigidBodyPart&>(e).setFreezed(freezed);
}

btDiscreteDynamicsWorld* FullBody::getDynmicsWorld() {
	return _gameWorld->getBtDynWorld();
}

#if 0
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
#endif 