#pragma once

#include "Resource.h"

class ResourceIDGenerator
{
public:
	ResourceIDGenerator();
	~ResourceIDGenerator();

	ResourceID generateNewID();

	void removeID(ResourceID id);

private:
	ResourceID _nextID = 1;
};


