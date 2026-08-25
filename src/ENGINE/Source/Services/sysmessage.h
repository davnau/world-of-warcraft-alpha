#pragma once

#include <cstdarg>
#include <cstdint>

enum SYSMSG_TYPE : std::int32_t {
    SYSMSG_INFO = 0,
    SYSMSG_WARNING = 1,
    SYSMSG_ERROR = 2,
    SYSMSG_FATAL = 3,
    SYSMSG_NUMTYPES = 4
};

class CStatus;

struct MSGBUFFER {
    char* string = nullptr;
    float timeVisible = 0.0F;
    SYSMSG_TYPE severity = SYSMSG_INFO;
    unsigned categoryMask = 0;

    MSGBUFFER() = default;
    MSGBUFFER(const MSGBUFFER&) = delete;
    MSGBUFFER& operator=(const MSGBUFFER&) = delete;
    ~MSGBUFFER();
    void SetInfo(const char* newString, SYSMSG_TYPE newSeverity, unsigned categories);
};

using SYSMSG_CALLBACK = void (*)(const char*, SYSMSG_TYPE);

int SysMsgAdd(const char* message, SYSMSG_TYPE severity, unsigned categoryMask);
int SysMsgAdd(CStatus* status, unsigned categoryMask);
void GenerateMaskString(char* buffer, unsigned size, unsigned mask);
int SysMsgVPrintf(SYSMSG_TYPE severity, unsigned categoryMask, const char* format, std::va_list args);
int SysMsgPrintf(SYSMSG_TYPE severity, unsigned categoryMask, const char* format, ...);
void SysMsgEnable(int enabled);
int SysMsgEnabled();
void SysMsgSetMinDisplayLevel(SYSMSG_TYPE severity);
void SysMsgSetMaxDisplayLevel(SYSMSG_TYPE severity);
SYSMSG_TYPE SysMsgGetMinDisplayLevel();
SYSMSG_TYPE SysMsgGetMaxDisplayLevel();
void SysMsgSetFilter(unsigned categoryMask);
unsigned SysMsgGetFilter();
void SysMsgGetSeverityColor(SYSMSG_TYPE severity, float* red, float* green, float* blue);
double SysMsgGetSeverityDuration(int severity);
void SysMsgInitialize();
void SysMsgShutdown();
void SysMsgEnableFileLog(const char* directory);
int DetermineFileName(const char* directory, char* buffer, unsigned size);
void SysMsgDisableFileLog();
void SysMsgSetCallback(SYSMSG_CALLBACK callback);
