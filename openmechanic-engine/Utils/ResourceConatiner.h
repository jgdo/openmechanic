#pragma once

#include <unordered_map>

#include "ResourceIDGenerator.h"

/*
	Class T must have a T::Ptr smart pointer type defined, which has a method get() to get the raw pointer
	Also T must have a static method T::Ptr T::create(RID, ...) 
*/
template<class T>
class ResourceContainer
{
public:
	typedef typename T::Ptr TPtr;

	template<class R, class ... ArgT>
	R* create(ArgT... args)
	{
		RID id = _ridGen.generateNewID();
		typename R::Ptr ptr = R::create(id, args...);
		_resourceMap.emplace(id, ptr);
		return ptr.get();
	}

	void destroy(RID id)
	{
		auto iter = _resourceMap.find(id);
		if (iter == _resourceMap.end())
			throw std::runtime_error("ResourceContainer::destroy(): given RID not found");

		// delete object by resetting smart pointer
		iter->second.reset();
	}

	template<class R>
	R* get(RID rid)
	{
		auto iter = _resourceMap.find(rid);
		if (iter == _resourceMap.end())
			throw std::runtime_error("ResourceContainer::get(): given RID not found");

		R* obj = dynamic_cast<R*>(iter->second.get());
		if(!obj)
			throw std::runtime_error("ResourceContainer::get(): Type of resource with give RID is not the requested type");

		return obj;
	}

	template<class R>
	R* tryGet(RID rid)
	{
		auto iter = _resourceMap.find(rid);
		if (iter == _resourceMap.end())
			return nullptr;

		return dynamic_cast<R*>(iter->second.get());
	}

private:
	std::unordered_map<RID, TPtr> _resourceMap;
	ResourceIDGenerator _ridGen;
};