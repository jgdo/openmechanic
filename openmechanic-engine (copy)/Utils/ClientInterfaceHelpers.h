#pragma once

#include "ObjectData.h"
#include "AsyncObjectContainer.h"

template<class D>
struct ServerInterfaceImplBase: public ServerInterface<D> {
  inline ServerInterfaceImplBase(AsyncObjectContainer& container, OID oid): myContainer(container) {
    myIndex = myContainer.newBin<D>(oid, *this);
  }
  
  inline ~ServerInterfaceImplBase() {
    myContainer.deleteBin(myIndex);
  }
  
protected:
  AsyncObjectContainer& myContainer;
  DataIndex myIndex;
};
