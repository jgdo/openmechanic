#pragma once

#include <osg/Object>

#include <Utils/Resource.h>

class GameObjectDataContainer: public osg::Referenced {
public:
    enum Type {
        NONE,
        BODY_BLOCK,
        LIFT,
        TERRAIN,
    };


    ResourceID rid;
};


