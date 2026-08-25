#pragma once

#include <memory>
#include <string>

class CStringRep {
public:
    explicit CStringRep(const char* value);
    void DecrRef(); int IsString(const char*) const; bool operator==(const CStringRep&) const;
    std::string m_string;
};
class CStringManager {
public:
    static CStringManager* Get(); static void DestroyManager();
    ~CStringManager(); std::shared_ptr<CStringRep> Add(const char*); std::shared_ptr<CStringRep> Find(const char*);
};
class RCString {
public:
    RCString()=default; RCString(const char* value); ~RCString();
    void Copy(const RCString&); void Copy(const char*); void Free();
    bool operator==(const RCString&) const; bool operator==(const char*) const;
    const char* GetString() const; operator const char*() const;
    RCString Cat(const char*) const; RCString& operator+=(const RCString&); RCString& operator+=(const char*);
    RCString SubString(unsigned start,unsigned count) const; static RCString Get(const char*);
private: std::shared_ptr<CStringRep> m_rep;
};
