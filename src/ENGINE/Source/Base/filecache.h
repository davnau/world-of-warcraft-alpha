#pragma once

#include <cstdint>
struct HSLOG__;
void BaseFileInitialize(); void BaseFileDestroy();
int BaseFilePrefetch(const char* filename); int BaseFileIsFetched(const char* filename);
int BaseFileLoad(const char* filename,void** fileBuffer,unsigned* fileSize);
void BaseFileFlush(); void BaseFileRegisterUncachable(const char* filename); void BaseFileUnregisterUncachable(const char* filename);
void BaseFileDumpStats(HSLOG__* log=nullptr);
int IBaseFileLoad(const char* filename,void** fileBuffer,unsigned* fileSize);
int IBaseFileStartLoad(const char* filename);
void IBaseFileWaitForLoad(const char* filename);
void IBaseFileUnload(const char* filename);
