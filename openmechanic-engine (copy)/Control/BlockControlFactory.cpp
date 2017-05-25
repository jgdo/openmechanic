#include "BlockControlFactory.h"

#include "GamePhysics/Blocks/BlockIDs.h"
#include "MotorControl.h"
#include "DriverSeatControl.h"


BlockControlFactory::BlockControlFactory()
{
}


BlockControlFactory::~BlockControlFactory()
{
}

ControlObject* BlockControlFactory::createControlForBlock(size_t blockID, ControlEngine* eng) {
	switch (blockID) {
	case BLOCK_MOTOR:
		return eng->getResourceManager()->create<MotorControl>(eng);

	case BLOCK_DRIVER_SEAT:
		return eng->getResourceManager()->create<DriverSeatControl>(eng);
	}

	return nullptr;
}