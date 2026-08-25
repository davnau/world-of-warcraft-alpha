#include "ENGINE/Source/SRP/srp6.h"
#include "ENGINE/Source/SRP/sha.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>

SRP6_Random::SRP6_Random(unsigned int seed) : m_randkey1{}, m_randkey2{}, m_randpool{}, m_inpool(0) {
    struct Preseed { std::int32_t ltime; unsigned int msec; std::int32_t random; } preseed{};
    preseed.ltime=static_cast<std::int32_t>(std::time(nullptr));preseed.msec=seed;std::srand(seed);preseed.random=std::rand();
    SHA1_CONTEXT context;SHA1_Init(&context);SHA1_Update(&context,reinterpret_cast<const std::uint8_t*>(&preseed),sizeof(preseed));SHA1_Final(m_randkey1,&context);std::memcpy(m_randkey2,m_randkey1,sizeof(m_randkey2));
}

void SRP6_Random::GenerateRandomBytes(std::uint8_t* data, unsigned int size) {
    if (!data) return;
    while(size){if(!m_inpool){SHA1_CONTEXT context;SHA1_Init(&context);SHA1_Update(&context,m_randkey1,20);SHA1_Update(&context,m_randpool,20);SHA1_Update(&context,m_randkey2,20);SHA1_Final(m_randpool,&context);m_inpool=20;}
        unsigned int take=std::min(size,m_inpool),offset=20-m_inpool;std::memcpy(data,m_randpool+offset,take);data+=take;size-=take;m_inpool-=take;}
}
