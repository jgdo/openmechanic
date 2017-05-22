//
// Created by jgdo on 20.05.16.
//

#include <fstream>
#include <GamePhysics/RigidBodyPart.h>

#include "GameSaver.h"

#include "GamePhysics/FullBody.h"
#include "GamePhysics/PlayerBody.h"
#include "GamePhysics/Lift.h"


struct BinarySaver {
	BinarySaver(std::ofstream& fout): fout(fout) {} 
	
	template<class T>
	BinarySaver& operator<<(const T& value) {
		for(size_t i = 0; i < sizeof(T); i++) {
			fout.write((const char*)&value, sizeof(T));
		}
		
		return *this;
	}
	
	std::ofstream& fout;
};

class GameSaver {
public:
	GameSaver(const GameWorld* world):
		_world(world) {
	}

	void saveWorld() {
        std::ofstream ofs("filename", std::ios_base::binary);
        BinarySaver oa(ofs);

		saveFullBodies(oa);
		saveLifts(oa);
		savePlayerBodies(oa);
		saveConnections(oa);
	}

protected:
	const GameWorld* _world;

	void saveFullBodies(BinarySaver& oa) {
		auto size = _world->getAllBodies().size();
		oa << size;

		for(const FullBody* body: _world->getAllBodies()) {
			saveBodyParts(oa, body);
			saveJoints(oa, body);
			saveSprings(oa, body);
		}
	}

	void saveBodyParts(BinarySaver& oa, const FullBody* body) {
		oa << body->getNumParts();
		body->foreachPart([this, &oa](RigidBodyPart* part) {saveBodyPart(oa, part);});
	}

	void saveBodyPart(BinarySaver& oa, const RigidBodyPart* part) {
		// save body info
		oa << part->resourceID;
		oa << part->getCurrentWorldTransform();
		
		// save blocks
		std::set<const BodyBlockInstance*> processedBlocks;
		auto range = part->getBlockDataDimensions();
		for(int z = range.first(2); z < range.second(2); z++) {
			for(int y = range.first(1); y < range.second(1); y++) {
				for(int x = range.first(0); x < range.second(0); x++) {
					auto bbInst = part->getBlockInstanceAt(BlockIndex(x,y,z));
					
					if(processedBlocks.find(bbInst) == processedBlocks.end()) {
						processedBlocks.insert(bbInst);
						
						auto blockIndex = bbInst->getCenterBlockIndex();
						
						oa << blockIndex;
						oa << bbInst->getBodyBlock()->getID();
						oa << bbInst->getRelativeOrientation();
					}
				}
			}
		}
	}

	void saveJoints(BinarySaver& oa, const FullBody* body) {
		oa << body->getNumJoints();
		for(auto joint: body->getJoints()) {
			oa << joint->resourceID;
			oa << joint->getJointAngle();
			oa << joint->getInputMultiplier();
			
			auto a = joint->getBodyPartA(), b = joint->getBodyPartB();
			
			if(a) {
				oa << a->resourceID;
				oa << joint->getRA();
				oa << joint->getAxA();
			} else
				oa << ResourceID(0);
			
			if(b) {
				oa << b->resourceID;
				oa << joint->getRB();
				oa << joint->getAxB();
			} else
				oa << ResourceID(0);
		}
	}

	void saveSprings(BinarySaver& oa, const FullBody* body) {
		oa << body->getNumSprings();
		for(auto spring: body->getSprings()) {
			auto a = spring->getBodyPartA(), b = spring->getBodyPartB();
			
			if(a) {
				oa << a->resourceID;
				oa << spring->getFrameA();
			} else
				oa << ResourceID(0);
			
			if(b) {
				oa << b->resourceID;
				oa << spring->getFrameB();
			} else
				oa << ResourceID(0);
		}
	}
	
	void saveLifts(BinarySaver& oa) {
		oa << _world->getAllLifts().size();
		for(auto lift: _world->getAllLifts()) {
			oa << lift->resourceID;
			oa << lift->getPositionOnGround();
			oa << lift->getHeight();
			oa << (lift->isBodyAttahed()? lift->getAttachedBody()->resourceID : ResourceID(0));
		}
	}

	void savePlayerBodies(BinarySaver& oa) {
		oa << _world->getAllPlayerBodies().size();
		for(auto body: _world->getAllPlayerBodies()) {
			oa << body->resourceID;
			oa << body->getBodyCenter();
			oa << body->getCamYaw();
			oa << body->getCamPitch();
		}
	}

	void saveConnections(BinarySaver& oa) {
		const ControlEngine* ce = _world->getControlEngine();
		oa << ce->getConnections().size();
		for(auto connection: ce->getConnections()) {
			oa << connection->resourceID;
			oa << connection->getSource()->getParentObject()->resourceID;
			oa << connection->getSource()->getConnectionState();
			oa << connection->getSink()->getParentObject()->resourceID;
			oa << connection->getSink()->getConnectionState();
		}
	}
};

void saveGameWorld(const GameWorld* gameWorld) {
	GameSaver(gameWorld).saveWorld();
}
