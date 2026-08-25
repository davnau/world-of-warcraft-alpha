#pragma once

#include "WoW/Source/clientruntime.h"

#include <cstdint>

class CVar;
using CVarCallback = unsigned char (__fastcall*)(CVar*, const char*, const char*, void*);

class CVar {
public:
    CVar();
    ~CVar();
    CVar(const CVar&) = delete;
    CVar& operator=(const CVar&) = delete;

    static void Initialize(const char* fileName);
    static void Destroy();
    static CVar* Register(const char* help, const char* name, unsigned flags,
                          const char* value, CVarCallback callback,
                          unsigned category, unsigned char setCommand, void* arg);
    static CVar* Lookup(const char* name);

    unsigned char Set(const char* value, unsigned char setValue = 1,
                      unsigned char setReset = 0, unsigned char setDefault = 0);
    void Reset();
    void Default();
    unsigned char Update();
    void InternalSet(const char* value, unsigned char setValue,
                     unsigned char setReset, unsigned char setDefault);

    const char* Name() const { return m_name; }
    const char* StringValue() const { return m_stringValue ? m_stringValue : ""; }
    int IntValue() const { return m_intValue; }
    float FloatValue() const { return m_floatValue; }
    unsigned Flags() const { return m_flags; }

    unsigned m_hashval = 0;
    char m_name[32]{};
    char* m_stringValue = nullptr;
    char* m_defaultValue = nullptr;
    char* m_resetValue = nullptr;
    char* m_latchedValue = nullptr;
    float m_floatValue = 0.0F;
    int m_intValue = 0;
    unsigned m_modified = 0;
    unsigned m_flags = 0;
    CVarCallback m_callback = nullptr;
    void* m_arg = nullptr;
    unsigned m_category = 0;
};

int __fastcall SetCommandHandler(const char* args, char* commandLine);
int __fastcall CvarResetCommandHandler(const char* args, char* commandLine);
int __fastcall CvarDefaultCommandHandler(const char* args, char* commandLine);
int __fastcall CvarListCommandHandler(const char* args, char* commandLine);
int CVarLoadFile();
int CVarSaveFile();
int __fastcall CvarCommandHandler(const char* command, const char* arguments, const char* value);
int s_CreatePathDirectories(const char* path);

extern CVar* s_errorsCvar;
extern float INFINITY_295;
extern float TWO_PI_290;
extern float OO_TWO_PI_291;

int _E16_22(); void _E13_10(); int _E15_12(); void _E14_15();
int _E2_295(); int _E1_295(); int _E5_292(); void _E4_292(); int _E8_292(); void _E7_292();

