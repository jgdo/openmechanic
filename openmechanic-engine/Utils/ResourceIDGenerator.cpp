#include "ResourceIDGenerator.h"


ResourceIDGenerator::ResourceIDGenerator()
{
}


ResourceID ResourceIDGenerator::generateNewID()
{
	return _nextID++;
}

void ResourceIDGenerator::removeID(ResourceID id)
{
	// empty for now
}

ResourceIDGenerator::~ResourceIDGenerator()
{
}
