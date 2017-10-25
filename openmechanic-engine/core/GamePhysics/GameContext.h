#pragma once

#include <ClientInterfaces/DataObjectContainer.h>
#include <core/GamePhysics/BodyBlockFactory.h>
#include "core/Utils/ObjectManger.h"
#include <ClientInterfaces/MessageQueue.h>

struct GameContext {
    DataObjectContainer dataObjectContainer;

    ObjectManager objectMan;

    BodyBlockFactory blockFactory;

    MessageQueue* outQueue;
};
