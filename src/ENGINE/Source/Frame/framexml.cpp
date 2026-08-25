#include "ENGINE/Source/Frame/framexml.h"

#include "ENGINE/Source/Base/status.h"
#include "ENGINE/Source/Frame/frame.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "Storm/h/storm.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
struct CaseInsensitiveHash {
    std::size_t operator()(std::string_view value) const {
        std::size_t hash = 2166136261U;
        for (unsigned char ch : value) {
            hash ^= static_cast<unsigned char>(std::tolower(ch));
            hash *= 16777619U;
        }
        return hash;
    }
};
struct CaseInsensitiveEqual {
    bool operator()(std::string_view left, std::string_view right) const {
        return left.size() == right.size() &&
            std::equal(left.begin(), left.end(), right.begin(),
                [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
    }
};

struct XMLTree {
    std::vector<std::unique_ptr<XMLNode>> nodes;
    XMLNode* root = nullptr;

    XMLNode* NewNode() {
        nodes.push_back(std::make_unique<XMLNode>());
        return nodes.back().get();
    }
};

class XMLParser {
public:
    XMLParser(const char* data, std::size_t bytes) : m_data(data), m_end(data ? data + bytes : nullptr) {}

    std::unique_ptr<XMLTree> Parse() {
        if (!m_data) return nullptr;
        auto tree = std::make_unique<XMLTree>();
        SkipMisc();
        tree->root = ParseElement(*tree);
        return tree->root ? std::move(tree) : nullptr;
    }

private:
    const char* m_data;
    const char* m_end;

    bool AtEnd() const { return m_data >= m_end; }
    bool StartsWith(std::string_view text) const {
        return static_cast<std::size_t>(m_end - m_data) >= text.size() &&
               std::memcmp(m_data, text.data(), text.size()) == 0;
    }
    void SkipWhitespace() {
        while (!AtEnd() && std::isspace(static_cast<unsigned char>(*m_data))) ++m_data;
    }
    void SkipUntil(std::string_view marker) {
        while (!AtEnd() && !StartsWith(marker)) ++m_data;
        if (StartsWith(marker)) m_data += marker.size();
    }
    void SkipDeclaration() {
        if (StartsWith("<!--")) { m_data += 4; SkipUntil("-->"); return; }
        if (StartsWith("<?")) { m_data += 2; SkipUntil("?>"); return; }
        if (StartsWith("<![CDATA[")) return;
        if (StartsWith("<!DOCTYPE")) {
            int subsetDepth = 0;
            bool quoted = false;
            char quote = 0;
            while (!AtEnd()) {
                const char ch = *m_data++;
                if (quoted) { if (ch == quote) quoted = false; continue; }
                if (ch == '\'' || ch == '"') { quoted = true; quote = ch; continue; }
                if (ch == '[') ++subsetDepth;
                else if (ch == ']') subsetDepth = std::max(0, subsetDepth - 1);
                else if (ch == '>' && subsetDepth == 0) break;
            }
            return;
        }
        if (StartsWith("<!")) { m_data += 2; SkipUntil(">"); }
    }
    void SkipMisc() {
        for (;;) {
            SkipWhitespace();
            if (StartsWith("<?") || StartsWith("<!--") || StartsWith("<!DOCTYPE")) SkipDeclaration();
            else break;
        }
    }
    static char DecodeEntity(std::string_view entity, bool& decoded) {
        decoded = true;
        if (entity == "lt") return '<';
        if (entity == "gt") return '>';
        if (entity == "amp") return '&';
        if (entity == "quot") return '"';
        if (entity == "apos") return '\'';
        if (entity.size() > 1 && entity[0] == '#') {
            unsigned value = 0;
            const bool hex = entity.size() > 2 && (entity[1] == 'x' || entity[1] == 'X');
            for (std::size_t index = hex ? 2 : 1; index < entity.size(); ++index) {
                const char ch = entity[index];
                unsigned digit = 0;
                if (ch >= '0' && ch <= '9') digit = ch - '0';
                else if (hex && ch >= 'a' && ch <= 'f') digit = ch - 'a' + 10;
                else if (hex && ch >= 'A' && ch <= 'F') digit = ch - 'A' + 10;
                else { decoded = false; return 0; }
                value = value * (hex ? 16U : 10U) + digit;
            }
            return value <= 0x7FU ? static_cast<char>(value) : '?';
        }
        decoded = false;
        return 0;
    }
    std::string Decode(const char* begin, const char* end) const {
        std::string result;
        while (begin < end) {
            if (*begin != '&') { result.push_back(*begin++); continue; }
            const char* semicolon = std::find(begin + 1, end, ';');
            if (semicolon == end) { result.push_back(*begin++); continue; }
            bool decoded = false;
            const char value = DecodeEntity(std::string_view(begin + 1, semicolon), decoded);
            if (decoded) result.push_back(value);
            else result.append(begin, semicolon + 1);
            begin = semicolon + 1;
        }
        return result;
    }
    std::string ParseName() {
        const char* begin = m_data;
        while (!AtEnd()) {
            const unsigned char ch = static_cast<unsigned char>(*m_data);
            if (!(std::isalnum(ch) || ch == '_' || ch == '-' || ch == ':' || ch == '.')) break;
            ++m_data;
        }
        return std::string(begin, m_data);
    }
    std::string ParseAttributeValue() {
        SkipWhitespace();
        if (AtEnd() || (*m_data != '\'' && *m_data != '"')) return {};
        const char quote = *m_data++;
        const char* begin = m_data;
        while (!AtEnd() && *m_data != quote) ++m_data;
        std::string result = Decode(begin, m_data);
        if (!AtEnd()) ++m_data;
        return result;
    }
    XMLNode* ParseElement(XMLTree& tree) {
        SkipMisc();
        if (AtEnd() || *m_data != '<' || StartsWith("</")) return nullptr;
        ++m_data;
        XMLNode* node = tree.NewNode();
        node->m_name = ParseName();
        if (node->m_name.empty()) return nullptr;
        for (;;) {
            SkipWhitespace();
            if (StartsWith("/>")) { m_data += 2; return node; }
            if (!AtEnd() && *m_data == '>') { ++m_data; break; }
            XMLAttribute attribute;
            attribute.name = ParseName();
            if (attribute.name.empty()) return nullptr;
            SkipWhitespace();
            if (AtEnd() || *m_data++ != '=') return nullptr;
            attribute.value = ParseAttributeValue();
            node->m_attributes.push_back(std::move(attribute));
        }

        XMLNode* lastChild = nullptr;
        while (!AtEnd()) {
            if (StartsWith("</")) {
                m_data += 2;
                const std::string closeName = ParseName();
                SkipWhitespace();
                if (!AtEnd() && *m_data == '>') ++m_data;
                return CaseInsensitiveEqual{}(node->m_name, closeName) ? node : nullptr;
            }
            if (StartsWith("<![CDATA[")) {
                m_data += 9;
                const char* begin = m_data;
                while (!AtEnd() && !StartsWith("]]>") ) ++m_data;
                node->m_value.append(begin, m_data);
                if (StartsWith("]]>") ) m_data += 3;
                continue;
            }
            if (StartsWith("<!--") || StartsWith("<?") || StartsWith("<!")) {
                SkipDeclaration();
                continue;
            }
            if (*m_data == '<') {
                XMLNode* child = ParseElement(tree);
                if (!child) return nullptr;
                if (!node->m_child) node->m_child = child;
                else lastChild->m_next = child;
                lastChild = child;
                continue;
            }
            const char* begin = m_data;
            while (!AtEnd() && *m_data != '<') ++m_data;
            node->m_value += Decode(begin, m_data);
        }
        return nullptr;
    }
};

using FactoryMap = std::unordered_map<std::string, FrameXMLFactory, CaseInsensitiveHash, CaseInsensitiveEqual>;
using VirtualMap = std::unordered_map<std::string, const XMLNode*, CaseInsensitiveHash, CaseInsensitiveEqual>;
FactoryMap s_factories;
VirtualMap s_virtualNodes;
std::vector<std::unique_ptr<XMLTree>> s_loadedTrees;
std::vector<std::unique_ptr<CSimpleFrame>> s_ownedFrames;
FrameXMLProgressFunction s_progressCallback = nullptr;
int s_debugLevel = 0;
unsigned s_loadDepth = 0;

std::string Trim(std::string value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) { return std::isspace(ch); });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) { return std::isspace(ch); }).base();
    return first < last ? std::string(first, last) : std::string();
}

std::string ResolveRelative(const char* owner, const char* requested) {
    if (!requested) return {};
    std::string result = requested;
    if (result.find('\\') != std::string::npos || result.find('/') != std::string::npos || !owner) return result;
    std::string source = owner;
    const std::size_t slash = source.find_last_of("\\/");
    return slash == std::string::npos ? result : source.substr(0, slash + 1) + result;
}

bool IsTrue(const char* value) { return value && _stricmp(value, "true") == 0; }

template<class T> CSimpleFrame* CreateFrame(CSimpleFrame* parent) { return new T(parent); }

void LoadFrameNode(CSimpleFrame* frame, const XMLNode* node, CStatus* status, bool loadChildren);

void LoadInherited(CSimpleFrame* frame, const char* inherited, CStatus* status) {
    if (!inherited) return;
    std::string names = inherited;
    std::size_t begin = 0;
    while (begin <= names.size()) {
        const std::size_t comma = names.find(',', begin);
        const std::string name = Trim(names.substr(begin, comma == std::string::npos ? std::string::npos : comma - begin));
        if (!name.empty()) {
            if (const XMLNode* base = FrameXML_FindHashNode(name.c_str())) LoadFrameNode(frame, base, status, true);
            else if (status) status->Add(STATUS_WARNING, "Could not find inherited frame '%s'", name.c_str());
        }
        if (comma == std::string::npos) break;
        begin = comma + 1;
    }
}

void CreateChildFrames(CSimpleFrame* parent, const XMLNode* node, CStatus* status) {
    for (const XMLNode* child = node ? node->m_child : nullptr; child; child = child->m_next) {
        if (s_factories.find(child->m_name) != s_factories.end()) {
            FrameXML_CreateFrame(child, parent, status);
        } else if (_stricmp(child->m_name.c_str(), "Frames") == 0) {
            CreateChildFrames(parent, child, status);
        }
    }
}

void LoadFrameNode(CSimpleFrame* frame, const XMLNode* node, CStatus* status, bool loadChildren) {
    if (!frame || !node) return;
    LoadInherited(frame, node->GetAttributeByName("inherits"), status);
    frame->LoadXML(node, status);
    for (const XMLNode* child = node->m_child; child; child = child->m_next) {
        if (_stricmp(child->m_name.c_str(), "Layers") == 0)
            frame->LoadXML_Layers(child, status);
        else if (_stricmp(child->m_name.c_str(), "Scripts") == 0)
            frame->LoadXML_Scripts(child, status);
    }
    if (loadChildren) CreateChildFrames(frame, node, status);
}
}

void FrameXML_LoadTextureNode(CSimpleTexture* texture, const XMLNode* node, CStatus* status) {
    if (!texture || !node) return;
    texture->PreLoadXML(node, status);
    std::function<void(const XMLNode*)> load = [&](const XMLNode* current) {
        if (!current) return;
        if (const char* inherited = current->GetAttributeByName("inherits"); inherited && *inherited) {
            std::string names = inherited;
            std::size_t begin = 0;
            while (begin <= names.size()) {
                const std::size_t comma = names.find(',', begin);
                const std::string name = Trim(names.substr(
                    begin, comma == std::string::npos ? std::string::npos : comma - begin));
                if (!name.empty()) {
                    if (const XMLNode* base = FrameXML_FindHashNode(name.c_str())) load(base);
                    else if (status) status->Add(STATUS_WARNING, "Could not find inherited texture '%s'", name.c_str());
                }
                if (comma == std::string::npos) break;
                begin = comma + 1;
            }
        }
        texture->LoadXML(current, status);
    };
    load(node);
    texture->PostLoadXML(node, status);
}

void FrameXML_LoadFontStringNode(CSimpleFontString* string, const XMLNode* node, CStatus* status) {
    if (!string || !node) return;
    string->PreLoadXML(node, status);
    std::function<void(const XMLNode*)> load = [&](const XMLNode* current) {
        if (!current) return;
        if (const char* inherited = current->GetAttributeByName("inherits"); inherited && *inherited) {
            std::string names = inherited;
            std::size_t begin = 0;
            while (begin <= names.size()) {
                const std::size_t comma = names.find(',', begin);
                const std::string name = Trim(names.substr(
                    begin, comma == std::string::npos ? std::string::npos : comma - begin));
                if (!name.empty()) {
                    if (const XMLNode* base = FrameXML_FindHashNode(name.c_str())) load(base);
                    else if (status) status->Add(STATUS_WARNING, "Could not find inherited font string '%s'", name.c_str());
                }
                if (comma == std::string::npos) break;
                begin = comma + 1;
            }
        }
        string->LoadXML(current, status);
    };
    load(node);
    string->PostLoadXML(node, status);
}

void FrameXML_SetDebugLevel(int level) { s_debugLevel = level; }
int FrameXML_GetDebugLevel() { return s_debugLevel; }

int FrameXML_CreateFrames(const char* tableOfContents, CStatus* status) {
    CStatus localStatus;
    if (!status) status = &localStatus;
    if (!tableOfContents) return 0;
    void* buffer = nullptr;
    unsigned bytes = 0;
    if (!SFile::LoadFile(tableOfContents, &buffer, &bytes, 1, nullptr)) {
        status->Add(STATUS_ERROR, "Couldn't open %s", tableOfContents);
        return 0;
    }
    ++s_loadDepth;
    std::string contents(static_cast<const char*>(buffer), bytes);
    SFile::Unload(buffer);
    std::vector<std::string> files;
    std::size_t begin = 0;
    while (begin <= contents.size()) {
        const std::size_t end = contents.find_first_of("\r\n", begin);
        std::string line = Trim(contents.substr(begin, end == std::string::npos ? std::string::npos : end - begin));
        if (!line.empty() && line[0] != '#') files.push_back(std::move(line));
        if (end == std::string::npos) break;
        begin = end + 1;
        while (begin < contents.size() && (contents[begin] == '\r' || contents[begin] == '\n')) ++begin;
    }
    int result = 1;
    for (std::size_t index = 0; index < files.size(); ++index) {
        const std::string filename = ResolveRelative(tableOfContents, files[index].c_str());
        if (s_debugLevel > 0) std::fprintf(stderr, "FrameXML: loading %s\n", filename.c_str());
        const char* extension = std::strrchr(filename.c_str(), '.');
        if (extension && _stricmp(extension, ".toc") == 0)
            result &= FrameXML_CreateFrames(filename.c_str(), status);
        else
            result &= FrameXML_ProcessFile(filename.c_str(), status);
        if (s_progressCallback) s_progressCallback(static_cast<int>(index), static_cast<int>(files.size()));
    }
    if (--s_loadDepth == 0) {
        s_virtualNodes.clear();
        s_loadedTrees.clear();
    }
    return result;
}

int FrameXML_ProcessFile(const char* filename, CStatus* status) {
    CStatus localStatus;
    if (!status) status = &localStatus;
    void* buffer = nullptr;
    unsigned bytes = 0;
    if (!SFile::LoadFile(filename, &buffer, &bytes, 0, nullptr)) {
        status->Add(STATUS_ERROR, "Couldn't open %s", filename ? filename : "<null>");
        return 0;
    }
    XMLParser parser(static_cast<const char*>(buffer), bytes);
    std::unique_ptr<XMLTree> tree = parser.Parse();
    SFile::Unload(buffer);
    if (!tree) {
        status->Add(STATUS_ERROR, "Couldn't parse XML in %s", filename ? filename : "<null>");
        return 0;
    }
    XMLNode* root = tree->root;
    s_loadedTrees.push_back(std::move(tree));
    for (const XMLNode* node = root->m_child; node; node = node->m_next) {
        if (_stricmp(node->m_name.c_str(), "Script") == 0) {
            if (const char* scriptFile = node->GetAttributeByName("file")) {
                const std::string path = ResolveRelative(filename, scriptFile);
                if (!FrameScript_ExecuteFile(path.c_str()))
                    status->Add(STATUS_ERROR, "Couldn't open %s", path.c_str());
            }
            if (!Trim(node->m_value).empty()) FrameScript_Execute(node->m_value.c_str(), filename);
            continue;
        }
        if (IsTrue(node->GetAttributeByName("virtual"))) {
            const char* name = node->GetAttributeByName("name");
            if (name && *name) FrameXML_StoreHashNode(node, name, status);
            else status->Add(STATUS_WARNING, "Unnamed virtual frame in %s", filename);
            continue;
        }
        FrameXML_CreateFrame(node, nullptr, status);
    }
    return 1;
}

CSimpleFrame* FrameXML_CreateFrame(const XMLNode* node, CSimpleFrame* parent, CStatus* status) {
    if (!node) return nullptr;
    if (s_debugLevel > 1) {
        const char* debugName = node->GetAttributeByName("name");
        std::fprintf(stderr, "FrameXML: create %s name=%s parent=%p\n", node->m_name.c_str(),
                     debugName ? debugName : "", static_cast<void*>(parent));
    }
    // Recovered FrameXML permits frames declared at the document root to name
    // their logical owner with parent="...".  Those frames are not children in
    // the XML tree (PaperDollFrame is the important example), so the parent has
    // to be resolved before invoking the factory/constructor.
    if (const char* declaredParent = node->GetAttributeByName("parent");
        declaredParent && *declaredParent) {
        std::string parentName = declaredParent;
        constexpr char parentToken[] = "$parent";
        if (_strnicmp(parentName.c_str(), parentToken, sizeof(parentToken) - 1) == 0) {
            const char* contextName = parent ? parent->GetName() : nullptr;
            parentName = contextName ? contextName : "TOP";
            parentName += declaredParent + sizeof(parentToken) - 1;
        }
        if (CSimpleFrame* declared = SimpleFrameRegistryGetEntry(nullptr, parentName.c_str())) {
            parent = declared;
        } else if (status) {
            status->Add(STATUS_WARNING, "Could not find parent frame '%s'", parentName.c_str());
        }
    }

    const auto found = s_factories.find(node->m_name);
    if (found == s_factories.end()) {
        if (status) status->Add(STATUS_WARNING, "Unknown frame type '%s'", node->m_name.c_str());
        return nullptr;
    }
    CSimpleFrame* frame = found->second(parent);
    if (!frame) return nullptr;
    s_ownedFrames.emplace_back(frame);
    frame->PreLoadXML(node, status);
    LoadFrameNode(frame, node, status, true);
    frame->PostLoadXML(node, status);
    if (s_debugLevel > 1) std::fprintf(stderr, "FrameXML: created %s\n", frame->GetName());
    return frame;
}

void FrameXML_StoreHashNode(const XMLNode* node, const char* name, CStatus* status) {
    if (!node || !name || !*name) return;
    if (s_virtualNodes.find(name) != s_virtualNodes.end()) {
        if (status) status->Add(STATUS_WARNING, "Virtual object named '%s' already exists", name);
        return;
    }
    s_virtualNodes.emplace(name, node);
}
const XMLNode* FrameXML_FindHashNode(const char* name) {
    const auto found = name ? s_virtualNodes.find(name) : s_virtualNodes.end();
    return found == s_virtualNodes.end() ? nullptr : found->second;
}
void FrameXML_ClearFactories() { s_factories.clear(); }
int FrameXML_RegisterFactory(const char* frameType, FrameXMLFactory factory) {
    if (!frameType || !*frameType || !factory || s_factories.find(frameType) != s_factories.end()) return 0;
    s_factories.emplace(frameType, factory);
    return 1;
}
int FrameXML_RegisterDefault() {
    CSimpleTexture::RegisterScriptMethods();
    CSimpleFontString::RegisterScriptMethods();
    CSimpleFrame::RegisterScriptMethods();
    CSimpleButton::RegisterScriptMethods();
    CSimpleCheckbox::RegisterScriptMethods();
    CSimpleEditBox::RegisterScriptMethods();
    CSimpleHTML::RegisterScriptMethods();
    CSimpleMessageFrame::RegisterScriptMethods();
    CSimpleMessageScrollFrame::RegisterScriptMethods();
    CSimpleModel::RegisterScriptMethods();
    CSimpleScrollFrame::RegisterScriptMethods();
    CSimpleSlider::RegisterScriptMethods();
    CSimpleStatusBar::RegisterScriptMethods();
    FrameXML_RegisterFactory("Button", CreateFrame<CSimpleButton>);
    FrameXML_RegisterFactory("CheckButton", CreateFrame<CSimpleCheckbox>);
    FrameXML_RegisterFactory("EditBox", CreateFrame<CSimpleEditBox>);
    FrameXML_RegisterFactory("Frame", CreateFrame<CSimpleFrame>);
    FrameXML_RegisterFactory("MessageFrame", CreateFrame<CSimpleMessageFrame>);
    FrameXML_RegisterFactory("Model", CreateFrame<CSimpleModel>);
    FrameXML_RegisterFactory("ScrollFrame", CreateFrame<CSimpleScrollFrame>);
    FrameXML_RegisterFactory("ScrollingMessageFrame", CreateFrame<CSimpleMessageScrollFrame>);
    FrameXML_RegisterFactory("Slider", CreateFrame<CSimpleSlider>);
    FrameXML_RegisterFactory("SimpleHTML", CreateFrame<CSimpleHTML>);
    FrameXML_RegisterFactory("StatusBar", CreateFrame<CSimpleStatusBar>);
    return 1;
}
void FrameXML_RegisterLoadProgressCallback(FrameXMLProgressFunction callback) { s_progressCallback = callback; }
void FrameXML_DestroyFrames() {
    while (!s_ownedFrames.empty()) {
        std::unique_ptr<CSimpleFrame> frame = std::move(s_ownedFrames.back());
        s_ownedFrames.pop_back();
        frame.reset();
    }
    delete CSimpleTop::s_instance;
    CSimpleTop::s_instance = nullptr;
    s_virtualNodes.clear();
    s_loadedTrees.clear();
}
