//
// Created by jgdo on 14.05.16.
//

#ifndef GAME_PLAYERBODY_H
#define GAME_PLAYERBODY_H

#include <btBulletDynamicsCommon.h>

#include "GamePhysics/PhysicalObject.h"

class DriverSeatControl;
class GameWorld;

ATTRIBUTE_ALIGNED16(class) PlayerBody: public PhysicalObject {
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	DECLARE_RESOURCE(PlayerBody);

	static const float BODY_HEIGHT, BODY_RADIUS;

	struct ClientMotion {
		float forward, sidewards, yaw, pitch;
		bool jump;
	};

	virtual ~PlayerBody();

	btVector3 getHeadPosition();
	btQuaternion getHeadOrientation();

	void attachDriverSeat(DriverSeatControl* dsc);
	/**
	 * informDSC if attached DSC should be informed about detachment
	 */
	void detachDriverSeat(bool informDSC = true);

	inline const DriverSeatControl* getAttachedDriverSeat() const {
		return _attachedDSC;
	}

	void insertClientMotion(const ClientMotion& motion);
	
	inline btVector3 getBodyCenter() const {
		return _body->getCenterOfMassPosition();
	}
	
	inline btScalar getCamYaw() const {
		return _camYaw;
	}
	
	inline btScalar getCamPitch() const {
		return _camPitch;
	}

protected:
	PlayerBody(RID rid, GameWorld* gameWorld, const btVector3& position = btVector3(0,0,0));

	GameWorld* _gameWorld;

	btCapsuleShape* _collisionShape = nullptr;
	btRigidBody* _body = nullptr;

	DriverSeatControl* _attachedDSC = nullptr;

	btScalar _camYaw = 0, _camPitch = 0;
};


#endif //GAME_PLAYERBODY_H
