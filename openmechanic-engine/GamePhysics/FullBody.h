#pragma once

#include "RigidBodyPart.h"
#include "Joint.h"
#include "Spring.h"
#include "PhysicalObject.h"

#include "Blocks/BlockIDs.h"

#include <list>
#include <map>

class GameWorld;

class FullBody: public PhysicalObject
{
public:
	DECLARE_RESOURCE(FullBody);
	
	class Listener {
	public:
		inline virtual ~Listener() {}

		virtual void onPartWasAdded(FullBody *fullBody, RigidBodyPart * bodyPart) = 0;

		virtual void onPartIsBeingRemoved(FullBody *fullBody, RigidBodyPart * bodyPart) = 0;
		
		virtual void onJointWasAdded(FullBody * fullBody, Joint *joint) = 0;
		
		virtual void onJointIsBeingRemoved(FullBody * fullBody, Joint *joint) = 0;

		virtual void onSpringWasAdded(FullBody * fullBody, Spring *spring) = 0;

		virtual void onSpringIsBeingRemoved(FullBody * fullBody, Spring *spring) = 0;

		// FIXME: deletion notification
	};

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

	inline void setListener(Listener * listener) {
		_listener = listener;
	}
	
	void resetPosition(btVector3 const& worldPos, btQuaternion const& worldOorientation);
	
	void activate();

	void setFreezed(bool freezed);

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

	template<class T> 
	void foreachPart(const T& func) const
	{
		for(auto const& part: _allBodyParts)
		{
			func(part.second->_bodyPart);
		}
	}

public:
	FullBody(RID rid, GameWorld* world);

	ATTRIBUTE_ALIGNED16(struct) PartEntry {
		typedef std::shared_ptr<PartEntry> Ptr;

		BT_DECLARE_ALIGNED_ALLOCATOR();

		btTransform _centerOffset;

		RigidBodyPart* _bodyPart = nullptr;
		
		PartEntry(RigidBodyPart* bodyPart, btTransform const& cO):
			_centerOffset(cO), _bodyPart(bodyPart) {
		}

		PartEntry() {}

		PartEntry& operator=(PartEntry const& other) {
			_bodyPart = other._bodyPart;
			_centerOffset = other._centerOffset;
			return *this;
		}
	};
	typedef PartEntry::Ptr PartEntryPtr;
	
	GameWorld* _gameWorld;
	
	std::map<RigidBodyPart*, PartEntryPtr> _allBodyParts;
	std::list<Joint*> _allJoints;
	std::list<Spring*> _allSprings;

	Listener * _listener = nullptr;

	bool _freezed = false;
};

typedef FullBody::Ptr FullBodyPtr;
typedef FullBody::ConstPtr FullBodyConstPtr;
