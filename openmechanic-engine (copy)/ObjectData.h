#pragma once

#include <functional>
#include <memory>
#include <list>

class AbstractDataBin;

struct AbstractGameEventHandler {
    virtual ~AbstractGameEventHandler() {}
};

template<class D>
struct ClientInterface;

typedef std::function<AbstractGameEventHandler*(void)> AbstractDataHandlerFactory;

template<class D>
struct ServerInterface;

typedef std::list<std::shared_ptr<AbstractDataBin>>::iterator DataIndex;

template<class D>
struct ObjectData: public D {
    std::size_t version = 0;

    using D::D;
};
