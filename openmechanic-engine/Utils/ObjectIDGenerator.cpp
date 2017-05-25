#include "ObjectIDGenerator.h"


ObjectIDGenerator::ObjectIDGenerator()
{
}


ObjectID ObjectIDGenerator::generateNewID()
{
	return _nextID++;
}

void ObjectIDGenerator::removeID(ObjectID id)
{
	// empty for now
}

ObjectIDGenerator::~ObjectIDGenerator()
{
}
