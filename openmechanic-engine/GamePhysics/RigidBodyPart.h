#pragma once

#include <memory>

#include <Eigen/Dense>

#include "Utils/DynamicArray.h"
#include "BodyBlockInstance.h"
#include "PhysicalObject.h"
#include "Blocks/BlockIDs.h"
#include "Blocks/BodyBlock.h"

#include <list>
#include <vector>

class Joint;
class Spring;
class FullBody;
class GameWorld;

ATTRIBUTE_ALIGNED16(class) RigidBodyPart : public PhysicalObject {
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	DECLARE_RESOURCE(RigidBodyPart);

	class Listener {
	public:

		inline virtual ~Listener() {
		}
		
		virtual void onBlockWasAdded(RigidBodyPart *bodyPart, BodyBlockInstance * bbInst) = 0;

		virtual void onBlockIsBeeingRemoved(RigidBodyPart *bodyPart, BodyBlockInstance * bbInst) = 0;
	};


	~RigidBodyPart();
	
	inline FullBody* getParent() {
		return _parent;
	}

	/**
			throws exception if block already filled

			If this is the first block, the body will be placed at 0, 0, 0
	 */
	bool addBlock(BlockID blockId, const BlockIndex& position, btQuaternion const& orientation);

	void getWorldTransformForPosition(Eigen::Vector3i const & index, btVector3 * pos, btQuaternion * rot);

	inline void setListener(Listener* listener) {
		_listener = listener;
	}

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

protected:
	RigidBodyPart(RID rid, FullBody* parent, GameWorld* gameWorld);

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

	btTransform _centerTransformInv;

	FullBody *_parent;

	DynamicArray<BodyBlockInstancePtr> _blockData;

	Listener * _listener = nullptr;

	GameWorld* _gameWorld;

	btCompoundShape* _collisionShapeAligned = nullptr; // null if this body part is empty
	btCompoundShape* _collisionShapeReal = nullptr; // null if this body part is empty
	btRigidBody* _btBody = nullptr;
	btScalar _mass = 0;
	int _numBlocks = 0;
	

	std::list<JointEntry> _connectedJoints;
	std::list<SpringEntry> _connectedSprings;

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
};

typedef RigidBodyPart::Ptr RigidBodyPartPtr;
typedef RigidBodyPart::ConstPtr RigidBodyPartConstPtr;
