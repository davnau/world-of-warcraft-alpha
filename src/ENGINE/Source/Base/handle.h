#pragma once

#include <cstdint>

struct CHandleObject { void* object; unsigned references; std::uint32_t generation; };
using HOBJECT = std::uint32_t;
void HandleInitialize(); void HandleDestroy();
HOBJECT HandleCreate(void* object); void HandleClose(HOBJECT handle);
CHandleObject* HandleDereference(HOBJECT handle); CHandleObject* HandleDuplicate(HOBJECT handle);
int HandleObjectCompare(void* object,HOBJECT handle);
