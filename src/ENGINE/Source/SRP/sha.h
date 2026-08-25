#pragma once

#include <cstdint>

struct SHA1_CONTEXT {
    std::uint32_t state[5];
    std::uint32_t count[2];
    std::uint8_t buffer[64];
};

static_assert(sizeof(SHA1_CONTEXT) == 92, "SHA1_CONTEXT layout must match the recovered client.");

void SHA1_Init(SHA1_CONTEXT* context);
void SHA1_Update(SHA1_CONTEXT* context, const std::uint8_t* data, unsigned int length);
void SHA1_Transform(SHA1_CONTEXT* context, const std::uint8_t buffer[64]);
void SHA1_Final(std::uint8_t digest[20], SHA1_CONTEXT* context);
unsigned int SHA1_InterleaveHash(std::uint8_t output[40], const std::uint8_t* data, unsigned int length);
