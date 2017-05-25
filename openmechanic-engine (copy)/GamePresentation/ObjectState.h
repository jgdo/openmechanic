#pragma once

#include <btBulletDynamicsCommon.h>

#include "Utils/GameObject.h"

#include "Dispatch.h"

class ObjectStateVisitor;

class ObjectState
{
public:
	typedef std::shared_ptr<const ObjectState> Ptr;
	const ObjectID resourceID;
	ObjectState(RID rid);
};

typedef ObjectState::Ptr ObjectStatePtr;

ATTRIBUTE_ALIGNED16(class) BodyPartState: public ObjectState, public Dispatchable<BodyPartState>
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	BodyPartState(ObjectID rID, const btTransform& transform) :
		ObjectState(rID), worldTransform(transform)
	{
	}

	const btTransform worldTransform;
};

ATTRIBUTE_ALIGNED16(class) JointState: public ObjectState, public Dispatchable<JointState>
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	JointState(ObjectID jointID, const btTransform& transform) :
		ObjectState(jointID), worldTransform(transform)
	{
	}

	const btTransform worldTransform;
};

ATTRIBUTE_ALIGNED16(class) LiftState: public ObjectState, public Dispatchable<LiftState>
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	LiftState(ObjectID jointID, const btVector3& pos, float height, int halfSize) :
		ObjectState(jointID), position(pos), actualHeight(height), platformHalfSize(halfSize)
	{
	}

	const btVector3 position;
	const btScalar actualHeight;
	const int platformHalfSize;
};

ATTRIBUTE_ALIGNED16(class) PlayerBodyState: public ObjectState, public Dispatchable<PlayerBodyState> {
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	PlayerBodyState(ObjectID playerID, const btVector3& pos, btQuaternion const& orientation) :
		ObjectState(playerID), headPosition(pos), headOrientation(orientation)
	{
	}

	const btVector3 headPosition;
	const btQuaternion headOrientation;
	// later maybe such things as attached control etc.
};

ATTRIBUTE_ALIGNED16(class) SpringState: public ObjectState, public Dispatchable<SpringState> {
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	SpringState(ObjectID springID, const btVector3& position, btQuaternion const& orientation, float elongation) :
			ObjectState(springID), position(position), orientaion(orientation), elongation(elongation)
	{
	}

	const btVector3 position;
	const btQuaternion orientaion;
	const float elongation;
	// later maybe such things as attached control etc.
};