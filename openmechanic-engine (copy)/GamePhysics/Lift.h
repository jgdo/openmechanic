/* 
 * File:   Lift.h
 * Author: jgdo
 *
 * Created on February 5, 2016, 12:13 AM
 */

#ifndef LIFT_H
#define	LIFT_H

#include "Blocks/BodyBlock.h"
#include "PhysicalObject.h"

#include <memory>

class FullBody;
class GameWorld;

ATTRIBUTE_ALIGNED16(class) Lift : public PhysicalObject {
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	DECLARE_RESOURCE(Lift);

	virtual ~Lift();

	/**
	 * Place the lift onto the world such that new structures can be build on it
	 * If already deployed just adjust position and height
	 * if body attached it will be detached unconditionally
	 */
	void deploy(const btVector3& position, btScalar initialHight);

	/**
	 * Fold the lift, i.e. make it go away, remove collision body from the world etc.
	 * if body attached it will be detached unconditionally
	 */
	void fold();

	/**
	 * does nothing if lift is folded
	 *
	 * @param direction if 0 raising stops at next position, otherwise sign determines direction
	 */
	void setRaiseStep(float direction);

	/*
		Called at every step to perform the raise
	*/
	void performRaise(float dt);

	/**
	 *
	 * @param position
	 * @return
	 */
	btTransform getTransformForPlacementPosition(BlockIndex const& position);

	inline bool isDeployed() const {
		return _body != nullptr;
	}

	btVector3 getPositionOnGround() const;
	btVector3 getCenterPositionOnTop(float heightOffset = 0) const;

	btScalar getHeightAboveGround() const;

	inline int getPlatformHalfSize() const {
		return _liftPlatformHalfSize;
	}

	/*
	returns true if index is in lift range
	otherwise *trans will be undefined
	*/
	bool getPlacementPosition(BlockIndex const& index, btTransform* trans);

	/************************************************************************/
	/* reset the given body, freeze it and couple its positions to the top of the lift
	// unfreeze the body attached before if any
	************************************************************************/
	void attachFullBody(FullBody * body, btVector3 const& attachOffset);

	inline void detachFullBody() {
		attachFullBody(nullptr, btVector3(0,0,0));
	}

	inline bool isBodyAttahed() const {
		return _attachedBody != nullptr;
	}
	
	inline FullBody* getAttachedBody() const {
		return _attachedBody;
	}

	float getHeight();

private:
	const static float RAISE_SPEED;

	Lift(RID rid, GameWorld* gameWorld);

	btVector3 _attachedBodyOffset;

	const float _liftPlatformHeight = BodyBlock::BlockSize / 4;
	const int _liftPlatformHalfSize = 1; // -> placement index is inside (-value, +value) => size is value*2 + 1

	GameWorld* _gameWorld;

	// owned by this Lift, manual mem management since destruction order is important
	btRigidBody* _body = nullptr;
	btCollisionShape* _collisionShape = nullptr;

	btScalar _height;

	FullBody * _attachedBody = nullptr;

	btScalar _heightTarget;

	btScalar limitHeight(btScalar height) const;

	btVector3 _basePosition;

	void applyCurrentHeight();

	void updateAttachedBodyPosition();
};

#endif	/* LIFT_H */

