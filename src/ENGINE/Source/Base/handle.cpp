#include "ENGINE/Source/Base/handle.h"
#include <mutex>
#include <vector>

namespace {std::mutex m;std::vector<CHandleObject> objects(1);}
void HandleInitialize(){std::lock_guard l(m);objects.assign(1,{});}
void HandleDestroy(){std::lock_guard l(m);objects.assign(1,{});}
HOBJECT HandleCreate(void*p){std::lock_guard l(m);for(std::uint32_t i=1;i<objects.size();++i)if(!objects[i].references){auto&x=objects[i];x.object=p;x.references=1;++x.generation;return (x.generation<<16)|i;}objects.push_back({p,1,1});return (1u<<16)|static_cast<HOBJECT>(objects.size()-1);}
CHandleObject*HandleDereference(HOBJECT h){std::lock_guard l(m);unsigned i=h&0xffff,g=h>>16;if(i>=objects.size()||!objects[i].references||objects[i].generation!=g)return nullptr;return &objects[i];}
CHandleObject*HandleDuplicate(HOBJECT h){std::lock_guard l(m);unsigned i=h&0xffff,g=h>>16;if(i>=objects.size()||!objects[i].references||objects[i].generation!=g)return nullptr;++objects[i].references;return &objects[i];}
void HandleClose(HOBJECT h){std::lock_guard l(m);unsigned i=h&0xffff,g=h>>16;if(i<objects.size()&&objects[i].references&&objects[i].generation==g&&!--objects[i].references)objects[i].object=nullptr;}
int HandleObjectCompare(void*p,HOBJECT h){std::lock_guard l(m);unsigned i=h&0xffff,g=h>>16;return i<objects.size()&&objects[i].references&&objects[i].generation==g&&objects[i].object==p;}
