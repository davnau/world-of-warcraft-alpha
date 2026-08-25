#pragma once

#include <cstdint>

int ToggleCollisionInfo();
void RenderCollisionInfo();
bool CollisionInfoEnabled();
std::uint64_t CollisionInfoWatchGUID();
void CollisionInfoSetWatchGUID(std::uint64_t guid);
void CollisionInfoReset();
