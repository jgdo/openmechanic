#pragma once

#include "ControlObject.h"

#include "ControlEngine.h"

class BlockControlFactory
{
public:
	BlockControlFactory();
	~BlockControlFactory();

	ControlObject* createControlForBlock(size_t blockID, ControlEngine* eng);
};

