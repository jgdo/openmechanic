#pragma once

#include <thread>

#include "GamePhysics/GameWorld.h"
#include "GamePhysics/FullBody.h"
#include "GamePhysics/Joint.h"
#include "GamePhysics/RigidBodyPart.h"

#include "GamePresentation/Commands.h"

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


class ThreadedLocalGameEngine;

class ThreadedLocalGameEngineListener: public GameWorld::Listener, public FullBody::Listener, public Joint::Listener, public RigidBodyPart::Listener, public ControlEngine::ConnectionListener
{
public:
	ThreadedLocalGameEngineListener(ThreadedLocalGameEngine* engine);

	virtual void onLiftWasAdded(GameWorld* gameWorld, Lift* lift) override;
	virtual void onLiftIsBeeingRemoved(GameWorld* gameWorld, Lift* lift) override;
	virtual void onBlockWasAdded(RigidBodyPart* bodyPart, BodyBlockInstance* bbInst) override;
	virtual void onBlockIsBeeingRemoved(RigidBodyPart* bodyPart, BodyBlockInstance* bbInst) override;
	virtual void onPartWasAdded(FullBody* fullBody, RigidBodyPart* bodyPart) override;
	virtual void onPartIsBeingRemoved(FullBody* fullBody, RigidBodyPart* bodyPart) override;
	virtual void onJointWasAdded(FullBody* fullBody, Joint* joint) override;
	virtual void onJointIsBeingRemoved(FullBody* fullBody, Joint* joint) override;
	virtual void onSpringWasAdded(FullBody *fullBody, Spring *spring) override;
	virtual void onSpringIsBeingRemoved(FullBody *fullBody, Spring *spring) override;
	virtual void onFullBodyWasAdded(GameWorld* gameWorld, FullBody* fullBody) override;
	virtual void onFullBodyIsBeeingRemoved(GameWorld* gameWorld, FullBody* fullBody) override;
	virtual void onConnectionAdded(Connection* connection) override;
	virtual void onConnectionIsBeeingRemoved(Connection* connection) override;
	virtual void onPlayerBodyIsBeeingRemoved(GameWorld *gameWorld, PlayerBody *player) override;
	virtual void onPlayerBodyWasAdded(GameWorld *gameWorld, PlayerBody *player) override;

private:
	ThreadedLocalGameEngine *_engine;
};

class ThreadedLocalGameEngineCommandExecutor: public DispatcherFor<
		RemoveFullBody,
		RemoveBodyPart,
		AddJoint,
		RemoveJoint,
		AddSpring,
		RemoveSpring,
		AddBlockToPart,
		AddBlockToFullBody,
		AddBlockToWorld,
		RemoveBlockFromPart,
		RemoveBlockOrJoint,
		DeployLift,
		FoldLift,
		RaiseLift,
		AddBlockToLift,
		ConnectControls,
		UseControl,
		ApplyUserControl>
{
public:
	ThreadedLocalGameEngineCommandExecutor(ThreadedLocalGameEngine* engine);

	void add(const DispatchableBase *ptr);
	
	void executeCalls();

	virtual void dispatch(RemoveFullBody const& obj) override;

	virtual void dispatch(RemoveBodyPart const& obj) override;

	virtual void dispatch(AddJoint const& obj) override;

	virtual void dispatch(RemoveJoint const& obj) override;

	virtual void dispatch(AddSpring const& obj) override ;
	virtual void dispatch(RemoveSpring const& obj) override;

	virtual void dispatch(AddBlockToPart const& obj) override;
	virtual void dispatch(AddBlockToFullBody const& obj) override;
	virtual void dispatch(AddBlockToWorld const& obj) override;
	virtual void dispatch(RemoveBlockFromPart const& obj) override;
	virtual void dispatch(RemoveBlockOrJoint const& obj) override;
	virtual void dispatch(DeployLift const& obj) override;
	virtual void dispatch(FoldLift const& obj) override;
	virtual void dispatch(RaiseLift const& obj) override;
	virtual void dispatch(AddBlockToLift const& obj) override;
	virtual void dispatch(ConnectControls const& obj) override;
	virtual void dispatch(UseControl const& obj) override;
	virtual void dispatch(ApplyUserControl const& obj) override;

private:
	std::recursive_mutex _mux;

	ThreadedLocalGameEngine *_engine;

	std::list<DispatchableBaseConstPtr> _commands;
};

class ThreadedLocalGameEngine
{
public:
	ThreadedLocalGameEngine();
	virtual ~ThreadedLocalGameEngine();

	void startEngine(const std::vector<float> &terrainData, int terrainDataSideLength, float terrainWorldSize);
	void stopEnginAndWait();

	inline GameWorld* getGameWorld()
	{
		return _gameWorld.get();
	}

	inline DispatcherBase* getCommandExecutor()
	{
		return &_commandExecutor;
	}

	void lockDynWorld(); // for debugging only
	void unlockDynWorld(); // for debugging only

private:
	ThreadedLocalGameEngineListener _listener;
	ThreadedLocalGameEngineCommandExecutor _commandExecutor;

	GameWorldPtr _gameWorld;

	std::recursive_mutex _dynWorldMutex;

	std::thread _engineThread;

	boost::asio::io_service _ioService;
	boost::posix_time::microseconds _interval;
	boost::asio::deadline_timer _timer;

	void gameThreadFunction();

	void timerTick();
};

