#include "WoW/common/compdecorationcore.h"

#include "ENGINE/Source/Services/sysmessage.h"
#include "ENGINE/Source/Services/texture.h"
#include "Storm/h/storm.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <limits>

ChrRacesDB g_chrRacesDB;
const char* s_sexSuffixNames[3] = {"Male", "Female", ""};
const char* s_sectionDirectorynames[NUM_TEXCOMPONENT_SECTIONS] = {
    "UpperArm", "LowerArm", "Hand", "UpperHead", "LowerHead",
    "UpperTorso", "LowerTorso", "LegUpper", "LegLower", "Feet",
};
const char* s_texComponentBasePath = "Item\\TextureComponents\\";

float INFINITY_255 = 0.0F;
float TWO_PI_252 = 0.0F;
float OO_TWO_PI_252 = 0.0F;

namespace {

void CopyString(char* destination, const char* source, unsigned int size) {
    if (destination == nullptr || size == 0) return;
    if (source == nullptr) source = "";
    std::snprintf(destination, size, "%s", source);
}

void AppendString(char* destination, const char* source, unsigned int size) {
    if (destination == nullptr || source == nullptr || size == 0) return;
    const std::size_t used = std::strlen(destination);
    if (used >= size - 1) return;
    std::snprintf(destination + used, size - used, "%s", source);
}

void FormatString(char* destination, unsigned int size, const char* format, ...) {
    if (destination == nullptr || size == 0) return;
    std::va_list args;
    va_start(args, format);
    std::vsnprintf(destination, size, format, args);
    va_end(args);
}

const char* Extension(const char* fileName) {
    return fileName == nullptr ? nullptr : std::strrchr(fileName, '.');
}

void CopyExtensionAndStrip(char* fileName, char* extension, unsigned int extensionSize) {
    if (fileName == nullptr || extension == nullptr || extensionSize == 0) return;
    extension[0] = '\0';
    if (char* dot = std::strrchr(fileName, '.'); dot != nullptr) {
        CopyString(extension, dot, extensionSize);
        *dot = '\0';
    }
}

} // namespace

int _E1_255() {
    INFINITY_255 = std::numeric_limits<float>::infinity();
    return 0x7F800000;
}

int _E2_255() {
    return _E1_255();
}

void __fastcall CompDecorateObjName(const char* fileName, char* buffer, unsigned int size,
                         unsigned int race, unsigned int sex) {
    if (buffer == nullptr || size == 0) return;
    buffer[0] = '\0';
    if (fileName == nullptr || *fileName == '\0') return;

    char finalName[260]{};
    BuildObjComponentPath(fileName, race, sex, finalName, sizeof(finalName));
    if (finalName[0] == '\0') {
        BuildObjComponentPath(fileName, race, UNITSEX_NONE, finalName, sizeof(finalName));
    }
    if (finalName[0] != '\0') {
        CopyString(buffer, finalName, size);
    } else {
        SysMsgPrintf(SYSMSG_ERROR, 0, "Error, object component file \"%s\" not found!", fileName);
    }
}

void __fastcall BuildObjComponentPath(const char* fileName, unsigned int race, unsigned int sex,
                           char* buffer, unsigned int size) {
    if (buffer == nullptr || size == 0) return;
    buffer[0] = '\0';
    if (fileName == nullptr || race == 0 || race > g_chrRacesDB.m_maxID) return;

    char inputFile[260]{};
    char extension[12]{};
    CopyString(inputFile, fileName, sizeof(inputFile));
    CopyExtensionAndStrip(inputFile, extension, sizeof(extension));

    char suffix[12]{};
    ConstructSuffixString(static_cast<int>(race), static_cast<int>(sex), 1, 1,
                          suffix, sizeof(suffix));
    char finalBuffer[260]{};
    FormatString(finalBuffer, sizeof(finalBuffer), "%s%s%s", inputFile, suffix, extension);
    CopyString(buffer, finalBuffer, size);
}

void __fastcall ConstructSuffixString(int race, int sex, int includeRace, int includeSex,
                           char* buffer, unsigned int size) {
    if (buffer == nullptr || size == 0) return;
    buffer[0] = '\0';
    if (!includeRace && !includeSex) return;
    if (includeRace && !includeSex) return;
    if (sex < 0 || sex >= 3) return;

    const char* suffix = s_sexSuffixNames[sex] != nullptr ? s_sexSuffixNames[sex] : "";
    if (includeRace) {
        const ChrRacesRec* record = race >= 0 ? g_chrRacesDB.Record(static_cast<unsigned int>(race)) : nullptr;
        if (record == nullptr || record->m_ClientPrefix == nullptr) return;
        FormatString(buffer, size, "_%s%s", record->m_ClientPrefix, suffix);
    } else if (includeSex) {
        FormatString(buffer, size, "_%s", suffix);
    }
}

int __fastcall CompDecorateUndecorateObjName(const char* fileName, char* buffer, unsigned int size) {
    if (buffer == nullptr || size == 0) return 0;
    buffer[0] = '\0';
    if (fileName == nullptr || *fileName == '\0') return 0;

    CopyString(buffer, fileName, size);
    const std::size_t length = std::strlen(buffer);
    if (length == 0) return 0;

    char extension[12]{};
    char fileNameBuffer[260]{};
    int state = 0;
    char* cursor = buffer + length - 1;
    while (cursor >= buffer) {
        if (state == 0) {
            if (*cursor == '.') {
                CopyString(extension, cursor, sizeof(extension));
                *cursor = '\0';
                state = 1;
            }
        } else if (state == 1) {
            if (*cursor == '\\') {
                CopyString(fileNameBuffer, cursor + 1, sizeof(fileNameBuffer));
                if (char* underscore = std::strrchr(fileNameBuffer, '_')) {
                    *underscore = '\0';
                }
                state = 2;
            }
        } else if (*cursor == '\\') {
            ++cursor;
            *cursor = '\0';
            AppendString(buffer, fileNameBuffer, size);
            AppendString(buffer, extension, size);
            return 1;
        }
        if (cursor == buffer) break;
        --cursor;
    }

    CopyString(buffer, fileName, size);
    return 0;
}

void _E4_252() {
    TWO_PI_252 = 3.1415927F + 3.1415927F;
}

int _E5_252() {
    _E4_252();
    return 0;
}

void _E7_252() {
    OO_TWO_PI_252 = 1.0F / TWO_PI_252;
}

int _E8_252() {
    _E7_252();
    return 0;
}

void __fastcall CompDecorateTexName(const char* section, int sectionNumber, char* buffer,
                         unsigned int size, UNIT_SEX sex, int includeSex) {
    if (buffer == nullptr || size == 0) return;
    buffer[0] = '\0';
    if (section == nullptr || *section == '\0' || sectionNumber < 0 ||
        sectionNumber >= NUM_TEXCOMPONENT_SECTIONS) return;

    char finalName[260]{};
    BuildTexComponentPath(section, sectionNumber, finalName, sizeof(finalName),
                          UNITSEX_NONE, includeSex);
    if (finalName[0] != '\0' && ComponentUtilImageFileExists(finalName)) {
        CopyString(buffer, finalName, size);
        return;
    }

    BuildTexComponentPath(section, sectionNumber, finalName, sizeof(finalName),
                          sex, includeSex);
    if (finalName[0] != '\0' && ComponentUtilImageFileExists(finalName)) {
        CopyString(buffer, finalName, size);
    } else {
        SysMsgPrintf(SYSMSG_ERROR, 0, "Error, texture component file \"%s\" not found!", finalName);
    }
}

void __fastcall BuildTexComponentPath(const char* section, int sectionNumber, char* buffer,
                           unsigned int size, UNIT_SEX sex, int includeSex) {
    if (buffer == nullptr || size == 0) return;
    buffer[0] = '\0';
    if (section == nullptr || sectionNumber < 0 || sectionNumber >= NUM_TEXCOMPONENT_SECTIONS) return;

    char stringBuffer[260]{};
    char suffix[16]{};
    CopyString(stringBuffer, section, sizeof(stringBuffer));
    ConstructSuffixString(0, static_cast<int>(sex), 0, includeSex,
                          suffix, sizeof(suffix));

    // The original always emits BLP component paths.  It still strips the
    // caller's extension while preparing the name; the data reference in the
    // recovered assembly is the fixed ".BLP" literal.
    if (char* dot = std::strrchr(stringBuffer, '.')) *dot = '\0';
    FormatString(buffer, size, "%s%s%s\\%s%s%s", "", s_texComponentBasePath,
                 s_sectionDirectorynames[sectionNumber], stringBuffer, suffix, ".BLP");
}

int __fastcall CompDecorateUndecorateTexName(const char* fileName, char* buffer, unsigned int size) {
    if (buffer == nullptr || size == 0) return 0;
    buffer[0] = '\0';
    if (fileName == nullptr || *fileName == '\0') return 0;
    CopyString(buffer, fileName, size);

    const std::size_t length = std::strlen(buffer);
    if (length == 0) return 0;
    char extension[12]{};
    char* cursor = buffer + length - 1;
    while (cursor >= buffer) {
        if (extension[0] == '\0' && *cursor == '.') {
            CopyString(extension, cursor, sizeof(extension));
        } else if (*cursor == '_') {
            *cursor = '\0';
            if (extension[0] == '\0') {
                CopyString(buffer, fileName, size);
                return 0;
            }
            AppendString(buffer, extension, size);
            return 1;
        }
        if (cursor == buffer) break;
        --cursor;
    }

    CopyString(buffer, fileName, size);
    return 0;
}

int __fastcall ComponentUtilImageFileExists(const char* fileName) {
    if (fileName == nullptr) return 0;
    if (SFile::FileExists(fileName) != 0) return 1;

    const int type = TextureDiscoverFileType(fileName);
    const char* alternateExtension = type == 1 ? ".TGA" : type == 2 ? ".BLP" : nullptr;
    if (alternateExtension == nullptr) return 0;

    char alternate[260]{};
    TexturePickAlternateFilename(fileName, alternateExtension, type, alternate, sizeof(alternate));
    return SFile::FileExists(alternate) != 0;
}
