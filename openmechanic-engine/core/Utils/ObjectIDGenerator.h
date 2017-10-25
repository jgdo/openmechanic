#pragma once

#include "GameObject.h"

class ObjectIDGenerator
{
public:
	ObjectIDGenerator();
	~ObjectIDGenerator();

	ObjectID generateNewID();

	void removeID(ObjectID id);

private:
	ObjectID _nextID = 1;
};


