#include "ENGINE/Source/Images/dxt.h"

DxtColorBlock::Tables DxtColorBlock::tables{};

DxtColorBlock::Tables::Tables()
{
    for (unsigned i = 0; i < 32; ++i) {
        dt135[i] = static_cast<std::uint16_t>((i << 8) / 3u + 1u);
        dt235[i] = static_cast<std::uint16_t>((i * 512u) / 3u + 1u);
    }
    for (unsigned i = 0; i < 64; ++i) {
        dt136[i] = static_cast<std::uint16_t>((i << 8) / 3u + 1u);
        dt236[i] = static_cast<std::uint16_t>((i * 512u) / 3u + 1u);
    }
}

