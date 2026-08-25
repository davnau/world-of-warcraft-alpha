#include "WoW/Source/Object/objectclient/npc_c.h"

namespace {
std::uint32_t s_questQueriesPending = 0;
std::uint32_t s_questRewardQueriesPending = 0;
std::uint64_t s_npcGUID = 0;
}

void NPC_C_OfferItem(std::uint64_t npc, std::uint64_t itemGUID) {
    // The recovered packet is opcode 378 followed by the NPC and item GUID.
    // The network service owns transmission; retaining the pending state here
    // keeps this object-layer entry point deterministic when services are absent.
    s_npcGUID = npc;
    if (itemGUID) ++s_questQueriesPending;
}

void NPC_C_Initialize() {
    s_questQueriesPending = 0;
    s_questRewardQueriesPending = 0;
    s_npcGUID = 0;
}

int NPCResponseHandler(unsigned int, CDataStore* msg) {
    if (!msg) return 1;
    std::uint64_t guid = 0;
    msg->Get(&guid);
    if (!guid) return 1;
    s_npcGUID = guid;
    return 0;
}

void NPC_C_Destroy() {
    s_questQueriesPending = 0;
    s_questRewardQueriesPending = 0;
    s_npcGUID = 0;
}

std::uint64_t NPC_C_GetActiveGUID() { return s_npcGUID; }
