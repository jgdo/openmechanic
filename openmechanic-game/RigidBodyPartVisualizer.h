#pragma once

#include "GamePhysics/Blocks/BodyBlock.h"

#include <memory>

#include "Utils/DynamicArray.h"

#include <GamePhysics/BodyBlockInstance.h>

#include "ClientInterfaces/RigidBodyPartInterface.h"
#include "GameVisualization.h"

#include <osg/Geode>
#include <osg/MatrixTransform>

class RigidBodyPartVisualizer: public ClientInterface<RigidBodyPartData>
{
public:
	/*
	struct PlacementResult
	{
		RigidBodyPartVisualizer* viz;
		ResourceID bodyPartID;
		BlockIndex baseIndex;
		BlockIndex placementDirection;
		bool placeable;
	};

	typedef std::shared_ptr<RigidBodyPartVisualizer> Ptr; */
	
	virtual void init(const AsyncObjectContainer::DataReadIndex<RigidBodyPartData>& idx, OID parentFullBodyID) override;
	
	virtual void changed(const AsyncObjectContainer::DataReadIndex< RigidBodyPartData >& idx) override;
	virtual void blockAdded(const AsyncObjectContainer::DataReadIndex< RigidBodyPartData >& idx, const BodyBlock* block, BlockIndex index, btQuaternion relRot, OID attachedControlOID) override;
	virtual void blockRemoved(const AsyncObjectContainer::DataReadIndex< RigidBodyPartData >& idx, BlockIndex index) override;


	/************************************************************************/
	/* returns true if a valid placement position was found                */
	/************************************************************************/
	// static bool getPlacementPosition(Ogre::SceneNode* blockNode, Ogre::Ray const& ray, PlacementResult* resultptr);

	RigidBodyPartVisualizer(GameVisualization* gameViz);
	~RigidBodyPartVisualizer();
	
	//virtual void onBlockWasAdded(RigidBodyPart *bodyPart, BodyBlockInstance * bbInst, BlockIndex const& index) override;
	
	//virtual void onBlockIsBeeingRemoved(RigidBodyPart* bodyPart, BodyBlockInstance* bbInst, const BlockIndex& index) override;

	
	inline ObjectID getBodyPartID() const {
		return _bodyPartID;
	}

	inline ObjectID getParentFullBodyID() const {
		return _parentFullBodyID;
	}

	inline size_t getNumberOfBlocks() const
	{
		return _numBlocks;
	}

	void getWorldTransformForPosition(Eigen::Vector3i const & index, btVector3 * pos, btQuaternion * rot);

private:
	class BlockEntry: public BodyBlockInstanceBase
	{
	public:
		typedef std::shared_ptr<BlockEntry> Ptr;

		BlockEntry(RigidBodyPartVisualizer* parent, osg::Geode* geode, const BodyBlock* bodyBlock, BlockIndex const& centerIndex, const btQuaternion& relRot, OID controlRID);
		
		/*
			also detaches and deletes node
		*/
		virtual ~BlockEntry();

		osg::ref_ptr<osg::Geode> _blockGeode;

		const OID attachedControlRID;

		RigidBodyPartVisualizer* const parentViz;
	};
	typedef BlockEntry::Ptr BlockEntryPtr;

	ObjectID _bodyPartID;
	ObjectID _parentFullBodyID;
	
	//static bool checkRayOnBlockSide(Ogre::Vector3 const& p, Ogre::Vector3 a, Ogre::Vector3 b, Ogre::Ray const& ray);

	// bool getPlacementPosition(BlockEntry* blockEntry, Ogre::Ray const& ray, PlacementResult* resultptr);

	DynamicArray<BlockEntryPtr> _blockData;
	size_t _numBlocks = 0;
	GameVisualization* _gameViz;
	
	osg::ref_ptr<osg::MatrixTransform> _transform;
};

// typedef RigidBodyPartVisualizer::Ptr RigidBodyPartVisualizerPtr;

