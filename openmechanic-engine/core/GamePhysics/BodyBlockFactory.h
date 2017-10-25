#pragma once

#include "Blocks/BodyBlock.h"

#include <memory>
#include <unordered_map>
#include <mutex>

class BodyBlockFactory
{
public:
	BodyBlockFactory();
	~BodyBlockFactory();

	// Thread save
	BodyBlock* getCreateBodyBlock(size_t blockID);

private:
	std::unordered_map<size_t, std::shared_ptr<BodyBlock>> _blockCollection;

	std::recursive_mutex _mutex; // since getCreateBodyBlock() might be called from any thread ; TODO
};

