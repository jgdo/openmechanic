#include "ControlObject.h"

#include "GamePhysics/BodyBlockInstance.h"
#include "GamePhysics/RigidBodyPart.h"

#include "ControlEngine.h"

ControlObject::ControlObject(RID rid, ControlEngine* controlEng):
	Resource(rid), _controlEngine(controlEng)
{
}


ControlObject::~ControlObject()
{
}

void ControlObject::disconnectControl()
{
	std::vector<ConnectionEndpoint*> eps = getConnectionEndpoints();
	for(ConnectionEndpoint* ep: eps)
	{
		Connection* con = ep->getConnection();
		if (con)
			_controlEngine->removeAndDeleteConnection(con);
	}
}

PhysicalControlObject::~PhysicalControlObject() {

}

BlockConctrolObject::~BlockConctrolObject() {

}

btVector3 BlockConctrolObject::getObjectPosition() const {
	if (!_bbInst) {
		throw std::runtime_error("BlockConctrolObject not attached to any BodyBlockInstance");
	}
	
	btVector3 pos;
	_bbInst->getParentPart()->getWorldTransformForPosition(_bbInst->getCenterBlockIndex(), &pos, nullptr);
	return pos;
}