#pragma once

#include <typeindex>
#include <map>
#include <typeinfo>

#include <iostream>

#include <type_traits>
#include <list>
#include <memory>
#include <set>

#include <Utils/GameObject.h>

#include "ObjectData.h"

struct AbstractDataBin {
    const ObjectID rID;
    
    inline AbstractDataBin(ObjectID rid): rID(rid) {}
    virtual ~AbstractDataBin() {}

    virtual void applyWrite(std::size_t currentVersion) = 0;
    
    virtual void readNext() = 0;
    virtual void notifyChanged() = 0;
};


template<class D>
struct DataBin: public AbstractDataBin {
    typedef GameEventHandler<D> HandlerType;
    typedef ClientRequestHandler<D> RequestHandlerType;

  DataBin(ObjectID rid, const AbstractDataHandlerFactory& handlerFactory, RequestHandlerType& reqHandler): AbstractDataBin(rid), handlerFactory(handlerFactory), requestHandler(reqHandler)   {
      writeActive = tripleBuffer+0;
      readActive = tripleBuffer+1;
      nextRead = tripleBuffer+2;
  }
  
  const D& getRead() const {
      return *readActive;
  }
  
   D& getWrite() {
      return *writeActive;
  }
   
  virtual void readNext() override {
      if(!handler) {
          HandlerType* h = dynamic_cast<typename DataBin<D>::HandlerType*>(handlerFactory());
          handler = std::shared_ptr<typename DataBin<D>::HandlerType>(h);
      }
      
      if(nextRead->version > readActive->version) {
          std::swap(nextRead, readActive);
      }
  }
   
  virtual void applyWrite(std::size_t currentVersion) override {
      writeActive->version = currentVersion;
      std::swap(writeActive, nextRead);
  }
  
  inline HandlerType& getHandler() {      
      return *handler;
  }
  
   RequestHandlerType& getRequestHandler() {      
      return requestHandler;
  }

 virtual void notifyChanged() override {
     getHandler().changed(rID, getRead());
 }
 
private:
  ObjectData<D>* writeActive, *readActive, *nextRead;
  ObjectData<D> tripleBuffer[3];
  
  AbstractDataHandlerFactory handlerFactory;
  std::shared_ptr<HandlerType> handler;
  
  RequestHandlerType& requestHandler;
};


inline bool operator<(std::list<std::shared_ptr<AbstractDataBin>>::iterator a, std::list<std::shared_ptr<AbstractDataBin>>::iterator b) 
{
    return &*a < &*b;
}

struct AsyncObjectContainer {
    enum {
        CREATED = 0,
        CHANGED = (1<<0),
        DELETED = (1<<1),
    };
    
    template<class H, class ...T>
    void registerHandlerType(T ...params) {
        ctorMap[std::type_index(typeid(typename H::DataType))] = [=] () {return new H(params...);};
    } 
    
   template<class D>
   D& getWrite(DataIndex iter) {
       changeSet[iter] |= CHANGED;
       return getBin<D>(iter).getWrite();
   }
   
   template<class D, class P, class ...T>
   void postEvent(DataIndex iter, P ptr, T ...params) {
       eventList.emplace_back([=] () { auto& bin = getBin<D>(iter); (bin.getHandler().*ptr)(bin.rID, bin.getRead(), params...); } );
   }
   
   template<class D, class P, class ...T>
   void postRequest(DataIndex iter, P ptr, T ...params) {
       requestList.emplace_back([=] () { auto& bin = getBin<D>(iter); (bin.getRequestHandler().*ptr)(iter, params...); } );
   }
   
   template<class D>
   DataIndex newBin(ObjectID rid, ClientRequestHandler<D>& reqHandler) {
       dataBins.push_back(std::make_shared<DataBin<D>>(rid, ctorMap[std::type_index(typeid(D))], reqHandler));
       DataIndex iter = --dataBins.end();
       changeSet[iter]; // make sure that changeSet[iter] is constructed
       return iter;
  }
  
  void deleteBin(DataIndex iter) {
    changeSet[iter] |= DELETED;
  }
  
  void updateReadStateAndNotify();
  
  void applyChanges();
  
private:
    std::list<std::shared_ptr<AbstractDataBin>> dataBins;
    std::list<std::function<void(void)>> eventList;
    std::list<std::function<void(void)>> requestList;
    std::map<std::type_index, AbstractDataHandlerFactory> ctorMap;
    std::map<DataIndex, int> changeSet;
    
    std::size_t version = 0;

    template<class D>
    static DataBin<D>& getBin(DataIndex iter) {
        return dynamic_cast<DataBin<D>&>(**iter);
    }
    
    void renewReadState();
};
