#include "RigidBodyPartVisualizer.h"

#include "BlockEntityFactory.h"

// #include "GameWorldVisualizer.h"

//#include "Application/Math.h"
//#include "Application/BtOgreExtras.h"

#include <iostream>

#include "osgbCollision/Utils.h"

void RigidBodyPartVisualizer::addBlock(const BodyBlock *block, const BlockIndex &index, const btQuaternion &relRot, RID attachedControlRID)
{
    osg::ref_ptr<osg::Geode> blockNode = new osg::Geode;

    // FIXME BlockEntityFactory creation
    std::pair<osg::Drawable*, osg::Vec3> blockEntity = BlockEntityFactory().createNewEntity(block->getID());
    if (blockEntity.first)
        blockNode->addDrawable(blockEntity.first);
    else
        std::cerr << "no entity for block id " << block->getID() << std::endl;

    osg::ref_ptr<osg::MatrixTransform> blockTransform = new osg::MatrixTransform;
    blockTransform->setMatrix(osgbCollision::asOsgMatrix(btTransform(relRot, BodyBlock::positionFormIndex(index))) * osg::Matrix::scale(blockEntity.second * BodyBlock::BlockSize));

    auto entry = BlockEntryPtr(new BlockEntry(this, blockNode, block, index, relRot, attachedControlRID));
    _blockData.ensureIndexExists(index.x(), index.y(), index.z());
    _blockData.at(index.x(), index.y(), index.z()) = entry; // TODO: aligned make_shared
    _numBlocks++;

    // blockNode->getUserObjectBindings().setUserAny("type", Ogre::Any(std::string("BlockEntry")));
    // blockNode->getUserObjectBindings().setUserAny("obj", Ogre::Any(entry.get()));

#if 0
    if (attachedControlRID) {
        Ogre::SceneNode* connectionNode = blockNode->createChildSceneNode();
        Ogre::Entity* connectionEntity = _gameViz->getSceneManager()->createEntity(Ogre::SceneManager::PT_SPHERE);
        connectionEntity->setMaterialName("Test/Connection");
        connectionEntity->setCastShadows(false);
        connectionEntity->setRenderQueueGroup(90);
        connectionNode->attachObject(connectionEntity);
        connectionNode->scale(1 / 3.0 / blockEntity.second.x, 1 / 3.0 / blockEntity.second.y, 1 / 3.0 / blockEntity.second.z);
        connectionNode->getUserObjectBindings().setUserAny("type", Ogre::Any(std::string("ControlObject")));
        connectionNode->getUserObjectBindings().setUserAny("obj", Ogre::Any(attachedControlRID));

        _gameViz->getWorldVisualizer()->registerControlNode(attachedControlRID, connectionNode);
    }
#endif

    blockTransform->addChild(blockNode.get());
    _transform->addChild(blockTransform.get());
}

void RigidBodyPartVisualizer::removeBlock(const BlockIndex &index)
{
    auto& ptr = _blockData.at(index.x(), index.y(), index.z());
    if (ptr)
        _numBlocks--;

#if 0
    // TODO: maybe put into destructor?
    if (ptr->attachedControlRID) {
        _gameViz->getWorldVisualizer()->unregisterControlNode(ptr->attachedControlRID);
    }

#endif

    ptr.reset();
}

RigidBodyPartVisualizer::RigidBodyPartVisualizer(GameVisualization* gameViz, ResourceID parentID, ResourceID bodyPartID) :
		_bodyPartID(0), _parentFullBodyID(0), _blockData(3,3,3), _gameViz(gameViz) {
	//_sceneNode->getUserObjectBindings().setUserAny("type", Ogre::Any(std::string("RigidBodyPartVisualizer")));
	//_sceneNode->getUserObjectBindings().setUserAny("obj", Ogre::Any(this));

    _bodyPartID = bodyPartID;
    _parentFullBodyID = parentID;

    _transform = new osg::MatrixTransform;
    _gameViz->rootNode->addChild(_transform.get());
}


RigidBodyPartVisualizer::BlockEntry::BlockEntry(RigidBodyPartVisualizer* parent, osg::Geode* geode, const BodyBlock* bodyBlock, BlockIndex const& centerIndex, const btQuaternion& relRot, RID controlRID):
	BodyBlockInstanceBase(bodyBlock, centerIndex, relRot), _blockGeode(geode), attachedControlRID(controlRID), parentViz(parent)
{
}

RigidBodyPartVisualizer::BlockEntry::~BlockEntry()
{
	parentViz->_transform->removeChild(_blockGeode.get());
#if 0
	if (_blockNode) {
		destroyAttachedObjects(_blockNode);
		_blockNode->removeAndDestroyAllChildren();
		_blockNode->getParentSceneNode()->removeChild(_blockNode);
		_blockNode->getCreator()->destroySceneNode(_blockNode);

		if (attachedControlRID) {
			parentViz->getGameVisualization()->getWorldVisualizer()->unregisterControlNode(attachedControlRID);
		}
	}
#endif 
}

RigidBodyPartVisualizer::~RigidBodyPartVisualizer() {
}

void RigidBodyPartVisualizer::updatePosition(const btTransform &transform)
{
    _transform->setMatrix(osgbCollision::asOsgMatrix(transform));
}

#if 0
void RigidBodyPartVisualizer::getWorldTransformForPosition(Eigen::Vector3i const & index, btVector3 * pos, btQuaternion * rot) {
	btTransform worldTrans(BtOgre::Convert::toBullet(_sceneNode->getOrientation()), BtOgre::Convert::toBullet(_sceneNode->getPosition()));
	
	if (pos)
		*pos = worldTrans(btVector3(index.x() * BodyBlock::BlockSize, index.y() * BodyBlock::BlockSize, index.z() * BodyBlock::BlockSize));
	if (rot)
		*rot = worldTrans.getRotation() * btQuaternion(0, 0, 0, 1); // FIXME: real block transform
}

bool RigidBodyPartVisualizer::getPlacementPosition(Ogre::SceneNode* blockNode, Ogre::Ray const& ray, PlacementResult* resultptr) {
	BlockEntry* entry = getObjectPtr<BlockEntry>(blockNode, "BlockEntry");
	RigidBodyPartVisualizer* viz = getObjectPtr<RigidBodyPartVisualizer>(blockNode->getParentSceneNode(), "RigidBodyPartVisualizer");

	return viz->getPlacementPosition(entry, ray, resultptr);
}

static inline Ogre::Vector3 normalSideA(const btVector3& normal) {
	return BtOgre::Convert::toOgre(btVector3(normal.z(), normal.x(), normal.y()) * (BodyBlock::BlockSize / 2));
}

static inline Ogre::Vector3 normalSideB(const btVector3& normal) {
	return BtOgre::Convert::toOgre(btVector3(normal.y(), normal.z(), normal.x()) * (BodyBlock::BlockSize / 2));
}

bool RigidBodyPartVisualizer::getPlacementPosition(BlockEntry* entry, Ogre::Ray const& ray, PlacementResult* resultptr) {
	Ogre::Vector3 rayPos = _sceneNode->convertWorldToLocalPosition(ray.getOrigin());
	Ogre::Vector3 rayTarget = _sceneNode->convertWorldToLocalPosition(ray.getPoint(1));
	btVector3 bbPos = entry->getRelativePosition();

	Ogre::Ray localRayPre(rayPos - BtOgre::Convert::toOgre(bbPos), rayTarget - rayPos);
	
	Ogre::Vector3 d = localRayPre.getDirection();
	Eigen::Vector3i blockIndexOffset(lrint(bbPos.x() / BodyBlock::BlockSize), lrint(bbPos.y() / BodyBlock::BlockSize), lrint(bbPos.z() / BodyBlock::BlockSize));

	/*

			^ Y
			|
			|
			|
			|
			|
			|
			|
			|
			------------------> X
		   /
		  /
		 /
		/
	   /
	  <   Z

	 */

	Eigen::Vector3i index(0, 0, 0);
	btTransform localTrans(entry->getRelativeOrientation());

	// check if ray intersects a face of the block
	for (BlockAttachInfoEntry const& attachEntry : entry->getBodyBlock()->getAttachInfos()) {
		BlockIndex bOff = toIndex(localTrans(toVector(attachEntry.index))); // rotate index according local block orientation

		Ogre::Ray localRay(localRayPre.getOrigin() - BtOgre::Convert::toOgre(btVector3(bOff.x(), bOff.y(), bOff.z())*BodyBlock::BlockSize), localRayPre.getDirection());

		for (btVector3 const& normalRaw : attachEntry.normals) {
			btVector3 normal = localTrans(normalRaw);
			if (d.dotProduct(-BtOgre::Convert::toOgre(normal)) > 0 && checkRayOnBlockSide(BtOgre::Convert::toOgre(normal) * BodyBlock::BlockSize / 2, normalSideA(normal), normalSideB(normal), localRay)) { 
				index = Eigen::Vector3i(lrint(normal.x()), lrint(normal.y()), lrint(normal.z()));

				if(resultptr)
				{
					resultptr->baseIndex = blockIndexOffset + bOff;
					resultptr->bodyPartID = _bodyPartID;
					resultptr->placeable = true;
					resultptr->placementDirection = index;
					resultptr->viz = this;
				}
				
				return true;
			}

			// FIXME: fill resultptr->placeable appropriately
		}
	}

	return false; // FIXME: fill resultptr->placeable appropriately
}

bool RigidBodyPartVisualizer::checkRayOnBlockSide(Ogre::Vector3 const& p, Ogre::Vector3 a, Ogre::Vector3 b, Ogre::Ray const& ray) {
	Ogre::Vector3 planePos = Math::rayPlaneIntersection(p, a, b, ray);

	return planePos.z > 0 && std::abs(planePos.x) < 1 && std::abs(planePos.y) < 1;
}

#endif
