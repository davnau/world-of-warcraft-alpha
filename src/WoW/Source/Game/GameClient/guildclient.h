#pragma once

#include "WoW/Source/DB/DBClient/dbcache.h"

#include <cstdint>

using GuildTabardCallback = void (__fastcall *)(int, const std::uint64_t&, void*, unsigned char);

unsigned char GuildGetGuildTabard(unsigned guildID, GuildTabardCallback callback,
                                  int& emblemStyle, int& emblemColor,
                                  int& borderStyle, int& borderColor,
                                  int& backgroundColor);
unsigned GuildGetTabardCost();

int _E153_4();
void _E150_4();
int _E152_3();
void _E151_3();
