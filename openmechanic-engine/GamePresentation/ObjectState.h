#pragma once

#include <btBulletDynamicsCommon.h>

#include "Utils/Resource.h"

#include "Dispatch.h"

class ObjectStateVisitor;

class ObjectState: public DispatchableBase
{
public:
	typedef std::shared_ptr<const ObjectState> Ptr;
	const ResourceID resourceID;
    ObjectState(RID rid): resourceID(rid) {}
};

template<class T>
class DispatchableObjectState: public ObjectState {
public:
    using ObjectState::ObjectState;

  virtual bool tryDispatchSelf(DispatcherBase* base) const override {
    if(auto dispatcher = dynamic_cast<Dispatcher<T>*>(base)) {
      dispatcher->dispatch(*static_cast<const T*>(this));
      return true;
    } else
      return false;
  }
};

typedef ObjectState::Ptr ObjectStatePtr;

ATTRIBUTE_ALIGNED16(class) BodyPartState: public DispatchableObjectState<BodyPartState>
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	BodyPartState(ResourceID rID, const btTransform& transform) :
        DispatchableObjectState(rID), worldTransform(transform)
	{
	}

	const btTransform worldTransform;
};

ATTRIBUTE_ALIGNED16(class) JointState: public DispatchableObjectState<JointState>
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	JointState(ResourceID jointID, const btTransform& transform) :
        DispatchableObjectState(jointID), worldTransform(transform)
	{
	}

	const btTransform worldTransform;
};

ATTRIBUTE_ALIGNED16(class) LiftState: public DispatchableObjectState<LiftState>
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	LiftState(ResourceID jointID, const btVector3& pos, float height, int halfSize) :
        DispatchableObjectState(jointID), position(pos), actualHeight(height), platformHalfSize(halfSize)
	{
	}

	const btVector3 position;
	const btScalar actualHeight;
	const int platformHalfSize;
};

ATTRIBUTE_ALIGNED16(class) PlayerBodyState: public DispatchableObjectState<PlayerBodyState> {
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	PlayerBodyState(ResourceID playerID, const btVector3& pos, btQuaternion const& orientation) :
        DispatchableObjectState(playerID), headPosition(pos), headOrientation(orientation)
	{
	}

	const btVector3 headPosition;
	const btQuaternion headOrientation;
	// later maybe such things as attached control etc.
};

ATTRIBUTE_ALIGNED16(class) SpringState: public DispatchableObjectState<SpringState> {
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	SpringState(ResourceID springID, const btVector3& position, btQuaternion const& orientation, float elongation) :
            DispatchableObjectState(springID), position(position), orientaion(orientation), elongation(elongation)
	{
	}

	const btVector3 position;
	const btQuaternion orientaion;
	const float elongation;
	// later maybe such things as attached control etc.
};
