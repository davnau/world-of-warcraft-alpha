#pragma once
#include <cstdint>
class COsSharedMemory { public:
 enum OpenMode{SMEM_OPEN_NEW=0,SMEM_OPEN_EXISTING=1,SMEM_OPEN_ALWAYS=2,SMEM_OPEN_READONLY=3};enum Access{SMEM_ACCESS_READ=0,SMEM_ACCESS_WRITE=1};
 COsSharedMemory();~COsSharedMemory();bool Initialize(const char* name,unsigned int size,int mode);bool ChangeAccess(unsigned int access);void Destroy();
 void* m_data;unsigned int m_size;std::uint8_t m_opaqueData[4];
private:void* Mapping()const;void SetMapping(void*);
};
#if defined(_M_IX86)
static_assert(sizeof(COsSharedMemory)==12,"COsSharedMemory x86 layout mismatch");
#endif
