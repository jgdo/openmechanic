#define _USE_MATH_DEFINES

#include <cmath>

#include "BlockDriverSeat.h"


std::vector<BlockAttachInfoEntry> BlockDriverSeat::_indices;

BlockDriverSeat::BlockDriverSeat() : BodyBlock(ID) {
	// FIXME: not thread save
	if (_indices.empty()) {
		// ground plate
		for (int y = -1; y < 2; y++) {
			for (int z = -3; z < 3; z++) {
				_indices.push_back(BlockAttachInfoEntry(BlockIndex(0, y, z), {btVector3(-1, 0, 0)}));
			}
		}

		// middle
		for (int y = -1; y < 2; y++) {
			for (int z = -3; z < 3; z++) {
				_indices.push_back(BlockAttachInfoEntry(BlockIndex(1, y, z), {}));
			}
		}

		// top
		for (int y = -1; y < 2; y++) {
			for (int z = -3; z < 3; z++) {
				_indices.push_back(BlockAttachInfoEntry(BlockIndex(2, y, z), {}));
			}
		}
	}
}

BlockDriverSeat::~BlockDriverSeat() {
}


bool BlockDriverSeat::hasOrientation() const {
	return true;
}

btScalar BlockDriverSeat::getMass() const {
	return 1;
}

btCollisionShape *BlockDriverSeat::getCreateCollisionShape() {
	if (!_collisionShape) {
		// todo: allocator for alignment
		//_collisionShape = std::make_shared <btBoxShape>(btVector3(BlockSize / 2, BlockSize / 2 , BlockSize / 2));
		_collisionShape = std::shared_ptr<btBoxShape>(
				new btBoxShape(btVector3(3* BlockSize / 2, 3* BlockSize / 2, 6*BlockSize / 2)));
	}

	return _collisionShape.get();
}

std::vector<BlockOrientation> BlockDriverSeat::getPossiblePlacementOrientations(BlockIndex const &orientation) const {
	// FIXME: make a pattern
	if (orientation == BlockIndex(1, 0, 0))
		return {
				BlockOrientation(btVector3(1, 0, 0), 0*M_PI_2),
				BlockOrientation(btVector3(1, 0, 0), 1*M_PI_2),
				BlockOrientation(btVector3(1, 0, 0), 2*M_PI_2),
				BlockOrientation(btVector3(1, 0, 0), 3*M_PI_2),
		};
	else if (orientation == BlockIndex(-1, 0, 0))
		return {
				BlockOrientation(btVector3(0, 1, 0), M_PI) * BlockOrientation(btVector3(1, 0, 0), 0*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), M_PI) * BlockOrientation(btVector3(1, 0, 0), 1*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), M_PI) * BlockOrientation(btVector3(1, 0, 0), 2*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), M_PI) * BlockOrientation(btVector3(1, 0, 0), 3*M_PI_2),
		};
	else if (orientation == BlockIndex(0, 0, 1))
		return {
				BlockOrientation(btVector3(0, 1, 0), -M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 0*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), -M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 1*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), -M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 2*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), -M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 3*M_PI_2),
		};
	else if (orientation == BlockIndex(0, 0, -1))
		return {
				BlockOrientation(btVector3(0, 1, 0), M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 0*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 1*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 2*M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 3*M_PI_2),
		};
	else if (orientation == BlockIndex(0, 1, 0))
		return {
				BlockOrientation(btVector3(0, 0, 1), M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 0*M_PI_2),
				BlockOrientation(btVector3(0, 0, 1), M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 1*M_PI_2),
				BlockOrientation(btVector3(0, 0, 1), M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 2*M_PI_2),
				BlockOrientation(btVector3(0, 0, 1), M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 3*M_PI_2),
		};
	if (orientation == BlockIndex(0, -1, 0))
		return {
				BlockOrientation(btVector3(0, 0, 1), -M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 0*M_PI_2),
				BlockOrientation(btVector3(0, 0, 1), -M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 1*M_PI_2),
				BlockOrientation(btVector3(0, 0, 1), -M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 2*M_PI_2),
				BlockOrientation(btVector3(0, 0, 1), -M_PI_2) * BlockOrientation(btVector3(1, 0, 0), 3*M_PI_2),
		};
	else
		return {
		}; // FIXME
}

const CubeBodyBlock *BlockDriverSeat::toCube() const {
	return nullptr;
}

CubeBodyBlock *BlockDriverSeat::toCube() {
	return nullptr;
}

bool BlockDriverSeat::isCube() const {
	return false;
}

std::vector<BlockAttachInfoEntry> const &BlockDriverSeat::getAttachInfos() const {
	return _indices;
}

btVector3 BlockDriverSeat::getCenterOfMassDisplacement() const {
	return btVector3(BlockSize, 0, -BlockSize / 2);
}


