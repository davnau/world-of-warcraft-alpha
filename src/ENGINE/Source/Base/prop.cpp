#include "ENGINE/Source/Base/prop.h"
#include <mutex>
#include <unordered_map>
#include <unordered_set>

struct HPROPCONTEXT__{std::unordered_map<unsigned,void*> values;};
namespace{std::mutex m;std::unordered_set<HPROPCONTEXT> contexts;thread_local HPROPCONTEXT selected=nullptr;}
void PropInitialize(){if(!selected)selected=PropCreateContext();}
void PropDestroy(){std::lock_guard l(m);for(auto*p:contexts)delete p;contexts.clear();selected=nullptr;}
HPROPCONTEXT PropCreateContext(){auto*p=new HPROPCONTEXT__;std::lock_guard l(m);contexts.insert(p);return p;}
void PropSelectContext(HPROPCONTEXT p){selected=p;}
HPROPCONTEXT PropGetSelectedContext(){return selected;}
void PropDeleteContext(HPROPCONTEXT p){if(!p)return;std::lock_guard l(m);if(contexts.erase(p)){if(selected==p)selected=nullptr;delete p;}}
void*PropGet(unsigned id){if(!selected)return nullptr;auto i=selected->values.find(id);return i==selected->values.end()?nullptr:i->second;}
void PropSet(unsigned id,void*v){if(!selected)PropInitialize();selected->values[id]=v;}
