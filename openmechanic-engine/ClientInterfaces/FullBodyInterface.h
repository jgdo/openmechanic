#pragma once

#include "Utils/GameObject.h"
#include "ObjectData.h"
#include "GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>
#include "AsyncObjectContainer.h"

struct FullBodyData  {
};

template<>
struct ClientInterface<FullBodyData>  {
	virtual void changed(const AsyncObjectContainer::DataReadIndex<FullBodyData>& idx) = 0;
};

template<>
struct ServerInterface<FullBodyData>  {
};

