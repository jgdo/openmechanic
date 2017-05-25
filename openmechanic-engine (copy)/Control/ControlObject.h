#pragma once

#include "ConnectionEndpoint.h"

#include  "Utils/GameObject.h"

#include <memory>
#include <vector>


#include <btBulletDynamicsCommon.h>

class ControlEngine;

class ControlObject: public GameObject
{
public:
	typedef std::shared_ptr<ControlObject> Ptr;
	typedef std::shared_ptr<ControlObject const> ConstPtr;

	typedef ControlObject BaseResourceType;

	ControlObject(RID rid, ControlEngine* controlEng);
	virtual ~ControlObject();

	inline ControlEngine* getControlEngine() {
		return _controlEngine;
	}

	virtual std::vector<ConnectionEndpoint*> getConnectionEndpoints() = 0;

protected:
	ControlEngine* _controlEngine;

	/*
		Disconnects all connections. Call in every ControlObject implementation destructor
	*/
	void disconnectControl();
};

typedef ControlObject::Ptr ControlObjectPtr;
typedef ControlObject::ConstPtr ControlObjectConstPtr;

class PhysicalControlObject: public ControlObject {
public:
	using ControlObject::ControlObject;

	virtual ~PhysicalControlObject();

	virtual btVector3 getObjectPosition() const = 0;
};

class BodyBlockInstance;

class BlockConctrolObject : public PhysicalControlObject {
public:
	using PhysicalControlObject::PhysicalControlObject;

	virtual ~BlockConctrolObject();

	virtual btVector3 getObjectPosition() const override;

	inline void setBlockInstance(BodyBlockInstance *bbInst) {
		_bbInst = bbInst;
	}

protected:
	BodyBlockInstance *_bbInst = nullptr;
};

