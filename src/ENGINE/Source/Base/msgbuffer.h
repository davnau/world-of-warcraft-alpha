#pragma once

#include <cstdint>

class CMsgBuffer {
public:
    CMsgBuffer();
    explicit CMsgBuffer(unsigned reserve);
    CMsgBuffer(void* data,unsigned bytes,int freeData=0);
    ~CMsgBuffer();
    void ReallocData(unsigned count);
    void Clear();

    void AddChar(char); void AddUchar(std::uint8_t); void AddByte(std::uint8_t); void AddTchar(char);
    void AddTcharArray(const char*,unsigned,int zeroExtra=0); void AddTcharString(const char*,int compress=0);
    void AddShort(std::int16_t); void AddUshort(std::uint16_t); void AddWord(std::uint16_t);
    void AddInt(std::int32_t); void AddUint(std::uint32_t); void AddLong(std::int32_t); void AddUlong(std::uint32_t); void AddDword(std::uint32_t);
    void AddFloat(float); void AddLongLong(std::int64_t); void AddUlongLong(std::uint64_t);
    void AddData(const void*,unsigned); void AddData(const CMsgBuffer&);
    void AddWordArray(const std::uint16_t*,unsigned); void AddDwordArray(const std::uint32_t*,unsigned); void AddUintArray(const std::uint32_t*,unsigned); void AddFloatArray(const float*,unsigned);
    char GetChar(); std::uint8_t GetUchar(); std::uint8_t GetByte(); char GetTchar();
    void GetTcharArray(char*,unsigned,int zeroExtra=0); unsigned GetTcharStringBufferLength(int* wide=nullptr); void GetTcharString(char*,unsigned,int wide=0);
    std::int16_t GetShort(); std::uint16_t GetUshort(); std::uint16_t GetWord(); std::int32_t GetInt(); std::uint32_t GetUint();
    std::int32_t GetLong(); std::uint32_t GetUlong(); std::uint32_t GetDword(); float GetFloat(); std::int64_t GetLongLong(); std::uint64_t GetUlongLong();
    void GetData(void*,unsigned); const void* GetData(unsigned); void GetWordArray(std::uint16_t*,unsigned); void GetDwordArray(std::uint32_t*,unsigned); void GetUintArray(std::uint32_t*,unsigned); void GetFloatArray(float*,unsigned);
    unsigned Remaining() const { return m_write-m_read; }

    unsigned m_alloc; int m_freeData; unsigned m_read,m_write; std::uint8_t* m_data;
};
