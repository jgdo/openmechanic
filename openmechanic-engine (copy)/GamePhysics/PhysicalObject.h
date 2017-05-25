#pragma once

#include  "Utils/GameObject.h"

#include <memory>

class PhysicalObject: public GameObject
{
public:
	typedef std::shared_ptr<PhysicalObject> Ptr;

	typedef PhysicalObject BaseResourceType;

	using GameObject::GameObject;

	virtual ~PhysicalObject();
};