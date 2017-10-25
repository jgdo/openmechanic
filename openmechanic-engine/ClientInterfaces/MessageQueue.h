#pragma once

#include "Messages/Msg.h"

class MessageQueue {
public:
    /**
     * @brief postMessage
     * @param msg Ownership transferred to this queue
     */
    virtual void postMessage(const Message* msg) = 0;
};
