#include "GameObject.h"
#include <core/GamePhysics/GameContext.h>

GameObject::GameObject(OID oid, GameContext *context):
    resourceID(oid), mContext(context) {
}
