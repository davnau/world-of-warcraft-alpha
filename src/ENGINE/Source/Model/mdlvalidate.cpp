#include "ENGINE/Source/Model/mdlvalidate.h"
#include <cstring>

bool MdlReadValidate(const std::uint8_t* fileBytes, std::size_t byteCount)
{
    if (!fileBytes || byteCount < 4U) return false;
    std::uint32_t magic = 0;
    std::memcpy(&magic, fileBytes, sizeof(magic));
    return magic == 0x4C444D4DU || magic == 0x3032444DU; // MDL / MD20
}
