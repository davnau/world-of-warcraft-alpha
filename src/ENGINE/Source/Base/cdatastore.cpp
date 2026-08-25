#include "ENGINE/Source/Base/cdatastore.h"
#include "ENGINE/Source/Base/convertutf.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

CDataStore::CDataStore(){InternalInitialize();}
CDataStore::CDataStore(void* data,unsigned size){InternalInitialize(data,size,size);m_freeData=0;}
CDataStore::~CDataStore(){InternalDestroy();}
void CDataStore::InternalInitialize(void* data,unsigned size,unsigned alloc){m_data=static_cast<unsigned char*>(data);m_alloc=alloc;m_size=size;m_read=0;m_write=size;m_freeData=data?0:1;m_failed=false;}
void CDataStore::InternalDestroy(){if(m_freeData)std::free(m_data);InternalInitialize();}
int CDataStore::FetchWrite(unsigned position,unsigned bytes,const char*,int){void*p=nullptr;if(position>m_write)m_write=position;return InternalFetchWrite(bytes,&p);}
void CDataStore::Reset(){m_size=0;m_read=static_cast<unsigned>(-1);m_write=0;m_failed=false;}
int CDataStore::InternalFetchRead(unsigned bytes,void** out){if(bytes>m_size-m_read){m_failed=true;if(out)*out=nullptr;return 0;}if(out)*out=m_data+m_read;m_read+=bytes;return 1;}
int CDataStore::InternalFetchWrite(unsigned bytes,void** out){if(bytes>~m_write){m_failed=true;return 0;}unsigned need=m_write+bytes;if(need>m_alloc){unsigned cap=std::max(need,std::max(64u,m_alloc+m_alloc/2));auto* p=static_cast<unsigned char*>(std::malloc(cap));if(!p){m_failed=true;return 0;}if(m_data&&m_size)std::memcpy(p,m_data,m_size);if(m_freeData)std::free(m_data);m_data=p;m_alloc=cap;m_freeData=1;}if(out)*out=m_data+m_write;m_write=need;m_size=std::max(m_size,m_write);return 1;}
CDataStore* CDataStore::PutData(const void* v,unsigned n){void*p=nullptr;if(n&&v&&InternalFetchWrite(n,&p))std::memcpy(p,v,n);else if(n&&(!v))m_failed=true;return this;}
CDataStore* CDataStore::GetData(void* v,unsigned n){void*p=nullptr;if(n&&v&&InternalFetchRead(n,&p))std::memcpy(v,p,n);else if(n&&!v)m_failed=true;return this;}
CDataStore* CDataStore::GetDataInSitu(void**v,unsigned n){void*p=nullptr;if(InternalFetchRead(n,&p)&&v)*v=p;else if(v)*v=nullptr;return this;}
CDataStore* CDataStore::PutString(const char*s){if(!s){m_failed=true;return this;}return PutData(s,static_cast<unsigned>(std::strlen(s))+1);}
CDataStore* CDataStore::PutString(const std::uint16_t*s){if(!s){m_failed=true;return this;}unsigned chars=0;while(s[chars])++chars;int bytes=ConvertUTF16toUTF8Length(s,chars,nullptr);if(bytes<0){m_failed=true;return this;}char*tmp=static_cast<char*>(std::malloc(static_cast<unsigned>(bytes)+1));unsigned written=0;ConvertUTF16toUTF8(tmp,static_cast<unsigned>(bytes)+1,s,chars,&written,nullptr);tmp[written]=0;PutData(tmp,written+1);std::free(tmp);return this;}
CDataStore* CDataStore::GetString(char*s,unsigned cap){if(!s||!cap){if(cap)m_failed=true;return this;}unsigned n=0;while(m_read+n<m_size&&m_data[m_read+n])++n;if(m_read+n>=m_size){s[0]=0;m_read=m_size+1;m_failed=true;return this;}unsigned take=std::min(n,cap-1);std::memcpy(s,m_data+m_read,take);s[take]=0;m_read+=n+1;if(n>=cap)m_failed=true;return this;}
CDataStore* CDataStore::GetString(std::uint16_t*s,unsigned cap){if(!s||!cap){if(cap)m_failed=true;return this;}unsigned n=0;while(m_read+n<m_size&&m_data[m_read+n])++n;if(m_read+n>=m_size){s[0]=0;m_read=m_size+1;m_failed=true;return this;}unsigned written=0,consumed=0;ConvertUTF8toUTF16(s,cap,m_data+m_read,n,&written,&consumed);m_read+=n+1;if(consumed<n)m_failed=true;return this;}
void CDataStore::GetBufferParams(void**d,unsigned*s,unsigned*a) const{if(d)*d=m_data;if(s)*s=m_size;if(a)*a=m_alloc;}
void CDataStore::DetachBuffer(void**d,unsigned*s,unsigned*a){GetBufferParams(d,s,a);m_data=nullptr;m_alloc=m_size=m_read=m_write=0;m_freeData=1;m_failed=false;}
