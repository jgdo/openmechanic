#pragma once

#include "GameObject.h"
#include "ObjectIDGenerator.h"

#include <memory>
#include <unordered_map>

class ObjectManager {
public:
    template<typename T, typename... Args>
    T* create(Args&&... args)
    {
        OID oid = mOIDGen.generateNewID();
        T* object = new T(oid, std::forward<Args>(args)...);
        mAllObjects.emplace(std::piecewise_construct,
                            std::forward_as_tuple(oid),
                            std::forward_as_tuple(object));
        return object;
    }

    void destroy(OID oid) {
        auto iter = mAllObjects.find(oid);
        if(iter != mAllObjects.end()) {
            mAllObjects.erase(iter);
        }
        // else TODO report error
    }

    template<class T>
    T* find(OID oid) {
       auto iter = mAllObjects.find(oid);
       if(iter != mAllObjects.end()) {
           if(T* object = dynamic_cast<T*>(iter->second.get())) {
               return object;
           }
       }

       return nullptr;
    }

private:
    ObjectIDGenerator mOIDGen;

    std::unordered_map<ObjectID, std::unique_ptr<GameObject>> mAllObjects;
};
