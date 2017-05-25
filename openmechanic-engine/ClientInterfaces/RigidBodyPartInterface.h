#pragma once

#include "Utils/GameObject.h"
#include "ObjectData.h"
#include "GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>

#include "AsyncObjectContainer.h"

struct RigidBodyPartData  {
  btTransform worldTransform;
};

template<>
struct ClientInterface<RigidBodyPartData>  {
	virtual void changed(const AsyncObjectContainer::DataReadIndex<RigidBodyPartData>& idx) = 0;
};

template<>
struct ServerInterface<RigidBodyPartData>  {
  virtual void addBlock(AsyncObjectContainer::DataWriteIndex<RigidBodyPartData> partIdx, BlockID blockId, BlockIndex basePosition, BlockIndex direction, btQuaternion orientation) = 0;
  virtual void removeBlockOrJoint(AsyncObjectContainer::DataWriteIndex<RigidBodyPartData> partIdx, BlockIndex index, BlockIndex direction) = 0;
};

