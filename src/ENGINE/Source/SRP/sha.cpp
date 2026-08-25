#include "ENGINE/Source/SRP/sha.h"

#include <algorithm>
#include <cstring>
#include <vector>

namespace {
std::uint32_t RotateLeft(std::uint32_t value, unsigned int bits) {
    return (value << bits) | (value >> (32U - bits));
}
}

void SHA1_Init(SHA1_CONTEXT* context) {
    if (!context) return;
    context->state[0] = 0x67452301U;
    context->state[1] = 0xEFCDAB89U;
    context->state[2] = 0x98BADCFEU;
    context->state[3] = 0x10325476U;
    context->state[4] = 0xC3D2E1F0U;
    context->count[0] = context->count[1] = 0;
    std::memset(context->buffer, 0, sizeof(context->buffer));
}

void SHA1_Transform(SHA1_CONTEXT* context, const std::uint8_t buffer[64]) {
    std::uint32_t words[80]{};
    for (unsigned int i = 0; i < 16; ++i)
        words[i] = (std::uint32_t(buffer[i*4]) << 24) | (std::uint32_t(buffer[i*4+1]) << 16) |
            (std::uint32_t(buffer[i*4+2]) << 8) | buffer[i*4+3];
    for (unsigned int i = 16; i < 80; ++i) words[i] = RotateLeft(words[i-3] ^ words[i-8] ^ words[i-14] ^ words[i-16], 1);
    std::uint32_t a=context->state[0],b=context->state[1],c=context->state[2],d=context->state[3],e=context->state[4];
    for (unsigned int i=0;i<80;++i) {
        std::uint32_t f,k;
        if(i<20){f=(b&c)|(~b&d);k=0x5A827999U;}else if(i<40){f=b^c^d;k=0x6ED9EBA1U;}
        else if(i<60){f=(b&c)|(b&d)|(c&d);k=0x8F1BBCDCU;}else{f=b^c^d;k=0xCA62C1D6U;}
        std::uint32_t next=RotateLeft(a,5)+f+e+k+words[i];e=d;d=c;c=RotateLeft(b,30);b=a;a=next;
    }
    context->state[0]+=a;context->state[1]+=b;context->state[2]+=c;context->state[3]+=d;context->state[4]+=e;
}

void SHA1_Update(SHA1_CONTEXT* context, const std::uint8_t* data, unsigned int length) {
    if (!context || (!data && length)) return;
    unsigned int index=(context->count[0]>>3)&63U;
    std::uint32_t bits=length<<3;context->count[0]+=bits;if(context->count[0]<bits)++context->count[1];context->count[1]+=length>>29;
    unsigned int part=64-index,offset=0;
    if(length>=part){std::memcpy(context->buffer+index,data,part);SHA1_Transform(context,context->buffer);offset=part;while(offset+63<length){SHA1_Transform(context,data+offset);offset+=64;}index=0;}
    if(offset<length)std::memcpy(context->buffer+index,data+offset,length-offset);
}

void SHA1_Final(std::uint8_t digest[20], SHA1_CONTEXT* context) {
    if (!digest || !context) return;
    std::uint8_t count[8];for(unsigned i=0;i<8;++i)count[i]=std::uint8_t(context->count[i<4?1:0]>>(24-8*(i&3)));
    static constexpr std::uint8_t one=0x80,zero=0;SHA1_Update(context,&one,1);while((context->count[0]&504U)!=448U)SHA1_Update(context,&zero,1);SHA1_Update(context,count,8);
    for(unsigned i=0;i<20;++i)digest[i]=std::uint8_t(context->state[i>>2]>>(24-8*(i&3)));
}

unsigned int SHA1_InterleaveHash(std::uint8_t output[40], const std::uint8_t* data, unsigned int length) {
    if (!output || (!data && length)) return 0;
    while(length && *data==0){++data;--length;}if(length&1U){++data;--length;}
    const unsigned int half=length/2;std::vector<std::uint8_t> lane(half);std::uint8_t digest[20];SHA1_CONTEXT context;
    for(unsigned parity=0;parity<2;++parity){for(unsigned i=0;i<half;++i)lane[i]=data[i*2+parity];SHA1_Init(&context);SHA1_Update(&context,lane.data(),half);SHA1_Final(digest,&context);for(unsigned i=0;i<20;++i)output[i*2+parity]=digest[i];}
    return 40;
}
