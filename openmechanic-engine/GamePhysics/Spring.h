//
// Created by jgdo on 16.05.16.
//

#ifndef GAME_SPRING_H
#define GAME_SPRING_H


#define _USE_MATH_DEFINES

#include <memory>
#include <cmath>

#include "RigidBodyPart.h"

#include "Utils/Resource.h"

class GameWorld;
class FullBody;


ATTRIBUTE_ALIGNED16(class) Spring: public PhysicalObject {
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	DECLARE_RESOURCE(Spring);

    virtual ~Spring();

	/*
	 *
	 * @param direction: exactly one element is 1, others have to be 0 !!!
		throws exception on error, e.g. if there are already two bodies attached to the spring
	 */
	void attachBodyPart(RigidBodyPart * bodyPart, BlockIndex const& index, BlockIndex const& direction);

	/*
		Throws exception on error, e.g. if given part was not connected to this spring
	 *  does nothing if bodyPart == nullptr
	 */
	void detachBodyPart(RigidBodyPart * bodyPart, bool informBody);

	/**
	 *
	 * @return true if spring attached to both bodies
	 */
	inline bool isAttached() const {
		return (bool) _springConstraint;
	}

	/**
	 *
	 * @return true if spring is not attached to any body and thus should be deleted
	 */
	inline bool isFloating() const {
		return !_bodyA && !_bodyB;
	}

	inline RigidBodyPart* getBodyPartA() {
		return _bodyA;
	}

	inline RigidBodyPart* getBodyPartB() {
		return _bodyB;
	}

	inline btGeneric6DofSpring2Constraint* getSpringConstraint() {
		return _springConstraint.get();
	}

	inline FullBody* getParent()
	{
		return _fullBody;
	}

	btTransform getCenterTransform();
	
	inline btTransform getFrameA() const {
		return btTransform(_axA, _rA);
	}
	
	inline btTransform getFrameB() const {
		return btTransform(_axB, _rB);
	}
	
	/**
	 * returns distance between both bodies, i.e. if bodies touch each other (spring is max. compressed) the return value is 0
	 * returns default spring length when unattached
	 */
	float getSpringElongation();

protected:

	Spring(RID rid, GameWorld* gameWorld, RigidBodyPart * bodyPart, BlockIndex const& index, BlockIndex const& direction);

	// neither axis or r values are valid if corresponding body is not attached
	btVector3 _rA, _rB; // pivot offset in local A and B body part frames (not in the world coordinate frame, since it changes when center of mass changes!)
	btQuaternion _axA, _axB; // axis in local body part frames

	GameWorld* _gameWorld;
	FullBody* _fullBody;

	std::shared_ptr<btGeneric6DofSpring2Constraint> _springConstraint = nullptr;

	RigidBodyPart *_bodyA = nullptr, *_bodyB = nullptr; // owner is creator, not this

	static btTransform getFrameTransformForPart(const RigidBodyPart* part, btVector3 const& pivot, btQuaternion const& rot);
};

typedef Spring::Ptr SpringPtr;
typedef Spring::ConstPtr SpringConstPtr;


#endif //GAME_SPRING_H
