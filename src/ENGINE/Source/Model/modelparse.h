#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>
#include <type_traits>

class ModelByteReader {
public:
    explicit ModelByteReader(std::span<const std::uint8_t> bytes) : m_bytes(bytes) {}

    template <class T> bool Read(T& value)
    {
        static_assert(std::is_trivially_copyable_v<T>);
        if (Remaining() < sizeof(T)) return false;
        std::memcpy(&value, m_bytes.data() + m_pos, sizeof(T));
        m_pos += sizeof(T);
        return true;
    }

    template <class T> bool ReadArray(std::vector<T>& values, std::size_t count)
    {
        static_assert(std::is_trivially_copyable_v<T>);
        if (count > Remaining() / sizeof(T)) return false;
        values.resize(count);
        if (count) std::memcpy(values.data(), m_bytes.data() + m_pos, count * sizeof(T));
        m_pos += count * sizeof(T);
        return true;
    }

    bool Skip(std::size_t count) { if (count > Remaining()) return false; m_pos += count; return true; }
    std::size_t Remaining() const { return m_bytes.size() - m_pos; }
    std::size_t Position() const { return m_pos; }
private:
    std::span<const std::uint8_t> m_bytes;
    std::size_t m_pos = 0;
};

