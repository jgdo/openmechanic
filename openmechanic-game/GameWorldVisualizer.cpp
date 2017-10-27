/*
 * File:   GameWorldVisualizer.cpp
 * Author: jgdo
 *
 * Created on February 5, 2016, 2:03 PM
 */

#include "GameWorldVisualizer.h"

GameWorldVisualizer::GameWorldVisualizer(GameVisualization* gameViz) :
mGameVizualization(gameViz) /*, _connectionViz(gameViz, parentNode) */ {

    //_sceneNode->getUserObjectBindings().setUserAny("type", Ogre::Any(std::string("GameWorldVisualizer")));
    //_sceneNode->getUserObjectBindings().setUserAny("obj", Ogre::Any(this));
}

GameWorldVisualizer::~GameWorldVisualizer() {
}

void GameWorldVisualizer::dispatch(const BodyPartChangeEvent &event)
{
    switch(event.type)
    {
    case BodyPartChangeEvent::ADDED:
        // _fullBodyParts[event.fullBodyRID].insert(event.bodyPartRID);
        _bodyPartVizMap[event.bodyPartRID] = std::make_shared<RigidBodyPartVisualizer>(mGameVizualization, event.fullBodyRID, event.bodyPartRID);
        break;

    case BodyPartChangeEvent::REMOVED:
        _bodyPartVizMap.erase(event.bodyPartRID);
        // _fullBodyParts[event.fullBodyRID].erase(event.bodyPartRID);
        break;
    }
}

void GameWorldVisualizer::dispatch(const BlockAddChangeEvent &event)
{
     _bodyPartVizMap[event.partRID]->addBlock(event.block, event.centerIndex, event.relRot, event.attachedControlRID);
}

void GameWorldVisualizer::dispatch(const BlockRemoveChangeEvent &event)
{
    _bodyPartVizMap[event.partRID]->removeBlock(event.centerIndex);
}

void GameWorldVisualizer::dispatch(const BodyPartState &state)
{
    _bodyPartVizMap[state.resourceID]->updatePosition(state.worldTransform);
}

#if 0

void GameWorldVisualizer::visitFullBodyChangeEvent(const FullBodyChangeEvent* event)
{
    switch(event.type)
    {
    case FullBodyChangeEvent::ADDED:
        _fullBodyParts[event.fullBodyRID] = BodyPartSet();
        break;

    case FullBodyChangeEvent::REMOVED:
        for(RID rid: _fullBodyParts[event.fullBodyRID])
        {
            _bodyPartVizMap.erase(rid);
        }
        _fullBodyParts.erase(event.fullBodyRID);
        break;
    }
}

void GameWorldVisualizer::visitBodyPartChangeEvent(const BodyPartChangeEvent* event)
{
    switch(event.type)
    {
    case BodyPartChangeEvent::ADDED:
        _fullBodyParts[event.fullBodyRID].insert(event.bodyPartRID);
        _bodyPartVizMap[event.bodyPartRID] = std::make_shared<RigidBodyPartVisualizer>(_gameViz, event.fullBodyRID, event.bodyPartRID, _sceneNode);
        break;

    case BodyPartChangeEvent::REMOVED:
        _bodyPartVizMap.erase(event.bodyPartRID);
        _fullBodyParts[event.fullBodyRID].erase(event.bodyPartRID);
        break;
    }
}

void GameWorldVisualizer::visitJointChangeEvent(const JointChangeEvent* event)
{
    switch(event.type)
    {
    case JointChangeEvent::ADDED:
        _jointsMap[event.jointRID] = std::make_shared<JointVisualizer>(_gameViz, event.jointRID, _sceneNode);
        break;

    case JointChangeEvent::REMOVED:
        _jointsMap.erase(event.jointRID);
        break;
    }
}

void GameWorldVisualizer::visitBlockAddChangeEvent(const BlockAddChangeEvent* event)
{
    _bodyPartVizMap[event.partRID]->addBlock(event.block, event.centerIndex, event.relRot, event.attachedControlRID);
}

void GameWorldVisualizer::visitLiftChangeEvent(const LiftChangeEvent* event)
{
    switch(event.type)
    {
    case LiftChangeEvent::ADDED:
        _liftsMap[event.liftID] = std::make_shared<LiftVisualizer>(_gameViz, _sceneNode, event.liftID);
        break;

    case LiftChangeEvent::REMOVED:
        _liftsMap.erase(event.liftID);
        break;
    }
}

void GameWorldVisualizer::visitBlockRemoveChangeEvent(const BlockRemoveChangeEvent* event)
{
    _bodyPartVizMap[event.partRID]->removeBlock(event.centerIndex);
}

void GameWorldVisualizer::visitConnectionChangeEvent(const ConnectionChangeEvent* event) {
    _connectionViz.addRemoveConnection(*event);
}

void GameWorldVisualizer::visitDriverSeatChangeEvent(const DriverSeatChangeEvent* event) {
    // FIXME: better design
//	if (_cameraMan->getAttachedPlayerNode()) {
//		_cameraMan->attachPlayerNode(nullptr);
//	}
//	else {
//		auto iter = _controlNodes.find(event.driverSeatID);
//		if (iter != _controlNodes.end()) {
//			_cameraMan->attachPlayerNode(iter->second);
//		}
//		else {
//			// FIXME: log error
//		}
//	}
}

void GameWorldVisualizer::visitBodyPartState(const BodyPartState* state)
{
    auto iter = _bodyPartVizMap.find(state->resourceID);
    // resource might be already deleted before
    if (iter != _bodyPartVizMap.end())
    {
        iter->second->updatePosition(state->worldTransform);
    }
}

void GameWorldVisualizer::visitLiftState(const LiftState* state)
{
    auto iter = _liftsMap.find(state->resourceID);
    // resource might be already deleted before
    if (iter != _liftsMap.end())
    {
        iter->second->updateState(state);
    }
}

void GameWorldVisualizer::visitJointState(const JointState* state)
{
    auto iter = _jointsMap.find(state->resourceID);
    // resource might be already deleted before
    if(iter != _jointsMap.end())
    {
        iter->second->updatePosition(state->worldTransform);
    }
}

#endif

#if 0
void GameWorldVisualizer::setupCamera(ThreadedLocalGameEngine* gameEngine) {
    _camera = _gameViz->getSceneManager()->createCamera("PlayerCam");
    _camera->setPosition(Ogre::Vector3(0, 150, 500));
    _camera->setNearClipDistance(5);
    // mCameraMan = new OgreBites::SdkCameraMan(mCamera);
    // mCameraMan->setStyle(OgreBites::CS_ORBIT);
    _cameraMan = std::make_shared<CameraMan>(_camera, gameEngine);
    _cameraMan->updateCameraOrientation();
}

#endif

RigidBodyPartVisualizerPtr GameWorldVisualizer::getBodyPartViz(ResourceID rid)
{
    return _bodyPartVizMap.at(rid);
}
#if 0
Ogre::SceneNode* GameWorldVisualizer::getControlNode(ResourceID rid) {
    return _controlNodes.at(rid);
}

void GameWorldVisualizer::registerControlNode(RID rid, Ogre::SceneNode* sceneNode) {
    Ogre::SceneNode*& slot = _controlNodes[rid];
    if (slot)
        throw std::runtime_error("GameWorldVisualizer::registerControlNode(): control node for RID already registered");

    slot = sceneNode;
}

void GameWorldVisualizer::unregisterControlNode(RID rid) {
    _controlNodes.erase(rid);
}


void GameWorldVisualizer::preRenderStep() {
    _connectionViz.updateBulletPosition();
}


void GameWorldVisualizer::visitPlayerBodyState(const PlayerBodyState *state) {
    auto iter = _playersMap.find(state->resourceID);
    // resource might be already deleted before
    if(iter != _playersMap.end())
    {
        iter->second->updateState(state);
    }
}

void GameWorldVisualizer::visitPlayerBodyChangeEvent(const PlayerBodyChangeEvent *event) {
    switch(event.type)
    {
        case PlayerBodyChangeEvent::ADDED: {
            auto & playerViz = _playersMap[event.playerBodyID] = std::make_shared<PlayerBodyVisualizer>(_gameViz, _sceneNode,
                                                                                            event.playerBodyID);

            // attach the first created PlayerBody to the camera
            if (!_cameraMan->getAttachedPlayerNode())
                _cameraMan->attachPlayerNode(playerViz->getSceneNode());
            break;
        }

        case PlayerBodyChangeEvent::REMOVED:
            _liftsMap.erase(event.playerBodyID);
            break;
    }
}


LiftVisualizerPtr GameWorldVisualizer::getLiftViz(ResourceID liftID) {
    return _liftsMap.at(liftID);
}

void GameWorldVisualizer::visitSpringState(const SpringState *state) {
    auto iter = _springMap.find(state->resourceID);
    // resource might be already deleted before
    if(iter != _springMap.end())
    {
        iter->second->updateState(state);
    }
}

void GameWorldVisualizer::visitSpringChangeEvent(const SpringChangeEvent *event) {
    switch(event.type)
    {
        case SpringChangeEvent::ADDED:
            _springMap[event.springRID] = std::make_shared<SpringVisualizer>(_gameViz, event.springRID, _sceneNode);
            break;

        case SpringChangeEvent::REMOVED:
            _springMap.erase(event.springRID);
            break;
    }
}

#endif
