#pragma once

#include "GamePhysics/Blocks/BodyBlock.h"
#include <osg/ShapeDrawable>

class BlockEntityFactory
{
public:
	BlockEntityFactory();
	~BlockEntityFactory();

	/************************************************************************/
	/* Ownership by caller
	/************************************************************************/
	std::pair<osg::Drawable*, osg::Vec3> createNewEntity(size_t blockID);
};

