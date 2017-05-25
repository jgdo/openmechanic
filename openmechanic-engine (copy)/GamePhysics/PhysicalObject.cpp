#include  "PhysicalObject.h"

#include "Utils/ResourceManager.h"

PhysicalObject::~PhysicalObject()
{
	// LARGE FIXME TODO: delete from resources
	//ResourceManager::destroy<PhysicalObject>(resourceID);
}