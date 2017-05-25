#pragma once

#include "Utils/GameObject.h"
#include "ObjectData.h"
#include "GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>

struct RigidBodyPartData  {
  btTransform worldTransform;
};

template<>
struct ClientInterface<RigidBodyPartData>  {
};

template<>
struct ServerInterface<RigidBodyPartData>  {
  virtual void addBlock(DataIndex partIdx, BlockID blockId, BlockIndex basePosition, BlockIndex direction, btQuaternion orientation) = 0;
  virtual void removeBlockOrJoint(DataIndex partIdx, BlockIndex index, BlockIndex direction) = 0;
};

