#pragma once

#include <cstdarg>
#include <list>
#include <string>

enum STATUS_TYPE { STATUS_INFO=0, STATUS_WARNING=1, STATUS_ERROR=2, STATUS_FATAL=3 };
class CStatus {
public:
    struct STATUSENTRY { std::string text; STATUS_TYPE severity; };
    virtual ~CStatus();
    void Prepend(const char* text,STATUS_TYPE severity=STATUS_ERROR);
    void Add(STATUS_TYPE severity,const char* format,...);
    void Add(const CStatus* source);
    void Display() const;
    int IsEmpty() const; void Clear();
    void GetErrorStr(char* buffer,unsigned bufchars,STATUS_TYPE minSeverity=STATUS_INFO) const;
    int GetErrorStrLen(STATUS_TYPE minSeverity=STATUS_INFO) const;
    char* GetErrorStrAlloc(STATUS_TYPE minSeverity=STATUS_INFO) const;
    STATUS_TYPE GetHighestSeverity() const;
private: std::list<STATUSENTRY> m_entries;
};
CStatus* GetGlobalStatusObj();
char* FormatStatusMessage(const char* format,va_list args);
