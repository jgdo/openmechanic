/* 
 * File:   Lift.cpp
 * Author: jgdo
 * 
 * Created on February 5, 2016, 12:13 AM
 */

#include "Lift.h"

#include "GameWorld.h"
#include "FullBody.h"

const float Lift::RAISE_SPEED = BodyBlock::BlockSize * 2; // 2 blocks per second

Lift::Lift(RID rid, GameWorld* gameWorld) :
	PhysicalObject(rid), _gameWorld(gameWorld) {
}

Lift::~Lift() {
	fold();

	delete _collisionShape;
}

void Lift::deploy(const btVector3& position, btScalar initialHight) {
	detachFullBody();

	if (!_collisionShape) {
		// FIXME: check if 
		btScalar size = (getPlatformHalfSize() * 2 + 1) * BodyBlock::BlockSize;
		_collisionShape = new btBoxShape(btVector3(size, _liftPlatformHeight, size) / 2);
	}

	if (!_body) {
		btMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, _collisionShape);

		_body = new btRigidBody(rigidBodyCI);
		_gameWorld->getBtDynWorld()->addRigidBody(_body);
	}

	_height = limitHeight(initialHight);
	_heightTarget = _height;

	_basePosition = position;
	applyCurrentHeight();
}

void Lift::fold() {
	detachFullBody();

	if (_body) {
		_gameWorld->getBtDynWorld()->removeCollisionObject(_body);
		delete _body->getMotionState();

		delete _body;
	}
}

btTransform Lift::getTransformForPlacementPosition(const BlockIndex& position) {
	// FIXME check if body exists
	return btTransform(btQuaternion(0, 0, 0, 1), _body->getCenterOfMassPosition() + btVector3(0, _liftPlatformHeight / 2 + BodyBlock::BlockSize/2, 0));
}

void Lift::setRaiseStep(float direction) {
	if (!isDeployed()) // ignore if not deployed
		return;

	if(direction > 0)
	{
		_heightTarget = limitHeight((std::floor(_height / BodyBlock::BlockSize) + 1) * BodyBlock::BlockSize);
	} else if(direction < 0)
	{
		_heightTarget = limitHeight((std::ceil(_height / BodyBlock::BlockSize) - 1) * BodyBlock::BlockSize);
	}

	//btScalar newHeight = limitHeight(_height + amount);
}

void Lift::performRaise(float dt)
{
	float amount = RAISE_SPEED * dt;

	if(_height > _heightTarget)
	{
		if (_height - _heightTarget <= amount)
			_height = _heightTarget;
		else
			_height -= amount;
	} else if(_height < _heightTarget)
	{
		if (_heightTarget - _height <= amount)
			_height = _heightTarget;
		else
			_height += amount;
	}
	else
		return;

	applyCurrentHeight();
	updateAttachedBodyPosition();
}

btScalar Lift::getHeightAboveGround() const {
	return _height + _basePosition.y();
}


bool Lift::getPlacementPosition(BlockIndex const& index, btTransform* trans)
{
	if (index.y() != 0)
		return false;

	if (std::abs(index.x()) > _liftPlatformHalfSize || std::abs(index.z()) > _liftPlatformHalfSize)
		return false;

	*trans = btTransform(btQuaternion(0,0,0,1), 
		getPositionOnGround() + btVector3(index.x() * BodyBlock::BlockSize, getHeightAboveGround()+BodyBlock::BlockSize/2, index.z()*BodyBlock::BlockSize));
	return true;
}


void Lift::attachFullBody(FullBody * body, btVector3 const& attachOffset)
{
	if (body == _attachedBody)
		return;

	if (_attachedBody) {
		_attachedBody->setFreezed(false);
		_attachedBody = nullptr;
	}

	if (body) {
		_attachedBody = body;
		_attachedBody->setFreezed(true);
		_attachedBodyOffset = attachOffset;
		updateAttachedBodyPosition();
	}
}

btVector3 Lift::getPositionOnGround() const {
	return _basePosition;
}


btVector3 Lift::getCenterPositionOnTop(float heightOffset) const
{
	return getPositionOnGround() + btVector3(0, _height + heightOffset, 0);
}

btScalar Lift::limitHeight(btScalar height) const {
	// FIXME: proper limits
	if (height < BodyBlock::BlockSize) {
		height = BodyBlock::BlockSize;
	}

	return height;
}

void Lift::updateAttachedBodyPosition()
{
	if (isBodyAttahed()) {
		_attachedBody->resetPosition(getCenterPositionOnTop() + _attachedBodyOffset, btQuaternion(0, 0, 0, 1));
	}
}

void Lift::applyCurrentHeight() {
	_body->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), _basePosition + btVector3(0, _height - _liftPlatformHeight / 2, 0)));
}

float Lift::getHeight() {
	return _height;
}




