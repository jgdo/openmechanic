#pragma once

#include "core/Utils/GameObject.h"
#include "core/ObjectData.h"
#include "core/GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>
#include "core/AsyncObjectContainer.h"

#include "Msg.h"

struct GameWorldData {
};

namespace Msg {

namespace GameWorld {

namespace To {
struct AddBlockAsNewBody: public Message {
    OID oid;
    BlockID blockId;
    btVector3 worldPosition;
    btQuaternion worldOrientation;
    btQuaternion localBlockOrientation;
};

}

}

}


