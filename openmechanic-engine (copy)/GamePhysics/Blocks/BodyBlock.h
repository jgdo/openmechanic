#pragma once

#include <btBulletDynamicsCommon.h>

#include <vector>
#include <Eigen/Dense>

class CubeBodyBlock;

typedef Eigen::Vector3i BlockIndex;
typedef btQuaternion BlockOrientation;

/************************************************************************/
/* Tells at which position and at which orientation blocks can be attached to a block                                                                     */
/************************************************************************/
struct BlockAttachInfoEntry {
	BlockIndex index;
	std::vector<btVector3> normals;

	inline BlockAttachInfoEntry(BlockIndex const& index, std::vector<btVector3> const& normals) :
		index(index), normals(normals) {}
};

class BodyBlock {
public:
    static const float BlockSize; // in bullet units (m)

	static inline btVector3 positionFormIndex(const BlockIndex& index)
	{
		return btVector3(index.x() * BlockSize, index.y() * BlockSize, index.z()*BlockSize);
	}

    BodyBlock(size_t id);
    virtual ~BodyBlock();

    inline size_t getID() const {
        return _ownID;
    }

    virtual bool hasOrientation() const = 0;

	virtual bool isCube() const = 0;
    virtual const CubeBodyBlock* toCube() const = 0;
    virtual CubeBodyBlock* toCube() = 0;

    virtual btScalar getMass() const = 0;

    /************************************************************************/
    /* Ownership remains by this block   
     * 
     * Orientation is in positive x
    /************************************************************************/
    virtual btCollisionShape* getCreateCollisionShape() = 0;

	virtual btVector3 getCenterOfMassDisplacement() const;

    /**
     * 
     * @param orientation one of the elements is 1 or -1, other are 0. E.g. (0, 0, 1) means this block is about to be placed on the positive Z side surface
     * @return 
     */
    virtual std::vector<BlockOrientation> getPossiblePlacementOrientations(BlockIndex const &orientation) const;

	/************************************************************************/
	/* return the indices this block will fill when placed at (0,0,0) with no rotation (placement direction x positive) */
	/************************************************************************/
	virtual std::vector<BlockAttachInfoEntry> const& getAttachInfos() const = 0;

protected:
    const size_t _ownID;
    
    static std::vector<BlockOrientation> getPlacementDirectionsAny();
};

class CubeBodyBlock : public BodyBlock {
public:
    // get texture(size_t face)

    CubeBodyBlock(size_t id);

	virtual const CubeBodyBlock* toCube() const override;
	virtual CubeBodyBlock* toCube() override;

	virtual bool isCube() const override;

	// cube has only (0,0,0)
	virtual std::vector<BlockAttachInfoEntry> const& getAttachInfos() const override;

private:
	static std::vector<BlockAttachInfoEntry> _indices;
};

inline btVector3 toVector(const BlockIndex& index) {
	return btVector3(index.x(), index.y(), index.z()) * BodyBlock::BlockSize;
}

inline BlockIndex toIndex(const btVector3& index) {
	btVector3 index2 = index / BodyBlock::BlockSize;
	return BlockIndex(lrint(index2.x()), lrint(index2.y()), lrint(index2.z()));
}

