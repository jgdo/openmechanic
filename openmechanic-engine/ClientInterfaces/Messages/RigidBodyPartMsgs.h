#pragma once

#include "core/Utils/GameObject.h"
#include "core/ObjectData.h"
#include "core/GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>

#include "core/AsyncObjectContainer.h"
#include "core/GamePhysics/Blocks/BodyBlock.h"

#include "Msg.h"

struct RigidBodyPartData  {
  btTransform worldTransform;
};

namespace Msg {

namespace RigidBodyPart {

namespace To {
struct AddBlock: public Message {
    const OID oid;
    const BlockID blockId;
    const BlockIndex basePosition;
    const BlockIndex direction;
    const btQuaternion orientation;
};

struct RemoveBlockOrJoint: public Message {
    const OID oid;
    const BlockIndex index;
    const BlockIndex direction;
};

}

namespace From {

struct BlockAdded: public Message {
    BlockAdded(const OID oid,
    const BodyBlock* const block,
    const BlockIndex index,
    const btQuaternion relRot,
    const OID attachedControlOID
    ):
        oid(oid),
        block(block),
        index(index),
        relRot(relRot),
        attachedControlOID(attachedControlOID)
    {}

    const OID oid;
    const BodyBlock* const block;
    const BlockIndex index;
    const btQuaternion relRot;
    const OID attachedControlOID;
};

struct BlockRemoved: public Message {
    BlockRemoved(
            const OID oid,
            const BlockIndex index
            ):
        oid(oid),
        index(index)
    {}

    const OID oid;
    const BlockIndex index;
};

}

}

}
