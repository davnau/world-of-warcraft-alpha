#include "WoW/Source/DB/DBClient/dbcache.h"

// These explicit instantiations retain the recovered three-parameter cache
// type and make every cache operation available to the rest of the client.
template class DBCache<CreatureStats_C, int, HASHKEY_INT>;
template class DBCache<GameObjectStats_C, int, HASHKEY_INT>;
template class DBCache<ItemStats_C, int, HASHKEY_INT>;
template class DBCache<NPCText, int, HASHKEY_INT>;
template class DBCache<NameCache, std::uint64_t, CHashKeyGUID>;
template class DBCache<GuildStats_C, int, HASHKEY_INT>;
template class DBCache<QuestCache, int, HASHKEY_INT>;
template class DBCache<PageTextCache_C, int, HASHKEY_INT>;
template class DBCache<PetNameCache, int, HASHKEY_INT>;
template class DBCache<CGPetition, int, HASHKEY_INT>;

