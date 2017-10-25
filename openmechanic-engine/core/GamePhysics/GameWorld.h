/* 
 * File:   GameWorld.h
 * Author: jgdo
 *
 * Created on February 5, 2016, 1:40 PM
 */

#ifndef GAMEWORLD_H
#define	GAMEWORLD_H

#include <set>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include "Blocks/BlockIDs.h"
#include "BodyBlockFactory.h"
// #include "Control/BlockControlFactory.h"

// #include "PlayerBody.h"

#include <core/GamePhysics/GameContext.h>

#include "ClientInterfaces/Messages/GameWorldMsgs.h"

#include <mutex>
#include "core/Utils/ObjectIDGenerator.h"
#include <map>

class FullBody;
class PlayerBody;
class Lift;

class GameWorld {
public:
    using DataHandler = DataObjectHandler<GameWorldData>*;
	
    DataHandler myIndex; // TODO private
	
	typedef std::shared_ptr<GameWorld> Ptr;
	typedef std::shared_ptr<const GameWorld> ConstPtr;

    GameWorld();
	virtual ~GameWorld();

	void initBulletWorld(const std::vector<float> &terrainData, int terrainDataSideLength, float terrainWorldSize);

	/**
	 * Creates a new full body, adds new body part at (0,0,0) and move the full body to given location
	 * 
	 *	Body will be owned by this GameWorld
	 * 
	 */
	FullBody* addNewFullBody(BlockID blockID, btVector3 const& worldPos, btQuaternion const& worldOorientation, btQuaternion const& localBlockOrientation);
	void removeAndDeleteFullBody(FullBody* fullBody);

	inline btDiscreteDynamicsWorld* getBtDynWorld() {
		return _dynamicsWorld.get();
	}
	
	
#if 0
	Lift* addLift(btVector3 const& position, float height);
	void removeAndDeleteLift(Lift* lift);

	PlayerBody* addPlayerBody(btVector3 const& position);
	void removeAndDeletePlayerBody(PlayerBody* player);
	

	inline ControlEngine* getControlEngine() {
		return &_controlEngine;
	}
	
	inline const ControlEngine* getControlEngine() const{
		return &_controlEngine;
	}
	
	inline BlockControlFactory* getBlockControlFactory() {
		return &_blockControlFactory;
	}
#endif 

	void makeStep(float timestep);

	/*
		the returned pointer the the only one to the queue

		TODO: naming
	*/
	// EventQueueConstPtr getLastEventQueue();

    inline std::set<FullBody*> const& getAllBodies() const {
		return _allBodies;
	}
	
#if 0
	// may only be called from game physics thread
	inline EventQueue* getActiveEventQueue()
	{
		return _activeStateQueue.get();
	}


	inline std::set<Lift*> const& getAllLifts() const {
		return _lifts;
	}

	inline std::set<PlayerBody*> const& getAllPlayerBodies() const {
		return _players;
	}
#endif

private:
    GameContext mContext;

	std::unique_ptr<btBroadphaseInterface> _broadphase;
	std::unique_ptr<btDefaultCollisionConfiguration> _collisionConfiguration;

	std::unique_ptr<btCollisionDispatcher> _collisionDispatcher;

	std::unique_ptr<btSequentialImpulseConstraintSolver> _solver;

	std::unique_ptr<btDiscreteDynamicsWorld> _dynamicsWorld;
	std::unique_ptr<btHeightfieldTerrainShape> _groundShape;
	std::unique_ptr<btDefaultMotionState> _groundMotionState;
	std::unique_ptr<btRigidBody> _groundRigidBody;

	std::vector<float> _terrainHeightData;

	// ControlEngine _controlEngine;

	// BlockControlFactory _blockControlFactory;
	
    std::set<FullBody*> _allBodies;
	
#if 0
	std::set<Lift*> _lifts;
	std::set<PlayerBody*> _players;
#endif

	void activate();
	

	//std::recursive_mutex _queueMutex;
	//EventQueuePtr _lastStateQueue, _activeStateQueue;
};

typedef GameWorld::Ptr GameWorldPtr;
typedef GameWorld::ConstPtr GameWorldConstPtr;

#endif	/* GAMEWORLD_H */

