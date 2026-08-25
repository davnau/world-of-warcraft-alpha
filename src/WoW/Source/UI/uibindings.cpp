#include "WoW/Source/UI/uibindings.h"

#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "Storm/h/storm.h"
#include "WoW/Source/UIUtil/camera.h"
#include "WoW/Source/UIUtil/inputcontrol.h"
#include "WoW/Source/WorldClient/World.h"

extern "C" {
#include "lauxlib.h"
}

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string_view>
#include <utility>

CGUIBindings* CGUIBindings::s_bindings = nullptr;

namespace {
struct CommandControl {
    const char* command;
    INPUT_CONTROL control;
};

constexpr CommandControl kControlCommands[] = {
    {"MOVEFORWARD", INPUT_MOVE_PLAYER_FORWARD_KEY},
    {"MOVEBACKWARD", INPUT_MOVE_PLAYER_BACKWARD_KEY},
    {"TURNLEFT", INPUT_TURN_PLAYER_LEFT_KEY},
    {"TURNRIGHT", INPUT_TURN_PLAYER_RIGHT_KEY},
    {"STRAFELEFT", INPUT_STRAFE_PLAYER_LEFT_KEY},
    {"STRAFERIGHT", INPUT_STRAFE_PLAYER_RIGHT_KEY},
    {"PITCHUP", INPUT_PITCH_PLAYER_UP_KEY},
    {"PITCHDOWN", INPUT_PITCH_PLAYER_DOWN_KEY},
    {"TURNORACTION", INPUT_TURN_PLAYER},
    {"CAMERAORSELECTORMOVE", INPUT_MOVE_PLAYER_OR_TURN_CAMERA},
};

void ExecuteUI(const char* body, const char* name) {
    FrameScript_Execute(body, name);
}

void AppendMetaPrefix(unsigned meta, std::string& text) {
    if (meta & (1U << KEY_SHIFT)) text += "SHIFT-";
    if (meta & (1U << KEY_CONTROL)) text += "CTRL-";
    if (meta & (1U << KEY_ALT)) text += "ALT-";
}

bool LoadTextFile(const char* filename, std::string& text) {
    if (!filename || !*filename) return false;
    void* buffer = nullptr;
    unsigned bytes = 0;
    if (!SFile::LoadFile(filename, &buffer, &bytes, 1U, nullptr) || !buffer)
        return false;
    text.assign(static_cast<const char*>(buffer), bytes);
    SFile::Unload(buffer);
    return true;
}

std::string Trim(std::string_view value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front())))
        value.remove_prefix(1);
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back())))
        value.remove_suffix(1);
    return std::string(value);
}

std::string XmlAttribute(std::string_view tag, std::string_view name) {
    std::size_t position = 0;
    while ((position = tag.find(name, position)) != std::string_view::npos) {
        const bool leftBoundary = position == 0 ||
            std::isspace(static_cast<unsigned char>(tag[position - 1]));
        std::size_t cursor = position + name.size();
        while (cursor < tag.size() &&
               std::isspace(static_cast<unsigned char>(tag[cursor]))) ++cursor;
        if (!leftBoundary || cursor >= tag.size() || tag[cursor] != '=') {
            position += name.size();
            continue;
        }
        ++cursor;
        while (cursor < tag.size() &&
               std::isspace(static_cast<unsigned char>(tag[cursor]))) ++cursor;
        if (cursor >= tag.size() || (tag[cursor] != '\'' && tag[cursor] != '"'))
            return {};
        const char quote = tag[cursor++];
        const std::size_t end = tag.find(quote, cursor);
        return end == std::string_view::npos
            ? std::string{} : std::string(tag.substr(cursor, end - cursor));
    }
    return {};
}

bool IsTrue(std::string_view value) {
    return _stricmp(std::string(value).c_str(), "true") == 0 || value == "1";
}

const char* KeyName(KEY key, char* storage, unsigned bytes) {
    if (key >= KEY_A && key <= KEY_Z) {
        storage[0] = static_cast<char>(key);
        storage[1] = 0;
        return storage;
    }
    if (key >= KEY_0 && key <= KEY_0 + 9) {
        storage[0] = static_cast<char>(key);
        storage[1] = 0;
        return storage;
    }
    if (key >= KEY_F1 && key <= KEY_F12) {
        std::snprintf(storage, bytes, "F%d", static_cast<int>(key - KEY_F1 + 1));
        return storage;
    }
    if (key >= KEY_NUMPAD0 && key <= KEY_NUMPAD9) {
        std::snprintf(storage, bytes, "NUMPAD%d", static_cast<int>(key - KEY_NUMPAD0));
        return storage;
    }
    switch (key) {
    case KEY_SPACE: return "SPACE";
    case KEY_SHIFT: return "SHIFT";
    case KEY_CONTROL: return "CTRL";
    case KEY_ALT: return "ALT";
    case KEY_GRAVE: return "TILDE";
    case KEY_NUMPADPLUS: return "NUMPADPLUS";
    case KEY_NUMPADMINUS: return "NUMPADMINUS";
    case KEY_NUMPADMULTIPLY: return "NUMPADMULTIPLY";
    case KEY_NUMPADDIVIDE: return "NUMPADDIVIDE";
    case KEY_NUMPADDECIMAL: return "NUMPADDECIMAL";
    case KEY_EQUALS: return "PLUS";
    case KEY_MINUS: return "MINUS";
    case KEY_LEFTBRACKET: return "LEFTBRACKET";
    case KEY_RIGHTBRACKET: return "RIGHTBRACKET";
    case KEY_SLASH: return "SLASH";
    case KEY_BACKSLASH: return "BACKSLASH";
    case KEY_SEMICOLON: return "SEMICOLON";
    case KEY_APOSTROPHE: return "APOSTROPHE";
    case KEY_COMMA: return "COMMA";
    case KEY_PERIOD: return "PERIOD";
    case KEY_ESCAPE: return "ESCAPE";
    case KEY_ENTER: return "ENTER";
    case KEY_BACKSPACE: return "BACKSPACE";
    case KEY_TAB: return "TAB";
    case KEY_LEFT: return "LEFT";
    case KEY_UP: return "UP";
    case KEY_RIGHT: return "RIGHT";
    case KEY_DOWN: return "DOWN";
    case KEY_INSERT: return "INSERT";
    case KEY_DELETE: return "DELETE";
    case KEY_HOME: return "HOME";
    case KEY_END: return "END";
    case KEY_PAGEUP: return "PAGEUP";
    case KEY_PAGEDOWN: return "PAGEDOWN";
    case KEY_CAPSLOCK: return "CAPSLOCK";
    case KEY_NUMLOCK: return "NUMLOCK";
    case KEY_SCROLLLOCK: return "SCROLLLOCK";
    case KEY_PAUSE: return "PAUSE";
    case KEY_PRINTSCREEN: return "PRINTSCREEN";
    default: return nullptr;
    }
}
}

CGUIBindings::CGUIBindings() { Initialize(); }
CGUIBindings::~CGUIBindings() { Shutdown(); }

CGUIBindings* CGUIBindings::Get() {
    if (!s_bindings) s_bindings = new CGUIBindings();
    return s_bindings;
}

std::string CGUIBindings::Normalize(const char* value) {
    std::string result = value ? value : "";
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return result;
}

void CGUIBindings::Initialize() {
    Shutdown();
    Load();
}

void CGUIBindings::Load() {
    if (!LoadCommands("Interface\\Bindings.xml"))
        std::fprintf(stderr, "UIBindings: couldn't load Interface\\Bindings.xml\n");
    LoadBindings(nullptr);
    std::fprintf(stderr,
                 "UI_BINDINGS_LOAD commands=%zu bindings=%zu key1=%s keyB=%s\n",
                 m_commands.size(), m_bindingList.size(),
                 GetCommandAction("1"), GetCommandAction("B"));
}

bool CGUIBindings::LoadCommands(const char* filename) {
    std::string xml;
    if (!LoadTextFile(filename, xml)) return false;

    for (auto& command : m_commands)
        if (command.function) FrameScript_ReleaseFunction(command.function);
    m_commands.clear();

    int visibleIndex = 0;
    int hiddenIndex = 0;
    std::size_t position = 0;
    while ((position = xml.find("<Binding ", position)) != std::string::npos) {
        const std::size_t tagEnd = xml.find('>', position);
        if (tagEnd == std::string::npos) break;
        const std::size_t bodyEnd = xml.find("</Binding>", tagEnd + 1);
        if (bodyEnd == std::string::npos) break;

        const std::string_view tag(xml.data() + position, tagEnd - position + 1);
        const std::string name = Normalize(XmlAttribute(tag, "name").c_str());
        const std::string body = Trim(std::string_view(
            xml.data() + tagEnd + 1, bodyEnd - tagEnd - 1));
        if (!name.empty() && !body.empty() && !FindCommand(name.c_str())) {
            KEYCOMMAND command;
            const std::string header = XmlAttribute(tag, "header");
            command.headerIndex = header.empty() ? 0 : std::atoi(header.c_str());
            command.runOnUp = IsTrue(XmlAttribute(tag, "runOnUp")) ? 1 : 0;
            command.index = IsTrue(XmlAttribute(tag, "hidden"))
                ? --hiddenIndex : visibleIndex++;
            command.command = name;
            const std::string debugName = std::string(filename) + ":" + name;
            command.function = FrameScript_CompileFunction(body.c_str(), debugName.c_str());
            if (command.function) m_commands.push_back(std::move(command));
        }
        position = bodyEnd + std::strlen("</Binding>");
    }
    return !m_commands.empty();
}

bool CGUIBindings::LoadBindingFile(const char* filename) {
    std::string text;
    if (!LoadTextFile(filename, text)) return false;
    std::istringstream stream(text);
    std::string line;
    bool loaded = false;
    while (std::getline(stream, line)) {
        std::istringstream tokens(line);
        std::string verb, key, command;
        tokens >> verb >> key >> command;
        if (_stricmp(verb.c_str(), "bind") != 0 || key.empty() || command.empty())
            continue;
        Bind(key.c_str(), command.c_str());
        loaded = true;
    }
    return loaded;
}

void CGUIBindings::LoadBindings(const char* accountBindings) {
    m_bindings.clear();
    m_bindingList.clear();
    if (accountBindings && *accountBindings && LoadBindingFile(accountBindings)) return;
    if (!accountBindings && LoadBindingFile("bindings.wtf")) return;
    BindDefaults();
}

void CGUIBindings::SaveBindings(int) {
    std::ofstream output("bindings.wtf", std::ios::binary | std::ios::trunc);
    if (!output) return;
    for (const auto& binding : m_bindingList)
        output << "bind " << binding.key << ' ' << binding.command << "\r\n";
}
void CGUIBindings::AddMetaPrefix() {}
void CGUIBindings::AdjustCommandKeyIndices(const char*, int) {}

void CGUIBindings::BindDefaults() {
    m_bindings.clear();
    m_bindingList.clear();
    if (LoadBindingFile("WTF\\DefaultBindings.wtf")) return;
    const std::pair<const char*, const char*> defaults[] = {
        {"W", "MOVEFORWARD"}, {"S", "MOVEBACKWARD"},
        {"A", "TURNLEFT"}, {"D", "TURNRIGHT"},
        {"Q", "STRAFELEFT"}, {"E", "STRAFERIGHT"},
        {"UP", "MOVEFORWARD"}, {"DOWN", "MOVEBACKWARD"},
        {"LEFT", "TURNLEFT"}, {"RIGHT", "TURNRIGHT"},
        {"NUMLOCK", "TOGGLEAUTORUN"},
        {"BUTTON1", "CAMERAORSELECTORMOVE"},
        {"BUTTON2", "TURNORACTION"},
        {"MOUSEWHEELUP", "CAMERAZOOMIN"},
        {"MOUSEWHEELDOWN", "CAMERAZOOMOUT"},
        {"SPACE", "JUMP"}, {"NUMPAD0", "JUMP"},
        {"BUTTON4", "TOGGLEAUTORUN"},
        {"1", "ACTIONBUTTON1"}, {"2", "ACTIONBUTTON2"},
        {"3", "ACTIONBUTTON3"}, {"4", "ACTIONBUTTON4"},
        {"5", "ACTIONBUTTON5"}, {"6", "ACTIONBUTTON6"},
        {"7", "ACTIONBUTTON7"}, {"8", "ACTIONBUTTON8"},
        {"9", "ACTIONBUTTON9"}, {"0", "ACTIONBUTTON10"},
        {"MINUS", "ACTIONBUTTON11"}, {"PLUS", "ACTIONBUTTON12"},
        {"C", "TOGGLECHARACTER0"}, {"B", "TOGGLEBACKPACK"},
        {"SHIFT-B", "OPENALLBAGS"}, {"ESCAPE", "TOGGLEGAMEMENU"},
        {"ENTER", "OPENCHAT"}, {"SLASH", "OPENCHATSLASH"},
    };
    for (const auto& [key, command] : defaults) Bind(key, command);
}

bool CGUIBindings::Bind(const char* key, const char* command) {
    const std::string normalizedKey = Normalize(key);
    const std::string normalizedCommand = Normalize(command);
    if (normalizedKey.empty()) return false;
    if (normalizedCommand.empty()) {
        m_bindings.erase(normalizedKey);
        return true;
    }
    m_bindings[normalizedKey] = normalizedCommand;
    auto found = std::find_if(m_bindingList.begin(), m_bindingList.end(),
        [&](const KEYBINDING& binding) { return binding.key == normalizedKey; });
    if (found == m_bindingList.end()) {
        m_bindingList.push_back({static_cast<int>(m_bindingList.size()),
                                 normalizedKey, normalizedCommand});
    } else {
        found->command = normalizedCommand;
    }
    return true;
}

int CGUIBindings::ExecKey(const char* keystring, unsigned timestamp, int down) {
    if (!keystring || !*keystring) return 0;
    const auto found = m_bindings.find(Normalize(keystring));
    return found == m_bindings.end()
        ? 0 : ExecCommand(found->second.c_str(), timestamp, down);
}

int CGUIBindings::ExecCommand(const char* inputCommand, unsigned timestamp,
                              int down) {
    const std::string command = Normalize(inputCommand);
    if (command.empty()) return 0;
    if (const KEYCOMMAND* entry = FindCommand(command.c_str())) {
        std::fprintf(stderr,
                     "UI_BINDING command=%s down=%d compiled=1 runOnUp=%d\n",
                     command.c_str(), down, entry->runOnUp);
        if (!down && !entry->runOnUp) return 0;
        FrameScript_SetVariable("keystate", down ? "down" : "up");
        FrameScript_Execute(entry->function, nullptr, "%u", timestamp);
        FrameScript_UnsetVariable("keystate");
        return 1;
    }
    std::fprintf(stderr, "UI_BINDING command=%s down=%d compiled=0\n",
                 command.c_str(), down);
    for (const auto& entry : kControlCommands) {
        if (command == entry.command) {
            if (CGInputControl* input = CGInputControl::GetActive())
                input->SetControlBit(entry.control, down, timestamp, 0);
            return 1;
        }
    }
    if (command == "TOGGLEAUTORUN") {
        if (down) Script_ToggleAutoRun(FrameScript_GetContext());
        return down ? 1 : 0;
    }
    if (command == "JUMP") {
        if (down) CWorld::Jump(timestamp);
        return down ? 1 : 0;
    }
    if (command == "CAMERAZOOMIN") {
        if (down) { if (CGCamera* camera = Camera()) camera->ZoomIn(1.0F, timestamp); }
        return down ? 1 : 0;
    }
    if (command == "CAMERAZOOMOUT") {
        if (down) { if (CGCamera* camera = Camera()) camera->ZoomOut(1.0F, timestamp); }
        return down ? 1 : 0;
    }
    if (!down) return 0;
    if (command == "TOGGLECHARACTER0") {
        ExecuteUI("if CharacterFrame then ToggleCharacter('CharacterFrame') end",
                  "Binding:TOGGLECHARACTER0");
        return 1;
    }
    if (command == "TOGGLEBACKPACK") {
        ExecuteUI("if ToggleBackpack then ToggleBackpack() end",
                  "Binding:TOGGLEBACKPACK");
        return 1;
    }
    if (command == "OPENALLBAGS") {
        ExecuteUI("if OpenAllBags then OpenAllBags() end",
                  "Binding:OPENALLBAGS");
        return 1;
    }
    if (command == "OPENCHAT") {
        ExecuteUI("if ChatFrame_OpenChat then ChatFrame_OpenChat('') end",
                  "Binding:OPENCHAT");
        return 1;
    }
    if (command == "OPENCHATSLASH") {
        ExecuteUI("if ChatFrame_OpenChat then ChatFrame_OpenChat('/') end",
                  "Binding:OPENCHATSLASH");
        return 1;
    }
    if (command == "TOGGLEGAMEMENU") {
        ExecuteUI("if CharacterFrame and CharacterFrame:IsVisible() then "
                  "HideUIPanel(CharacterFrame) elseif ToggleGameMenu then "
                  "ToggleGameMenu() end", "Binding:TOGGLEGAMEMENU");
        return 1;
    }
    return 0;
}

char* CGUIBindings::KeyEventToString(const CKeyEvent* event, char* string,
                                     unsigned maxLen) {
    if (!event || !string || maxLen == 0 || event->key < 0 || event->repeat > 1)
        return nullptr;
    char local[16]{};
    const char* name = KeyName(event->key, local, sizeof(local));
    if (!name) return nullptr;
    std::string result;
    unsigned meta = event->metaKeyState;
    if (event->key >= KEY_SHIFT && event->key <= KEY_ALT)
        meta &= ~(1U << static_cast<unsigned>(event->key));
    AppendMetaPrefix(meta, result);
    result += name;
    strncpy_s(string, maxLen, result.c_str(), _TRUNCATE);
    return string;
}

char* CGUIBindings::MouseEventToString(const CMouseEvent* event, char* string,
                                       unsigned maxLen) {
    if (!event || !string || maxLen == 0) return nullptr;
    const char* name = nullptr;
    if (event->id == INPUT_EVENT_MOUSEWHEEL) {
        name = event->wheelDistance >= 0 ? "MOUSEWHEELUP" : "MOUSEWHEELDOWN";
    } else {
        switch (event->button) {
        case MOUSE_BUTTON_LEFT: name = "BUTTON1"; break;
        case MOUSE_BUTTON_RIGHT: name = "BUTTON2"; break;
        case MOUSE_BUTTON_MIDDLE: name = "BUTTON3"; break;
        case MOUSE_BUTTON_XBUTTON1: name = "BUTTON4"; break;
        case MOUSE_BUTTON_XBUTTON2: name = "BUTTON5"; break;
        default: return nullptr;
        }
    }
    std::string result;
    AppendMetaPrefix(event->metaKeyState, result);
    result += name;
    strncpy_s(string, maxLen, result.c_str(), _TRUNCATE);
    return string;
}

const KEYCOMMAND* CGUIBindings::FindCommand(const char* inputCommand) const {
    const std::string command = Normalize(inputCommand);
    const auto found = std::find_if(m_commands.begin(), m_commands.end(),
        [&](const KEYCOMMAND& entry) { return entry.command == command; });
    return found == m_commands.end() ? nullptr : &*found;
}

const char* CGUIBindings::GetCommand(int index) const {
    const auto found = std::find_if(m_commands.begin(), m_commands.end(),
        [&](const KEYCOMMAND& command) { return command.index == index; });
    return found == m_commands.end() ? nullptr : found->command.c_str();
}
const char* CGUIBindings::GetHiddenCommand(int index) const {
    const auto found = std::find_if(m_commands.begin(), m_commands.end(),
        [&](const KEYCOMMAND& command) { return command.index == -index - 1; });
    return found == m_commands.end() ? nullptr : found->command.c_str();
}
const char* CGUIBindings::GetCommandAction(const char* keystring) const {
    const auto found = m_bindings.find(Normalize(keystring));
    return found == m_bindings.end() ? "" : found->second.c_str();
}
const char* CGUIBindings::GetCommandKey(const char* command, int keyindex) const {
    const std::string normalized = Normalize(command);
    for (const auto& binding : m_bindingList) {
        if (binding.command == normalized && keyindex-- == 0)
            return binding.key.c_str();
    }
    return nullptr;
}
int CGUIBindings::GetNumCommandKeys(const char* command) const {
    const std::string normalized = Normalize(command);
    return static_cast<int>(std::count_if(m_bindingList.begin(), m_bindingList.end(),
        [&](const KEYBINDING& binding) { return binding.command == normalized; }));
}
void CGUIBindings::Shutdown() {
    for (auto& command : m_commands)
        if (command.function) FrameScript_ReleaseFunction(command.function);
    m_bindings.clear(); m_bindingList.clear(); m_commands.clear();
}
void CGUIBindingsStatus::Add() {}

int Bind_CommandHandler(const char*, char*) { return 0; }
int Script_DefaultBindings(lua_State*) { CGUIBindings::Get()->LoadBindings("WTF\\DefaultBindings.wtf"); return 0; }
int Script_GetBinding(lua_State* state) {
    const int index = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    if (const char* command = CGUIBindings::Get()->GetCommand(index)) {
        lua_pushstring(state, command); return 1;
    }
    return 0;
}
int Script_GetBindingAction(lua_State* state) {
    lua_pushstring(state, CGUIBindings::Get()->GetCommandAction(luaL_checkstring(state, 1)));
    return 1;
}
int Script_GetBindingKey(lua_State* state) {
    const char* command = luaL_checkstring(state, 1);
    int count = 0;
    for (int i = 0; i < 2; ++i) {
        if (const char* key = CGUIBindings::Get()->GetCommandKey(command, i)) {
            lua_pushstring(state, key); ++count;
        }
    }
    return count;
}
int Script_GetNumBindings(lua_State* state) {
    int count = 0;
    while (CGUIBindings::Get()->GetCommand(count)) ++count;
    lua_pushnumber(state, count);
    return 1;
}
int Script_ResetBindings(lua_State*) { CGUIBindings::Get()->LoadBindings("WTF\\DefaultBindings.wtf"); return 0; }
int Script_RunBinding(lua_State* state) {
    return CGUIBindings::Get()->ExecCommand(luaL_checkstring(state, 1),
                                             OsGetAsyncTimeMs(), 1) ? 0 : 0;
}
int Script_SaveBindings(lua_State*) { CGUIBindings::Get()->SaveBindings(); return 0; }
int Script_SetBinding(lua_State* state) {
    const char* key = luaL_checkstring(state, 1);
    const char* command = lua_gettop(state) >= 2 && !lua_isnil(state, 2)
        ? luaL_checkstring(state, 2) : "";
    lua_pushboolean(state, CGUIBindings::Get()->Bind(key, command));
    return 1;
}

namespace {
constexpr std::array<std::pair<const char*, lua_CFunction>, 9> kBindingScripts{{
    {"SetBinding", Script_SetBinding}, {"GetBindingKey", Script_GetBindingKey},
    {"GetBindingAction", Script_GetBindingAction}, {"GetNumBindings", Script_GetNumBindings},
    {"GetBinding", Script_GetBinding}, {"SaveBindings", Script_SaveBindings},
    {"ResetBindings", Script_ResetBindings}, {"RunBinding", Script_RunBinding},
    {"DefaultBindings", Script_DefaultBindings},
}};
}

void UIBindingsRegisterScriptFunctions() {
    CGUIBindings::Get();
    for (const auto& [name, function] : kBindingScripts)
        FrameScript_RegisterFunction(name, function);
}
void UIBindingsUnegisterScriptFunctions() {
    for (const auto& [name, function] : kBindingScripts) {
        (void)function;
        FrameScript_UnregisterFunction(name);
    }
}
