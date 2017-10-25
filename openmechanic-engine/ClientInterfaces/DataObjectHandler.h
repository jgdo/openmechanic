#pragma once

template<class TData>
struct DataObjectHandler {
    virtual TData* setData() = 0;
};
