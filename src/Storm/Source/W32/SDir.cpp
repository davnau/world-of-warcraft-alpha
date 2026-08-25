// Reconstructed from D:\build\buildWoW\Storm\Source\W32\SDir.cpp
// (SFile::OpenDir RVA 0x254E30, ReadDir 0x254EF0, CloseDir 0x254F80).

#include "Storm/h/storm.h"

#include <cstring>
#include <memory>
#include <sys/stat.h>

SDIR* __stdcall SFile::OpenDir(const char* name)
{
    if (!name || !*name)
        return nullptr;

    auto dir = std::make_unique<SDIR>();
    strncpy_s(dir->name, name, _TRUNCATE);
    char* last = dir->name + std::strlen(dir->name);
    while (last > dir->name && last[-1] == '\\')
        *--last = '\0';

    struct _stat32 stats{};
    if (_stat32(dir->name, &stats) != 0 || (stats.st_mode & _S_IFDIR) == 0)
        return nullptr;

    const size_t length = std::strlen(dir->name);
    if (length + 3 > sizeof(dir->name))
        return nullptr;
    dir->name[length] = '\\';
    dir->name[length + 1] = '*';
    dir->name[length + 2] = '\0';
    dir->handle = nullptr;
    return dir.release();
}

SDIRENT* __stdcall SFile::ReadDir(SDIR* dir)
{
    if (!dir)
        return nullptr;

    if (dir->handle == nullptr) {
        dir->handle = FindFirstFileA(dir->name, &dir->findData);
        if (dir->handle == INVALID_HANDLE_VALUE) {
            dir->handle = nullptr;
            return nullptr;
        }
    } else if (!FindNextFileA(dir->handle, &dir->findData)) {
        return nullptr;
    }
    strncpy_s(dir->dirent.d_name, dir->findData.cFileName, _TRUNCATE);
    return &dir->dirent;
}

void __stdcall SFile::CloseDir(SDIR* dir)
{
    if (!dir)
        return;
    if (dir->handle && dir->handle != INVALID_HANDLE_VALUE)
        FindClose(dir->handle);
    delete dir;
}
