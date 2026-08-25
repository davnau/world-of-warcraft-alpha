#pragma once

#include "ENGINE/Source/Base/cdatastore.h"

#include <cstdint>

void NPC_C_OfferItem(std::uint64_t npc, std::uint64_t itemGUID);
void NPC_C_Initialize();
int NPCResponseHandler(unsigned int eventTime, CDataStore* msg);
void NPC_C_Destroy();
std::uint64_t NPC_C_GetActiveGUID();
