#pragma once

#include "Utils/GameObject.h"

#include "GamePhysics/Blocks/BodyBlock.h"

#include "Dispatch.h"

DECLARE_DISPATCHABLE_CLASS(FullBodyChangeEvent)
{
public:
	enum Type
	{
		ADDED,
		REMOVED
	};

	const ObjectID fullBodyRID;
	const Type type;

	inline FullBodyChangeEvent(RID rid, Type type):
		fullBodyRID(rid), type(type) {}
};

DECLARE_DISPATCHABLE_CLASS(BodyPartChangeEvent)
{
public:
	enum Type
	{
		ADDED,
		REMOVED
	};

	const ObjectID fullBodyRID;
	const ObjectID bodyPartRID;
	const Type type;

	inline BodyPartChangeEvent(RID fullRID, RID partRID, Type type) :
		fullBodyRID(fullRID), bodyPartRID(partRID), type(type) {}
};

DECLARE_DISPATCHABLE_CLASS(JointChangeEvent)
{
public:
	enum Type
	{
		ADDED,
		REMOVED
	};

	const ObjectID fullBodyRID;
	const ObjectID jointRID;
	const Type type;

	inline JointChangeEvent(RID fullRID, RID jointRID, Type type) :
		fullBodyRID(fullRID), jointRID(jointRID), type(type) {}
};

DECLARE_DISPATCHABLE_CLASS(BlockAddChangeEvent)
{
public:
	const ObjectID partRID;
	const BodyBlock* const block;
	const BlockIndex centerIndex;
	const btQuaternion relRot;
	const ObjectID attachedControlRID; // 0 if no control

	inline BlockAddChangeEvent(RID partRID, const BodyBlock* block, const BlockIndex& index, const btQuaternion& relRot, ObjectID controlRID) :
		partRID(partRID), block(block), centerIndex(index), relRot(relRot), attachedControlRID(controlRID) {}
};

DECLARE_DISPATCHABLE_CLASS(BlockRemoveChangeEvent)
{
public:
	const ObjectID partRID;
	const BlockIndex centerIndex;

	inline BlockRemoveChangeEvent(RID partRID, const BlockIndex& index) :
		partRID(partRID), centerIndex(index){}
};

DECLARE_DISPATCHABLE_CLASS(LiftChangeEvent)
{
public:
	enum Type
	{
		ADDED, REMOVED
	};

	const Type type;

	const ObjectID liftID;

	inline LiftChangeEvent(Type type, RID liftID):
		type(type), liftID(liftID) {}
};

DECLARE_DISPATCHABLE_CLASS(ConnectionChangeEvent)
{
public:
	enum Type
	{
		ADDED, REMOVED
	};

	const Type type;

	const ObjectID connectionID;
	const ObjectID sourceControlRID, sinkControlRID;

	inline ConnectionChangeEvent(Type type, RID connectionID, RID sourceRID, RID sinkRID) :
		type(type), connectionID(connectionID), sourceControlRID(sourceRID), sinkControlRID(sinkRID) {}
};

DECLARE_DISPATCHABLE_CLASS(DriverSeatChangeEvent)
{
public:
	enum Type
	{
		ENTER, EXIT
	};

	const Type type;

	const ObjectID driverSeatID;

	inline DriverSeatChangeEvent(Type type, RID driverSeatID) :
		type(type), driverSeatID(driverSeatID) {}
};

DECLARE_DISPATCHABLE_CLASS(PlayerBodyChangeEvent)
{
public:
	enum Type
	{
		ADDED, REMOVED
	};

	const Type type;

	const ObjectID playerBodyID;

	inline PlayerBodyChangeEvent(Type type, RID playerBodyID) :
			type(type), playerBodyID(playerBodyID) {}
};

DECLARE_DISPATCHABLE_CLASS(SpringChangeEvent)
{
public:
	enum Type
	{
		ADDED,
		REMOVED
	};

	const ObjectID fullBodyRID;
	const ObjectID springRID;
	const Type type;

	inline SpringChangeEvent(Type type, RID fullRID, RID springRID) :
			fullBodyRID(fullRID), springRID(springRID), type(type) {}
};

