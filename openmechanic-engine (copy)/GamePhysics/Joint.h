#pragma once

#define _USE_MATH_DEFINES

#include <memory>
#include <cmath>

#include "RigidBodyPart.h"
#include "Utils/GameObject.h"

#include "Control/ControlObject.h"

class GameWorld;
class FullBody;

/*

General policy: axis points from body A into body b
=> i.e. if we attach a the second part to the joint, if this second part will be the B part, the axis should point into the part.
If the new part is the A part, the joint axis should point out of the part.

 */
ATTRIBUTE_ALIGNED16(class) Joint: public PhysicalControlObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	DECLARE_RESOURCE(Joint);

	class Listener {
	public:
	};


	virtual ~Joint();

	/*
		throws exception on error, e.g. if there are already two bodies attached to the joint
	 */
	void attachBodyPart(RigidBodyPart * bodyPart, BlockIndex const& index, BlockIndex const& direction);

	/*
		Throws exception on error, e.g. if given part was not connected to this joint
	 *  does nothing if bodyPart == nullptr
	 */
	void detachBodyPart(RigidBodyPart * bodyPart, bool informBody);

	/**
	 * 
	 * @return true if joint attached to both bodies
	 */
	inline bool isAttached() const {
		return (bool) _btHinge;
	}
	
	/**
	 * 
	 * @return true if joint is not attached to any body and thus should be deleted
	 */
	inline bool isFloating() const {
		return !_bodyA && !_bodyB;
	}

	/**
	 * valid only if attached to exactly one body
	 * otherwise exception
	 * @return 
	 */
	btTransform getCurrentWorldTransform() const;
	
	inline RigidBodyPart* getBodyPartA() {
		return _bodyA;
	}
	
	inline RigidBodyPart* getBodyPartB() {
		return _bodyB;
	}

	inline btHingeConstraint* getHinge() {
		return _btHinge.get();
	}

	inline void setAngleOffset(float off)
	{
		// FIXME adjustment
		while (off > M_PI)
			off -= M_PI;
		while (off < -M_PI)
			off = M_PI;

		_angleOffset = off;
	}

	inline float getAngleOffset() const
	{
		return _angleOffset;
	}

	inline FullBody* getParent()
	{
		return _fullBody;
	}

	inline void setInputMultiplier(float f)
	{
		_inputMultiplier = f;
	}

	inline float getInputMultiplier() const
	{
		return _inputMultiplier;
	}
	

	float getJointAngle() const;
	
	inline btVector3 getRA() const {
		return _rA;
	}
	
	inline btVector3 getRB() const {
		return _rB;
	}
	
	inline btVector3 getAxA() const {
		return _axA;
	}
	
	inline btVector3 getAxB() const {
		return _axB;
	}

	virtual std::vector<ConnectionEndpoint*> getConnectionEndpoints() override;

	virtual btVector3 getObjectPosition() const override;


private:
	class JointConnectionEndpoint: public ConnectionEndpointBase<Joint> {
	public:
		JointConnectionEndpoint(Joint *co) : ConnectionEndpointBase(co) { }

		virtual int getConnectionType() const override;

		virtual void setConnectionState(int state) override;

		virtual int getConnectionState() const override;

		virtual float computeOutput() override;

		virtual void applyInput(float in) override;

	protected:
		int _connectionState = NO_CONNECTION;
	};

	Joint(RID rid, GameWorld* gameWorld, RigidBodyPart * bodyPart, BlockIndex const& index, BlockIndex const& direction);

	// neither axis or r values are valid if corresponding body is not attached
	btVector3 _rA, _rB; // pivot offset in local A and B body part frames (not in the world coordinate frame, since it changes when center of mass changes!)
	btVector3 _axA, _axB; // axis in local body part frames

	GameWorld* _gameWorld;
	FullBody* _fullBody;

	std::shared_ptr<btHingeConstraint> _btHinge = nullptr;

	RigidBodyPart *_bodyA = nullptr, *_bodyB = nullptr; // owner is creator, not this

	float _angleOffset = 0;

	float _inputMultiplier = 1;

	JointConnectionEndpoint _jointCEP;
};

typedef Joint::Ptr JointPtr;
typedef Joint::ConstPtr JointConstPtr;

