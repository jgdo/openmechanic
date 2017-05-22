#pragma once

#include  "Utils/Resource.h"

#include <memory>

class PhysicalObject: public Resource
{
public:
	typedef std::shared_ptr<PhysicalObject> Ptr;

	typedef PhysicalObject BaseResourceType;

	using Resource::Resource;

	virtual ~PhysicalObject();
};