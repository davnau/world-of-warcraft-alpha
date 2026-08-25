#include "ENGINE/Source/Os/W32/osmemory.h"
#include <windows.h>
#include <cstring>
#include <mutex>
#include <unordered_map>
namespace { std::mutex s_mappingLock; std::unordered_map<const COsSharedMemory*,HANDLE> s_mappings; }
COsSharedMemory::COsSharedMemory():m_data(nullptr),m_size(0),m_opaqueData{}{}COsSharedMemory::~COsSharedMemory(){Destroy();}
void* COsSharedMemory::Mapping()const{std::lock_guard lock(s_mappingLock);auto i=s_mappings.find(this);return i==s_mappings.end()?nullptr:i->second;}void COsSharedMemory::SetMapping(void*p){std::lock_guard lock(s_mappingLock);if(p)s_mappings[this]=static_cast<HANDLE>(p);else s_mappings.erase(this);}
bool COsSharedMemory::Initialize(const char*name,unsigned size,int mode){Destroy();DWORD protect=mode==SMEM_OPEN_READONLY?PAGE_READONLY:PAGE_READWRITE;HANDLE map=nullptr;if(mode==SMEM_OPEN_EXISTING||mode==SMEM_OPEN_READONLY)map=OpenFileMappingA(mode==SMEM_OPEN_READONLY?FILE_MAP_READ:FILE_MAP_ALL_ACCESS,FALSE,name);else map=CreateFileMappingA(INVALID_HANDLE_VALUE,nullptr,protect,0,size,name);if(!map)return false;DWORD access=mode==SMEM_OPEN_READONLY?FILE_MAP_READ:FILE_MAP_ALL_ACCESS;m_data=MapViewOfFile(map,access,0,0,size);if(!m_data){CloseHandle(map);return false;}m_size=size;SetMapping(map);return true;}
bool COsSharedMemory::ChangeAccess(unsigned access){if(!Mapping())return false;HANDLE map=static_cast<HANDLE>(Mapping());if(m_data)UnmapViewOfFile(m_data);m_data=MapViewOfFile(map,access==SMEM_ACCESS_READ?FILE_MAP_READ:FILE_MAP_ALL_ACCESS,0,0,m_size);return m_data!=nullptr;}void COsSharedMemory::Destroy(){if(m_data)UnmapViewOfFile(m_data);if(Mapping())CloseHandle(static_cast<HANDLE>(Mapping()));SetMapping(nullptr);m_data=nullptr;m_size=0;std::memset(m_opaqueData,0,sizeof(m_opaqueData));}
