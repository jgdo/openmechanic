#pragma once

#include "Utils/GameObject.h"
#include "ObjectData.h"
#include "GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>

struct FullBodyData  {
};

template<>
struct ClientInterface<FullBodyData>  {
};

template<>
struct ServerInterface<FullBodyData>  {
};

