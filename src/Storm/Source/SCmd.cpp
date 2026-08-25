#include "Storm/h/SCmd.h"
#include "Storm/h/SMem.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace {
constexpr unsigned int TYPE_MASK = 0x30000U;
constexpr unsigned int TYPE_NUMBER = 0x10000U;
constexpr unsigned int TYPE_STRING = 0x20000U;
constexpr unsigned int ERROR_UNKNOWN = 0x85100065U;
constexpr unsigned int ERROR_REQUIRED = 0x8510006DU;
constexpr unsigned int ERROR_FILE = 110U;

std::recursive_mutex s_commandLock;
std::vector<std::unique_ptr<CMDDEF>> s_arguments;

bool EqualName(const CMDDEF& definition, const std::string& token) {
    if (token.size() < static_cast<std::size_t>(definition.namelength)) return false;
    for (int index = 0; index < definition.namelength; ++index) {
        char left = definition.name[index], right = token[index];
        if (!(definition.flags & 0x100U)) {
            left = static_cast<char>(std::tolower(static_cast<unsigned char>(left)));
            right = static_cast<char>(std::tolower(static_cast<unsigned char>(right)));
        }
        if (left != right) return false;
    }
    return true;
}

CMDDEF* FindById(unsigned int id) {
    for (const auto& definition : s_arguments) if (definition->id == id) return definition.get();
    return nullptr;
}

void Error(void (__stdcall* callback)(_CMDERROR*), unsigned int code, const char* item) {
    if (!callback) return;
    const char* message = code == ERROR_FILE ? "Unable to open command file"
        : code == ERROR_REQUIRED ? "Required argument missing" : "Unknown command argument";
    _CMDERROR error{code, item, message};
    callback(&error);
}

std::vector<std::string> Tokenize(const std::string& commandLine) {
    std::vector<std::string> tokens;
    std::string current;
    bool quoted = false;
    for (std::size_t index = 0; index < commandLine.size(); ++index) {
        const char ch = commandLine[index];
        if (ch == '"') { quoted = !quoted; continue; }
        if (!quoted && std::isspace(static_cast<unsigned char>(ch))) {
            if (!current.empty()) { tokens.push_back(current); current.clear(); }
        } else current.push_back(ch);
    }
    if (!current.empty()) tokens.push_back(current);
    return tokens;
}

bool Convert(CMDDEF& definition, const std::string& text) {
    const unsigned int type = definition.flags & TYPE_MASK;
    if (type == TYPE_STRING) {
        delete[] definition.value.currvaluestr;
        definition.value.currvaluestr = new (std::nothrow) char[text.size() + 1];
        if (!definition.value.currvaluestr) return false;
        std::memcpy(definition.value.currvaluestr, text.c_str(), text.size() + 1);
        if (definition.variableptr && definition.variablebytes)
            strncpy_s(static_cast<char*>(definition.variableptr), definition.variablebytes, text.c_str(), _TRUNCATE);
    } else if (type == TYPE_NUMBER) {
        char* end = nullptr;
        const unsigned long value = (definition.flags & 1U)
            ? static_cast<unsigned long>(std::strtol(text.c_str(), &end, 0)) : std::strtoul(text.c_str(), &end, 0);
        if (!end || *end) return false;
        definition.value.currvalue = value;
        if (definition.variableptr) std::memcpy(definition.variableptr, &definition.value.currvalue,
            std::min<unsigned int>(definition.variablebytes, sizeof(definition.value.currvalue)));
    } else {
        bool enabled = (definition.flags & 1U) != 1U;
        if (!text.empty() && text.back() == '-') enabled = false;
        else if (!text.empty() && text.back() == '+') enabled = true;
        definition.value.currvalue &= ~definition.setmask;
        if (enabled) definition.value.currvalue |= definition.setvalue;
        if (definition.variableptr) {
            auto* variable = static_cast<unsigned int*>(definition.variableptr);
            *variable = (*variable & ~definition.setmask) | (enabled ? definition.setvalue : 0U);
        }
    }
    definition.found = 1;
    _CMDPARAMS parameters{definition.flags, definition.id, definition.name, definition.variableptr,
        definition.setvalue, definition.setmask, {definition.value.currvalue}};
    if (type == TYPE_STRING) parameters.value.strvalue = definition.value.currvaluestr;
    return !definition.callback || definition.callback(&parameters, text.c_str()) != 0;
}

bool ProcessTokens(std::vector<std::string> tokens, std::size_t first,
    int (__stdcall* extraCallback)(const char*), void (__stdcall* errorCallback)(_CMDERROR*)) {
    std::vector<CMDDEF*> positionals;
    for (const auto& definition : s_arguments) if (!definition->name[0]) positionals.push_back(definition.get());
    std::size_t positional = 0;
    for (std::size_t index = first; index < tokens.size(); ++index) {
        std::string token = tokens[index];
        if (!token.empty() && token[0] == '@') {
            std::ifstream input(token.substr(1), std::ios::binary);
            if (!input) { Error(errorCallback, ERROR_FILE, token.c_str() + 1); return false; }
            const std::string contents((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
            if (!ProcessTokens(Tokenize(contents), 0, extraCallback, errorCallback)) return false;
            continue;
        }
        CMDDEF* definition = nullptr;
        std::string value = token;
        if (!token.empty() && (token[0] == '-' || token[0] == '/')) {
            const std::string flag = token.substr(1);
            for (const auto& candidate : s_arguments) {
                if (candidate->name[0] && EqualName(*candidate, flag)) { definition = candidate.get(); break; }
            }
            if (!definition) { Error(errorCallback, ERROR_UNKNOWN, token.c_str()); return false; }
            value = flag.substr(definition->namelength);
            while (!value.empty() && (value[0] == '=' || value[0] == ':')) value.erase(value.begin());
            if ((definition->flags & TYPE_MASK) && value.empty() && index + 1 < tokens.size()) value = tokens[++index];
        } else if (positional < positionals.size()) definition = positionals[positional++];
        if (!definition) {
            if (!extraCallback || !extraCallback(token.c_str())) { Error(errorCallback, ERROR_UNKNOWN, token.c_str()); return false; }
            continue;
        }
        if (!Convert(*definition, value)) return false;
    }
    for (const auto& definition : s_arguments) {
        if ((definition->flags & 0x3000000U) == 0x2000000U && !definition->found) {
            Error(errorCallback, ERROR_REQUIRED, definition->name);
            return false;
        }
    }
    return true;
}

} // namespace

CMDDEF::CMDDEF() : m_previousLink(nullptr), m_nextLink(nullptr), flags(0), id(0), name{}, namelength(0),
    setvalue(0), setmask(0), variableptr(nullptr), variablebytes(0), callback(nullptr), found(0), value{} {}
CMDDEF::~CMDDEF() { if ((flags & TYPE_MASK) == TYPE_STRING) delete[] value.currvaluestr; }

int __stdcall SCmdCheckId(unsigned int id) { std::lock_guard lock(s_commandLock); CMDDEF* d=FindById(id); return d && d->found; }
int __stdcall SCmdDestroy() { std::lock_guard lock(s_commandLock); s_arguments.clear(); return 1; }
BOOL __stdcall SCmdGetBool(unsigned int id) { std::lock_guard lock(s_commandLock); CMDDEF* d=FindById(id); return d && d->value.currvalue; }
unsigned int __stdcall SCmdGetNum(unsigned int id) { std::lock_guard lock(s_commandLock); CMDDEF* d=FindById(id); return d ? d->value.currvalue : 0; }
int __stdcall SCmdGetString(unsigned int id, char* buffer, unsigned int bufferChars) {
    if (!buffer || !bufferChars) return 0;
    std::lock_guard lock(s_commandLock); CMDDEF* d=FindById(id);
    if (!d) return 0;
    strncpy_s(buffer, bufferChars, d->value.currvaluestr ? d->value.currvaluestr : "", _TRUNCATE);
    return 1;
}
int __stdcall SCmdGetStringAlloc(unsigned int id, char** buffer) {
    if (!buffer) return 0;
    char value[4096]{};
    if (!SCmdGetString(id, value, sizeof(value))) { *buffer = nullptr; return 0; }
    const std::size_t bytes = std::strlen(value) + 1;
    *buffer = static_cast<char*>(SMemAlloc(static_cast<unsigned int>(bytes), __FILE__, __LINE__, 0));
    if (*buffer) std::memcpy(*buffer, value, bytes);
    return *buffer != nullptr;
}
int __stdcall SCmdProcess(const char* commandLine, int skipProgramName,
    int (__stdcall* extraCallback)(const char*), void (__stdcall* errorCallback)(_CMDERROR*)) {
    if (!commandLine) return 0;
    std::lock_guard lock(s_commandLock);
    auto tokens = Tokenize(commandLine);
    const std::size_t first = skipProgramName && !tokens.empty() ? 1U : 0U;
    return ProcessTokens(std::move(tokens), first, extraCallback, errorCallback);
}
int __stdcall SCmdProcessCommandLine(int (__stdcall* extraCallback)(const char*),
    void (__stdcall* errorCallback)(_CMDERROR*)) {
    return SCmdProcess(GetCommandLineA(), 1, extraCallback, errorCallback);
}
int __stdcall SCmdRegisterArgList(const _ARGLIST* list, unsigned int numArgs) {
    if (!list && numArgs) return 0;
    for (unsigned int index = 0; index < numArgs; ++index)
        if (!SCmdRegisterArgument(list[index].flags, list[index].id, list[index].name,
            nullptr, 0, 1, UINT_MAX, list[index].callback)) return 0;
    return 1;
}
int __stdcall SCmdRegisterArgument(unsigned int flags, unsigned int id, const char* name,
    void* variablePtr, unsigned int variableBytes, unsigned int setValue, unsigned int setMask,
    SCMD_CALLBACK callback) {
    std::lock_guard lock(s_commandLock);
    auto definition = std::make_unique<CMDDEF>();
    definition->flags = flags; definition->id = id; definition->variableptr = variablePtr;
    definition->variablebytes = variableBytes; definition->setvalue = setValue; definition->setmask = setMask;
    definition->callback = callback;
    if (name) strncpy_s(definition->name, name, _TRUNCATE);
    definition->namelength = static_cast<int>(std::strlen(definition->name));
    s_arguments.push_back(std::move(definition));
    return 1;
}
