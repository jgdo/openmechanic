#pragma once

#include "RigidBodyPart.h"
// #include "Joint.h"
// #include "Spring.h"

#include "ClientInterfaces/Messages/FullBodyMsgs.h"

#include <list>
#include <map>

class GameWorld;

inline bool operator<(const FullBody& lhs, const FullBody& rhs) {
	return &lhs < &rhs;
}

class FullBody: public GameObject
{
public:
    using DataHandler = DataObjectHandler<FullBodyData>*;
	
    FullBody(OID oid, GameContext *context, GameWorld* world);

	virtual ~FullBody();

	/**
		Part will be owned by this FullBody
	 * 
	 * local block position always (0, 0, 0) for new parts
		*/
	RigidBodyPart* addNewBodyPart(BlockID blockId, btVector3 const& worldPos, btQuaternion const& worldOorientation, btQuaternion const& localBlockOrientation, RigidBodyPart* reference);
	
	/*
		Remove Body Part and also remove any joints which became floating
	*/
	void removeAndDeleteBodyPart(RigidBodyPart* bodyPart);

	/**
	 * Joint will be owned by this FullBody
	 * 
     * @param bodyPart
     * @param position
     * @param direction
     * @return 
     */
	Joint* addJoint(RigidBodyPart * bodyPart, BlockIndex const& position, BlockIndex const& direction);
	
	void removeAndDeleteJoint(Joint* joint);

	/**
	 * Spring will be owned by this FullBody
	 *
     * @param bodyPart
     * @param position
     * @param direction
     * @return
     */
	Spring* addSpring(RigidBodyPart * bodyPart, BlockIndex const& position, BlockIndex const& direction);

	void removeAndDeleteSpring(Spring* spring);

	inline btDiscreteDynamicsWorld* getDynmicsWorld();

	void resetPosition(btVector3 const& worldPos, btQuaternion const& worldOorientation);
	
	void activate();

	void setFreezed(bool freezed);
	
#if 0

	inline size_t getNumParts() const {
		return _allBodyParts.size();
	}

	inline size_t getNumJoints() const {
		return _allJoints.size();
	}

	inline const std::list<Joint*>& getJoints() const // TODO
	{
		return _allJoints;
	}

	inline size_t getNumSprings() const {
		return _allSprings.size();
	}

	inline const std::list<Spring*>& getSprings() const // TODO
	{
		return _allSprings;
	}
	
#endif

	template<class T> 
	void foreachPart(const T& func) const
	{
		for(auto const& part: _allBodyParts)
		{
			func(&part);
		}
	}

public:	
	GameWorld* _gameWorld;
	
    std::set<RigidBodyPart*> _allBodyParts;
	
	//std::list<Joint*> _allJoints;
	//std::list<Spring*> _allSprings;

	bool _freezed = false;
	
public:
    DataHandler myIndex; // TODO private
};

typedef FullBody::Ptr FullBodyPtr;
typedef FullBody::ConstPtr FullBodyConstPtr;
