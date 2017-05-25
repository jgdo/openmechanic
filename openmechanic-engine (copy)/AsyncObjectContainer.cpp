#include "AsyncObjectContainer.h"

void AsyncObjectContainer::updateReadStateAndNotify() {
    renewReadState();
        
  for(auto& event: eventList)
    event();
  
  for(auto entry: changeSet)
    if(entry.second & CHANGED)
        (*entry.first)->notifyChanged();
        
  for(auto entry: changeSet)
    if(entry.second & DELETED)
        dataBins.erase(entry.first);
    
   // TODO: mutex
   changeSet.clear();
   eventList.clear();
}

void AsyncObjectContainer::applyChanges() {
    version++;
      
  // TODO mutex
  for(auto entry: changeSet)
    (*entry.first)->applyWrite(version);
    
   for(auto& request: requestList)
    request();
}

void AsyncObjectContainer::renewReadState() {
    // TODO mutex
    for(auto iter: changeSet)
        (*iter.first)->readNext();
}