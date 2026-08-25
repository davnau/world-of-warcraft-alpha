#pragma once

#include "Storm/h/sapibase.h"

struct _CMDPARAMS {
    unsigned int flags;
    unsigned int id;
    const char* name;
    void* variable;
    unsigned int setvalue;
    unsigned int setmask;
    union { unsigned int boolvalue; unsigned int numvalue; const char* strvalue; } value;
};

struct _CMDERROR {
    unsigned int errorcode;
    const char* itemstr;
    const char* errorstr;
};

using SCMD_CALLBACK = int (__stdcall*)(_CMDPARAMS*, const char*);
struct _ARGLIST {
    unsigned int flags;
    unsigned int id;
    const char* name;
    SCMD_CALLBACK callback;
};

struct CMDDEF {
    void* m_previousLink;
    void* m_nextLink;
    unsigned int flags;
    unsigned int id;
    char name[16];
    int namelength;
    unsigned int setvalue;
    unsigned int setmask;
    void* variableptr;
    unsigned int variablebytes;
    SCMD_CALLBACK callback;
    int found;
    union { unsigned int currvalue; char* currvaluestr; } value;
    CMDDEF();
    ~CMDDEF();
};

int __stdcall SCmdCheckId(unsigned int id);
int __stdcall SCmdDestroy();
BOOL __stdcall SCmdGetBool(unsigned int id);
unsigned int __stdcall SCmdGetNum(unsigned int id);
int __stdcall SCmdGetString(unsigned int id, char* buffer, unsigned int bufferChars);
int __stdcall SCmdGetStringAlloc(unsigned int id, char** buffer);
int __stdcall SCmdProcess(const char* commandLine, int skipProgramName,
    int (__stdcall* extraCallback)(const char*), void (__stdcall* errorCallback)(_CMDERROR*));
int __stdcall SCmdProcessCommandLine(int (__stdcall* extraCallback)(const char*),
    void (__stdcall* errorCallback)(_CMDERROR*));
int __stdcall SCmdRegisterArgList(const _ARGLIST* list, unsigned int numArgs);
int __stdcall SCmdRegisterArgument(unsigned int flags, unsigned int id, const char* name,
    void* variablePtr, unsigned int variableBytes, unsigned int setValue, unsigned int setMask,
    SCMD_CALLBACK callback);

#if defined(_M_IX86)
static_assert(sizeof(_CMDPARAMS) == 28);
static_assert(sizeof(_CMDERROR) == 12);
static_assert(sizeof(_ARGLIST) == 16);
static_assert(sizeof(CMDDEF) == 64);
#endif
