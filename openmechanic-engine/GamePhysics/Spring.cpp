//
// Created by jgdo on 16.05.16.
//

#include "Spring.h"
#include "GameWorld.h"

#include <iostream>


#define DEFAULT_SPRING_LENGTH ( BodyBlock::BlockSize * 3)

Spring::Spring(RID rid, GameWorld *gameWorld, RigidBodyPart *bodyPart, BlockIndex const &position, BlockIndex const &direction):
		PhysicalObject(rid), _gameWorld(gameWorld), _fullBody(bodyPart->getParent()), _bodyA(bodyPart), _bodyB(nullptr) {
	// TODO: make nicer, maybe extra methods in RigidBodyPart??
	// btVector3 axisOrig(direction.x() * BodyBlock::BlockSize / 2, direction.y() * BodyBlock::BlockSize / 2, direction.z() * BodyBlock::BlockSize / 2);
	btVector3 rOrig = btVector3(position.x() * BodyBlock::BlockSize, position.y() * BodyBlock::BlockSize, position.z() * BodyBlock::BlockSize);
	_rA = rOrig;

	if (direction == BlockIndex(1, 0, 0))
		_axA = btQuaternion(btVector3(0, 1, 0), 0);
	else if (direction == BlockIndex(-1, 0, 0))
		_axA = btQuaternion(btVector3(0, 1, 0), M_PI)
		;
	else if (direction == BlockIndex(0, 0, 1))
_axA = btQuaternion(btVector3(0, 1, 0), -M_PI_2)
		;
	else if (direction == BlockIndex(0, 0, -1))
_axA = btQuaternion(btVector3(0, 1, 0), M_PI_2)
		;
	else if (direction == BlockIndex(0, 1, 0))
_axA = btQuaternion(btVector3(0, 0, 1), M_PI_2)
		;
	else if (direction == BlockIndex(0, -1, 0))
_axA = btQuaternion(btVector3(0, 0, 1), -M_PI_2)
		;
	else
		throw std::runtime_error("Spring::Spring"); // FIXME

	bodyPart->addSpring(this, position, direction);
}

Spring::~Spring() {
	std::cout << "~Spring" << std::endl;

	if (_springConstraint) {
		_gameWorld->getBtDynWorld()->removeConstraint(_springConstraint.get());
	}

	if (_bodyA) {
		_bodyA->removeSpring(this);
	}

	if (_bodyB) {
		_bodyB->removeSpring(this);
	}
}

std::ostream& operator<<(std::ostream& lhs, const btVector3& rhs) {
	lhs << "(" << rhs.x() << ", " << rhs.y() << ", " << rhs.z() << ")" << std::endl;
	return lhs;
}

btMatrix3x3 getAxisRotation(const btMatrix3x3& rot, const btVector3& axis) {
	auto axisN = axis.normalized();
	btVector3 axisAux(axisN.y(), axisN.z(), axisN.x());
	btVector3 axis2 = axisN.cross(axisAux);
	btVector3 axis3 = axisN.cross(axis2);

	return btMatrix3x3(axisN.x(), axis2.x(), axis3.x(), axisN.y(), axis2.y(), axis3.y(), axisN.z(), axis2.z(), axis3.z());
}

void Spring::attachBodyPart(RigidBodyPart *bodyPart, BlockIndex const &position, BlockIndex const &direction) {
	if (_springConstraint)
		throw std::runtime_error("spring already attached to two bodies");

	// TODO: make nicer, maybe extra methods in RigidBodyPart??
	// btVector3 axisNew(direction.x() * BodyBlock::BlockSize / 2, direction.y() * BodyBlock::BlockSize / 2, direction.z() * BodyBlock::BlockSize / 2);
	btVector3 rNew = btVector3(position.x() * BodyBlock::BlockSize, position.y() * BodyBlock::BlockSize, position.z() * BodyBlock::BlockSize);
	// axisNew.normalize();

	btQuaternion quat;
	if (direction == BlockIndex(1, 0, 0))
		quat = btQuaternion(btVector3(0, 1, 0), 0);
	else if (direction == BlockIndex(-1, 0, 0))
		quat = btQuaternion(btVector3(0, 1, 0), M_PI)
				;
	else if (direction == BlockIndex(0, 0, 1))
		quat = btQuaternion(btVector3(0, 1, 0), -M_PI_2)
				;
	else if (direction == BlockIndex(0, 0, -1))
		quat = btQuaternion(btVector3(0, 1, 0), M_PI_2)
				;
	else if (direction == BlockIndex(0, 1, 0))
		quat = btQuaternion(btVector3(0, 0, 1), M_PI_2)
				;
	else if (direction == BlockIndex(0, -1, 0))
		quat = btQuaternion(btVector3(0, 0, 1), -M_PI_2)
				;
	else
		throw std::runtime_error("Spring::attachBodyPart"); // FIXME

	if (_bodyA) { // new body will be B -> invert axis
		_bodyB = bodyPart;
		_rB = rNew;
		_axB = quat;
	} else { // new body will be A
		_bodyA = bodyPart;
		_rA = rNew;
		_axA = quat;
	}

	bodyPart->addSpring(this, position, direction);

	if (_bodyA && _bodyB) { // both bodies attached now, create bullet hinge!
		btTransform m_rbAFrame = getFrameTransformForPart(_bodyA, _rA, _axA);
		btTransform m_rbBFrame = getFrameTransformForPart(_bodyB, _rB, _axB);

		_springConstraint = std::shared_ptr<btGeneric6DofSpring2Constraint>(new btGeneric6DofSpring2Constraint(*_bodyA->getBtBody(), *_bodyB->getBtBody(), m_rbAFrame, m_rbBFrame));

		_springConstraint->setLimit(0, BodyBlock::BlockSize,5*BodyBlock::BlockSize);
		_springConstraint->setLimit(1, 0, 0);
		_springConstraint->setLimit(2, 0, 0);
		_springConstraint->setLimit(3, 0, 0);
		_springConstraint->setLimit(4, 0, 0);
		_springConstraint->setLimit(5, 0, 0);
		_springConstraint->enableSpring(0, true);
		_springConstraint->setStiffness(0, 1000);
		_springConstraint->setDamping(0, 5);
		_springConstraint->setEquilibriumPoint(0, DEFAULT_SPRING_LENGTH);
		_springConstraint->setParam(BT_CONSTRAINT_ERP, 0.95);
		_springConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 0.95);
		_springConstraint->setParam(BT_CONSTRAINT_CFM, 0.0);
		_springConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0);
		_gameWorld->getBtDynWorld()->addConstraint(_springConstraint.get(), true); // FIXME: pivot correction on mass center change in RigidBodyPart fails if collisions not disabled
	}
}

void Spring::detachBodyPart(RigidBodyPart *bodyPart, bool informBody) {
	if (!bodyPart)
		return; // nothing to remove

	if (_springConstraint) {
		_gameWorld->getBtDynWorld()->removeConstraint(_springConstraint.get());
		_springConstraint.reset();
	}

	if (_bodyA == bodyPart) {
		if (informBody)
			_bodyA->removeSpring(this);
		_bodyA = nullptr;
	} else if (_bodyB == bodyPart) {
		if (informBody)
			_bodyB->removeSpring(this);
		_bodyB = nullptr;
	} else
		throw std::runtime_error("Given body part was not attached to this spring");
}

btTransform Spring::getCenterTransform() {
	if(_springConstraint) {
		btTransform trans = _springConstraint->getRigidBodyA().getWorldTransform()*_springConstraint->getFrameOffsetA();
		_springConstraint->calculateTransforms();
		float pivotPos = _springConstraint->getRelativePivotPosition(0);
		trans.getOrigin() += (pivotPos/2) * _springConstraint->getAxis(0);
		return trans;
	} else if (_bodyA) {
		btTransform trans = _bodyA->getCurrentWorldTransform()*btTransform(_axA, _rA);
		trans.getOrigin() += trans.getBasis()*btVector3(1,0,0)*DEFAULT_SPRING_LENGTH/2;
		return trans;
	} else if (_bodyB) {
		btTransform trans = _bodyB->getCurrentWorldTransform()*btTransform(_axB, _rB);
		trans.getOrigin() += trans.getBasis()*btVector3(1,0,0)*DEFAULT_SPRING_LENGTH/2;
		return trans;
	} else
		throw std::logic_error("spring is not attached to any body");
}

float Spring::getSpringElongation() {
	if(_springConstraint) {
		_springConstraint->calculateTransforms();
		return _springConstraint->getRelativePivotPosition(0) - BodyBlock::BlockSize;

	} else
		return DEFAULT_SPRING_LENGTH - BodyBlock::BlockSize;
}


btTransform Spring::getFrameTransformForPart(const RigidBodyPart *part, btVector3 const& pivot, btQuaternion const& rot) {
	btTransform partTrans = part->getLocalBodyTransform().inverse();
	btTransform frameTrans;
	frameTrans.getOrigin() = partTrans*pivot;
	frameTrans.setRotation(partTrans.getRotation()*rot);
	return frameTrans;
}




