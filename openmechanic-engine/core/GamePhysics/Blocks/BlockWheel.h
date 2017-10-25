/* 
 * File:   BlockWheel.h
 * Author: jgdo
 *
 * Created on February 2, 2016, 4:02 PM
 */

#ifndef BLOCKWHEEL_H
#define	BLOCKWHEEL_H

#include "BodyBlock.h"
#include "BlockIDs.h"

#include <memory>

class BlockWheel : public BodyBlock {
public:
	const static size_t ID = BLOCK_WHEEL;

	BlockWheel();
	virtual ~BlockWheel();

	virtual bool isCube() const override;

	virtual bool hasOrientation() const override;

	virtual const CubeBodyBlock* toCube() const override;
	virtual CubeBodyBlock* toCube() override;

	virtual btScalar getMass() const override;

	/************************************************************************/
	/* Ownership remains by this block   
	/************************************************************************/
	virtual btCollisionShape* getCreateCollisionShape() override;

	virtual std::vector<BlockOrientation> getPossiblePlacementOrientations(BlockIndex const &orientation) const override;

	virtual std::vector<BlockAttachInfoEntry> const& getAttachInfos() const override;

private:
	std::shared_ptr<btCollisionShape> _collisionShape;

	static std::vector<BlockAttachInfoEntry> _blockIndices;
};

#endif	/* BLOCKWHEEL_H */

