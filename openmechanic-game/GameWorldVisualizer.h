/*
 * File:   GameWorldVisualizer.h
 * Author: jgdo
 *
 * Created on February 5, 2016, 2:03 PM
 */

#ifndef GAMEWORLDVISUALIZER_H
#define	GAMEWORLDVISUALIZER_H

#include <GamePresentation/Dispatch.h>
#include <GamePresentation/ChangeEvents.h>
#include <GamePresentation/ObjectState.h>

// #include "CameraMan.h"

// #include "VisualizerBase.h"
#include "RigidBodyPartVisualizer.h"
//#include "JointVisualizer.h"
//#include "LiftVizualizer.h"
//#include "PlayerBodyVisualizer.h"
//#include "ConnectionVisualizer.h"
//#include "SpringVisualizer.h"

#include "GameVisualization.h"


class ThreadedLocalGameEngine;

class GameWorldVisualizer: public DispatcherFor<
        BodyPartChangeEvent,
        BlockAddChangeEvent,
        BlockRemoveChangeEvent,
        BodyPartState> {
public:
    GameWorldVisualizer(GameVisualization* gameViz);
    virtual ~GameWorldVisualizer();

    virtual void dispatch(const BodyPartChangeEvent& event) override;
    virtual void dispatch(const BlockAddChangeEvent& event) override;
    virtual void dispatch(const BlockRemoveChangeEvent& event) override;

    virtual void dispatch(const BodyPartState& state) override;

    RigidBodyPartVisualizerPtr getBodyPartViz(ResourceID rid);



#if 0
    virtual void visitBodyPartState(const BodyPartState* state) override;
    virtual void visitJointState(const JointState* state) override;
    virtual void visitLiftState(const LiftState* state) override;
    virtual void visitPlayerBodyState(const PlayerBodyState* state) override;
    virtual void visitSpringState(const SpringState *state) override;


    LiftVisualizerPtr getLiftViz(ResourceID liftID);

    void setupCamera(ThreadedLocalGameEngine* gameEngine);

    Ogre::SceneNode* getControlNode(ResourceID rid);

    void registerControlNode(RID rid, Ogre::SceneNode* sceneNode);
    void unregisterControlNode(RID rid);

    void preRenderStep();

    inline Ogre::Camera* getCamera() {
        return _camera;
    }

    inline CameraMan* getCameraMan() {
        return _cameraMan.get();
    }

#endif

private:
    GameVisualization* mGameVizualization;

    typedef std::set<ResourceID> BodyPartSet;

    std::map<ResourceID, RigidBodyPartVisualizerPtr> _bodyPartVizMap;

#if 0
    std::map<ResourceID, BodyPartSet> _fullBodyParts;

    std::map<ResourceID, JointVisualizerPtr> _jointsMap;

    std::map<ResourceID, Ogre::SceneNode*> _controlNodes;

    std::map<ResourceID, LiftVisualizerPtr> _liftsMap;

    std::map<ResourceID, PlayerBodyVisualizerPtr> _playersMap;

    std::map<ResourceID, SpringVisualizerPtr> _springMap;

    // ConnectionVisualizer _connectionViz;

    //Ogre::Camera* _camera;
    //std::shared_ptr<CameraMan> _cameraMan;

#endif
};

#endif	/* GAMEWORLDVISUALIZER_H */
