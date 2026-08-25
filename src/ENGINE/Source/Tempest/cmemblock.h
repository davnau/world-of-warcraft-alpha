#pragma once
#include <cstddef>
namespace NTempest {
class CEntity { public: virtual ~CEntity()=default; };
class CMemBlock : public CEntity { public:
 char* mem_; unsigned int size_; char* mem; unsigned int size; const char* filen_; int linen_;
 CMemBlock(unsigned int size=0,const char* file=nullptr,int line=0); CMemBlock(const CMemBlock&); CMemBlock& operator=(const CMemBlock&); ~CMemBlock() override;
 void Constructor_(unsigned int,const char*,int); void Destructor_(); int Compare(const CMemBlock&)const; int Compare_(const CMemBlock&)const; std::size_t Copy(const CMemBlock&); std::size_t Copy_(const CMemBlock&); bool Swap(CMemBlock&); bool Resize(unsigned int,bool=true); void Detach(char**,unsigned int*); void Attach(char*,unsigned int); void Detach_(char**,unsigned int*); void Attach_(char*,unsigned int); const char* FileN_()const; int LineN_()const; void SetFileN_(const char*); void SetLineN_(int);
 static void Set32b_(void*,unsigned char,unsigned int); static void SetM_(void*,unsigned char,unsigned int);
};
}
