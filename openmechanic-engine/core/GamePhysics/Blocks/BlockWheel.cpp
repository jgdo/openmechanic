/* 
 * File:   BlockWheel.cpp
 * Author: jgdo
 * 
 * Created on February 2, 2016, 4:02 PM
 */

#define _USE_MATH_DEFINES
#include <cmath>

#include "BlockWheel.h"

std::vector<BlockAttachInfoEntry> BlockWheel::_blockIndices;

BlockWheel::BlockWheel() : BodyBlock(ID) {
	// FIXME: not thread save
	if (_blockIndices.empty()) {
		for (int y = -1; y < 2; y++) {
			for (int z = -1; z < 2; z++) {
				_blockIndices.push_back(BlockAttachInfoEntry(BlockIndex(0, y, z), { btVector3(1,0,0), btVector3(-1,0,0) }));
			}
		}
	}
}

BlockWheel::~BlockWheel() {
}

bool BlockWheel::isCube() const {
	return false;
}

const CubeBodyBlock* BlockWheel::toCube() const {
	return nullptr;
}

CubeBodyBlock* BlockWheel::toCube() {
	return nullptr;
}

bool BlockWheel::hasOrientation() const {
	return true;
}

btScalar BlockWheel::getMass() const{
	return 1;
}

btCollisionShape* BlockWheel::getCreateCollisionShape() {
	if (!_collisionShape) {
		// todo: allocator for allignment
		//_collisionShape = std::make_shared <btBoxShape>(btVector3(BlockSize / 2, BlockSize / 2 , BlockSize / 2));

		// FIXME: check if it works good
		// make shape a little bit smaller
		//_collisionShape = std::shared_ptr<btCollisionShape>(new btCylinderShapeX(btVector3(3*BlockSize / 2, BlockSize * 1.5, BlockSize / 2)* 0.99));
		//_collisionShape = std::shared_ptr<btCollisionShape>(new btCapsuleShapeX(3*BlockSize/2, -BlockSize*1.5));

		auto compound = new btCompoundShape();
		compound->addChildShape(btTransform(btQuaternion(0,0,0)), new btConeShapeX(3*BlockSize / 2, BlockSize));
		compound->addChildShape(btTransform(btQuaternion(M_PI, 0, 0)), new btConeShapeX(3*BlockSize / 2, BlockSize));
		_collisionShape = std::shared_ptr<btCollisionShape>(compound);
	}

	return _collisionShape.get();
}

std::vector<BlockOrientation> BlockWheel::getPossiblePlacementOrientations(const BlockIndex& orientation) const {
	if (orientation == BlockIndex(1, 0, 0))
		return {
		BlockOrientation(btVector3(0, 1, 0), 0)
	};
	else if (orientation == BlockIndex(-1, 0, 0))
		return {
		BlockOrientation(btVector3(0, 1, 0), M_PI)
	};
	else if (orientation == BlockIndex(0, 0, 1))
		return {
		BlockOrientation(btVector3(0, 1, 0), -M_PI_2)
	};
	else if (orientation == BlockIndex(0, 0, -1))
		return {
		BlockOrientation(btVector3(0, 1, 0), M_PI_2)
	};
	else if (orientation == BlockIndex(0, 1, 0))
		return {
		BlockOrientation(btVector3(0, 0, 1), M_PI_2)
	};
	if (orientation == BlockIndex(0, -1, 0))
		return {
		BlockOrientation(btVector3(0, 0, 1), -M_PI_2)
	};
	else
		return {
	}; // FIXME
}

std::vector<BlockAttachInfoEntry> const& BlockWheel::getAttachInfos() const
{
	return _blockIndices;
}
