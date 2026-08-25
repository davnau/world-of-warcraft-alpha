#include "ENGINE/Source/Os/W32/osisoundcache.h"
#include <cstdio>
#include <cstring>
#include <memory>
#include <mutex>
#include <unordered_map>
namespace{std::mutex s_lock;std::unordered_map<unsigned,std::FILE*>s_files;unsigned s_next=1;int s_cacheMB=0;}
SoundFileDataCacheBlock::SoundFileDataCacheBlock():data{}{}SoundFileDataCacheBlock::~SoundFileDataCacheBlock()=default;SoundFileObjectCacheNode::SoundFileObjectCacheNode():object(nullptr){}void DataCacheInitialize(int n){s_cacheMB=n;}void DataCacheShutdown(){s_cacheMB=0;}SoundFileDataCacheBlock* AllocCacheBlock(){return new(std::nothrow)SoundFileDataCacheBlock;}
void SoundFileCache::Initialize(int n){DataCacheInitialize(n);}void SoundFileCache::Shutdown(){std::lock_guard lock(s_lock);for(auto&[h,f]:s_files)std::fclose(f);s_files.clear();DataCacheShutdown();}int __stdcall SoundFileCache::Open(const char*n){if(!n)return 0;FILE*f=nullptr;if(fopen_s(&f,n,"rb")||!f)return 0;std::lock_guard lock(s_lock);unsigned h=s_next++;s_files[h]=f;return int(h);}int __stdcall SoundFileCache::Read(char*b,unsigned n,unsigned h){std::lock_guard lock(s_lock);auto i=s_files.find(h);return i==s_files.end()||!b?-1:int(std::fread(b,1,n,i->second));}int __stdcall SoundFileCache::Seek(unsigned h,int p,char m){std::lock_guard lock(s_lock);auto i=s_files.find(h);if(i==s_files.end())return -1;int origin=m==1?SEEK_CUR:m==2?SEEK_END:SEEK_SET;return _fseeki64(i->second,p,origin);}int __stdcall SoundFileCache::Tell(unsigned h){std::lock_guard lock(s_lock);auto i=s_files.find(h);return i==s_files.end()?-1:int(_ftelli64(i->second));}void __stdcall SoundFileCache::Close(unsigned h){std::lock_guard lock(s_lock);auto i=s_files.find(h);if(i!=s_files.end()){std::fclose(i->second);s_files.erase(i);}}
