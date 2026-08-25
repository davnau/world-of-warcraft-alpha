#include "ENGINE/Source/Base/base.h"
#include <mutex>
namespace{std::mutex m;unsigned globalRefs=0;thread_local unsigned contextRefs=0;}
void BaseInitializeGlobal(){std::lock_guard l(m);if(globalRefs++==0){HandleInitialize();BaseFileInitialize();UnrealInitialize();ActivityResetTimes();}}
void BaseDestroyGlobal(){std::lock_guard l(m);if(globalRefs&&!--globalRefs){BaseFileDestroy();HandleDestroy();CStringManager::DestroyManager();UnrealDestroy();}}
void BaseInitializeContext(){if(contextRefs++==0)PropInitialize();}
void BaseDestroyContext(){if(contextRefs&&!--contextRefs)PropDestroy();}
