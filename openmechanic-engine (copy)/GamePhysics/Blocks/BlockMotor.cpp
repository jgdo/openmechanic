#include "BlockMotor.h"


BlockMotor::BlockMotor() : CubeBodyBlock(ID)
{
}


BlockMotor::~BlockMotor()
{
}

bool BlockMotor::hasOrientation() const
{
	return true;
}

btScalar BlockMotor::getMass() const
{
	return 1;
}

btCollisionShape* BlockMotor::getCreateCollisionShape()
{
	if (!_collisionShape) {
		// todo: allocator for alignment
		//_collisionShape = std::make_shared <btBoxShape>(btVector3(BlockSize / 2, BlockSize / 2 , BlockSize / 2));
		_collisionShape = std::shared_ptr<btBoxShape>(new btBoxShape(btVector3(BlockSize / 2, BlockSize / 2, BlockSize / 2)));
	}

	return _collisionShape.get();
}
