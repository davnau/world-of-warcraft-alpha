#include "ENGINE/Source/Base/cdatarecycler.h"
#include <algorithm>
#include <cstdlib>

CDataRecycler::CDataRecycler(unsigned maxNodes,unsigned nodesPerBlock):m_maxNodes(std::max(1u,maxNodes)),m_nodesPerBlock(std::clamp(nodesPerBlock,1u,std::max(1u,maxNodes))),m_nodeCount(0),m_nodeFullList(nullptr),m_nodeEmptyList(nullptr){}
CDataRecycler::~CDataRecycler(){Clear();}
void CDataRecycler::Link(Node*&l,Node*n){n->next=l;l=n;}
CDataRecycler::Node*CDataRecycler::Unlink(Node*&l){Node*n=l;if(n)l=n->next;return n;}
void CDataRecycler::Clear(){std::lock_guard lock(m_mutex);while(auto*n=Unlink(m_nodeFullList)){FreeData(n->data,nullptr,0);delete n;}while(auto*n=Unlink(m_nodeEmptyList))delete n;m_nodeCount=0;}
void*CDataRecycler::GetData(unsigned*bytes){std::lock_guard lock(m_mutex);Node*n=Unlink(m_nodeFullList);if(!n){if(bytes)*bytes=0;return nullptr;}void*d=n->data;if(bytes)*bytes=n->bytes;n->data=nullptr;n->bytes=0;Link(m_nodeEmptyList,n);return d;}
void CDataRecycler::PutData(int*refs,void*data,unsigned bytes,const char*file,int line){if(!data)return;if(refs&&--*refs<0){++*refs;FreeData(data,file,line);return;}std::lock_guard lock(m_mutex);Node*n=Unlink(m_nodeEmptyList);if(!n&&m_nodeCount<m_maxNodes){n=new Node{};++m_nodeCount;}if(!n){FreeData(data,file,line);return;}n->data=data;n->bytes=bytes;Link(m_nodeFullList,n);}
void*CDataRecycler::AllocData(unsigned n,const char*,int){return std::malloc(n);}
void*CDataRecycler::ReallocData(void*p,unsigned n,const char*,int){return std::realloc(p,n);}
void CDataRecycler::FreeData(void*p,const char*,int){std::free(p);}
