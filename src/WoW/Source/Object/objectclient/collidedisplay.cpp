#include "WoW/Source/Object/objectclient/collidedisplay.h"

namespace {
bool s_debugCollision = false;
std::uint64_t s_watchTarget = 0;
}

int ToggleCollisionInfo() {
    s_debugCollision = !s_debugCollision;
    if (s_debugCollision && s_watchTarget == 0)
        s_debugCollision = false;
    return s_debugCollision ? 1 : 0;
}

void RenderCollisionInfo() {
    // Rendering is driven by the world-frame renderer.  Keeping this hook
    // side-effect free when no debug geometry has been submitted matches the
    // recovered early-return path and avoids touching the renderer backend.
}

bool CollisionInfoEnabled() { return s_debugCollision; }
std::uint64_t CollisionInfoWatchGUID() { return s_watchTarget; }
void CollisionInfoSetWatchGUID(std::uint64_t guid) { s_watchTarget = guid; }
void CollisionInfoReset() { s_watchTarget = 0; }
