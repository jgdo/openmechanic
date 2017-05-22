#pragma once

typedef int ResourceID;
typedef ResourceID RID;

#include <memory>

class Resource
{
public:
	typedef std::shared_ptr<Resource> Ptr;
	typedef std::shared_ptr<const Resource> ConstPtr;

	const ResourceID resourceID;

	inline Resource(ResourceID rID):
		resourceID(rID) {}

	inline virtual ~Resource() {}
};

typedef Resource::Ptr ResourcePtr;
typedef Resource::ConstPtr ResourceConstPtr;

#define DECLARE_RESOURCE(name) typedef std::shared_ptr<name> Ptr; \
typedef std::shared_ptr<const name> ConstPtr; \
template<typename ...Arg> std::shared_ptr<name> static create(Arg&&...arg) { \
	return Ptr(new name(arg...)); \
}

 /*struct EnableMakeShared : public name {
		EnableMakeShared(Arg&&...arg) :name(std::forward<Arg>(arg)...) {}
	};
	return std::make_shared<EnableMakeShared>(std::forward<Arg>(arg)...);*/
	// TODO: need an aligned allocator
