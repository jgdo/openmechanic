#pragma once

#include "ObjectState.h"

#include "ChangeEvents.h"

#include <unordered_map>
#include <list>

class EventQueue
{
public:
	typedef std::shared_ptr<EventQueue> Ptr;
	typedef std::shared_ptr<const EventQueue> ConstPtr;

	EventQueue();
	~EventQueue();

	void addState(const ObjectStatePtr& state);

	void addChangeEvent(DispatchableBaseConstPtr const& changeEvent);
	
	/*
		updates only entries whose RIDs are not contained in this list, i.e. assumes that this queue is the more recent one
	*/
	void mergeFromOther(EventQueue&& other);

	inline std::list<DispatchableBaseConstPtr> const& getChangeEvents() const
	{
		return _changeEvents;
	}

	inline std::unordered_map<ResourceID, ObjectStatePtr> const& getStateData() const
	{
		return _data;
	}

private:
	std::unordered_map<ResourceID, ObjectStatePtr> _data;

	std::list<DispatchableBaseConstPtr> _changeEvents;
};

typedef EventQueue::Ptr EventQueuePtr;
typedef EventQueue::ConstPtr EventQueueConstPtr;