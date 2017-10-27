#pragma once

// #include "VisualizerBase.h"
#include "GamePhysics/Blocks/BodyBlock.h"

#include <memory>

#include "Utils/DynamicArray.h"

#include "Utils/Resource.h"
#include <GamePhysics/BodyBlockInstance.h>

#include "GameVisualization.h"

#include <osg/Geode>
#include <osg/MatrixTransform>

class RigidBodyPartVisualizer
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

    */

    typedef std::shared_ptr<RigidBodyPartVisualizer> Ptr;


    void addBlock(const BodyBlock* block, BlockIndex const& index, btQuaternion const& relRot, RID attachedControlRID);
    void removeBlock(BlockIndex const& index);

	/************************************************************************/
	/* returns true if a valid placement position was found                */
	/************************************************************************/
	// static bool getPlacementPosition(Ogre::SceneNode* blockNode, Ogre::Ray const& ray, PlacementResult* resultptr);

    RigidBodyPartVisualizer(GameVisualization* gameViz, ResourceID parentID, ResourceID bodyPartID);
	~RigidBodyPartVisualizer();
	
	//virtual void onBlockWasAdded(RigidBodyPart *bodyPart, BodyBlockInstance * bbInst, BlockIndex const& index) override;
	
	//virtual void onBlockIsBeeingRemoved(RigidBodyPart* bodyPart, BodyBlockInstance* bbInst, const BlockIndex& index) override;

	
    inline ResourceID getBodyPartID() const {
		return _bodyPartID;
	}

    inline ResourceID getParentFullBodyID() const {
		return _parentFullBodyID;
	}

    void updatePosition(btTransform const& transform);

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

        BlockEntry(RigidBodyPartVisualizer* parent, osg::Geode* geode, const BodyBlock* bodyBlock, BlockIndex const& centerIndex, const btQuaternion& relRot, RID controlRID);
		
		/*
			also detaches and deletes node
		*/
		virtual ~BlockEntry();

		osg::ref_ptr<osg::Geode> _blockGeode;

        const RID attachedControlRID;

		RigidBodyPartVisualizer* const parentViz;
	};
	typedef BlockEntry::Ptr BlockEntryPtr;

    ResourceID _bodyPartID;
    ResourceID _parentFullBodyID;
	
	//static bool checkRayOnBlockSide(Ogre::Vector3 const& p, Ogre::Vector3 a, Ogre::Vector3 b, Ogre::Ray const& ray);

	// bool getPlacementPosition(BlockEntry* blockEntry, Ogre::Ray const& ray, PlacementResult* resultptr);

	DynamicArray<BlockEntryPtr> _blockData;
	size_t _numBlocks = 0;
	GameVisualization* _gameViz;
	
	osg::ref_ptr<osg::MatrixTransform> _transform;
};

typedef RigidBodyPartVisualizer::Ptr RigidBodyPartVisualizerPtr;

