#include "RigidBodyPart.h"
#include "Joint.h"
#include "Spring.h"
#include "GameWorld.h"

#include <stdexcept>
#include <iostream>

RigidBodyPart::RigidBodyPart(AsyncObjectContainer& container, OID oid, FullBody* parent, GameWorld* gameWorld) :
PhysicalObject(oid), ServerInterfaceImplBase< RigidBodyPartData>(container, oid), _parent(parent), _blockData(3, 3, 3), _gameWorld(gameWorld) {
	_centerTransformInv.setIdentity();
	
	container.getWrite<RigidBodyPartData>(myIndex).worldTransform.setIdentity();
}

RigidBodyPart::~RigidBodyPart() {	
	for (JointEntry& je : _connectedJoints) {
		je._joint->detachBodyPart(this, false);
	}

	for (SpringEntry& je : _connectedSprings) {
		je._spring->detachBodyPart(this, false);
	}
	
	if (_btBody) {
		_gameWorld->getBtDynWorld()->removeCollisionObject(_btBody);
		delete _btBody->getMotionState();
		delete _btBody;
	}

	delete _collisionShapeAligned;
	delete _collisionShapeReal;
}

void RigidBodyPart::addBlock(DataIndex partIdx, BlockID blockId, BlockIndex position, BlockIndex direction, btQuaternion orientation)
{
    position += direction;
    // FIXME check if added on joint/spring
  
      ensureIndexExits(position);

	if (getBlockAt(position))
		return;

	btVector3 finalPos(position.x() * BodyBlock::BlockSize, position.y() * BodyBlock::BlockSize, position.z() * BodyBlock::BlockSize);

	BodyBlock* bodyBlock = _gameWorld->getBlockFactory()->getCreateBodyBlock(blockId);

	auto blockInstance = std::shared_ptr<BodyBlockInstance>(new BodyBlockInstance(this, bodyBlock, position, orientation)); 
	// todo: aligned allocator
	// std::make_shared<BodyBlockInstance>(this, bodyBlock, position, orientation

	// fill all the slots in block data that this blocks should fill
	for (BlockIndex const& ind : blockInstance->getIndiciesInBody()) {
		ensureIndexExits(ind);
		BodyBlockInstancePtr& slot = getBlockAt(ind.x(), ind.y(), ind.z());

		if (slot) {
			// a slot is already occupied => this block can not be placed
			// remove this block instance from _blockData entirely
			for (BlockIndex const& ind : blockInstance->getIndiciesInBody()) {
				BodyBlockInstancePtr& bbInstAtInd = getBlockAt(ind);
				if(bbInstAtInd != blockInstance) {
					// we have reached the point where a slot is taken by an other bbInst, which means that we have deleted all the block entries
					return;
				}
			}

			throw std::runtime_error("Internal error in RigidBodyPart::addBlock(): this code should never be reached");
		}

		slot = blockInstance;
		// std::cout << "add ind: " << ind << std::endl;
	}
	
	if (_listener)
		_listener->onBlockWasAdded(this, blockInstance.get());

	// create new compound collision shape if necessary
	if (!_collisionShapeAligned)
		_collisionShapeAligned = new btCompoundShape();

	// compute position of new block and add it to collision shape
	_collisionShapeAligned->addChildShape(btTransform(orientation, finalPos + btTransform(orientation)* bodyBlock->getCenterOfMassDisplacement()), bodyBlock->getCreateCollisionShape());

	// update part mass
	_mass += bodyBlock->getMass();
	_numBlocks++;

	// update/create body
	if (!_btBody) { // this is the first block
		btMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(1,1,1)));
		btVector3 inertia(0, 0, 0);
		float mass = 0;
		if (!isFreezed()) {
			mass = _mass;
			_collisionShapeAligned->calculateLocalInertia(mass, inertia);
		}
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, _collisionShapeAligned, inertia);
		rigidBodyCI.m_restitution = 0.00;
		rigidBodyCI.m_friction = 2;
		// rigidBodyCI.m_linearDamping = 0.99;

		_btBody = new btRigidBody(rigidBodyCI);
		_btBody->forceActivationState(_isFrezed? DISABLE_SIMULATION : DISABLE_DEACTIVATION);
		_gameWorld->getBtDynWorld()->addRigidBody(_btBody);
	} else { // update collision body
		updateShapeMassAndPosition();
	}

	return;
}

void RigidBodyPart::removeBlockOrJoint(DataIndex partIdx, BlockIndex index, BlockIndex direction)
{
  // FIXME check if joint, if bodypart/body is empty after deleting
  removeBlock(index);
}

void RigidBodyPart::getWorldTransformForPosition(Eigen::Vector3i const & index, btVector3 * pos, btQuaternion * rot) {
	btTransform worldTrans = getCurrentWorldTransform();

	if (pos)
		*pos = worldTrans(btVector3(index.x() * BodyBlock::BlockSize, index.y() * BodyBlock::BlockSize, index.z() * BodyBlock::BlockSize));
	if (rot)
		*rot = worldTrans.getRotation() * btQuaternion(0, 0, 0, 1); // FIXME: real block transform
}

btTransform RigidBodyPart::getCurrentWorldTransform() const {
	// FIXME: if no body attached
	return _btBody->getWorldTransform() * _centerTransformInv;
}

void RigidBodyPart::setWorldTransform(btTransform const& trans) {
	// FIXME: if no body attached
	_btBody->setWorldTransform(trans * _centerTransformInv.inverse());
	_btBody->activate(true); // maybe the forces/contacts have changed after re-positioning
	
	myContainer.getWrite<RigidBodyPartData>(myIndex).worldTransform = trans;
}

void RigidBodyPart::addShit() {
	// no shit for now
}

void RigidBodyPart::addJoint(Joint* joint, const BlockIndex& pos, const BlockIndex& dir) {
	_connectedJoints.emplace_back(joint, pos, dir);
}

Joint* RigidBodyPart::getJointAt(const BlockIndex& pos, const BlockIndex& dir) {
	for (JointEntry& je : _connectedJoints) {
		if (je._position == pos && je._direction == dir) {
			return je._joint;
		}
	}

	return nullptr;
}

void RigidBodyPart::removeJoint(Joint* joint) {
	// TODO: better than linear search
	for (auto iter = _connectedJoints.begin(); iter != _connectedJoints.end(); ++iter) {
		if (iter->_joint == joint) {
			_connectedJoints.erase(iter);
			return;
		}
	}

	// FIXME what to do if joint not present?
}

std::vector<Joint*> RigidBodyPart::removeAllJoints() {
	std::cout << "removing " << _connectedJoints.size() << " joints" << std::endl;
	std::vector<Joint*> joints;
	joints.reserve(_connectedJoints.size());
	
	for (JointEntry& je : _connectedJoints) {
		je._joint->detachBodyPart(this, false);
		joints.push_back(je._joint);
	}
	
	_connectedJoints.clear();
	return joints;
}

void RigidBodyPart::addSpring(Spring *spring, BlockIndex const &pos, BlockIndex const &dir) {
	_connectedSprings.emplace_back(spring, pos, dir);
}

void RigidBodyPart::removeSpring(Spring *spring) {
	// TODO: better than linear search
	for (auto iter = _connectedSprings.begin(); iter != _connectedSprings.end(); ++iter) {
		if (iter->_spring == spring) {
			_connectedSprings.erase(iter);
			return;
		}
	}

	// FIXME what to do if spring not present?
}

std::vector<Spring *> RigidBodyPart::removeAllSprings() {
	std::cout << "removing " << _connectedSprings.size() << " springs" << std::endl;
	std::vector<Spring*> springs;
	springs.reserve(_connectedSprings.size());

	for (SpringEntry& se : _connectedSprings) {
		se._spring->detachBodyPart(this, false);
		springs.push_back(se._spring);
	}

	_connectedSprings.clear();
	return springs;
}


void RigidBodyPart::removeBlock(const BlockIndex& index) {
	// get collision shape
	// TODO: maybe there is something better than linear search??
	// make a shared pointer such that block instance will not be deleted after all data block cells have been reset
	BodyBlockInstancePtr bbInst = getBlockAt(index);
	BlockIndex centerIndex = bbInst->getCenterBlockIndex(); // the collision shape center is at the block center. The given 'index' might be different since a block might be big
	if (bbInst) {
		int numShapes = _collisionShapeAligned->getNumChildShapes();
		for (int i = 0; i < numShapes; i++) {
			btVector3 shapePos = _collisionShapeAligned->getChildTransform(i).getOrigin() / BodyBlock::BlockSize;
			BlockIndex blockIndex(lrint(shapePos.x()), lrint(shapePos.y()), lrint(shapePos.z()));
			if (blockIndex == centerIndex) {
				// inform listener at beginning
				if (_listener)
					_listener->onBlockIsBeeingRemoved(this, bbInst.get());

				_collisionShapeAligned->removeChildShapeByIndex(i);
				// reset collisions since the collision with the removed child might be cached and cause SEGFAULTs if the shape is removed from the compound
				_gameWorld->getBtDynWorld()->getCollisionWorld()->getBroadphase()->resetPool(_gameWorld->getBtDynWorld()->getDispatcher());
				// FIXME above doesn't really work^^
				_mass -= bbInst->getBodyBlock()->getMass();
				_numBlocks--;

				for (BlockIndex const& ind : bbInst->getIndiciesInBody()) {
					// std::cout << "remove ind: " << ind << std::endl;
					getBlockAt(ind).reset();  // reset pointer and such delete block instance
				} 
				// now bbInst is now the only pointer to the block instance
				bbInst.reset();
				
				updateShapeMassAndPosition();

				return;
			}
		}
	}

	// FIXME: throw error if block not found?
}

void RigidBodyPart::setFreezed(bool freezed)
{
	std::cout << "RigidBodyPart::setFreezed() " << freezed << std::endl;

	if (freezed != isFreezed()) {
		_isFrezed = freezed;

		if (_btBody) {
			_btBody->forceActivationState(freezed ? DISABLE_SIMULATION : DISABLE_DEACTIVATION);
			if (!freezed)
				_btBody->activate(true);
		}
	}
}

void RigidBodyPart::updateShapeMassAndPosition() {
	int n = _collisionShapeAligned->getNumChildShapes();
	std::vector<btScalar> masses(n);

	for (int i = 0; i < n; i++)
		masses[i] = 1; // TODO FIXME use real mass of block

	btTransform principal;
	btVector3 principalInertia;
	_collisionShapeAligned->calculatePrincipalAxisTransform(masses.data(), principal, principalInertia);
	btTransform principalInv = principal.inverse();

	btCompoundShape* newShape = new btCompoundShape();

	for (int i = 0; i < n; i++) {
		btTransform newChildTransform = principalInv * _collisionShapeAligned->getChildTransform(i);
		newShape->addChildShape(newChildTransform, _collisionShapeAligned->getChildShape(i));
	}

	// adjust constraint pivot point to new center of mass
	for(const auto& entry: _connectedJoints)
	{
		btHingeConstraint* hc = entry._joint->getHinge();
		if(hc)
		{
			volatile float jointAnglePrev = entry._joint->getJointAngle();

			volatile float prevAngle = hc->getHingeAngle();

			btTransform prevTrans;
			if (&(hc->getRigidBodyA()) == _btBody)
				prevTrans = hc->getFrameOffsetA();
			else
				prevTrans = hc->getFrameOffsetB();

			prevTrans = principalInv * _centerTransformInv.inverse() * prevTrans;

			if (&(hc->getRigidBodyA()) == _btBody)
				hc->setFrames(prevTrans, hc->getFrameOffsetB());
			else
				hc->setFrames(hc->getFrameOffsetA(), prevTrans);

			float nowAngle = hc->getHingeAngle();
			float angleDiff = nowAngle - prevAngle;
			float prevOffset = entry._joint->getAngleOffset();
			entry._joint->setAngleOffset(prevOffset + angleDiff);

			volatile float jointAngleNow= entry._joint->getJointAngle();

			if (std::abs(jointAngleNow - jointAnglePrev) > 0.00001) {
				// throw std::runtime_error("�hror");
				std::cerr << "#################   ERROR: std::abs(jointAngleNow - jointAnglePrev) > 0.00001   #######################" << std::endl;
			}
		}
	}

	// adjust spring constraints to new center of mass
	for(const auto& entry: _connectedSprings)
	{
		Spring* spring = entry._spring;
		btGeneric6DofSpring2Constraint* sc = spring->getSpringConstraint();
		if(sc)
		{
			btTransform prevTrans;
			if (&(sc->getRigidBodyA()) == _btBody)
				prevTrans = sc->getFrameOffsetA();
			else
				prevTrans = sc->getFrameOffsetB();

			prevTrans = principalInv * _centerTransformInv.inverse() * prevTrans;

			if (&(sc->getRigidBodyA()) == _btBody)
				sc->setFrames(prevTrans, sc->getFrameOffsetB());
			else
				sc->setFrames(sc->getFrameOffsetA(), prevTrans);
		}
	}

	_gameWorld->getBtDynWorld()->removeRigidBody(_btBody);
	_btBody->setWorldTransform(getCurrentWorldTransform() * principal);
	_centerTransformInv = principalInv;
	_btBody->setMassProps(_mass, principalInertia);
	_btBody->setCollisionShape(newShape);
	delete _collisionShapeReal; 
	_collisionShapeReal = newShape;
	_gameWorld->getBtDynWorld()->addRigidBody(_btBody);
	_btBody->forceActivationState(_isFrezed? DISABLE_SIMULATION : DISABLE_DEACTIVATION);
}

void RigidBodyPart::ensureIndexExits(BlockIndex const& index)
{
	int x = index.x(), y = index.y(), z = index.z();

	_blockData.ensureIndexExists(x, y, z);
}

std::pair<BlockIndex, BlockIndex> RigidBodyPart::getBlockDataDimensions() const {
	auto x = _blockData.rangeX();
	auto y = _blockData.rangeY();
	auto z = _blockData.rangeZ();
	
	return std::make_pair(BlockIndex(x.first, y.first, z.first), BlockIndex(x.second, y.second, z.second));
}


