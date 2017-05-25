#pragma once

#include "Utils/GameObject.h"
#include "ObjectData.h"
#include "GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>

struct GameWorldData  {
};

template<>
struct ClientInterface<GameWorldData>  {
};

template<>
class ServerInterface<GameWorldData>  {
  virtual void addBlockAsNewBody(DataIndex idx, BlockID blockId, btVector3 worldPosition; btQuaternion worldOrientation, btQuaternion localBlockOrientation) = 0;
};

