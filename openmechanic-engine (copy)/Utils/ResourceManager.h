#pragma once

#include "TypeMap.h"
#include "ResourceConatiner.h"

class ResourceManager
{
public:
	template<class R, class ... ArgT>
	R* create(ArgT... args)
	{
		return _container.create<R>(args...);
	}
	
	inline void destroy(RID id)
	{
		_container.destroy(id);
	}

	inline void destroy(GameObject* resource)
	{
		destroy(resource->resourceID);
	}

	template<class R> 
	R* get(RID rid)
	{
		return _container.get<R>(rid);
	}

	template<class R>
	R* tryGet(RID rid)
	{
		return _container.tryGet<R>(rid);
	}

private:
	ResourceContainer<GameObject> _container;
};


