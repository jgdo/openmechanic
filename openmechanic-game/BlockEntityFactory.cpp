#include "BlockEntityFactory.h"

#include "GamePhysics/Blocks/BlockIDs.h"

#include <osg/ShapeDrawable>

BlockEntityFactory::BlockEntityFactory()
{
}


BlockEntityFactory::~BlockEntityFactory()
{
}

std::pair<osg::Drawable*, osg::Vec3> BlockEntityFactory::createNewEntity(size_t blockID)
{
	if (blockID == BLOCK_WOOD) {
		osg::ShapeDrawable* shape1 = new osg::ShapeDrawable;
		shape1->setShape( new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 1.0f) );
        shape1->setColor( osg::Vec4(0.6f, 0.6f, 0.0f, 1.0f));
		
		// Ogre::Entity* thisEntity = sceneMan->createEntity(Ogre::SceneManager::PT_CUBE);
		// thisEntity->setMaterialName("wood/wood");
		// thisEntity->setCastShadows(true);
		return { shape1, osg::Vec3(1,1,1)};
		
		
	}
#if 0
	else if (blockID == BLOCK_WHEEL) {
		Ogre::Entity* thisEntity = sceneMan->createEntity("Wheel.mesh");
		thisEntity->setCastShadows(true);
		return{ thisEntity, Ogre::Vector3(100,100,100) };
	} else if (blockID == BLOCK_MOTOR) {
		Ogre::Entity* thisEntity = sceneMan->createEntity("Motor.mesh");
		thisEntity->setCastShadows(true);
		return{ thisEntity, Ogre::Vector3(100,100,100) };
	} else if (blockID == BLOCK_DRIVER_SEAT) {
		Ogre::Entity* thisEntity = sceneMan->createEntity("DriverSeat.mesh");
		thisEntity->setCastShadows(true);
		return{ thisEntity, Ogre::Vector3(100,100,100) };
	}
#endif 
	return { nullptr, osg::Vec3(1,1,1) };
}
