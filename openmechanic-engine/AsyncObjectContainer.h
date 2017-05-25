#pragma once

#include <typeindex>
#include <map>
#include <typeinfo>

#include <iostream>

#include <type_traits>
#include <list>
#include <memory>
#include <set>

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


template<class D>
struct ObjectData: public D {
    std::size_t version = 0;

    using D::D;
};


#include <Utils/GameObject.h>

#include "ObjectData.h"

class AsyncObjectContainer;

struct AbstractDataBin {
    const ObjectID rID;
    
    inline AbstractDataBin(ObjectID rid): rID(rid) {}
    virtual ~AbstractDataBin() {}

    virtual void applyWrite(std::size_t currentVersion) = 0;
    
    virtual void readNext() = 0;
    virtual void notifyChanged(AsyncObjectContainer& container, std::list<std::shared_ptr<AbstractDataBin>>::iterator iter) = 0;
};


template<class D>
struct DataBin: public AbstractDataBin {
    typedef ClientInterface<D> HandlerType;
    typedef ServerInterface<D> RequestHandlerType;

  DataBin(ObjectID rid, const AbstractDataHandlerFactory& handlerFactory, RequestHandlerType& reqHandler): AbstractDataBin(rid), handlerFactory(handlerFactory), requestHandler(reqHandler)   {
      writeActive = tripleBuffer+0;
      readActive = tripleBuffer+1;
      nextRead = tripleBuffer+2;
  }
  
  const D* getRead() const {
      return readActive;
  }
  
   D* getWrite() {
      return writeActive;
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

 virtual void notifyChanged(AsyncObjectContainer& container, std::list<std::shared_ptr<AbstractDataBin>>::iterator iter) override;
 
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

class AsyncObjectContainer {
	using Iterator = std::list<std::shared_ptr<AbstractDataBin>>::iterator;
	
public:
	template<class D>
	struct DataReadIndex {
		Iterator iter;
		AsyncObjectContainer& container;
		DataBin<D>& bin;
		
		inline DataReadIndex(Iterator iter, AsyncObjectContainer& container, DataBin<D>& bin):
			iter(iter), container(container), bin(bin) {
		}
		
		const D* operator->() {
			return bin.getRead();
		}
	};
	
	template<class D>
	struct DataWriteIndex: protected DataReadIndex<D> {
		using DataReadIndex<D>::container;
		
		inline DataWriteIndex(Iterator iter, AsyncObjectContainer& container, DataBin<D>& bin): DataReadIndex<D>(iter, container, bin) {}
		
		D* operator->() {
			return this->bin.getWrite();
		}
		
		DataReadIndex<D>& asReadIndex() {
			return *this;
		}
	};
	
    enum {
        CREATED = 0,
        CHANGED = (1<<0),
        DELETED = (1<<1),
    };
    
    template<class H, class ...T>
    void registerHandlerType(T ...params) {
        ctorMap[std::type_index(typeid(typename H::DataType))] = [=] () {return new H(params...);};
    } 
    
   
   template<class D, class P, class ...T>
   void postEvent(DataWriteIndex<D>& iter, P ptr, T ...params) {
       eventList.emplace_back([=] () { (iter.bin.getHandler().*ptr)(iter, params...); } );
   }
   
   template<class D, class P, class ...T>
   void postRequest(DataReadIndex<D> iter, P ptr, T ...params) {
       requestList.emplace_back([=] () { (iter.bin.getRequestHandler().*ptr)(DataWriteIndex<D>{iter}, params...); } );
   }
   
   template<class D>
   DataWriteIndex<D> newBin(ObjectID rid, ServerInterface<D>& reqHandler) {
	   auto ptr = std::make_shared<DataBin<D>>(rid, ctorMap[std::type_index(typeid(D))], reqHandler);
       dataBins.push_back(ptr);
       auto iter = --dataBins.end();
       changeSet[iter]; // make sure that changeSet[iter] is constructed
       return DataWriteIndex<D> {iter, *this, *ptr};
  }
  
  void deleteBin(Iterator iter) {
    changeSet[iter] |= DELETED;
  }
  
  template<class D>
    static DataBin<D>& getBin(Iterator iter) {
        return dynamic_cast<DataBin<D>&>(**iter);
    }
    
  void updateReadStateAndNotify();
  
  void applyChanges();
  
private:	
    std::list<std::shared_ptr<AbstractDataBin>> dataBins;
    std::list<std::function<void(void)>> eventList;
    std::list<std::function<void(void)>> requestList;
    std::map<std::type_index, AbstractDataHandlerFactory> ctorMap;
    std::map<Iterator, int> changeSet;
    
    std::size_t version = 0;

    
    
    void renewReadState();
};

template<class D>
void DataBin<D>::notifyChanged(AsyncObjectContainer& container, std::list<std::shared_ptr<AbstractDataBin>>::iterator iter) {
     getHandler().changed(AsyncObjectContainer::DataReadIndex<D>(iter, container, container.getBin<D>(iter)));
 }