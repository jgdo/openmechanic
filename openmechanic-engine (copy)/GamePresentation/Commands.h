#pragma once

#include "Dispatch.h"
#include "Utils/GameObject.h"

#include "GamePhysics/Blocks/BodyBlock.h"
#include "GamePhysics/Blocks/BlockIDs.h"
#include "GamePhysics/PlayerBody.h"


DECLARE_DISPATCHABLE_STRUCT(RemoveFullBody) { RID rid; };

DECLARE_DISPATCHABLE_STRUCT(RemoveBodyPart) { RID partRID;};

DECLARE_DISPATCHABLE_STRUCT(AddJoint) { RID bodyPartRID; BlockIndex const position; BlockIndex const direction;};

DECLARE_DISPATCHABLE_STRUCT(RemoveJoint) { RID jointRID;};

DECLARE_DISPATCHABLE_STRUCT(AddSpring) { RID bodyPartRID; BlockIndex const position; BlockIndex const direction;};

DECLARE_DISPATCHABLE_STRUCT(RemoveSpring) { RID springRID;};

DECLARE_DISPATCHABLE_STRUCT(AddBlockToPart) { RID partRID; BlockID blockId; const BlockIndex basePosition; const BlockIndex direction; btQuaternion const orientation;};

DECLARE_DISPATCHABLE_STRUCT(AddBlockToFullBody) { RID bodyPartRID; BlockID blockId; btVector3 const relativePosition; btQuaternion const relativeOrientation;
						   btQuaternion const localBlockOrientation;};

DECLARE_DISPATCHABLE_STRUCT(AddBlockToWorld) { BlockID blockId; btVector3 const worldPosition; btQuaternion const worldOrientation;
						   btQuaternion const localBlockOrientation;};

DECLARE_DISPATCHABLE_STRUCT(RemoveBlockFromPart) { RID partRID; const BlockIndex index;};

DECLARE_DISPATCHABLE_STRUCT(RemoveBlockOrJoint) { RID partRID; const BlockIndex index; const BlockIndex direction;};

DECLARE_DISPATCHABLE_STRUCT(DeployLift) { RID liftID; btVector3 const pos; float initialHeight;};

DECLARE_DISPATCHABLE_STRUCT(FoldLift) { RID liftID;};

DECLARE_DISPATCHABLE_STRUCT(RaiseLift) { RID liftID; float direction;};

DECLARE_DISPATCHABLE_STRUCT(AddBlockToLift) { BlockID blockId; RID liftID; btVector3 const worldPosition; btQuaternion const localBlockOrientation;};

DECLARE_DISPATCHABLE_STRUCT(ConnectControls) { RID a; RID b;};

DECLARE_DISPATCHABLE_STRUCT(UseControl) { RID controlID; RID playerID;};

DECLARE_DISPATCHABLE_STRUCT(ApplyUserControl) { RID playerID; const PlayerBody::ClientMotion motion;};
