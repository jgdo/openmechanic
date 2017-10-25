#pragma once

#include <btBulletDynamicsCommon.h>

#include <memory>
#include <vector>

#include "Blocks/BodyBlock.h"
// #include "Control/ControlObject.h"

class RigidBodyPart;

ATTRIBUTE_ALIGNED16(class) BodyBlockInstanceBase
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	// creator keeps ownership of bodyBlock
	BodyBlockInstanceBase(const BodyBlock* bodyBlock, BlockIndex const& centerIndex, const btQuaternion& relRot);
	virtual ~BodyBlockInstanceBase();

	inline const btVector3& getRelativePosition() const {
		return _relPos;
	}

	inline const btQuaternion& getRelativeOrientation() const {
		return _relRot;
	}

	inline const BodyBlock* getBodyBlock() const {
		return _bodyBlock;
	}

	inline std::vector<BlockIndex> const& getIndiciesInBody() const {
		return _indiciesInBody;
	}

	inline BlockIndex getCenterBlockIndex() const {
		return _centerBlockIndex;
	}

protected:
	// relative to bodypart
	btVector3 _relPos;
	btQuaternion _relRot;

	const BodyBlock* _bodyBlock; // ownership is by creator/factory!!
	BlockIndex _centerBlockIndex;

	// all positions in the parent body which contain this block, since a single block can take up multiple slots
	// TODO: deduce from _centerBlockIndex and the intrinsic block properties instead of implicit storage
	std::vector<BlockIndex> _indiciesInBody;
};

class BodyBlockInstance: public BodyBlockInstanceBase
{
public:
	typedef std::shared_ptr<BodyBlockInstance> Ptr;
	typedef std::shared_ptr<const BodyBlockInstance> ConstPtr;

	BodyBlockInstance(RigidBodyPart* parentPart, const BodyBlock* bodyBlock, BlockIndex const& centerIndex, const btQuaternion& relRot);
	virtual ~BodyBlockInstance();

	inline RigidBodyPart* getParentPart() {
		return _parentPart;
	}

#if 0
	inline ControlObject* getAttachedControl()
	{
		return _attachedControl;
	}
#endif 
private:
	RigidBodyPart* _parentPart;

	// ControlObject* _attachedControl;
};

typedef BodyBlockInstance::Ptr BodyBlockInstancePtr;
typedef BodyBlockInstance::ConstPtr BodyBlockInstanceConstPtr;
