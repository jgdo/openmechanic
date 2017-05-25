#include "BodyBlockInstance.h"

#include "GameWorld.h"

#include "RigidBodyPart.h"
#include "GameWorld.h"

BodyBlockInstanceBase::BodyBlockInstanceBase(const BodyBlock* bodyBlock, BlockIndex const& centerIndex, const btQuaternion& relRot):
	_relPos(centerIndex.x() * BodyBlock::BlockSize, centerIndex.y() * BodyBlock::BlockSize, centerIndex.z() * BodyBlock::BlockSize), _relRot(relRot), _bodyBlock(bodyBlock), _centerBlockIndex(centerIndex)
{
	std::vector<BlockAttachInfoEntry> const& absInd = _bodyBlock->getAttachInfos();
	_indiciesInBody.reserve(absInd.size());

	for (auto const& entry : absInd) {
		auto const& ind = entry.index;
		// FIXme: find out how to use quaternion
		btVector3 rotated = btTransform(relRot, btVector3(0,0,0))*btVector3(ind.x(), ind.y(), ind.z());
		_indiciesInBody.push_back(centerIndex + BlockIndex(lrint(rotated.x()), lrint(rotated.y()), lrint(rotated.z())));
	}
}


BodyBlockInstanceBase::~BodyBlockInstanceBase()
{
}

BodyBlockInstance::BodyBlockInstance(RigidBodyPart* parentPart, const BodyBlock* bodyBlock, BlockIndex const& centerIndex, const btQuaternion& relRot) :
	BodyBlockInstanceBase(bodyBlock, centerIndex, relRot), _parentPart(parentPart), 
	_attachedControl(parentPart->getGameWorld()->getBlockControlFactory()->createControlForBlock(bodyBlock->getID(), parentPart->getGameWorld()->getControlEngine()))
{
	if (_attachedControl) {
		BlockConctrolObject* bco = dynamic_cast<BlockConctrolObject*>(_attachedControl);
		if (bco)
			bco->setBlockInstance(this);
	}
}

BodyBlockInstance::~BodyBlockInstance()
{
	if(_attachedControl)
	{
		_parentPart->getGameWorld()->getResoureManager()->destroy(_attachedControl);
	}
}