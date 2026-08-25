#pragma once

#include <cstdarg>

extern "C" {
#include "lua.h"
}

class FrameScript_Object;

enum FRAMESCRIPT_GENDER {
    GENDER_MALE = 0,
    GENDER_FEMALE = 1,
    GENDER_NEUTER = 2,
    GENDER_NOT_APPLICABLE = 3,
};

int FrameScript_Initialize();
void FrameScript_Flush();
void FrameScript_Destroy();
void FrameScript_MemoryCleanup(int disableAfterCollection);
int FrameScript_LoadTextTables(const char* filename);
const char* FrameScript_GetText(const char* name, int count, FRAMESCRIPT_GENDER gender);
unsigned FrameScript_GetPluralIndex(int count);
void FrameScript_CreateEvents(const char* const* eventNames, unsigned count);
void FrameScript_SignalEvent(unsigned eventId);
void FrameScript_SignalEvent(unsigned eventId, const char* format, ...);
void FrameScript_DestroyEvents();
lua_State* FrameScript_GetContext();
FrameScript_Object* FrameScript_GetObject(lua_State* state, int index);
void FrameScript_DisplayError(const char* format, ...);
void FrameScript_RegisterFunction(const char* name, lua_CFunction function);
void FrameScript_UnregisterFunction(const char* name);
void FrameScript_SetVariable(const char* name, int value);
int FrameScript_GetVariable(const char* name, int& value);
void FrameScript_SetVariable(const char* name, double value);
int FrameScript_GetVariable(const char* name, double& value);
void FrameScript_SetVariable(const char* name, const char* value);
int FrameScript_GetVariable(const char* name, const char*& value);
void FrameScript_UnsetVariable(const char* name);
int FrameScript_ExecuteFile(const char* filename);
int FrameScript_ExecuteBuffer(const void* data, unsigned bytes, const char* debugName);
int FrameScript_CompileFunction(const char* body, const char* debugName);
void FrameScript_ReleaseFunction(int functionReference);
void FrameScript_Execute(const char* body, const char* debugName);
void FrameScript_Execute(int functionReference);
void FrameScript_Execute(int functionReference, FrameScript_Object* object);
void FrameScript_Execute(int functionReference, FrameScript_Object* object,
                         const char* format, ...);
void FrameScript_ExecuteV(int functionReference, FrameScript_Object* object,
                          const char* format, va_list arguments);

void lua_disablegc(lua_State* state);
void lua_enablegc(lua_State* state);
