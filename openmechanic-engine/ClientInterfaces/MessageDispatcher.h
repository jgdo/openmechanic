#pragma once

#include <ClientInterfaces/Messages/Msg.h>

#include <functional>
#include <unordered_map>

namespace detail {

template<class T>
class MessageDispatcherImpl {
public:
    MessageDispatcher(std::unordered_map<std::size_t, std::function<void(const Message*)>>& dispatchers) {
        dispatchers[Message::ID<T>()] = std::bind(this, &MessageDispatcherImpl::dispatchImpl);
    }

protected:
    virtual void dispatchImpl(const T* msg) = 0;
};

}


template<class... Ts>
class MessageDispatcher: public MessageDispatcherImpl<Ts>... {
public:
    MessageDispatcher():
        MessageDispatcherImpl(mDispatchers) {
    }

void dispatch(const Message* msg) {
    auto iter = mDispatchers.find(msg->getID());
    if(iter != mDispatchers.end()) {
        (*iter)(msg);
    } else {
        // TODO error log
    }
}

private:
    std::unordered_map<std::size_t, std::function<void(const Message*)>> mDispatchers;

};
