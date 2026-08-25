#pragma once

#include "ENGINE/Source/Event/event.h"

#include <string>
#include <unordered_map>
#include <vector>

struct lua_State;

struct KEYBINDING {
    int index = 0;
    std::string key;
    std::string command;
};

struct KEYCOMMAND {
    int index = 0;
    int headerIndex = 0;
    int function = 0;
    int runOnUp = 0;
    std::string command;
};

class CGUIBindings {
public:
    CGUIBindings();
    ~CGUIBindings();

    static CGUIBindings* Get();
    static CGUIBindings* s_bindings;

    void AddMetaPrefix();
    void AdjustCommandKeyIndices(const char* command, int index);
    bool Bind(const char* key, const char* command);
    int ExecCommand(const char* command, unsigned timestamp, int down);
    int ExecKey(const char* keystring, unsigned timestamp, int down);
    const char* GetCommand(int index) const;
    const char* GetCommandAction(const char* keystring) const;
    const char* GetCommandKey(const char* command, int keyindex) const;
    const char* GetHiddenCommand(int index) const;
    int GetNumCommandKeys(const char* command) const;
    void Initialize();
    static char* KeyEventToString(const CKeyEvent* event, char* string,
                                  unsigned maxLen);
    void Load();
    void LoadBindings(const char* accountBindings = nullptr);
    static char* MouseEventToString(const CMouseEvent* event, char* string,
                                    unsigned maxLen);
    void SaveBindings(int accountBindings = 0);
    void Shutdown();

private:
    static std::string Normalize(const char* value);
    void BindDefaults();
    bool LoadBindingFile(const char* filename);
    bool LoadCommands(const char* filename);
    const KEYCOMMAND* FindCommand(const char* command) const;

    std::unordered_map<std::string, std::string> m_bindings;
    std::vector<KEYBINDING> m_bindingList;
    std::vector<KEYCOMMAND> m_commands;
};

class CGUIBindingsStatus {
public:
    void Add();
};

int Bind_CommandHandler(const char* arguments, char* result = nullptr);
int Script_DefaultBindings(lua_State* state);
int Script_GetBinding(lua_State* state);
int Script_GetBindingAction(lua_State* state);
int Script_GetBindingKey(lua_State* state);
int Script_GetNumBindings(lua_State* state);
int Script_ResetBindings(lua_State* state);
int Script_RunBinding(lua_State* state);
int Script_SaveBindings(lua_State* state);
int Script_SetBinding(lua_State* state);
void UIBindingsRegisterScriptFunctions();
void UIBindingsUnegisterScriptFunctions();
