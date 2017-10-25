#include "BodyBlockFactory.h"

#include "Blocks/BlockIDs.h"

#include "Blocks/BlockWood.h"
#include "Blocks/BlockWheel.h"
#include "Blocks/BlockMotor.h"
#include "Blocks/BlockDriverSeat.h"

BodyBlockFactory::BodyBlockFactory() {
}

BodyBlockFactory::~BodyBlockFactory() {
}

BodyBlock * BodyBlockFactory::getCreateBodyBlock(size_t blockID) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	auto iter = _blockCollection.find(blockID);
	if (iter != _blockCollection.end())
		return iter->second.get();

	std::shared_ptr<BodyBlock> ptr;
	switch (blockID) {
		case BLOCK_WOOD:
			ptr = std::make_shared<BlockWood>();
			break;

		case BLOCK_WHEEL:
			ptr = std::make_shared<BlockWheel>();
			break;

		case BLOCK_MOTOR:
			ptr = std::make_shared<BlockMotor>();
			break;

		case BLOCK_DRIVER_SEAT:
			ptr = std::make_shared<BlockDriverSeat>();
			break;

		default:
			throw std::logic_error("Cannot generate block for given blockID"); // TODO: more info
	}

	_blockCollection[blockID] = ptr;
	return ptr.get();
}
