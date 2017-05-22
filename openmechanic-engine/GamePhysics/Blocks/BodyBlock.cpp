#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "BodyBlock.h"

const float BodyBlock::BlockSize = 0.25; // in m

std::vector<BlockAttachInfoEntry> CubeBodyBlock::_indices = { BlockAttachInfoEntry(BlockIndex(0,0,0),{
	btVector3(1, 0, 0), btVector3(-1, 0, 0),btVector3(0, 1, 0), btVector3(0, -1, 0), btVector3(0, 0, 1), btVector3(0, 0, -1) }) };


BodyBlock::BodyBlock(size_t id) : _ownID(id) {
}

BodyBlock::~BodyBlock() {
}

CubeBodyBlock::CubeBodyBlock(size_t id) : BodyBlock(id) {

}

const CubeBodyBlock* CubeBodyBlock::toCube() const
{
	return this;
}

CubeBodyBlock* CubeBodyBlock::toCube()
{
	return this;
}

bool CubeBodyBlock::isCube() const
{
	return true;
}

std::vector<BlockAttachInfoEntry> const& CubeBodyBlock::getAttachInfos() const
{
	return _indices;
}

std::vector<BlockOrientation> BodyBlock::getPlacementDirectionsAny() {
	return
	{
		BlockOrientation(btVector3(0, 1, 0), 0),
				BlockOrientation(btVector3(0, 1, 0), M_PI_2),
				BlockOrientation(btVector3(0, 1, 0), M_PI),
				BlockOrientation(btVector3(0, 1, 0), M_PI + M_PI_2),
				BlockOrientation(btVector3(0, 0, 1), M_PI_2),
				BlockOrientation(btVector3(0, 0, 1), -M_PI_2),
	};
}

std::vector<BlockOrientation> BodyBlock::getPossiblePlacementOrientations(BlockIndex const&) const {
	return getPlacementDirectionsAny();
}

btVector3 BodyBlock::getCenterOfMassDisplacement() const {
	return btVector3(0,0,0);
}
