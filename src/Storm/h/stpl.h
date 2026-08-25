#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>

// PDB layout: vftable, m_alloc, m_count, m_data (16 bytes on x86).
template <typename T>
class TSBaseArray {
public:
    unsigned int m_alloc;
    unsigned int m_count;
    T* m_data;

    TSBaseArray() : m_alloc(0), m_count(0), m_data(nullptr) {}
    virtual ~TSBaseArray() = default;

    unsigned int Count() const { return m_count; }
    T* Ptr() { return m_data; }
    const T* Ptr() const { return m_data; }
    T& operator[](unsigned int index) { return m_data[index]; }
    const T& operator[](unsigned int index) const { return m_data[index]; }

protected:
    virtual bool ReallocData(unsigned int count)
    {
        T* replacement = count ? new T[count]{} : nullptr;
        const unsigned int copied = (std::min)(m_count, count);
        for (unsigned int index = 0; index < copied; ++index)
            replacement[index] = std::move(m_data[index]);
        delete[] m_data;
        m_data = replacement;
        m_alloc = count;
        if (m_count > count) m_count = count;
        return count == 0 || replacement != nullptr;
    }
};

template <typename T>
class TSFixedArray : public TSBaseArray<T> {
public:
    TSFixedArray() = default;
    TSFixedArray(const TSFixedArray& other) { Set(other.m_data, other.m_count); }
    TSFixedArray& operator=(const TSFixedArray& other)
    {
        if (this != &other) Set(other.m_data, other.m_count);
        return *this;
    }
    ~TSFixedArray() override { delete[] this->m_data; this->m_data = nullptr; }

    bool SetCount(unsigned int count)
    {
        if (count > this->m_alloc && !this->ReallocData(count)) return false;
        this->m_count = count;
        return true;
    }

    bool Set(const T* source, unsigned int count)
    {
        if (!SetCount(count)) return false;
        for (unsigned int index = 0; index < count; ++index) this->m_data[index] = source[index];
        return true;
    }

    bool ReallocAndClearData(unsigned int count)
    {
        delete[] this->m_data;
        this->m_data = nullptr;
        this->m_alloc = this->m_count = 0;
        if (!this->ReallocData(count)) return false;
        this->m_count = count;
        return true;
    }
};

template <typename T>
class TSGrowableArray : public TSFixedArray<T> {
public:
    unsigned int m_chunk;

    explicit TSGrowableArray(unsigned int chunk = 16) : m_chunk(chunk ? chunk : 1) {}

    bool Reserve(unsigned int count)
    {
        if (count <= this->m_alloc) return true;
        return this->ReallocData(RoundToChunk(count));
    }

    bool SetCount(unsigned int count)
    {
        if (!Reserve(count)) return false;
        this->m_count = count;
        return true;
    }

    T* New()
    {
        if (!SetCount(this->m_count + 1)) return nullptr;
        return &this->m_data[this->m_count - 1];
    }

    bool Add(const T& value)
    {
        T* slot = New();
        if (!slot) return false;
        *slot = value;
        return true;
    }

    unsigned int CalcChunkSize(unsigned int count) const { return RoundToChunk(count); }

private:
    unsigned int RoundToChunk(unsigned int count) const
    {
        return ((count + m_chunk - 1) / m_chunk) * m_chunk;
    }
};

template <typename T, unsigned int FileTag, unsigned int LineTag>
class TSGrowableArray_ : public TSGrowableArray<T> {
public:
    TSGrowableArray_() = default;
    static const char* MemFileName() { return "recovered"; }
    static int MemLineNo() { return static_cast<int>(LineTag); }
};

template <typename T> struct TSLink;
template <typename T> struct TSLinkedNode;
template <typename T> struct TSGetLink;
template <typename T, typename LinkAccessor> class TSList;

template <typename T>
void TSSwap(T& left, T& right)
{
    T temporary = std::move(left);
    left = std::move(right);
    right = std::move(temporary);
}

#if defined(_M_IX86)
static_assert(sizeof(TSBaseArray<unsigned int>) == 16, "TSBaseArray x86 PDB layout mismatch.");
static_assert(sizeof(TSFixedArray<unsigned int>) == 16, "TSFixedArray x86 PDB layout mismatch.");
static_assert(sizeof(TSGrowableArray<unsigned int>) == 20, "TSGrowableArray x86 PDB layout mismatch.");
#endif
