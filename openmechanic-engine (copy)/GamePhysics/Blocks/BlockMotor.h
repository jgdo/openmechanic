#pragma once

#include "BodyBlock.h"

#include "BlockIDs.h"

#include <memory>

class BlockMotor : public CubeBodyBlock
{
public:
	const static size_t ID = BLOCK_MOTOR;

	BlockMotor();
	virtual ~BlockMotor();

	virtual bool hasOrientation() const override;

	virtual btScalar getMass() const override;

	virtual btCollisionShape* getCreateCollisionShape() override;

private:
	std::shared_ptr<btCollisionShape> _collisionShape;
};

