#include "ThreadedLocalGameEngine.h"

#include "Control/DriverSeatControl.h" // FIXME remove

#include <boost/bind.hpp>

#include "GamePhysics/Lift.h"


#include <algorithm>

#define FREQ 60.0

void ThreadedLocalGameEngineListener::onLiftWasAdded(GameWorld* gameWorld, Lift* lift)
{
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<LiftChangeEvent>(
		LiftChangeEvent::ADDED, lift->resourceID));
}

void ThreadedLocalGameEngineListener::onLiftIsBeeingRemoved(GameWorld* gameWorld, Lift* lift)
{
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<LiftChangeEvent>(
		LiftChangeEvent::REMOVED, lift->resourceID));
}

ThreadedLocalGameEngineListener::ThreadedLocalGameEngineListener(ThreadedLocalGameEngine* engine):
	_engine(engine)
{
}

void ThreadedLocalGameEngineListener::onBlockWasAdded(RigidBodyPart* bodyPart, BodyBlockInstance* bbInst)
{
	ControlObject* attachedControl = bbInst->getAttachedControl();
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<BlockAddChangeEvent>(
		bodyPart->resourceID, bbInst->getBodyBlock(), bbInst->getCenterBlockIndex(), bbInst->getRelativeOrientation(), attachedControl? attachedControl->resourceID : 0));
}

void ThreadedLocalGameEngineListener::onBlockIsBeeingRemoved(RigidBodyPart* bodyPart, BodyBlockInstance* bbInst)
{
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<BlockRemoveChangeEvent>(
		bodyPart->resourceID, bbInst->getCenterBlockIndex()));
}

void ThreadedLocalGameEngineListener::onPartWasAdded(FullBody* fullBody, RigidBodyPart* bodyPart)
{
	bodyPart->setListener(this);

	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<BodyPartChangeEvent>(
		fullBody->resourceID, bodyPart->resourceID, BodyPartChangeEvent::ADDED));
}

void ThreadedLocalGameEngineListener::onPartIsBeingRemoved(FullBody* fullBody, RigidBodyPart* bodyPart)
{
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<BodyPartChangeEvent>(
		fullBody->resourceID, bodyPart->resourceID, BodyPartChangeEvent::REMOVED));
}

void ThreadedLocalGameEngineListener::onJointWasAdded(FullBody* fullBody, Joint* joint)
{
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<JointChangeEvent>(
		fullBody->resourceID, joint->resourceID, JointChangeEvent::ADDED));
}

void ThreadedLocalGameEngineListener::onJointIsBeingRemoved(FullBody* fullBody, Joint* joint)
{
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<JointChangeEvent>(
		fullBody->resourceID, joint->resourceID, JointChangeEvent::REMOVED));
}

void ThreadedLocalGameEngineListener::onFullBodyWasAdded(GameWorld* gameWorld, FullBody* fullBody)
{
	fullBody->setListener(this);

	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<FullBodyChangeEvent>(
		fullBody->resourceID, FullBodyChangeEvent::ADDED));
}

void ThreadedLocalGameEngineListener::onFullBodyIsBeeingRemoved(GameWorld* gameWorld, FullBody* fullBody)
{
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<FullBodyChangeEvent>(
		fullBody->resourceID, FullBodyChangeEvent::REMOVED));
}

void ThreadedLocalGameEngineListener::onConnectionAdded(Connection* connection) {
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<ConnectionChangeEvent>(
		ConnectionChangeEvent::ADDED, connection->resourceID, connection->getSource()->getParentObject()->resourceID, connection->getSink()->getParentObject()->resourceID));
}

void ThreadedLocalGameEngineListener::onConnectionIsBeeingRemoved(Connection* connection) {
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<ConnectionChangeEvent>(
		ConnectionChangeEvent::REMOVED, connection->resourceID, connection->getSource()->getParentObject()->resourceID, connection->getSink()->getParentObject()->resourceID));
}

void ThreadedLocalGameEngineListener::onPlayerBodyIsBeeingRemoved(GameWorld *gameWorld, PlayerBody *player) {
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<PlayerBodyChangeEvent>(
			PlayerBodyChangeEvent::REMOVED, player->resourceID));
}

void ThreadedLocalGameEngineListener::onPlayerBodyWasAdded(GameWorld *gameWorld, PlayerBody *player) {
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<PlayerBodyChangeEvent>(
			PlayerBodyChangeEvent::ADDED, player->resourceID));
}

void ThreadedLocalGameEngineListener::onSpringWasAdded(FullBody *fullBody, Spring *spring) {
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<SpringChangeEvent>(
			SpringChangeEvent::ADDED, fullBody->resourceID, spring->resourceID));
}

void ThreadedLocalGameEngineListener::onSpringIsBeingRemoved(FullBody *fullBody, Spring *spring) {
	_engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<SpringChangeEvent>(
			SpringChangeEvent::REMOVED, fullBody->resourceID, spring->resourceID));
}


ThreadedLocalGameEngineCommandExecutor::ThreadedLocalGameEngineCommandExecutor(ThreadedLocalGameEngine* engine): _engine(engine)
{
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(RemoveFullBody const &obj) {
	FullBody *fullbody = _engine->getGameWorld()->getResoureManager()->tryGet<FullBody>(obj.rid);
	if (fullbody) {
		_engine->getGameWorld()->removeAndDeleteFullBody(fullbody);
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(RemoveBodyPart const &obj) {
	RigidBodyPart *bodyPart = _engine->getGameWorld()->getResoureManager()->tryGet<RigidBodyPart>(obj.partRID);
	if (bodyPart) {
		FullBody* fullBody = bodyPart->getParent();
		fullBody->removeAndDeleteBodyPart(bodyPart); // bodyPart is now a dangling pointer
		if (fullBody->getNumParts() == 0)
		{
			_engine->getGameWorld()->removeAndDeleteFullBody(fullBody);
		}
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(AddJoint const &obj) {
	RigidBodyPart *bodyPart = _engine->getGameWorld()->getResoureManager()->tryGet<RigidBodyPart>(obj.bodyPartRID);
	if (bodyPart) {
		bodyPart->getParent()->addJoint(bodyPart, obj.position, obj.direction);
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(RemoveJoint const &obj) {
	Joint *joint = _engine->getGameWorld()->getResoureManager()->tryGet<Joint>(obj.jointRID);
	if (joint) {
		joint->getParent()->removeAndDeleteJoint(joint);
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(AddSpring const &obj) {
	RigidBodyPart *bodyPart = _engine->getGameWorld()->getResoureManager()->tryGet<RigidBodyPart>(obj.bodyPartRID);
	if (bodyPart) {
		Spring* spring = bodyPart->getParent()->addSpring(bodyPart, obj.position, obj.direction);

		// FIXME: just for first tests, remove
		btVector3 off;
		btQuaternion rot;
		bodyPart->getWorldTransformForPosition(obj.position + obj.direction + obj.direction + obj.direction, &off, &rot);

		FullBody* fullBody = bodyPart->getParent();
		RigidBodyPart *newPart = fullBody->addNewBodyPart(BLOCK_WOOD, off, rot, btQuaternion(0,0,0,1), bodyPart);
		spring->attachBodyPart(newPart, BlockIndex(0, 0, 0), obj.direction);
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(RemoveSpring const &obj) {
	Spring *spring = _engine->getGameWorld()->getResoureManager()->tryGet<Spring>(obj.springRID);
	if (spring) {
		spring->getParent()->removeAndDeleteSpring(spring);
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(AddBlockToPart const &obj) {
	RigidBodyPart *bodyPart = _engine->getGameWorld()->getResoureManager()->tryGet<RigidBodyPart>(obj.partRID);
	if (bodyPart) {
		Joint* joint = bodyPart->getJointAt(obj.basePosition, obj.direction);
		if (joint) {
			btVector3 off;
			btQuaternion rot;
			bodyPart->getWorldTransformForPosition(obj.basePosition + obj.direction, &off, &rot);

			FullBody* fullBody = bodyPart->getParent();
			RigidBodyPart *newPart = fullBody->addNewBodyPart(obj.blockId, off, rot, obj.orientation, bodyPart);
			joint->attachBodyPart(newPart, BlockIndex(0, 0, 0), -obj.direction);
		}
		else
		{
			bodyPart->addBlock(obj.blockId, obj.basePosition + obj.direction, obj.orientation);
		}
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(AddBlockToFullBody const &obj) {
	RigidBodyPart *bodyPart = _engine->getGameWorld()->getResoureManager()->tryGet<RigidBodyPart>(obj.bodyPartRID);
	if (bodyPart) {
		bodyPart->getParent()->addNewBodyPart(obj.blockId, obj.relativePosition, obj.relativeOrientation, obj.localBlockOrientation, bodyPart);
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(AddBlockToWorld const &obj) {
	_engine->getGameWorld()->addNewFullBody(obj.blockId, obj.worldPosition, obj.worldOrientation, obj.localBlockOrientation);
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(RemoveBlockFromPart const &obj) {
	RigidBodyPart *bodyPart = _engine->getGameWorld()->getResoureManager()->tryGet<RigidBodyPart>(obj.partRID);
	if (bodyPart) {
		bodyPart->removeBlock(obj.index);

		if (bodyPart->getNumBlocks() == 0)
		{
			FullBody* fullBody = bodyPart->getParent();
			fullBody->removeAndDeleteBodyPart(bodyPart); // bodyPart is now a dangling pointer
			bodyPart = 0;
			if (fullBody->getNumParts() == 0)
			{
				_engine->getGameWorld()->removeAndDeleteFullBody(fullBody);
			}
		}
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(RemoveBlockOrJoint const &obj) {
	RigidBodyPart *bodyPart = _engine->getGameWorld()->getResoureManager()->tryGet<RigidBodyPart>(obj.partRID);
	if (bodyPart) {
		// check whether we have to delete a joint or the block
		Joint* joint = bodyPart->getJointAt(obj.index, obj.direction);
		if (joint) { // joint present -> delete only joint for now
			bodyPart->getParent()->removeAndDeleteJoint(joint);
		}
		else { // no joint present, delete block and the body if it was the last block
			if (bodyPart->getNumBlocks() > 1)
			{
				bodyPart->removeBlock(obj.index);
			}
			else
			{
				FullBody* fullBody = bodyPart->getParent();
				fullBody->removeAndDeleteBodyPart(bodyPart); // bodyPart is now a dangling pointer
				if (fullBody->getNumParts() == 0)
				{
					_engine->getGameWorld()->removeAndDeleteFullBody(fullBody);
				}
			}
		}
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(DeployLift const &obj) {
	if (obj.liftID) {
		Lift* lift = _engine->getGameWorld()->getResoureManager()->tryGet<Lift>(obj.liftID);
		if (lift)
		{
			lift->deploy(obj.pos, obj.initialHeight);
		}
		else
		{
			// FIXME: log error
		}
	}
	else
	{
		_engine->getGameWorld()->addLift(obj.pos, obj.initialHeight);
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(FoldLift const &obj) {
	Lift* lift = _engine->getGameWorld()->getResoureManager()->tryGet<Lift>(obj.liftID);
	if (lift)
	{
		_engine->getGameWorld()->removeAndDeleteLift(lift);
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(RaiseLift const &obj) {
	Lift* lift = _engine->getGameWorld()->getResoureManager()->tryGet<Lift>(obj.liftID);
	if (lift)
	{
		lift->setRaiseStep(obj.direction);
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(AddBlockToLift const &obj) {
	Lift* lift = _engine->getGameWorld()->getResoureManager()->tryGet<Lift>(obj.liftID);
	if (lift)
	{
		FullBody* fullBody = _engine->getGameWorld()->addNewFullBody(obj.blockId, obj.worldPosition, btQuaternion(0, 0, 0, 1), obj.localBlockOrientation);
		lift->attachFullBody(fullBody, obj.worldPosition - lift->getCenterPositionOnTop());
	}
	else
	{
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(ConnectControls const &obj) {

	ControlObject* controlA = _engine->getGameWorld()->getResoureManager()->tryGet<ControlObject>(obj.a);
	ControlObject* controlB = _engine->getGameWorld()->getResoureManager()->tryGet<ControlObject>(obj.b);

	if (controlA && controlB) {
		Connection* con = ControlEngine::conrolsAreConnected(controlA, controlB);

		if (con)
		{
			_engine->getGameWorld()->getControlEngine()->removeAndDeleteConnection(con);
		}
		else
		{
			_engine->getGameWorld()->getControlEngine()->tryCreateConnection(controlA, controlB);
		}
	}
	else {
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(UseControl const &obj) {
	ControlObject* control = _engine->getGameWorld()->getResoureManager()->tryGet<ControlObject>(obj.controlID);
	if (control) {
		// FIXME: proper design
		DriverSeatControl* dsc = dynamic_cast<DriverSeatControl*>(control);
		PlayerBody* player = _engine->getGameWorld()->getResoureManager()->tryGet<PlayerBody>(obj.playerID);
		if (dsc) {
			// _engine->getGameWorld()->getActiveEventQueue()->addChangeEvent(std::make_shared<DriverSeatChangeEvent>(DriverSeatChangeEvent::ENTER, controlID)); // FIXME
			if(player->getAttachedDriverSeat())
				player->detachDriverSeat();
			else
				player->attachDriverSeat(dsc);
		}

		Joint* joint = dynamic_cast<Joint*>(control);
		if(joint)
		{
			joint->setInputMultiplier(joint->getInputMultiplier() > 0 ? -1 : 1);
		}
	}
	else {
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::dispatch(ApplyUserControl const &obj) {
	PlayerBody* player = _engine->getGameWorld()->getResoureManager()->tryGet<PlayerBody>(obj.playerID);
	if (player) {
		player->insertClientMotion(obj.motion);
	} else {
		// FIXME: log error
	}
}

void ThreadedLocalGameEngineCommandExecutor::add(const DispatchableBase *ptr) {
	_mux.lock();
	_commands.emplace_back(ptr);
	_mux.unlock();
}

void ThreadedLocalGameEngineCommandExecutor::executeCalls() {
	_mux.lock();
	std::list<DispatchableBaseConstPtr> com(std::move(_commands));
	_mux.unlock();
	
	for(auto& c: com)
		c->tryDispatchSelf(this);
}


ThreadedLocalGameEngine::ThreadedLocalGameEngine():
	_listener(this), _commandExecutor(this), _interval(1000000.0 / FREQ), _timer(_ioService, _interval)
{
}

ThreadedLocalGameEngine::~ThreadedLocalGameEngine()
{
}

void ThreadedLocalGameEngine::startEngine(const std::vector<float> &terrainData, int terrainDataSideLength,
										  float terrainWorldSize)
{
	_gameWorld = std::make_shared<GameWorld>();
	_gameWorld->initBulletWorld(terrainData, terrainDataSideLength, terrainWorldSize);
	_gameWorld->setListener(&_listener);
	_gameWorld->getControlEngine()->setListener(&_listener);

	_engineThread = std::thread([this] { this->gameThreadFunction(); });
}

// See http://stackoverflow.com/questions/4267546/c-boost-asio-simple-periodic-timer
void ThreadedLocalGameEngine::gameThreadFunction()
{
	_gameWorld->addPlayerBody(btVector3(0,1,0));

    _gameWorld->addNewFullBody(BLOCK_WOOD, btVector3(0, 0, 1), btQuaternion(0,0,0), btQuaternion(0,0,0));

	_timer.async_wait(boost::bind(&ThreadedLocalGameEngine::timerTick, this));
	_ioService.run();
}

void ThreadedLocalGameEngine::timerTick()
{
	// process commands
	_dynWorldMutex.lock();
	_commandExecutor.executeCalls();
	_gameWorld->makeStep(_interval.total_microseconds() / 1000000.0);
	_dynWorldMutex.unlock();

	_timer.expires_at(_timer.expires_at() + _interval);
	_timer.async_wait(boost::bind(&ThreadedLocalGameEngine::timerTick, this));
}

void ThreadedLocalGameEngine::stopEnginAndWait() {
	_ioService.stop();
	_engineThread.join();
}

void ThreadedLocalGameEngine::lockDynWorld() {
	_dynWorldMutex.lock();
}

void ThreadedLocalGameEngine::unlockDynWorld() {
	_dynWorldMutex.unlock();
}
