#pragma once

#include "DataObjectHandler.h"

class DataObjectContainer {
public:
    template<class TData>
    DataObjectHandler<TData>* createDataBin() {
        return nullptr; // TODO
    }
};
