#include "Joint.h"
#include "RigidBodyPart.h"
#include "GameWorld.h"

#include <iostream>

Joint::Joint(RID rid, GameWorld* gameWorld, RigidBodyPart* bodyPart, BlockIndex const& position, BlockIndex const& direction) :
	PhysicalControlObject(rid, gameWorld->getControlEngine()),
	_gameWorld(gameWorld), _fullBody(bodyPart->getParent()), _bodyA(bodyPart), _bodyB(nullptr), _jointCEP(this) {
	// TODO: make nicer, maybe extra methods in RigidBodyPart??
	btVector3 axisOrig(direction.x() * BodyBlock::BlockSize / 2, direction.y() * BodyBlock::BlockSize / 2, direction.z() * BodyBlock::BlockSize / 2);
	btVector3 rOrig = axisOrig + btVector3(position.x() * BodyBlock::BlockSize, position.y() * BodyBlock::BlockSize, position.z() * BodyBlock::BlockSize);
	axisOrig.normalize();

	_rA = rOrig;
	_axA = axisOrig;

	bodyPart->addJoint(this, position, direction);
}

Joint::~Joint() {
	std::cout << "~Joint" << std::endl;

	disconnectControl();

	if (_btHinge) {
		_gameWorld->getBtDynWorld()->removeConstraint(_btHinge.get());
	}

	if (_bodyA) {
		_bodyA->removeJoint(this);
	}

	if (_bodyB) {
		_bodyB->removeJoint(this);
	}
}

void Joint::attachBodyPart(RigidBodyPart * bodyPart, BlockIndex const& position, BlockIndex const& direction) {
	if (_btHinge)
		throw std::runtime_error("joint already attached to two bodies");

	// TODO: make nicer, maybe extra methods in RigidBodyPart??
	btVector3 axisNew(direction.x() * BodyBlock::BlockSize / 2, direction.y() * BodyBlock::BlockSize / 2, direction.z() * BodyBlock::BlockSize / 2);
	btVector3 rNew = axisNew + btVector3(position.x() * BodyBlock::BlockSize, position.y() * BodyBlock::BlockSize, position.z() * BodyBlock::BlockSize);
	axisNew.normalize();

	if (_bodyA) { // new body will be B -> invert axis
		axisNew = -axisNew;

		_bodyB = bodyPart;
		_rB = rNew;
		_axB = axisNew;
	} else { // new body will be A
		_bodyA = bodyPart;
		_rA = rNew;
		_axA = axisNew;
	}

	bodyPart->addJoint(this, position, direction);

	if (_bodyA && _bodyB) { // both bodies attached now, create bullet hinge!
		// TODO: method for computing the real axis and pivots
		btTransform transA = _bodyA->getLocalBodyTransform().inverse();
		btTransform transB = _bodyB->getLocalBodyTransform().inverse();

		btVector3 realRA = transA(_rA);
		btVector3 realRB = transB(_rB);

		btVector3 realAxA = transA.getBasis() * _axA;
		btVector3 realAxB = transB.getBasis() * _axB;

		_btHinge = std::make_shared<btHingeConstraint>(*_bodyA->getBtBody(), *_bodyB->getBtBody(), realRA, realRB, realAxA, realAxB);
		_gameWorld->getBtDynWorld()->addConstraint(_btHinge.get(), true); // FIXME: pivot correction on mass center change in RigidBodyPart fails if collisions not disabled

		// _btHinge->setOverrideNumSolverIterations(300); // FIXME: find out proper value
		_btHinge->setParam(BT_CONSTRAINT_STOP_ERP, 0.9);
		_btHinge->setParam(BT_CONSTRAINT_CFM, 0.1);
		_angleOffset = _btHinge->getHingeAngle();
	}
}

void Joint::detachBodyPart(RigidBodyPart * bodyPart, bool informBody) {
	if (!bodyPart)
		return; // nothing to remove

	if (_btHinge) {
		_gameWorld->getBtDynWorld()->removeConstraint(_btHinge.get());
		_btHinge.reset();
	}

	if (_bodyA == bodyPart) {
		if (informBody)
			_bodyA->removeJoint(this);
		_bodyA = nullptr;
	} else if (_bodyB == bodyPart) {
		if (informBody)
			_bodyB->removeJoint(this);
		_bodyB = nullptr;
	} else
		throw std::runtime_error("Given body part was not attached to this joint");
}

btTransform Joint::getCurrentWorldTransform() const {
	/*if (isAttached()) {
		throw std::logic_error("there is no world transform to a fully attached joint");
	}*/

	// FIXME rotation
	if (_bodyA) {
		return _bodyA->getCurrentWorldTransform() * btTransform(btQuaternion(0, 0, 0, 1), _rA);
	} else if (_bodyB) {
		return _bodyB->getCurrentWorldTransform() * btTransform(btQuaternion(0, 0, 0, 1), _rB);
	} else
		throw std::logic_error("joint is not attached to any body");
}

float Joint::getJointAngle() const {
	if (!isAttached()) {
		throw std::runtime_error("Cannot get joint angle of unconnected joint");
	}

	return _btHinge->getHingeAngle() - _angleOffset;
}

std::vector<ConnectionEndpoint*> Joint::getConnectionEndpoints() {
	return {&_jointCEP};
}

btVector3 Joint::getObjectPosition() const {
	if (_bodyA) {
		return _bodyA->getCurrentWorldTransform() * _rA;
	}
	else if (_bodyB) {
		return _bodyB->getCurrentWorldTransform() * _rB;
	}
	else
		throw std::logic_error("joint is not attached to any body");
}

float Joint::JointConnectionEndpoint::computeOutput()
{
	throw std::runtime_error("JointConnectionEndpoint::computeOutput(): Joint has no output. Check connection matchmaking");
}

void Joint::JointConnectionEndpoint::applyInput(float in)
{
	in *=  _parentObject->_inputMultiplier;

	if(_parentObject->_btHinge)
	{
		if(_connectionState == FORCE_SINK)
		{
			_parentObject->_btHinge->enableAngularMotor(true, in, 10000); // FIXME magic number
			if (std::abs(in) > 0.001) // FIXME
			{
				_parentObject->_bodyA->getBtBody()->activate(true);
				_parentObject->_bodyB->getBtBody()->activate(true);
			}
		}
		else if (_connectionState == SIGNAL_SINK)
		{
			float dAngle = in*0.5 - _parentObject->getJointAngle();
			// FIXME: check if overshoot will happen in next time step

			static const float speed = 1;
			float v = 0;
			if (dAngle > 0.001)
				v = speed;
			else if (dAngle < -0.001)
				v = -speed;

			_parentObject->_btHinge->enableAngularMotor(true, v, 5000000); // FIXME magic number
			if (std::abs(v) > 0.001) // FIXME
			{
				_parentObject->_bodyA->getBtBody()->activate(true);
				_parentObject->_bodyB->getBtBody()->activate(true);
			}
		}
	}
}

void Joint::JointConnectionEndpoint::setConnectionState(int state)
{
	if (state == NO_CONNECTION || state == FORCE_SINK || state == SIGNAL_SINK)
		_connectionState = state;
	else
		throw std::runtime_error("Joint::setConnectionState(): Only NO_CONNECTION, FORCE_SINK or SIGNAL_SINK allowed");

	if (_parentObject->_btHinge && state == NO_CONNECTION) {
		_parentObject->_btHinge->enableMotor(false);
	}
}

int Joint::JointConnectionEndpoint::getConnectionState() const
{
	return _connectionState;
}

int Joint::JointConnectionEndpoint::getConnectionType() const {
	return FORCE_SINK | SIGNAL_SINK;
}
