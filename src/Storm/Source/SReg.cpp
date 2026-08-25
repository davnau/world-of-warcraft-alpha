// Reconstructed from D:\build\buildWoW\Storm\Source\SReg.cpp
// Public signatures are declared in Storm/h/storm.h; internal helper names and
// registry flag behavior follow the PDB-mapped bodies at RVA 0x23AEB0-0x23BC00.

#include "Storm/h/storm.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

constexpr unsigned int kUseMachineOnly = 0x04;
constexpr unsigned int kSkipMachine = 0x01;
constexpr unsigned int kBattleNetBase = 0x02;
constexpr unsigned int kFlush = 0x08;
constexpr unsigned int kAbsoluteKey = 0x10;

HKEY SaveRoot(unsigned int flags) { return (flags & kUseMachineOnly) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER; }

bool ValidName(const char* value) { return value && *value; }

void CopyString(char* destination, unsigned int capacity, const char* source)
{
    if (!destination || capacity == 0) return;
    strncpy_s(destination, capacity, source ? source : "", _TRUNCATE);
}

} // namespace

int __stdcall SRegGetBaseKey(unsigned int flags, char* buffer, unsigned int bufferChars)
{
    if (!buffer || bufferChars == 0) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    CopyString(buffer, bufferChars, (flags & kBattleNetBase) ? "Software\\Battle.net\\" : "Software\\Blizzard Entertainment\\");
    SErrSetLastError(ERROR_SUCCESS);
    return 1;
}

void __fastcall BuildFullKeyName(const char* keyName, unsigned int flags, char* buffer, unsigned int bufferChars)
{
    if (!buffer || bufferChars == 0) return;
    buffer[0] = '\0';
    if (!(flags & kAbsoluteKey)) SRegGetBaseKey(flags, buffer, bufferChars);
    if (!keyName || !*keyName) return;
    const size_t length = std::strlen(buffer);
    const bool needSlash = length != 0 && buffer[length - 1] != '\\' && keyName[0] != '\\';
    if (needSlash) strcat_s(buffer, bufferChars, "\\");
    strcat_s(buffer, bufferChars, keyName);
}

LSTATUS __fastcall ILoadValue(HKEY root, const char* subKeyName, const char* valueName, DWORD* dataType, BYTE* buffer, DWORD bytes, DWORD* bytesRead)
{
    HKEY key = nullptr;
    LSTATUS status = RegOpenKeyExA(root, subKeyName, 0, KEY_QUERY_VALUE, &key);
    if (status != ERROR_SUCCESS) return status;
    *bytesRead = bytes;
    status = RegQueryValueExA(key, valueName, nullptr, dataType, buffer, bytesRead);
    RegCloseKey(key);
    return status;
}

int __fastcall InternalLoadEntry(const char* keyName, const char* valueName, unsigned int flags, DWORD* dataType, BYTE* buffer, DWORD bytes, DWORD* bytesRead)
{
    *bytesRead = 0;
    *dataType = REG_DWORD;
    char fullKeyName[MAX_PATH];
    BuildFullKeyName(keyName, flags, fullKeyName, MAX_PATH);
    LSTATUS status = ERROR_CANTREAD;
    if (!(flags & kUseMachineOnly)) {
        status = ILoadValue(HKEY_CURRENT_USER, fullKeyName, valueName, dataType, buffer, bytes, bytesRead);
        if (status == ERROR_SUCCESS) return 1;
    }
    if (!(flags & kSkipMachine)) status = ILoadValue(HKEY_LOCAL_MACHINE, fullKeyName, valueName, dataType, buffer, bytes, bytesRead);
    SErrSetLastError(status);
    return status == ERROR_SUCCESS;
}

int __stdcall SRegLoadData(const char* keyname, const char* valuename, unsigned int flags, void* buffer, unsigned int bufferSize, unsigned int* bytesRead)
{
    if (!ValidName(keyname) || !ValidName(valuename) || (!buffer && bufferSize)) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    DWORD localRead = 0, dataType = 0;
    DWORD* read = bytesRead ? reinterpret_cast<DWORD*>(bytesRead) : &localRead;
    return InternalLoadEntry(keyname, valuename, flags, &dataType, static_cast<BYTE*>(buffer), bufferSize, read);
}

int __stdcall SRegLoadString(const char* keyname, const char* valuename, unsigned int flags, char* buffer, unsigned int bufferSize)
{
    if (!buffer || bufferSize == 0) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    DWORD dataType = 0, bytesRead = 0;
    if (!InternalLoadEntry(keyname, valuename, flags, &dataType, reinterpret_cast<BYTE*>(buffer), bufferSize, &bytesRead)) return 0;
    if (dataType == REG_DWORD && bytesRead >= sizeof(DWORD)) _snprintf_s(buffer, bufferSize, _TRUNCATE, "%u", *reinterpret_cast<DWORD*>(buffer));
    else buffer[std::min<unsigned int>(bytesRead, bufferSize - 1)] = '\0';
    return 1;
}

int __stdcall SRegLoadValue(const char* keyname, const char* valuename, unsigned int flags, unsigned int* value)
{
    if (!value) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    BYTE buffer[256]{};
    DWORD dataType = 0, bytesRead = 0;
    if (!InternalLoadEntry(keyname, valuename, flags, &dataType, buffer, sizeof(buffer), &bytesRead)) return 0;
    if (dataType == REG_DWORD && bytesRead >= sizeof(DWORD)) *value = *reinterpret_cast<DWORD*>(buffer);
    else *value = std::strtoul(reinterpret_cast<const char*>(buffer), nullptr, 0);
    return 1;
}

int __fastcall InternalSaveEntry(const char* keyName, const char* valueName, unsigned int flags, DWORD dataType, const BYTE* buffer, DWORD bytes)
{
    char fullKeyName[MAX_PATH];
    BuildFullKeyName(keyName, flags, fullKeyName, MAX_PATH);
    HKEY key = nullptr;
    DWORD disposition = 0;
    LSTATUS status = RegCreateKeyExA(SaveRoot(flags), fullKeyName, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, &disposition);
    if (status == ERROR_SUCCESS) {
        status = RegSetValueExA(key, valueName, 0, dataType, buffer, bytes);
        if (status == ERROR_SUCCESS && (flags & kFlush)) status = RegFlushKey(key);
        const LSTATUS closeStatus = RegCloseKey(key);
        if (status == ERROR_SUCCESS) status = closeStatus;
    }
    SErrSetLastError(status);
    return status == ERROR_SUCCESS;
}

int __stdcall SRegSaveData(const char* keyname, const char* valuename, unsigned int flags, const void* data, unsigned int dataBytes)
{
    if (!ValidName(keyname) || !ValidName(valuename) || (!data && dataBytes)) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    const DWORD dataType = (flags & 0xE0U) >> 5U;
    return InternalSaveEntry(keyname, valuename, flags, dataType, static_cast<const BYTE*>(data), dataBytes);
}

int __stdcall SRegSaveString(const char* keyname, const char* valuename, unsigned int flags, const char* string)
{
    if (!ValidName(keyname) || !ValidName(valuename) || !string) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    return InternalSaveEntry(keyname, valuename, flags, REG_SZ, reinterpret_cast<const BYTE*>(string), static_cast<DWORD>(std::strlen(string) + 1));
}

int __stdcall SRegSaveValue(const char* keyname, const char* valuename, unsigned int flags, unsigned int value)
{
    if (!ValidName(keyname) || !ValidName(valuename)) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    return InternalSaveEntry(keyname, valuename, flags, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
}

LSTATUS __fastcall IDeleteValue(HKEY root, const char* subKeyName, const char* valueName)
{
    HKEY key = nullptr;
    LSTATUS status = RegOpenKeyExA(root, subKeyName, 0, KEY_SET_VALUE, &key);
    if (status == ERROR_SUCCESS) { status = RegDeleteValueA(key, valueName); RegCloseKey(key); }
    return status;
}

int __fastcall InternalDeleteEntry(const char* keyName, const char* valueName, unsigned int flags)
{
    char fullKeyName[MAX_PATH];
    BuildFullKeyName(keyName, flags, fullKeyName, MAX_PATH);
    LSTATUS currentUserStatus = ERROR_FILE_NOT_FOUND, status = ERROR_FILE_NOT_FOUND;
    bool success = false;
    if (!(flags & kUseMachineOnly)) { currentUserStatus = IDeleteValue(HKEY_CURRENT_USER, fullKeyName, valueName); success = currentUserStatus == ERROR_SUCCESS; }
    if (!(flags & kSkipMachine)) { status = IDeleteValue(HKEY_LOCAL_MACHINE, fullKeyName, valueName); success = success || status == ERROR_SUCCESS; }
    if (!success) SErrSetLastError(currentUserStatus != ERROR_FILE_NOT_FOUND ? currentUserStatus : status);
    return success;
}

int __stdcall SRegDeleteValue(const char* keyname, const char* valuename, unsigned int flags)
{
    if (!ValidName(keyname) || !ValidName(valuename)) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    return InternalDeleteEntry(keyname, valuename, flags);
}

LSTATUS __fastcall IDeleteKey(HKEY root, const char* subKeyName) { return RegDeleteKeyA(root, subKeyName); }

int __fastcall InternalDeleteKey(const char* keyName, unsigned int flags)
{
    char fullKeyName[MAX_PATH];
    BuildFullKeyName(keyName, flags, fullKeyName, MAX_PATH);
    LSTATUS currentUserStatus = ERROR_FILE_NOT_FOUND, status = ERROR_FILE_NOT_FOUND;
    bool success = false;
    if (!(flags & kUseMachineOnly)) { currentUserStatus = IDeleteKey(HKEY_CURRENT_USER, fullKeyName); success = currentUserStatus == ERROR_SUCCESS; }
    if (!(flags & kSkipMachine)) { status = IDeleteKey(HKEY_LOCAL_MACHINE, fullKeyName); success = success || status == ERROR_SUCCESS; }
    if (!success) SErrSetLastError(currentUserStatus != ERROR_FILE_NOT_FOUND ? currentUserStatus : status);
    return success;
}

int __stdcall SRegDeleteKey(const char* keyname, unsigned int flags)
{
    if (!ValidName(keyname)) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    return InternalDeleteKey(keyname, flags);
}

int __stdcall SRegEnumKey(const char* baseKeyName, unsigned int flags, DWORD subKeyIndex, char* keyNameBuffer, unsigned int bufferChars)
{
    if (!ValidName(baseKeyName) || !keyNameBuffer || bufferChars == 0) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    char fullName[MAX_PATH], child[MAX_PATH];
    BuildFullKeyName(baseKeyName, flags, fullName, MAX_PATH);
    HKEY key = nullptr;
    LSTATUS status = RegOpenKeyExA(SaveRoot(flags), fullName, 0, KEY_ENUMERATE_SUB_KEYS, &key);
    if (status == ERROR_SUCCESS) {
        DWORD childChars = MAX_PATH;
        FILETIME writeTime{};
        status = RegEnumKeyExA(key, subKeyIndex, child, &childChars, nullptr, nullptr, nullptr, &writeTime);
        RegCloseKey(key);
    }
    if (status != ERROR_SUCCESS) { SErrSetLastError(status); return 0; }
    _snprintf_s(keyNameBuffer, bufferChars, _TRUNCATE, "%s\\%s", baseKeyName, child);
    return 1;
}

int __stdcall SRegGetNumSubKeys(const char* keyName, unsigned int flags, unsigned int* numSubKeys)
{
    if (!ValidName(keyName) || !numSubKeys) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    *numSubKeys = 0;
    char fullName[MAX_PATH];
    BuildFullKeyName(keyName, flags, fullName, MAX_PATH);
    HKEY key = nullptr;
    LSTATUS status = RegOpenKeyExA(SaveRoot(flags), fullName, 0, KEY_QUERY_VALUE, &key);
    if (status == ERROR_SUCCESS) {
        DWORD count = 0;
        status = RegQueryInfoKeyA(key, nullptr, nullptr, nullptr, &count, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        RegCloseKey(key);
        *numSubKeys = count;
    }
    if (status != ERROR_SUCCESS) { SErrSetLastError(status); return 0; }
    return 1;
}
