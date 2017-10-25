#pragma once

#include <memory>

#include <eigen3/Eigen/Dense>

#include "core/Utils/DynamicArray.h"
#include "BodyBlockInstance.h"
#include "Blocks/BlockIDs.h"
#include "Blocks/BodyBlock.h"
#include "core/Utils/ClientInterfaceHelpers.h"
#include <ClientInterfaces/Messages/RigidBodyPartMsgs.h>

#include <core/GamePhysics/GameContext.h>

#include <list>
#include <vector>

class Joint;
class Spring;
class FullBody;
class GameWorld;

inline bool operator<(const RigidBodyPart& lhs, const RigidBodyPart& rhs) {
	return &lhs < &rhs;
}

ATTRIBUTE_ALIGNED16(class) RigidBodyPart : public GameObject {
public:
    using DataHandler = DataObjectHandler<RigidBodyPartData>*;
	
     /**
			throws exception if block already filled

			If this is the first block, the body will be placed at 0, 0, 0
	 */
      void addBlock(BlockID blockId, BlockIndex basePosition, BlockIndex direction, btQuaternion orientation);
      void removeBlockOrJoint(BlockIndex index, BlockIndex direction);
      
	BT_DECLARE_ALIGNED_ALLOCATOR();

    RigidBodyPart(OID oid, GameContext *context, FullBody* parent, GameWorld* gameWorld);
	
	~RigidBodyPart();
	
	inline FullBody* getParent() {
		return _parent;
	}
	
	void getWorldTransformForPosition(Eigen::Vector3i const & index, btVector3 * pos, btQuaternion * rot);

	btTransform getCurrentWorldTransform() const;

	inline btTransform getLocalBodyTransform() const {
		return _centerTransformInv.inverse();
	}

	void setWorldTransform(btTransform const& trans);

	void addShit();

	inline btRigidBody * getBtBody() {
		return _btBody;
	}
	
	inline const btRigidBody * getBtBody() const {
		return _btBody;
	}
	
	inline btScalar getMass() const {
		return _mass;
	}

#if 0
	void addJoint(Joint * joint, BlockIndex const& pos, BlockIndex const& dir);

	/**
	 * Does not inform the joint 
     * @param joint
     */
	void removeJoint(Joint* joint);
	
	/**
	 * Does not inform joints
     * @return 
     */
	std::vector<Joint*> removeAllJoints();

	void addSpring(Spring * spring, BlockIndex const& pos, BlockIndex const& dir);

	/**
	 * Does not inform the spring
     * @param spring
     */
	void removeSpring(Spring* spring);

	/**
	 * Does not inform springs
     * @return
     */
	std::vector<Spring*> removeAllSprings();

	/**
	 * 
	 * @param pos
	 * @param dir
	 * @return nullptr if there is no such joint
	 */
	Joint* getJointAt(BlockIndex const& pos, BlockIndex const& dir);
	
#endif

	inline BodyBlockInstance const* getBlockInstanceAt(BlockIndex const& pos) const {
		return getBlockAt(pos).get();
	}

	inline BodyBlockInstance* getBlockInstanceAt(BlockIndex const& pos) {
		return getBlockAt(pos).get();
	}

	void removeBlock(BlockIndex const& index);
	
	inline int getNumBlocks() const {
		return _numBlocks;
	}

	/************************************************************************/
	/* makes the object unmovable by setting mass to 0                                                                     */
	/************************************************************************/
	void setFreezed(bool freezed);

	inline bool isFreezed() const {
		return _isFrezed;
	}

	inline GameWorld* getGameWorld() {
		return _gameWorld;
	}
	
	std::pair<BlockIndex, BlockIndex> getBlockDataDimensions() const;

	btTransform _centerOffset;
	
protected:

#if 0
	struct JointEntry {
		Joint * _joint;
		// FIXME: theoretically unnecessary since joint has already position and axis
		BlockIndex _position;
		BlockIndex _direction;

		inline JointEntry(Joint * joint, BlockIndex const& pos, BlockIndex const& dir) :
		_joint(joint), _position(pos), _direction(dir) {

		}
	};

	struct SpringEntry {
		Spring * _spring;
		// FIXME: theoretically unnecessary since spring has already position and axis
		BlockIndex _position;
		BlockIndex _direction;

		inline SpringEntry(Spring * spring, BlockIndex const& pos, BlockIndex const& dir) :
				_spring(spring), _position(pos), _direction(dir) {

		}
	};
	
#endif

	btTransform _centerTransformInv;

	FullBody *_parent;

	DynamicArray<BodyBlockInstancePtr> _blockData;

	GameWorld* _gameWorld;

	btCompoundShape* _collisionShapeAligned = nullptr; // null if this body part is empty
	btCompoundShape* _collisionShapeReal = nullptr; // null if this body part is empty
	btRigidBody* _btBody = nullptr;
	btScalar _mass = 0;
	int _numBlocks = 0;
	
#if 0
	std::list<JointEntry> _connectedJoints;
	std::list<SpringEntry> _connectedSprings;
#endif

	bool _isFrezed = false;

	inline BodyBlockInstancePtr& getBlockAt(int x, int y, int z) {
		return _blockData.at(x , y , z);
	}

	inline BodyBlockInstancePtr& getBlockAt(BlockIndex const& index) {
		return getBlockAt(index.x(), index.y(), index.z());
	}

	inline const BodyBlockInstancePtr& getBlockAt(int x, int y, int z) const {
		return _blockData.at(x, y , z );
	}

	inline const BodyBlockInstancePtr& getBlockAt(BlockIndex const& index) const {
		return getBlockAt(index.x(), index.y(), index.z());
	}

	inline bool indexInRange(int x, int y, int z) const {
		return _blockData.contains(x , y , z);
	}

	/**
	 * Re-compute collision shape center of mass and local body transform offset
	 */
	void updateShapeMassAndPosition();
	
	void ensureIndexExits(BlockIndex const& index);
	
public:
    DataHandler myIndex; // TODO private
};

typedef RigidBodyPart::Ptr RigidBodyPartPtr;
typedef RigidBodyPart::ConstPtr RigidBodyPartConstPtr;
