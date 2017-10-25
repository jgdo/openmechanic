#pragma once

#include <ClientInterfaces/Messages/FullBodyMsgs.h>
#include <ClientInterfaces/Messages/GameWorldMsgs.h>
#include <ClientInterfaces/Messages/RigidBodyPartMsgs.h>

#include <ClientInterfaces/MessageDispatcher.h>

class ServerMessageDispatcher: MessageDispatcher<
        Message::GameWorld::To::AddBlockAsNewBody,
        Message::RigidBodyPart::To::AddBlock,
        Message::RigidBodyPart::To::RemoveBlockOrJoint
    >
{
protected:
    virtual void dispatchImpl(const Msg::GameWorld::To::AddBlockAsNewBody* msg) override {

    }

    virtual void dispatchImpl(const Msg::RigidBodyPart::To::AddBlock* msg) override {

    }

    virtual void dispatchImpl(const Msg::RigidBodyPart::To::RemoveBlockOrJoint* msg) override {

    }
};
