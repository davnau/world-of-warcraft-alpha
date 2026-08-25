#include "ENGINE/Source/Base/filecache.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace{std::mutex m;std::unordered_map<std::string,std::vector<unsigned char>> cache;std::unordered_set<std::string> noCache;bool Read(const char*f,std::vector<unsigned char>&v){std::ifstream s(f,std::ios::binary|std::ios::ate);if(!s)return false;auto n=s.tellg();if(n<0)return false;v.resize(static_cast<size_t>(n));s.seekg(0);return v.empty()||static_cast<bool>(s.read(reinterpret_cast<char*>(v.data()),n));}}
void BaseFileInitialize(){std::lock_guard l(m);cache.clear();noCache.clear();}void BaseFileDestroy(){BaseFileFlush();std::lock_guard l(m);noCache.clear();}
int BaseFilePrefetch(const char*f){if(!f)return 0;std::vector<unsigned char>v;if(!Read(f,v))return 0;std::lock_guard l(m);if(!noCache.contains(f))cache[f]=std::move(v);return 1;}
int BaseFileIsFetched(const char*f){std::lock_guard l(m);return f&&cache.contains(f);}
int BaseFileLoad(const char*f,void**out,unsigned*size){if(!f||!out||!size)return 0;std::vector<unsigned char>v;{std::lock_guard l(m);auto i=cache.find(f);if(i!=cache.end())v=i->second;}if(v.empty()&&!Read(f,v))return 0;void*p=std::malloc(v.size()?v.size():1);if(!p)return 0;if(!v.empty())std::memcpy(p,v.data(),v.size());*out=p;*size=static_cast<unsigned>(v.size());return 1;}
void BaseFileFlush(){std::lock_guard l(m);cache.clear();}void BaseFileRegisterUncachable(const char*f){if(!f)return;std::lock_guard l(m);noCache.insert(f);cache.erase(f);}void BaseFileUnregisterUncachable(const char*f){if(!f)return;std::lock_guard l(m);noCache.erase(f);}void BaseFileDumpStats(HSLOG__*){}
int IBaseFileLoad(const char*f,void**p,unsigned*n){return BaseFileLoad(f,p,n);}int IBaseFileStartLoad(const char*f){return BaseFilePrefetch(f);}void IBaseFileWaitForLoad(const char*){}void IBaseFileUnload(const char*f){if(!f)return;std::lock_guard l(m);cache.erase(f);}
