#pragma once

#include "Utils/GameObject.h"
#include "ObjectData.h"
#include "GamePhysics/Blocks/BlockIDs.h"
#include <btBulletDynamicsCommon.h>

#include "AsyncObjectContainer.h"
#include "GamePhysics/Blocks/BodyBlock.h"

struct RigidBodyPartData  {
  btTransform worldTransform;
};

template<>
struct ClientInterface<RigidBodyPartData>  {
	inline virtual ~ClientInterface() {}
	
	virtual void init(const AsyncObjectContainer::DataReadIndex<RigidBodyPartData>& idx, OID parentFullBodyID) = 0;
	
	virtual void changed(const AsyncObjectContainer::DataReadIndex<RigidBodyPartData>& idx) = 0;
	virtual void blockAdded(const AsyncObjectContainer::DataReadIndex<RigidBodyPartData>& idx, const BodyBlock* block, BlockIndex index, btQuaternion relRot, OID attachedControlOID) = 0;
	virtual void blockRemoved(const AsyncObjectContainer::DataReadIndex<RigidBodyPartData>& idx, BlockIndex index) = 0;
};

template<>
struct ServerInterface<RigidBodyPartData>  {
  virtual void addBlock(AsyncObjectContainer::DataWriteIndex<RigidBodyPartData> partIdx, BlockID blockId, BlockIndex basePosition, BlockIndex direction, btQuaternion orientation) = 0;
  virtual void removeBlockOrJoint(AsyncObjectContainer::DataWriteIndex<RigidBodyPartData> partIdx, BlockIndex index, BlockIndex direction) = 0;
};

