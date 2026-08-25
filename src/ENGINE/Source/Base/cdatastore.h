#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

class CDataStore {
public:
    CDataStore();
    CDataStore(void* data, unsigned size);
    ~CDataStore();
    CDataStore(const CDataStore&) = delete;
    CDataStore& operator=(const CDataStore&) = delete;

    void InternalInitialize(void* data = nullptr, unsigned size = 0, unsigned alloc = 0);
    void InternalDestroy();
    void Initialize() { InternalInitialize(); }
    int FetchWrite(unsigned position, unsigned bytes, const char* fileName = nullptr, int lineNumber = 0);
    int InternalFetchRead(unsigned bytes, void** data);
    int InternalFetchWrite(unsigned bytes, void** data);

    template<class T> CDataStore* Set(unsigned pos, T value) { static_assert(std::is_trivially_copyable_v<T>); unsigned save=m_write; m_write=pos; Put(value); m_write=save; return this; }
    template<class T> CDataStore* Put(T value) { void* p=nullptr; if(InternalFetchWrite(sizeof(T),&p)) std::memcpy(p,&value,sizeof(T)); return this; }
    template<class T> CDataStore* Get(T* value) { void* p=nullptr; if(value && InternalFetchRead(sizeof(T),&p)) std::memcpy(value,p,sizeof(T)); return this; }
    template<class T> CDataStore* PutArray(const T* values,unsigned count) { return PutData(values,count*sizeof(T)); }
    template<class T> CDataStore* GetArray(T* values,unsigned count) { return GetData(values,count*sizeof(T)); }

    CDataStore* PutString(const char* value);
    CDataStore* PutString(const std::uint16_t* value);
    CDataStore* GetString(char* value,unsigned maxChars);
    CDataStore* GetString(std::uint16_t* value,unsigned maxChars);
    CDataStore* PutData(const void* value,unsigned bytes);
    CDataStore* GetData(void* value,unsigned bytes);
    CDataStore* GetDataInSitu(void** value,unsigned bytes);
    void GetBufferParams(void** data,unsigned* size,unsigned* alloc) const;
    void DetachBuffer(void** data,unsigned* size,unsigned* alloc);
    void ResetRead() { m_read=0; m_failed=false; }
    int IsRead() const { return m_read == m_size; }
    void Reset();
    void Finalize() { m_read = 0; }
    bool Failed() const { return m_failed; }

    unsigned char* m_data;
    unsigned m_alloc;
    unsigned m_size;
    unsigned m_read;
    unsigned m_write;
    int m_freeData;
    bool m_failed;
};
