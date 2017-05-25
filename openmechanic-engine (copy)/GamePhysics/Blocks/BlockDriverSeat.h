#pragma once

#include "BodyBlock.h"
#include "BlockIDs.h"

#include <memory>

class BlockDriverSeat : public BodyBlock
{
public:
	const static size_t ID = BLOCK_DRIVER_SEAT;

	BlockDriverSeat();
	virtual ~BlockDriverSeat();

	virtual bool hasOrientation() const override;

	virtual btScalar getMass() const override;

	virtual btCollisionShape* getCreateCollisionShape() override;

	virtual std::vector<BlockOrientation> getPossiblePlacementOrientations(BlockIndex const &orientation) const;

	virtual btVector3 getCenterOfMassDisplacement() const override;

	virtual const CubeBodyBlock* toCube() const override;
	virtual CubeBodyBlock* toCube() override;

	virtual bool isCube() const override;
	
	virtual std::vector<BlockAttachInfoEntry> const& getAttachInfos() const override;

private:
	std::shared_ptr<btCollisionShape> _collisionShape;

	static std::vector<BlockAttachInfoEntry> _indices;
};

