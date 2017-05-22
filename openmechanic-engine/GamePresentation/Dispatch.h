#pragma once

#include <memory>

class DispatcherBase {
public:
  inline virtual ~DispatcherBase() {}
};

template <class T>
class Dispatcher: public virtual DispatcherBase {
public:
  virtual void dispatch(const T& t) = 0;
};

class DispatchableBase {
public:
  typedef std::shared_ptr<DispatchableBase> Ptr;
  typedef std::shared_ptr<const DispatchableBase> ConstPtr;
  
  virtual bool tryDispatchSelf(DispatcherBase* base) const = 0;
  virtual inline ~DispatchableBase() {}
};

typedef DispatchableBase::Ptr DispatchableBasePtr;
typedef DispatchableBase::ConstPtr DispatchableBaseConstPtr;

template<class T>
class Dispatchable: public DispatchableBase {
public:
  virtual bool tryDispatchSelf(DispatcherBase* base) const override {
    if(auto dispatcher = dynamic_cast<Dispatcher<T>*>(base)) {
      dispatcher->dispatch(*static_cast<const T*>(this));
      return true;
    } else
      return false;
  }
};

#define DECLARE_DISPATCHABLE_CLASS(name) class name: public Dispatchable<name>
#define DECLARE_DISPATCHABLE_STRUCT(name) struct name: public Dispatchable<name>

template <class... T>
class DispatcherFor: public Dispatcher<T>... { };
