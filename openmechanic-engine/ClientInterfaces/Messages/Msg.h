#pragma once

#include <typeindex>


struct Message {
    virtual ~Message() {}

    virtual std::size_t getID() const {
        return std::type_index(typeid(*this)).hash_code();
    }

    template<class T>
    static std::size_t ID() {
        return std::type_index(typeid(T)).hash_code();
    }
};
