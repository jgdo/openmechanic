#pragma once

typedef int ObjectID;
typedef ObjectID OID;

class GameContext;

#include <memory>

class GameObject
{
public:
	typedef std::shared_ptr<GameObject> Ptr;
	typedef std::shared_ptr<const GameObject> ConstPtr;

	const ObjectID resourceID;
    GameContext* const mContext;

    GameObject(OID oid, GameContext* context);

	inline virtual ~GameObject() {}
};

using GameObjectPtr = GameObject::Ptr;
using GameObjectConstPtr = GameObject::ConstPtr;

#define DECLARE_RESOURCE(name) typedef std::shared_ptr<name> Ptr; \
typedef std::shared_ptr<const name> ConstPtr; \
template<typename ...Arg> std::shared_ptr<name> static create(Arg&&...arg) { \
	return Ptr(new name(arg...)); \
}

 /*struct EnableMakeShared : public name {
		EnableMakeShared(Arg&&...arg) :name(std::forward<Arg>(arg)...) {}
	};
	return std::make_shared<EnableMakeShared>(std::forward<Arg>(arg)...);*/
	// TODO: need an aligned allocator
