#include "ENGINE/Source/Frame/framescript.h"

#include "ENGINE/Source/Frame/frame.h"
#include "Storm/h/storm.h"

extern "C" {
#include "lauxlib.h"
#include "lualib.h"
}

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {
struct FrameScriptEvent {
    std::string name;
    std::vector<FrameScript_Object*> listeners;
};

lua_State* s_context = nullptr;
int s_errorFunction = 0;
std::vector<FrameScriptEvent> s_events;
std::vector<FrameScript_Object*> s_thisStack;

void PushGlobal(lua_State* state, const char* name) {
    lua_pushstring(state, name);
    lua_gettable(state, LUA_GLOBALSINDEX);
}

void SetGlobal(lua_State* state, const char* name) {
    lua_pushstring(state, name);
    lua_insert(state, -2);
    lua_settable(state, LUA_GLOBALSINDEX);
}

void ReportLuaError(lua_State* state, const char* operation) {
    const char* error = state && lua_isstring(state, -1) ? lua_tostring(state, -1) : "unknown Lua error";
    std::fprintf(stderr, "%s: %s\n", operation ? operation : "FrameScript", error ? error : "unknown Lua error");
    if (state) lua_pop(state, 1);
}

int ScriptGetGlobal(lua_State* state) {
    const char* name = luaL_checkstring(state, 1);
    PushGlobal(state, name);
    return 1;
}

int ScriptNext(lua_State* state) {
    luaL_checktype(state, 1, LUA_TTABLE);
    lua_settop(state, 2);
    if (lua_next(state, 1)) return 2;
    lua_pushnil(state);
    return 1;
}

int ScriptDebugInfo(lua_State*) { return 0; }

int FindEvent(const char* eventName) {
    if (!eventName) return -1;
    for (std::size_t index = 0; index < s_events.size(); ++index) {
        if (_stricmp(s_events[index].name.c_str(), eventName) == 0) return static_cast<int>(index);
    }
    return -1;
}

void PushArguments(lua_State* state, const char* format, va_list arguments) {
    unsigned argumentIndex = 1;
    for (const char* cursor = format ? format : ""; *cursor; ++cursor) {
        if (*cursor != '%') continue;
        ++cursor;
        if (!*cursor) break;
        switch (*cursor) {
        case 's': {
            const char* value = va_arg(arguments, const char*);
            if (value) lua_pushstring(state, value); else lua_pushnil(state);
            break;
        }
        case 'd': case 'i': case 'u':
            lua_pushnumber(state, static_cast<lua_Number>(va_arg(arguments, int)));
            break;
        case 'f': case 'g':
            lua_pushnumber(state, static_cast<lua_Number>(va_arg(arguments, double)));
            break;
        default:
            continue;
        }
        char name[24]{};
        std::snprintf(name, sizeof(name), "arg%u", argumentIndex++);
        SetGlobal(state, name);
    }
    lua_pushnil(state);
    char name[24]{};
    std::snprintf(name, sizeof(name), "arg%u", argumentIndex);
    SetGlobal(state, name);
}

void CreateScriptObjectTable(lua_State* state, FrameScript_Object* object) {
    lua_newtable(state);
    lua_pushnumber(state, 0);
    lua_pushlightuserdata(state, object);
    lua_rawset(state, -3);
    PushGlobal(state, "__framescript_meta");
    if (lua_istable(state, -1)) lua_setmetatable(state, -2);
    else lua_pop(state, 1);
}

void PushScriptObject(lua_State* state, FrameScript_Object* object) {
    if (!object) { lua_pushnil(state); return; }
    if (object->m_luaRegistered > 0 && object->m_luaObjectRef >= 0)
        lua_rawgeti(state, LUA_REGISTRYINDEX, object->m_luaObjectRef);
    else
        CreateScriptObjectTable(state, object);
}

FrameScript_Object* GetScriptObject(lua_State* state, int index) {
    if (!state) return nullptr;
    if (lua_istable(state, index)) {
        if (index < 0) index = lua_gettop(state) + index + 1;
        lua_rawgeti(state, index, 0);
        auto* object = static_cast<FrameScript_Object*>(lua_touserdata(state, -1));
        lua_pop(state, 1);
        return object;
    }
    return lua_isuserdata(state, index)
        ? static_cast<FrameScript_Object*>(lua_touserdata(state, index)) : nullptr;
}
}

void lua_disablegc(lua_State* state) {
    if (state) lua_setgcthreshold(state, INT_MAX);
}

void lua_enablegc(lua_State* state) {
    if (state && lua_getgcthreshold(state) == INT_MAX) lua_setgcthreshold(state, 0);
}

int FrameScript_Initialize() {
    if (s_context) return 1;
    s_context = lua_open();
    if (!s_context) return 0;
    lua_disablegc(s_context);

    lua_pushstring(s_context, "__framescript_meta");
    lua_newtable(s_context);
    lua_pushstring(s_context, "__index");
    lua_pushcfunction(s_context, FrameScript_Object::LookupScriptMethod);
    lua_settable(s_context, -3);
    lua_settable(s_context, LUA_GLOBALSINDEX);

    FrameScript_RegisterFunction("getglobal", ScriptGetGlobal);
    FrameScript_RegisterFunction("next", ScriptNext);
    FrameScript_RegisterFunction("debuginfo", ScriptDebugInfo);
    luaopen_string(s_context);
    luaopen_table(s_context);
    luaopen_math(s_context);

    void* buffer = nullptr;
    unsigned bytes = 0;
    if (SFile::LoadFile("Interface\\FrameXML\\compat.lua", &buffer, &bytes, 0, nullptr)) {
        if (luaL_loadbuffer(s_context, static_cast<const char*>(buffer), bytes, "compat.lua") != 0 ||
            lua_pcall(s_context, 0, 0, 0) != 0) {
            ReportLuaError(s_context, "Error executing Interface\\FrameXML\\compat.lua");
        }
        SFile::Unload(buffer);
    }
    PushGlobal(s_context, "ERRORMESSAGE");
    if (lua_isfunction(s_context, -1)) s_errorFunction = luaL_ref(s_context, LUA_REGISTRYINDEX);
    else lua_pop(s_context, 1);
    return 1;
}

void FrameScript_Flush() {
    if (s_context) { FrameScript_Destroy(); FrameScript_Initialize(); }
}

void FrameScript_Destroy() {
    if (!s_context) return;
    lua_close(s_context);
    s_context = nullptr;
    s_errorFunction = 0;
    s_events.clear();
    s_thisStack.clear();
}

void FrameScript_MemoryCleanup(int disableAfterCollection) {
    if (!s_context) return;
    lua_enablegc(s_context);
    lua_setgcthreshold(s_context, 0);
    if (!disableAfterCollection) lua_disablegc(s_context);
}

int FrameScript_LoadTextTables(const char* filename) { return FrameScript_ExecuteFile(filename); }

const char* FrameScript_GetText(const char* name, int, FRAMESCRIPT_GENDER) {
    static std::string value;
    const char* text = nullptr;
    if (FrameScript_GetVariable(name, text) && text) value = text; else value.clear();
    return value.c_str();
}

unsigned FrameScript_GetPluralIndex(int count) { return count == 1 ? 0U : 1U; }

void FrameScript_CreateEvents(const char* const* eventNames, unsigned count) {
    s_events.clear();
    s_events.resize(count);
    for (unsigned index = 0; index < count; ++index)
        s_events[index].name = eventNames && eventNames[index] ? eventNames[index] : "";
}

void FrameScript_SignalEvent(unsigned eventId) {
    if (eventId >= s_events.size()) return;
    FrameScriptEvent& event = s_events[eventId];
    const auto listeners = event.listeners;
    if ((eventId >= 0xC9U && eventId <= 0xD0U) || eventId == 0xB9U) {
        std::fprintf(stderr, "ACTIONBAR_EVENT id=%u name=%s listeners=%zu\n",
                     eventId, event.name.c_str(), listeners.size());
    }
    for (FrameScript_Object* listener : listeners)
        if (listener) listener->OnScriptEvent(event.name.c_str());
}

void FrameScript_SignalEvent(unsigned eventId, const char* format, ...) {
    if (eventId >= s_events.size()) return;
    FrameScriptEvent& event = s_events[eventId];
    const auto listeners = event.listeners;
    if (eventId >= 0xC9U && eventId <= 0xD0U) {
        std::fprintf(stderr, "ACTIONBAR_EVENT id=%u name=%s listeners=%zu\n",
                     eventId, event.name.c_str(), listeners.size());
    }
    if (eventId >= 0xD9U && eventId <= 0xF1U) {
        std::fprintf(stderr, "CHAT_EVENT id=%u name=%s listeners=%zu\n",
                     eventId, event.name.c_str(), listeners.size());
    }
    va_list arguments;
    va_start(arguments, format);
    for (FrameScript_Object* listener : listeners) {
        if (!listener) continue;
        va_list copy;
        va_copy(copy, arguments);
        listener->OnScriptEvent(event.name.c_str(), format, copy);
        va_end(copy);
    }
    va_end(arguments);
}

void FrameScript_DestroyEvents() { s_events.clear(); }
lua_State* FrameScript_GetContext() { return s_context; }
FrameScript_Object* FrameScript_GetObject(lua_State* state, int index) {
    return GetScriptObject(state, index);
}

void FrameScript_DisplayError(const char* format, ...) {
    char text[1024]{};
    va_list arguments;
    va_start(arguments, format);
    std::vsnprintf(text, sizeof(text), format, arguments);
    va_end(arguments);
    if (s_context) luaL_error(s_context, "%s", text);
}

void FrameScript_RegisterFunction(const char* name, lua_CFunction function) {
    if (!s_context || !name || !function) return;
    lua_pushcfunction(s_context, function);
    SetGlobal(s_context, name);
}

void FrameScript_UnregisterFunction(const char* name) {
    if (!s_context || !name) return;
    lua_pushnil(s_context);
    SetGlobal(s_context, name);
}

void FrameScript_SetVariable(const char* name, int value) {
    if (!s_context || !name) return;
    lua_pushnumber(s_context, static_cast<lua_Number>(value)); SetGlobal(s_context, name);
}
int FrameScript_GetVariable(const char* name, int& value) {
    if (!s_context || !name) return 0;
    PushGlobal(s_context, name);
    const int found = lua_isnumber(s_context, -1);
    if (found) value = static_cast<int>(lua_tonumber(s_context, -1));
    lua_pop(s_context, 1); return found;
}
void FrameScript_SetVariable(const char* name, double value) {
    if (!s_context || !name) return;
    lua_pushnumber(s_context, value); SetGlobal(s_context, name);
}
int FrameScript_GetVariable(const char* name, double& value) {
    if (!s_context || !name) return 0;
    PushGlobal(s_context, name);
    const int found = lua_isnumber(s_context, -1);
    if (found) value = lua_tonumber(s_context, -1);
    lua_pop(s_context, 1); return found;
}
void FrameScript_SetVariable(const char* name, const char* value) {
    if (!s_context || !name) return;
    if (value) lua_pushstring(s_context, value); else lua_pushnil(s_context);
    SetGlobal(s_context, name);
}
int FrameScript_GetVariable(const char* name, const char*& value) {
    if (!s_context || !name) return 0;
    PushGlobal(s_context, name);
    const int found = lua_isstring(s_context, -1);
    value = found ? lua_tostring(s_context, -1) : nullptr;
    lua_pop(s_context, 1); return found;
}
void FrameScript_UnsetVariable(const char* name) {
    if (!s_context || !name) return;
    lua_pushnil(s_context); SetGlobal(s_context, name);
}

int FrameScript_ExecuteFile(const char* filename) {
    if (!s_context || !filename) return 0;
    void* buffer = nullptr;
    unsigned bytes = 0;
    if (!SFile::LoadFile(filename, &buffer, &bytes, 0, nullptr)) return 0;
    const int result = FrameScript_ExecuteBuffer(buffer, bytes, filename);
    SFile::Unload(buffer);
    return result;
}

int FrameScript_ExecuteBuffer(const void* data, unsigned bytes, const char* debugName) {
    if (!s_context || (!data && bytes)) return 0;
    if (luaL_loadbuffer(s_context, static_cast<const char*>(data), bytes,
                        debugName ? debugName : "FrameScript") != 0 ||
        lua_pcall(s_context, 0, 0, 0) != 0) {
        ReportLuaError(s_context, debugName);
        return 0;
    }
    return 1;
}

int FrameScript_CompileFunction(const char* body, const char* debugName) {
    if (!s_context || !body || !*body) return 0;
    std::string wrapped = "return function()\n";
    wrapped += body;
    wrapped += "\nend";
    if (luaL_loadbuffer(s_context, wrapped.c_str(), wrapped.size(), debugName ? debugName : "FrameScript") != 0 ||
        lua_pcall(s_context, 0, 1, 0) != 0) {
        ReportLuaError(s_context, debugName);
        return 0;
    }
    return luaL_ref(s_context, LUA_REGISTRYINDEX);
}

void FrameScript_ReleaseFunction(int functionReference) {
    if (s_context && functionReference) luaL_unref(s_context, LUA_REGISTRYINDEX, functionReference);
}
void FrameScript_Execute(const char* body, const char* debugName) {
    if (body) FrameScript_ExecuteBuffer(body, static_cast<unsigned>(std::strlen(body)), debugName);
}
void FrameScript_Execute(int functionReference) { FrameScript_Execute(functionReference, nullptr); }
void FrameScript_Execute(int functionReference, FrameScript_Object* object) {
    if (!s_context || !functionReference) return;
    lua_rawgeti(s_context, LUA_REGISTRYINDEX, functionReference);
    if (!lua_isfunction(s_context, -1)) { lua_pop(s_context, 1); return; }
    if (object) {
        if (!s_thisStack.empty()) s_thisStack.back()->UnregisterScriptObject("this");
        s_thisStack.push_back(object);
        object->RegisterScriptObject("this");
    }
    if (lua_pcall(s_context, 0, 0, 0) != 0) ReportLuaError(s_context, "FrameScript event");
    if (object) {
        object->UnregisterScriptObject("this");
        s_thisStack.pop_back();
        if (!s_thisStack.empty()) s_thisStack.back()->RegisterScriptObject("this");
    }
}
void FrameScript_Execute(int functionReference, FrameScript_Object* object, const char* format, ...) {
    va_list arguments; va_start(arguments, format);
    FrameScript_ExecuteV(functionReference, object, format, arguments);
    va_end(arguments);
}
void FrameScript_ExecuteV(int functionReference, FrameScript_Object* object,
                          const char* format, va_list arguments) {
    if (!s_context || !functionReference) return;
    va_list copy; va_copy(copy, arguments); PushArguments(s_context, format, copy); va_end(copy);
    FrameScript_Execute(functionReference, object);
}

FrameScript_Object::FrameScript_Object() = default;
FrameScript_Object::~FrameScript_Object() {
    UnregisterAllScriptEvents();
    if (m_onEvent) FrameScript_ReleaseFunction(m_onEvent);
    UnregisterScriptObject(m_scriptObjectName.c_str());
}
void FrameScript_Object::RegisterScriptObject(const char* name) {
    if (!s_context) return;
    if (m_luaRegistered > 0 && m_luaObjectRef >= 0)
        lua_rawgeti(s_context, LUA_REGISTRYINDEX, m_luaObjectRef);
    else
        CreateScriptObjectTable(s_context, this);
    if (!name || !*name) return;
    if (m_luaRegistered == 0) {
        m_luaObjectRef = luaL_ref(s_context, LUA_REGISTRYINDEX);
        lua_rawgeti(s_context, LUA_REGISTRYINDEX, m_luaObjectRef);
    }
    ++m_luaRegistered;
    if (m_scriptObjectName.empty() && std::strcmp(name, "this") != 0) m_scriptObjectName = name;
    SetGlobal(s_context, name);
}
void FrameScript_Object::UnregisterScriptObject(const char* name) {
    if (!s_context || !name || !*name) return;
    if (m_luaRegistered <= 0) return;
    FrameScript_UnsetVariable(name);
    if (--m_luaRegistered == 0) {
        luaL_unref(s_context, LUA_REGISTRYINDEX, m_luaObjectRef);
        m_luaObjectRef = -2;
    }
    if (m_scriptObjectName == name) m_scriptObjectName.clear();
}
int FrameScript_Object::RegisterScriptEvent(const char* eventName) {
    const int index = FindEvent(eventName);
    if (index < 0) return 0;
    auto& listeners = s_events[index].listeners;
    if (std::find(listeners.begin(), listeners.end(), this) == listeners.end()) listeners.push_back(this);
    if (std::find(m_scriptEvents.begin(), m_scriptEvents.end(), static_cast<unsigned>(index)) == m_scriptEvents.end())
        m_scriptEvents.push_back(static_cast<unsigned>(index));
    return 1;
}
void FrameScript_Object::UnregisterScriptEvent(const char* eventName) {
    const int index = FindEvent(eventName);
    if (index < 0) return;
    auto& listeners = s_events[index].listeners;
    listeners.erase(std::remove(listeners.begin(), listeners.end(), this), listeners.end());
    m_scriptEvents.erase(std::remove(m_scriptEvents.begin(), m_scriptEvents.end(), static_cast<unsigned>(index)),
                         m_scriptEvents.end());
}
void FrameScript_Object::UnregisterAllScriptEvents() {
    for (unsigned index : m_scriptEvents) {
        if (index >= s_events.size()) continue;
        auto& listeners = s_events[index].listeners;
        listeners.erase(std::remove(listeners.begin(), listeners.end(), this), listeners.end());
    }
    m_scriptEvents.clear();
}
void FrameScript_Object::SetEventScript(int& slot, const char* body, const char* debugName) {
    if (slot) FrameScript_ReleaseFunction(slot);
    slot = body && *body ? FrameScript_CompileFunction(body, debugName) : 0;
}
void FrameScript_Object::OnScriptEvent(const char* eventName) {
    if (!m_onEvent || !s_context) return;
    FrameScript_SetVariable("event", eventName);
    FrameScript_Execute(m_onEvent, this);
}
void FrameScript_Object::OnScriptEvent(const char* eventName, const char* format, va_list arguments) {
    if (!m_onEvent || !s_context) return;
    FrameScript_SetVariable("event", eventName);
    FrameScript_ExecuteV(m_onEvent, this, format, arguments);
}
void FrameScript_Object::FillScriptMethodTable(std::vector<FrameScript_Method>& table,
                                               std::initializer_list<FrameScript_Method> methods) {
    table.assign(methods.begin(), methods.end());
}
void FrameScript_Object::EmptyScriptMethodTable(std::vector<FrameScript_Method>& table) { table.clear(); }
int FrameScript_Object::LookupScriptMethod(lua_State* state) {
    auto* object = GetScriptObject(state, 1);
    const char* name = lua_tostring(state, 2);
    if (!object) { lua_pushnil(state); return 1; }
    const int result = object->LookupScriptMethod(state, name);
    if (result) return result;
    if (dynamic_cast<CSimpleButton*>(object)) {
        const int buttonResult = LookupScriptMethod(state, name, CSimpleButton::s_scriptMethods);
        if (buttonResult) return buttonResult;
    }
    if (dynamic_cast<CSimpleFrame*>(object)) {
        const int baseResult = LookupScriptMethod(state, name, CSimpleFrame::s_scriptMethods);
        if (baseResult) return baseResult;
    }
    lua_pushnil(state);
    return 1;
}
int FrameScript_Object::LookupScriptMethod(lua_State* state, const char* name,
                                           const std::vector<FrameScript_Method>& table) {
    if (!state || !name) return 0;
    for (const auto& method : table) {
        if (method.name && method.function && std::strcmp(method.name, name) == 0) {
            lua_pushcfunction(state, method.function);
            return 1;
        }
    }
    return 0;
}
int FrameScript_Object::LookupScriptMethod(lua_State*, const char*) { return 0; }

int FrameScript_DispatchBuiltin(lua_State* state, const char* functionName) {
    constexpr float kScriptToDdc = 0.0009765625f * 0.800000011920929f;
    constexpr float kDdcToScript = 1024.0f * 1.25f;
    if (!state || !functionName) return 0;
    auto* object = GetScriptObject(state, 1);
    auto* frame = dynamic_cast<CSimpleFrame*>(object);
    auto* region = dynamic_cast<CSimpleRegion*>(object);
    auto* texture = dynamic_cast<CSimpleTexture*>(region);
    auto* fontString = dynamic_cast<CSimpleFontString*>(region);

    if (fontString && std::strcmp(functionName, "CSimpleFontString_SetText") == 0) {
        fontString->SetText(lua_isnil(state, 2) ? "" : luaL_checkstring(state, 2));
        return 0;
    }
    if (fontString && std::strcmp(functionName, "CSimpleFontString_GetText") == 0) {
        lua_pushstring(state, fontString->GetText());
        return 1;
    }
    if (fontString && std::strcmp(functionName, "CSimpleFontString_GetWidth") == 0) {
        lua_pushnumber(state, fontString->GetWidth() * kDdcToScript);
        return 1;
    }
    if (fontString && std::strcmp(functionName, "CSimpleFontString_GetHeight") == 0) {
        lua_pushnumber(state, fontString->GetHeight() * kDdcToScript);
        return 1;
    }
    if (region && (std::strcmp(functionName, "CSimpleTexture_Show") == 0 ||
                   std::strcmp(functionName, "CSimpleFontString_Show") == 0)) {
        region->Show();
        return 0;
    }
    if (region && (std::strcmp(functionName, "CSimpleTexture_Hide") == 0 ||
                   std::strcmp(functionName, "CSimpleFontString_Hide") == 0)) {
        region->Hide();
        return 0;
    }
    if (region && (std::strcmp(functionName, "CSimpleTexture_IsVisible") == 0 ||
                   std::strcmp(functionName, "CSimpleFontString_IsVisible") == 0)) {
        lua_pushboolean(state, region->IsVisible());
        return 1;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_GetAlpha") == 0) {
        lua_pushnumber(state, texture->GetAlpha());
        return 1;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_GetName") == 0) {
        const char* name = texture->GetName();
        if (name && *name) lua_pushstring(state, name);
        else lua_pushnil(state);
        return 1;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_SetVertexColor") == 0) {
        const auto channel = [](lua_Number value) {
            return static_cast<unsigned>(
                std::clamp(value, 0.0, 1.0) * 255.0 + 0.5);
        };
        const unsigned r = channel(lua_tonumber(state, 2));
        const unsigned g = channel(lua_tonumber(state, 3));
        const unsigned b = channel(lua_tonumber(state, 4));
        const unsigned a = lua_isnumber(state, 5)
            ? channel(lua_tonumber(state, 5)) : 255U;
        texture->SetVertexColor(
            NTempest::CImVector((a << 24) | (r << 16) | (g << 8) | b));
        return 0;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_SetAlpha") == 0) {
        texture->SetAlpha(static_cast<float>(luaL_checknumber(state, 2)));
        return 0;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_SetTexture") == 0) {
        if (lua_isstring(state, 2)) {
            texture->SetTexture(lua_tostring(state, 2), 0);
        } else {
            const auto channel = [](lua_Number value) {
                return static_cast<unsigned>(
                    std::clamp(value, 0.0, 1.0) * 255.0 + 0.5);
            };
            const unsigned r = channel(lua_tonumber(state, 2));
            const unsigned g = channel(lua_tonumber(state, 3));
            const unsigned b = channel(lua_tonumber(state, 4));
            const unsigned a = lua_isnumber(state, 5)
                ? channel(lua_tonumber(state, 5)) : 255U;
            const NTempest::CImVector color(
                (a << 24) | (r << 16) | (g << 8) | b);
            texture->SetTexture(&color);
        }
        return 0;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_SetTexCoord") == 0) {
        texture->SetTexCoord(
            static_cast<float>(lua_tonumber(state, 2)),
            static_cast<float>(lua_tonumber(state, 3)),
            static_cast<float>(lua_tonumber(state, 4)),
            static_cast<float>(lua_tonumber(state, 5)));
        return 0;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_ClearAllPoints") == 0) {
        texture->ClearAllPoints();
        return 0;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_GetWidth") == 0) {
        lua_pushnumber(state, texture->GetWidth() * kDdcToScript);
        return 1;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_SetWidth") == 0) {
        texture->SetWidth(static_cast<float>(luaL_checknumber(state, 2)) * kScriptToDdc);
        return 0;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_GetHeight") == 0) {
        lua_pushnumber(state, texture->GetHeight() * kDdcToScript);
        return 1;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_SetHeight") == 0) {
        texture->SetHeight(static_cast<float>(luaL_checknumber(state, 2)) * kScriptToDdc);
        return 0;
    }
    if (texture && std::strcmp(functionName, "CSimpleTexture_SetPoint") == 0) {
        const auto framePoint = [](const char* value, FRAMEPOINT fallback) {
            if (!value) return fallback;
            if (_stricmp(value, "TOPLEFT") == 0) return FRAMEPOINT_TOPLEFT;
            if (_stricmp(value, "TOP") == 0) return FRAMEPOINT_TOP;
            if (_stricmp(value, "TOPRIGHT") == 0) return FRAMEPOINT_TOPRIGHT;
            if (_stricmp(value, "LEFT") == 0) return FRAMEPOINT_LEFT;
            if (_stricmp(value, "CENTER") == 0) return FRAMEPOINT_CENTER;
            if (_stricmp(value, "RIGHT") == 0) return FRAMEPOINT_RIGHT;
            if (_stricmp(value, "BOTTOMLEFT") == 0) return FRAMEPOINT_BOTTOMLEFT;
            if (_stricmp(value, "BOTTOM") == 0) return FRAMEPOINT_BOTTOM;
            if (_stricmp(value, "BOTTOMRIGHT") == 0) return FRAMEPOINT_BOTTOMRIGHT;
            return fallback;
        };
        const FRAMEPOINT point = framePoint(
            luaL_checkstring(state, 2), FRAMEPOINT_CENTER);
        CLayoutFrame* relative = texture->m_frame;
        if (lua_istable(state, 3))
            relative = dynamic_cast<CLayoutFrame*>(GetScriptObject(state, 3));
        else if (lua_isstring(state, 3))
            relative = texture->GetLayoutFrameByName(nullptr, lua_tostring(state, 3));
        const FRAMEPOINT relativePoint = lua_isstring(state, 4)
            ? framePoint(lua_tostring(state, 4), point) : point;
        const float x = lua_isnumber(state, 5)
            ? static_cast<float>(lua_tonumber(state, 5)) * kScriptToDdc : 0.0f;
        const float y = lua_isnumber(state, 6)
            ? static_cast<float>(lua_tonumber(state, 6)) * kScriptToDdc : 0.0f;
        if (relative)
            texture->SetPoint(point, relative, relativePoint, x, y);
        return 0;
    }
    if (!frame) return 0;
    auto* model = dynamic_cast<CSimpleModel*>(frame);
    auto* button = dynamic_cast<CSimpleButton*>(frame);
    auto* checkbox = dynamic_cast<CSimpleCheckbox*>(frame);
    auto* editBox = dynamic_cast<CSimpleEditBox*>(frame);
    auto* messageScroll = dynamic_cast<CSimpleMessageScrollFrame*>(frame);
    auto* statusBar = dynamic_cast<CSimpleStatusBar*>(frame);

    if (statusBar &&
        std::strcmp(functionName, "CSimpleStatusBar_SetMinMaxValues") == 0) {
        statusBar->SetMinMaxValues(
            static_cast<float>(luaL_checknumber(state, 2)),
            static_cast<float>(luaL_checknumber(state, 3)));
        return 0;
    }
    if (statusBar &&
        std::strcmp(functionName, "CSimpleStatusBar_GetMinMaxValues") == 0) {
        lua_pushnumber(state, statusBar->m_minValue);
        lua_pushnumber(state, statusBar->m_maxValue);
        return 2;
    }
    if (statusBar && std::strcmp(functionName, "CSimpleStatusBar_SetValue") == 0) {
        statusBar->SetValue(static_cast<float>(luaL_checknumber(state, 2)));
        return 0;
    }
    if (statusBar && std::strcmp(functionName, "CSimpleStatusBar_GetValue") == 0) {
        lua_pushnumber(state, statusBar->m_value);
        return 1;
    }
    if (statusBar &&
        std::strcmp(functionName, "CSimpleStatusBar_SetStatusBarColor") == 0) {
        const auto channel = [](lua_Number value) {
            return static_cast<unsigned>(
                std::clamp(value, 0.0, 1.0) * 255.0 + 0.5);
        };
        const unsigned r = channel(luaL_checknumber(state, 2));
        const unsigned g = channel(luaL_checknumber(state, 3));
        const unsigned b = channel(luaL_checknumber(state, 4));
        const unsigned a = lua_isnumber(state, 5)
            ? channel(lua_tonumber(state, 5)) : 255U;
        statusBar->SetStatusBarColor(
            NTempest::CImVector((a << 24) | (r << 16) | (g << 8) | b));
        return 0;
    }

    if (messageScroll && std::strcmp(functionName, "CSimpleMessageScrollFrame_AddMessage") == 0) {
        if (!lua_isstring(state, 2)) return 0;
        const char* message = lua_tostring(state, 2);
        if (!message || !*message) return 0;
        if (_stricmp(messageScroll->GetName(), "ChatFrame") == 0)
            std::fprintf(stderr, "CHAT_DISPLAY text=%s\n", message);
        if (!lua_isnumber(state, 3) || !lua_isnumber(state, 4) ||
            !lua_isnumber(state, 5)) {
            messageScroll->AddMessage(message, nullptr);
            return 0;
        }
        const auto channel = [](lua_Number value) {
            return static_cast<unsigned>(std::clamp(value, 0.0, 1.0) * 255.0 + 0.5);
        };
        CSimpleFontStringAttributes attrib = messageScroll->m_attrib;
        const unsigned r = channel(lua_tonumber(state, 3));
        const unsigned g = channel(lua_tonumber(state, 4));
        const unsigned b = channel(lua_tonumber(state, 5));
        const unsigned a = lua_isnumber(state, 6) ? channel(lua_tonumber(state, 6)) : 255U;
        attrib.m_color = NTempest::CImVector((a << 24) | (r << 16) | (g << 8) | b);
        attrib.m_flags = 31;
        messageScroll->AddMessage(message, &attrib);
        return 0;
    }
    if (messageScroll && std::strcmp(functionName, "CSimpleMessageScrollFrame_ScrollUp") == 0) {
        messageScroll->ScrollUp(); return 0;
    }
    if (messageScroll && std::strcmp(functionName, "CSimpleMessageScrollFrame_ScrollDown") == 0) {
        messageScroll->ScrollDown(); return 0;
    }
    if (messageScroll && std::strcmp(functionName, "CSimpleMessageScrollFrame_PageUp") == 0) {
        messageScroll->PageUp(); return 0;
    }
    if (messageScroll && std::strcmp(functionName, "CSimpleMessageScrollFrame_PageDown") == 0) {
        messageScroll->PageDown(); return 0;
    }
    if (messageScroll && std::strcmp(functionName, "CSimpleMessageScrollFrame_ScrollToTop") == 0) {
        messageScroll->ScrollToTop(); return 0;
    }
    if (messageScroll && std::strcmp(functionName, "CSimpleMessageScrollFrame_ScrollToBottom") == 0) {
        messageScroll->ScrollToBottom(); return 0;
    }
    if (messageScroll && std::strcmp(functionName, "CSimpleMessageScrollFrame_AtBottom") == 0) {
        if (messageScroll->IsAtBottom()) lua_pushnumber(state, 1.0);
        else lua_pushnil(state);
        return 1;
    }

    if (editBox && std::strcmp(functionName, "CSimpleEditBox_Insert") == 0) {
        if (lua_isstring(state, 2)) editBox->Insert(lua_tostring(state, 2));
        return 0;
    }
    if (editBox && std::strcmp(functionName, "CSimpleEditBox_SetText") == 0) {
        if (lua_gettop(state) != 2)
            return luaL_error(state, "Usage: SetText(\"text\")");
        editBox->SetText(lua_tostring(state, 2));
        return 0;
    }
    if (editBox && std::strcmp(functionName, "CSimpleEditBox_GetText") == 0) {
        lua_pushstring(state, editBox->m_text.c_str());
        return 1;
    }
    if (editBox && std::strcmp(functionName, "CSimpleEditBox_AddHistoryLine") == 0) {
        if (lua_gettop(state) != 2)
            return luaL_error(state, "Usage: AddHistoryLine(\"text\")");
        editBox->AddHistoryLine(lua_tostring(state, 2));
        return 0;
    }
    if (editBox && std::strcmp(functionName, "CSimpleEditBox_SetTextInsets") == 0) {
        const float left = static_cast<float>(luaL_checknumber(state, 2)) * kScriptToDdc;
        const float right = static_cast<float>(luaL_checknumber(state, 3)) * kScriptToDdc;
        const float top = static_cast<float>(luaL_checknumber(state, 4)) * kScriptToDdc;
        const float bottom = static_cast<float>(luaL_checknumber(state, 5)) * kScriptToDdc;
        if (_stricmp(editBox->GetName(), "ChatFrameEditBox") == 0) {
            std::fprintf(stderr,
                "CHAT_EDIT_INSETS script=(%.3f,%.3f,%.3f,%.3f) ddc=(%.6f,%.6f,%.6f,%.6f)\n",
                static_cast<double>(lua_tonumber(state, 2)),
                static_cast<double>(lua_tonumber(state, 3)),
                static_cast<double>(lua_tonumber(state, 4)),
                static_cast<double>(lua_tonumber(state, 5)), left, right, top, bottom);
        }
        editBox->SetEditTextInsets({left, top, right, bottom});
        return 0;
    }
    if (editBox && std::strcmp(functionName, "CSimpleEditBox_SetTextColor") == 0) {
        const auto channel = [](lua_Number value) {
            return static_cast<unsigned>(std::clamp(value, 0.0, 1.0) * 255.0 + 0.5);
        };
        const unsigned r = channel(luaL_checknumber(state, 2));
        const unsigned g = channel(luaL_checknumber(state, 3));
        const unsigned b = channel(luaL_checknumber(state, 4));
        const unsigned a = lua_isnumber(state, 5) ? channel(lua_tonumber(state, 5)) : 255u;
        if (editBox->m_string)
            editBox->m_string->SetTextColor(NTempest::CImVector((a << 24) | (r << 16) | (g << 8) | b));
        return 0;
    }
    if (editBox && std::strcmp(functionName, "CSimpleEditBox_SetFocus") == 0) {
        editBox->SetKeyboardFocus();
        return 0;
    }
    if (editBox && std::strcmp(functionName, "CSimpleEditBox_ClearFocus") == 0) {
        editBox->ClearKeyboardFocus();
        return 0;
    }

    if (checkbox && std::strcmp(functionName, "CSimpleCheckbox_SetChecked") == 0) {
        int checked = 0;
        if (lua_isnumber(state, 2)) {
            checked = static_cast<int>(lua_tonumber(state, 2));
        } else if (lua_isstring(state, 2)) {
            const char* value = lua_tostring(state, 2);
            checked = value && (_stricmp(value, "true") == 0 ||
                                _stricmp(value, "yes") == 0 ||
                                _stricmp(value, "on") == 0 ||
                                std::strcmp(value, "1") == 0);
        }
        checkbox->SetChecked(checked, 0);
        return 0;
    }
    if (checkbox && std::strcmp(functionName, "CSimpleCheckbox_GetChecked") == 0) {
        if (checkbox->m_checked) lua_pushnumber(state, 1.0);
        else lua_pushnil(state);
        return 1;
    }
    if (checkbox && std::strcmp(functionName, "CSimpleCheckbox_SetCheckedTexture") == 0) {
        checkbox->SetCheckedTexture(luaL_checkstring(state, 2));
        return 0;
    }
    if (checkbox && std::strcmp(functionName, "CSimpleCheckbox_SetDisabledCheckedTexture") == 0) {
        checkbox->SetDisabledCheckedTexture(luaL_checkstring(state, 2));
        return 0;
    }

    if (button && std::strcmp(functionName, "CSimpleButton_Enable") == 0) {
        button->Enable(true);
        return 0;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_Disable") == 0) {
        button->Enable(false);
        return 0;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_IsEnabled") == 0) {
        lua_pushnumber(state, button->m_state != BUTTONSTATE_DISABLED ? 1.0 : 0.0);
        return 1;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_GetButtonState") == 0) {
        const char* value = "UNKNOWN";
        switch (button->m_state) {
        case BUTTONSTATE_DISABLED: value = "DISABLED"; break;
        case BUTTONSTATE_NORMAL: value = "NORMAL"; break;
        case BUTTONSTATE_PUSHED: value = "PUSHED"; break;
        default: break;
        }
        lua_pushstring(state, value);
        return 1;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_SetButtonState") == 0) {
        const char* value = luaL_checkstring(state, 2);
        CSimpleButtonState buttonState = BUTTONSTATE_DISABLED;
        if (_stricmp(value, "NORMAL") == 0) buttonState = BUTTONSTATE_NORMAL;
        else if (_stricmp(value, "PUSHED") == 0) buttonState = BUTTONSTATE_PUSHED;
        else if (_stricmp(value, "DISABLED") != 0)
            return luaL_error(state, "Usage: SetButtonState(\"state\" [, lock])");
        bool lock = false;
        if (lua_isnumber(state, 3)) lock = lua_tonumber(state, 3) > 0.0;
        else if (lua_isstring(state, 3)) {
            const char* text = lua_tostring(state, 3);
            lock = text && (_stricmp(text, "true") == 0 ||
                            _stricmp(text, "yes") == 0 ||
                            _stricmp(text, "on") == 0 ||
                            std::strcmp(text, "1") == 0);
        }
        button->SetButtonState(buttonState, lock);
        return 0;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_SetText") == 0) {
        button->SetTextString(lua_isnil(state, 2) ? "" : luaL_checkstring(state, 2));
        return 0;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_GetText") == 0) {
        lua_pushstring(state, button->m_text ? button->m_text->GetText() : "");
        return 1;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_GetTextWidth") == 0) {
        lua_pushnumber(state, button->m_text ? button->m_text->GetStringWidth() * kDdcToScript : 0.0);
        return 1;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_GetTextHeight") == 0) {
        lua_pushnumber(state, button->m_text ? button->m_text->GetStringHeight() * kDdcToScript : 0.0);
        return 1;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_LockHighlight") == 0) {
        button->LockHighlight(1);
        return 0;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_UnlockHighlight") == 0) {
        button->LockHighlight(0);
        return 0;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_Click") == 0) {
        MOUSEBUTTON click = MOUSE_BUTTON_LEFT;
        if (lua_isstring(state, 2)) {
            const char* value = lua_tostring(state, 2);
            if (_stricmp(value, "MiddleButton") == 0) click = MOUSE_BUTTON_MIDDLE;
            else if (_stricmp(value, "RightButton") == 0) click = MOUSE_BUTTON_RIGHT;
            else if (_stricmp(value, "LeftButton") != 0) click = MOUSE_BUTTON_NONE;
        }
        button->OnClick(click);
        return 0;
    }
    if (button && std::strcmp(functionName, "CSimpleButton_RegisterForClicks") == 0) {
        unsigned actions = 0;
        for (int index = 2; lua_isstring(state, index); ++index) {
            const char* value = lua_tostring(state, index);
            if (_stricmp(value, "LeftButtonDown") == 0) actions |= 1U;
            else if (_stricmp(value, "MiddleButtonDown") == 0) actions |= 2U;
            else if (_stricmp(value, "RightButtonDown") == 0) actions |= 4U;
            else if (_stricmp(value, "Button4Down") == 0) actions |= 8U;
            else if (_stricmp(value, "Button5Down") == 0) actions |= 16U;
            else if (_stricmp(value, "LeftButtonUp") == 0) actions |= 256U;
            else if (_stricmp(value, "MiddleButtonUp") == 0) actions |= 512U;
            else if (_stricmp(value, "RightButtonUp") == 0) actions |= 1024U;
            else if (_stricmp(value, "Button4Up") == 0) actions |= 2048U;
            else if (_stricmp(value, "Button5Up") == 0) actions |= 4096U;
        }
        button->SetClickAction(actions);
        return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetModel") == 0) {
        const char* filename = luaL_checkstring(state, 2);
        model->SetModel(filename);
        return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_ClearModel") == 0) {
        model->ClearModel(); return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetPosition") == 0) {
        model->SetPosition(NTempest::C3Vector(
            static_cast<float>(luaL_checknumber(state, 2)),
            static_cast<float>(luaL_checknumber(state, 3)),
            static_cast<float>(luaL_checknumber(state, 4))));
        return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetFacing") == 0) {
        model->SetFacing(static_cast<float>(luaL_checknumber(state, 2))); return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetScale") == 0) {
        model->SetScale(static_cast<float>(luaL_checknumber(state, 2))); return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetSequence") == 0) {
        model->SetSequence(static_cast<unsigned>(luaL_checknumber(state, 2))); return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetSequenceTime") == 0) {
        model->SetSequenceTime(static_cast<unsigned>(luaL_checknumber(state, 2)),
                               static_cast<float>(luaL_checknumber(state, 3)));
        return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetAlpha") == 0) {
        model->SetAlpha(static_cast<float>(luaL_checknumber(state, 2))); return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetCamera") == 0) {
        model->SetCameraByIndex(static_cast<unsigned>(luaL_checknumber(state, 2))); return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetLight") == 0) {
        CGxLight light{};
        if (lua_isnumber(state, 2) && lua_tonumber(state, 2) != 0.0) {
            if (lua_gettop(state) >= 7) {
                light.m_dir = NTempest::C3Vector(
                    static_cast<float>(lua_tonumber(state, 4)),
                    static_cast<float>(lua_tonumber(state, 5)),
                    static_cast<float>(lua_tonumber(state, 6)));
                light.m_ambIntensity = static_cast<float>(lua_tonumber(state, 7));
            }
        } else {
            light.m_ambIntensity = 0.0f;
            light.m_dirIntensity = 0.0f;
        }
        model->SetLight(&light);
        return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_GetPosition") == 0) {
        lua_pushnumber(state, model->m_position.x);
        lua_pushnumber(state, model->m_position.y);
        lua_pushnumber(state, model->m_position.z);
        return 3;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_GetFacing") == 0) {
        lua_pushnumber(state, model->m_facing); return 1;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_GetScale") == 0) {
        lua_pushnumber(state, model->m_scale); return 1;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_AdvanceTime") == 0) {
        const float elapsed = lua_isnumber(state, 2)
            ? static_cast<float>(lua_tonumber(state, 2)) : 0.0f;
        model->AdvanceTime(elapsed);
        return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_ReplaceIconTexture") == 0) {
        (void)luaL_checkstring(state, 2);
        return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetFogColor") == 0) {
        const auto channel = [](lua_Number value) {
            return static_cast<unsigned>(std::clamp(value, 0.0, 1.0) * 255.0 + 0.5);
        };
        const unsigned r = channel(lua_tonumber(state, 2));
        const unsigned g = channel(lua_tonumber(state, 3));
        const unsigned b = channel(lua_tonumber(state, 4));
        const unsigned a = lua_isnumber(state, 5) ? channel(lua_tonumber(state, 5)) : 255u;
        model->SetFogColor(NTempest::CImVector((a << 24) | (r << 16) | (g << 8) | b));
        model->m_fogEnabled = true;
        return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetFogNear") == 0) {
        model->m_fogNear = static_cast<float>(luaL_checknumber(state, 2)); return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_SetFogFar") == 0) {
        model->m_fogFar = static_cast<float>(luaL_checknumber(state, 2)); return 0;
    }
    if (model && std::strcmp(functionName, "CSimpleModel_ClearFog") == 0) {
        model->m_fogEnabled = false; return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_GetParent") == 0) {
        PushScriptObject(state, frame->ParentFrame()); return 1;
    }
    if (std::strcmp(functionName, "CSimpleFrame_GetName") == 0) {
        lua_pushstring(state, frame->GetName()); return 1;
    }
    if (std::strcmp(functionName, "CSimpleFrame_GetFrameLevel") == 0) {
        lua_pushnumber(state, frame->m_level); return 1;
    }
    if (std::strcmp(functionName, "CSimpleFrame_SetFrameLevel") == 0) {
        frame->SetFrameLevel(static_cast<int>(luaL_checknumber(state, 2))); return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_RegisterEvent") == 0)
        return object->RegisterScriptEvent(luaL_checkstring(state, 2)) ? 0 : 0;
    if (std::strcmp(functionName, "CSimpleFrame_UnregisterEvent") == 0) {
        object->UnregisterScriptEvent(luaL_checkstring(state, 2)); return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_Show") == 0) {
        frame->m_shown = true;
        frame->ShowThis();
        return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_Hide") == 0) {
        frame->m_shown = false;
        frame->HideThis();
        return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_IsShown") == 0) {
        lua_pushboolean(state, frame->m_shown); return 1;
    }
    if (std::strcmp(functionName, "CSimpleFrame_IsVisible") == 0) {
        lua_pushboolean(state, frame->m_visible); return 1;
    }
    if (std::strcmp(functionName, "CSimpleFrame_GetWidth") == 0) {
        lua_pushnumber(state, frame->GetWidth() * kDdcToScript); return 1;
    }
    if (std::strcmp(functionName, "CSimpleFrame_SetWidth") == 0) {
        frame->SetWidth(static_cast<float>(luaL_checknumber(state, 2)) * kScriptToDdc); return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_GetHeight") == 0) {
        lua_pushnumber(state, frame->GetHeight() * kDdcToScript); return 1;
    }
    if (std::strcmp(functionName, "CSimpleFrame_SetHeight") == 0) {
        frame->SetHeight(static_cast<float>(luaL_checknumber(state, 2)) * kScriptToDdc); return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_ClearAllPoints") == 0) {
        frame->ClearAllPoints();
        return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_SetAllPoints") == 0) {
        CSimpleFrame* relative = frame->ParentFrame();
        if (lua_istable(state, 2))
            relative = dynamic_cast<CSimpleFrame*>(GetScriptObject(state, 2));
        else if (lua_isstring(state, 2))
            relative = dynamic_cast<CSimpleFrame*>(frame->GetLayoutFrameByName(nullptr,
                                                                                lua_tostring(state, 2)));
        if (relative)
            frame->SetAllPoints(relative);
        return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_SetPoint") == 0) {
        const auto framePoint = [](const char* value, FRAMEPOINT fallback) {
            if (!value) return fallback;
            if (_stricmp(value, "TOPLEFT") == 0) return FRAMEPOINT_TOPLEFT;
            if (_stricmp(value, "TOP") == 0) return FRAMEPOINT_TOP;
            if (_stricmp(value, "TOPRIGHT") == 0) return FRAMEPOINT_TOPRIGHT;
            if (_stricmp(value, "LEFT") == 0) return FRAMEPOINT_LEFT;
            if (_stricmp(value, "CENTER") == 0) return FRAMEPOINT_CENTER;
            if (_stricmp(value, "RIGHT") == 0) return FRAMEPOINT_RIGHT;
            if (_stricmp(value, "BOTTOMLEFT") == 0) return FRAMEPOINT_BOTTOMLEFT;
            if (_stricmp(value, "BOTTOM") == 0) return FRAMEPOINT_BOTTOM;
            if (_stricmp(value, "BOTTOMRIGHT") == 0) return FRAMEPOINT_BOTTOMRIGHT;
            return fallback;
        };
        const FRAMEPOINT point = framePoint(luaL_checkstring(state, 2), FRAMEPOINT_CENTER);
        CSimpleFrame* relative = frame->ParentFrame();
        if (lua_istable(state, 3))
            relative = dynamic_cast<CSimpleFrame*>(GetScriptObject(state, 3));
        else if (lua_isstring(state, 3))
            relative = dynamic_cast<CSimpleFrame*>(frame->GetLayoutFrameByName(nullptr,
                                                                                lua_tostring(state, 3)));
        const FRAMEPOINT relativePoint = lua_isstring(state, 4)
            ? framePoint(lua_tostring(state, 4), point) : point;
        const float x = lua_isnumber(state, 5)
            ? static_cast<float>(lua_tonumber(state, 5)) * kScriptToDdc : 0.0f;
        const float y = lua_isnumber(state, 6)
            ? static_cast<float>(lua_tonumber(state, 6)) * kScriptToDdc : 0.0f;
        if (relative)
            frame->SetPoint(point, relative, relativePoint, x, y);
        return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_SetAlpha") == 0) {
        frame->SetAlpha(static_cast<float>(luaL_checknumber(state, 2))); return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_GetAlpha") == 0) {
        lua_pushnumber(state, frame->m_alpha / 255.0); return 1;
    }
    if (std::strcmp(functionName, "CSimpleFrame_SetID") == 0) {
        frame->m_id = static_cast<int>(luaL_checknumber(state, 2));
        return 0;
    }
    if (std::strcmp(functionName, "CSimpleFrame_GetID") == 0) {
        lua_pushnumber(state, frame->m_id); return 1;
    }
    return 0;
}
