#include "ENGINE/Source/Base/msgbuffer.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace { template<class T> void Add(CMsgBuffer&b,T v){b.AddData(&v,sizeof(v));} template<class T>T Get(CMsgBuffer&b){T v{};b.GetData(&v,sizeof(v));return v;} }
CMsgBuffer::CMsgBuffer():m_alloc(0),m_freeData(1),m_read(0),m_write(0),m_data(nullptr){}
CMsgBuffer::CMsgBuffer(unsigned n):CMsgBuffer(){ReallocData(n);}
CMsgBuffer::CMsgBuffer(void*d,unsigned n,int f):m_alloc(n),m_freeData(f),m_read(0),m_write(n),m_data(static_cast<std::uint8_t*>(d)){}
CMsgBuffer::~CMsgBuffer(){if(m_freeData)std::free(m_data);}
void CMsgBuffer::Clear(){m_read=m_write=0;}
void CMsgBuffer::ReallocData(unsigned n){if(n<=m_alloc)return;unsigned cap=std::max(n,std::max(64u,m_alloc+m_alloc/2));auto*p=static_cast<std::uint8_t*>(std::malloc(cap));if(!p)throw std::bad_alloc();if(m_data&&m_write)std::memcpy(p,m_data,m_write);if(m_freeData)std::free(m_data);m_data=p;m_alloc=cap;m_freeData=1;}
void CMsgBuffer::AddData(const void*p,unsigned n){if(!p&&n)throw std::invalid_argument("data");ReallocData(m_write+n);if(n)std::memcpy(m_data+m_write,p,n);m_write+=n;}
void CMsgBuffer::AddData(const CMsgBuffer&b){AddData(b.m_data+b.m_read,b.m_write-b.m_read);}
const void*CMsgBuffer::GetData(unsigned n){if(n>Remaining())throw std::out_of_range("message buffer");const void*p=m_data+m_read;m_read+=n;return p;}
void CMsgBuffer::GetData(void*p,unsigned n){const void*s=GetData(n);if(n)std::memcpy(p,s,n);}
#define ADDGET(Name,Type) void CMsgBuffer::Add##Name(Type v){Add(*this,v);} Type CMsgBuffer::Get##Name(){return Get<Type>(*this);}
ADDGET(Char,char) ADDGET(Uchar,std::uint8_t) ADDGET(Byte,std::uint8_t) ADDGET(Tchar,char) ADDGET(Short,std::int16_t) ADDGET(Ushort,std::uint16_t) ADDGET(Word,std::uint16_t) ADDGET(Int,std::int32_t) ADDGET(Uint,std::uint32_t) ADDGET(Long,std::int32_t) ADDGET(Ulong,std::uint32_t) ADDGET(Dword,std::uint32_t) ADDGET(Float,float) ADDGET(LongLong,std::int64_t) ADDGET(UlongLong,std::uint64_t)
#undef ADDGET
void CMsgBuffer::AddTcharArray(const char*s,unsigned n,int z){if(!s&&n)throw std::invalid_argument("string");ReallocData(m_write+n);for(unsigned i=0;i<n;++i){char c=*s++;m_data[m_write++]=static_cast<std::uint8_t>(c);if(z&&c==0){std::memset(m_data+m_write,0,n-i-1);m_write+=n-i-1;break;}}}
void CMsgBuffer::AddTcharString(const char*s,int compress){unsigned n=s?std::min(0x3fffu,static_cast<unsigned>(std::strlen(s))):0;std::uint8_t prefix=static_cast<std::uint8_t>(n&0x3f);if(n>0x3f)prefix|=0x40;if(!compress)prefix|=0x80;AddByte(prefix);if(prefix&0x40)AddByte(static_cast<std::uint8_t>(n>>8));AddTcharArray(s,n,0);}
void CMsgBuffer::GetTcharArray(char*s,unsigned n,int z){GetData(s,n);if(z&&n)s[n-1]=0;}
unsigned CMsgBuffer::GetTcharStringBufferLength(int*wide){std::uint8_t prefix=GetByte();unsigned n=prefix&0x3f;if(prefix&0x40)n|=static_cast<unsigned>(GetByte())<<8;if(wide)*wide=(prefix&0x80)!=0;return n+1;}
void CMsgBuffer::GetTcharString(char*s,unsigned cap,int){if(!s||!cap)return;GetTcharArray(s,cap-1);s[cap-1]=0;}
#define ARRAY(Name,Type) void CMsgBuffer::Add##Name##Array(const Type*p,unsigned n){AddData(p,n*sizeof(Type));} void CMsgBuffer::Get##Name##Array(Type*p,unsigned n){GetData(p,n*sizeof(Type));}
ARRAY(Word,std::uint16_t) ARRAY(Dword,std::uint32_t) ARRAY(Uint,std::uint32_t) ARRAY(Float,float)
#undef ARRAY
