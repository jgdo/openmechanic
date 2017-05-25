#include "EventQueue.h"


EventQueue::EventQueue()
{
}


void EventQueue::addChangeEvent(DispatchableBaseConstPtr const& changeEvent)
{
	_changeEvents.emplace_back(changeEvent);
}

void EventQueue::addState(const ObjectStatePtr& state)
{
	// FIXME make faster,  no copying
	_data.emplace(state->resourceID, state);
}

void EventQueue::mergeFromOther(EventQueue&& other)
{
	// FIXME make faster,  no copying

	for(auto const& e: other._data)
	{
		if(_data.find(e.first) == _data.end()) // rid is not in own data
		{
			_data.emplace(e.first, e.second);
		} 
		// otherwise ignore
	}

	_changeEvents.splice(_changeEvents.begin(), std::move(other._changeEvents)); // insert other elements at beginning
}

EventQueue::~EventQueue()
{
}
