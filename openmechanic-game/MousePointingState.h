#pragma once

#include <btBulletDynamicsCommon.h>

#include <GamePhysics/Blocks/BodyBlock.h>
#include <Utils/Resource.h>

struct MousePointingState {
    enum TargetType {
        NONE,
        // GROUND,
        LIFT,
        LIFT_SIDE,
        BLOCK,
    };

    TargetType targetType = NONE;

    const BodyBlock* block = nullptr;

    ResourceID targetID = 0; // liftID if LIFT|LIFT_SIDE, bodyPartID if BLOCK

    btVector3 position; // for LIFT

    BlockIndex bodyBaseIndex; // for BLOCK
    BlockIndex placeDirection; // for BLOCK and LIFT, should be 'up' if LIFT

    size_t orientationIndex = 0;
};
