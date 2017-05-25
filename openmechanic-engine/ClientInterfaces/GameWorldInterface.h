#pragma once

#include "Utils/GameObject.h"
#include "ObjectData.h"
#include "GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>
#include "AsyncObjectContainer.h"

struct GameWorldData  {
};

template<>
struct ClientInterface<GameWorldData>  {
	virtual void changed(const AsyncObjectContainer::DataReadIndex<GameWorldData>& idx) = 0;
};

template<>
struct ServerInterface<GameWorldData>  {
  virtual void addBlockAsNewBody(AsyncObjectContainer::DataWriteIndex<GameWorldData>& idx, BlockID blockId, btVector3 worldPosition, btQuaternion worldOrientation, btQuaternion localBlockOrientation) = 0;
};

