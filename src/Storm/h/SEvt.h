#pragma once

#include "Storm/h/storm.h"

using SEVT_HANDLER = void (__stdcall*)(void*);

struct _IDHASHENTRY {
    unsigned int id;
    unsigned int sequence;
    SEVT_HANDLER handler;
    _IDHASHENTRY* next;
};

struct _IDHASHTABLE {
    _IDHASHENTRY** data;
    unsigned int size;
    unsigned int used;
    _IDHASHTABLE* next;
};

struct _TYPEHASHENTRY {
    unsigned int type;
    unsigned int subtype;
    unsigned int sequence;
    _IDHASHTABLE* idhashtable;
    _TYPEHASHENTRY* next;
};

struct BREAKCMD {
    void* m_previousLink;
    void* m_nextLink;
    void* data;
};

int __stdcall SEvtBreakHandlerChain(void* data);
int __stdcall SEvtDestroy();
int __stdcall SEvtDispatch(unsigned int type, unsigned int subtype, unsigned int id, void* data);
int __stdcall SEvtPopState(unsigned int type, unsigned int subtype);
int __stdcall SEvtPushState(unsigned int type, unsigned int subtype);
int __stdcall SEvtRegisterHandler(unsigned int type, unsigned int subtype, unsigned int id,
    void* flags, SEVT_HANDLER handler);
int __stdcall SEvtUnregisterHandler(unsigned int type, unsigned int subtype, unsigned int id,
    SEVT_HANDLER handler);
int __stdcall SEvtUnregisterType(unsigned int type, unsigned int subtype);

#if defined(_M_IX86)
static_assert(sizeof(_IDHASHENTRY) == 16, "_IDHASHENTRY x86 PDB layout mismatch.");
static_assert(sizeof(_IDHASHTABLE) == 16, "_IDHASHTABLE x86 PDB layout mismatch.");
static_assert(sizeof(_TYPEHASHENTRY) == 20, "_TYPEHASHENTRY x86 PDB layout mismatch.");
static_assert(sizeof(BREAKCMD) == 12, "BREAKCMD x86 PDB layout mismatch.");
#endif
