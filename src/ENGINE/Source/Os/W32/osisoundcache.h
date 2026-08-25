#pragma once
#include <cstdint>
class SoundFileCache{public:static void Initialize(int cacheSizeMB);static void Shutdown();static int __stdcall Open(const char* filename);static int __stdcall Read(char* buffer,unsigned int size,unsigned int handle);static int __stdcall Seek(unsigned int handle,int position,char mode);static int __stdcall Tell(unsigned int handle);static void __stdcall Close(unsigned int handle);};
struct SoundFileDataCacheBlock{SoundFileDataCacheBlock();~SoundFileDataCacheBlock();std::uint8_t data[4096];};struct SoundFileObjectCacheNode{SoundFileObjectCacheNode();void* object;};void DataCacheInitialize(int cacheSizeMB);void DataCacheShutdown();SoundFileDataCacheBlock* AllocCacheBlock();
