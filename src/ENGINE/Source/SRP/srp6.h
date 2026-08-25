#pragma once
#include <cstdint>
class SRP6_Random {
public:
    explicit SRP6_Random(unsigned int seed);
    void GenerateRandomBytes(std::uint8_t* data, unsigned int size);
    std::uint8_t m_randkey1[20];
    std::uint8_t m_randkey2[20];
    std::uint8_t m_randpool[20];
    unsigned int m_inpool;
};
static_assert(sizeof(SRP6_Random) == 64, "SRP6_Random layout must match the recovered client.");
