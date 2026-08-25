#include "ENGINE/Source/Tempest/cmemblock.h"
#include <algorithm>
#include <cstring>
#include <utility>
namespace NTempest {
void CMemBlock::Set32b_(void*c,unsigned char v,unsigned int n){std::memset(c,v,n);}void CMemBlock::SetM_(void*c,unsigned char v,unsigned int n){std::memset(c,v,n);}
CMemBlock::CMemBlock(unsigned int n,const char*f,int l){Constructor_(n,f,l);}void CMemBlock::Constructor_(unsigned int n,const char*f,int l){mem_=mem=nullptr;size_=size=0;filen_=f;linen_=l;Resize(n,false);}CMemBlock::~CMemBlock(){Destructor_();}void CMemBlock::Destructor_(){delete[] mem_;mem_=mem=nullptr;size_=size=0;}
CMemBlock::CMemBlock(const CMemBlock&o){Constructor_(o.size_,o.filen_,o.linen_);Copy_(o);}CMemBlock& CMemBlock::operator=(const CMemBlock&o){if(this!=&o){Resize(o.size_,false);Copy_(o);filen_=o.filen_;linen_=o.linen_;}return *this;}
int CMemBlock::Compare(const CMemBlock&o)const{return Compare_(o);}int CMemBlock::Compare_(const CMemBlock&o)const{unsigned n=std::min(size_,o.size_);int r=n?std::memcmp(mem_,o.mem_,n):0;return r?r:(size_<o.size_?-1:size_>o.size_?1:0);}std::size_t CMemBlock::Copy(const CMemBlock&o){return Copy_(o);}std::size_t CMemBlock::Copy_(const CMemBlock&o){unsigned n=std::min(size_,o.size_);if(n)std::memcpy(mem_,o.mem_,n);return n;}
bool CMemBlock::Swap(CMemBlock&o){std::swap(mem_,o.mem_);std::swap(size_,o.size_);std::swap(mem,o.mem);std::swap(size,o.size);std::swap(filen_,o.filen_);std::swap(linen_,o.linen_);return true;}bool CMemBlock::Resize(unsigned n,bool preserve){char*p=n?new(std::nothrow)char[n]:nullptr;if(n&&!p)return false;if(p){if(preserve&&mem_)std::memcpy(p,mem_,std::min(n,size_));if(n>size_)std::memset(p+size_,0,n-size_);}delete[] mem_;mem_=mem=p;size_=size=n;return true;}
void CMemBlock::Detach(char**p,unsigned*n){if(p)*p=mem_;if(n)*n=size_;mem_=mem=nullptr;size_=size=0;}void CMemBlock::Attach(char*p,unsigned n){Destructor_();mem_=mem=p;size_=size=n;}void CMemBlock::Detach_(char**p,unsigned*n){Detach(p,n);}void CMemBlock::Attach_(char*p,unsigned n){Attach(p,n);}const char* CMemBlock::FileN_()const{return filen_;}int CMemBlock::LineN_()const{return linen_;}void CMemBlock::SetFileN_(const char*f){filen_=f;}void CMemBlock::SetLineN_(int l){linen_=l;}
}
