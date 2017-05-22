#include "BlockWood.h"


BlockWood::BlockWood(): CubeBodyBlock(ID)
{
}


BlockWood::~BlockWood()
{
}

bool BlockWood::hasOrientation() const
{
	return false;
}


btScalar BlockWood::getMass() const
{
	return 1;
}

btCollisionShape* BlockWood::getCreateCollisionShape()
{
	if (!_collisionShape) {
		// todo: allocator for alignment
		//_collisionShape = std::make_shared <btBoxShape>(btVector3(BlockSize / 2, BlockSize / 2 , BlockSize / 2));
		_collisionShape = std::shared_ptr<btBoxShape>(new btBoxShape(btVector3(BlockSize / 2, BlockSize / 2, BlockSize / 2)));
	}

	return _collisionShape.get();
}
