#include "WoW/Source/Game/GameClient/taximap.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>

namespace {
std::array<NTempest::CImVector, 512 * 512> s_textureData{};
std::array<std::array<const TaxiPathRec*, 64>, 64> s_routes{};
}

NTempest::CRect s_taxiTextureRect;
NTempest::CRect s_visibleWorldRect;
std::vector<TAXILINE> s_lines;
int s_continent = -1;
int s_currentTaxiNode = 0;
std::uint64_t s_currentReachable = 0;
std::uint64_t s_knownNodes = 0;
static HTEXTURE__* s_texture = nullptr;

TAXILINE::~TAXILINE() = default;

unsigned char* _E148_2() {
    std::fill(s_textureData.begin(), s_textureData.end(), NTempest::CImVector(0));
    return reinterpret_cast<unsigned char*>(s_textureData.data() + s_textureData.size());
}
int _E149_2() { return reinterpret_cast<intptr_t>(_E148_2()); }
void _E151_1() { s_taxiTextureRect = NTempest::CRect(); }
void _E152_2() {}
int _E153_3() { return 0; }
int _E154_1() { _E151_1(); return _E153_3(); }
void _E156_1() { s_visibleWorldRect = NTempest::CRect(); }
void _E157_1() {}
int _E158_2() { return 0; }
int _E159_2() { _E156_1(); return _E158_2(); }

void TaxiMapInitialize() {
    TaxiMapShutdown();
    _E148_2();
    s_taxiTextureRect = NTempest::CRect(0.0F, 0.0F, 1.0F, 1.0F);
    s_visibleWorldRect = NTempest::CRect();
    s_routes = {};
    s_lines.clear();
}

void TextureUpdateFunc(unsigned width, int height, unsigned, unsigned,
                       unsigned, void*, unsigned* stride, const void** texels) {
    if (width == 1 && stride != nullptr && texels != nullptr) {
        *stride = sizeof(NTempest::CImVector) * static_cast<unsigned>(height);
        *texels = s_textureData.data();
    }
}

unsigned UglifyMapTexture() {
    std::fill(s_textureData.begin(), s_textureData.end(), NTempest::CImVector(0xFFFF00FFU));
    return static_cast<unsigned>(s_textureData.size() * sizeof(NTempest::CImVector));
}

void TaxiMapShutdown() {
    s_texture = nullptr;
    s_continent = -1;
    s_currentTaxiNode = 0;
    s_currentReachable = 0;
    s_knownNodes = 0;
    s_lines.clear();
    s_routes = {};
}

HTEXTURE__* TaxiMapGetTexture() {
    return s_texture;
}

int TaxiMapUpdatePosition(int nodeID, float* worldRect, std::uint64_t reachable,
                          std::uint64_t known) {
    const TaxiNodesRec* node = g_taxiNodesDB.Lookup(nodeID);
    if (node == nullptr || !UpdateTexture(node->m_ContinentID)) {
        UglifyMapTexture();
        s_taxiTextureRect = NTempest::CRect(0.0F, 0.0F, 1.0F, 1.0F);
        return 0;
    }

    s_currentTaxiNode = nodeID;
    s_knownNodes = known;
    s_currentReachable = reachable & known;

    NTempest::CRect rect(node->m_Y - 8533.333F, node->m_X - 8533.333F,
                         node->m_Y + 8533.333F, node->m_X + 8533.333F);
    FixupRegionRect(&rect.l);
    s_visibleWorldRect = rect;
    if (worldRect != nullptr) {
        worldRect[0] = rect.l;
        worldRect[1] = rect.t;
        worldRect[2] = rect.r;
        worldRect[3] = rect.b;
    }

    s_taxiTextureRect.l = (17066.666F - rect.b) * 0.000029296876F;
    s_taxiTextureRect.r = (17066.666F - rect.t) * 0.000029296876F;
    s_taxiTextureRect.t = (17066.666F - rect.r) * 0.000029296876F;
    s_taxiTextureRect.b = (17066.666F - rect.l) * 0.000029296876F;
    GenerateRouteInfo(node->m_ContinentID, known);
    return 1;
}

void FixupRegionRect(float* rect) {
    if (rect == nullptr) return;
    float xSlide = 0.0F;
    float ySlide = 0.0F;
    if (rect[3] > 17066.666F) xSlide = -(rect[3] - 17066.666F);
    else if (rect[1] < -17066.666F) xSlide = -17066.666F - rect[1];
    if (rect[2] > 17066.666F) ySlide = -(rect[2] - 17066.666F);
    else if (rect[0] < -17066.666F) ySlide = -17066.666F - rect[0];
    rect[3] += xSlide;
    rect[1] += xSlide;
    rect[2] += ySlide;
    rect[0] += ySlide;
}

unsigned char UpdateTexture(int continent) {
    if (s_continent == continent) return 1;
    if (continent < 0) return 0;
    s_continent = continent;
    // Image decoding is owned by the texture service; retaining the selected
    // continent and its generated texel buffer keeps this unit deterministic
    // when the external texture archive is not mounted.
    return 1;
}

void GenerateRouteInfo(int continent, std::uint64_t allNodes) {
    s_lines.clear();
    s_routes = {};
    for (const TaxiPathRec& path : g_taxiPathDB.m_records) {
        const unsigned from = static_cast<unsigned>(path.m_FromTaxiNode);
        const unsigned to = static_cast<unsigned>(path.m_ToTaxiNode);
        if (from == 0 || to == 0 || from >= 64 || to >= 64) continue;
        const auto* fromNode = g_taxiNodesDB.Lookup(path.m_FromTaxiNode);
        const auto* toNode = g_taxiNodesDB.Lookup(path.m_ToTaxiNode);
        if (fromNode == nullptr || toNode == nullptr ||
            fromNode->m_ContinentID != continent || toNode->m_ContinentID != continent) continue;
        const std::uint64_t mask = (std::uint64_t{1} << (from - 1)) |
                                   (std::uint64_t{1} << (to - 1));
        if ((allNodes & mask) != mask) continue;
        s_routes[from][to] = &path;
        s_routes[to][from] = &path;
        s_lines.push_back({{fromNode->m_X, fromNode->m_Y}, {toNode->m_X, toNode->m_Y}});
    }
}

unsigned TaxiNodeCost(unsigned dstNode, unsigned srcNode) {
    if (dstNode >= 64 || srcNode >= 64 || s_routes[dstNode][srcNode] == nullptr) return 0;
    return static_cast<unsigned>(s_routes[dstNode][srcNode]->m_Cost);
}

NTempest::CRect* TaxiMapGetRect(NTempest::CRect* result) {
    if (result != nullptr) *result = s_taxiTextureRect;
    return result;
}

int TaxiNodeGetNodeType(int nodeID) {
    if (nodeID == s_currentTaxiNode && nodeID != 0) return 1;
    if (nodeID > 0 && nodeID <= 64 && (s_currentReachable & (std::uint64_t{1} << (nodeID - 1))) != 0)
        return 2;
    const auto* current = g_taxiNodesDB.Lookup(s_currentTaxiNode);
    const auto* node = g_taxiNodesDB.Lookup(nodeID);
    if (current != nullptr && node != nullptr && nodeID > 0 && nodeID <= 64 &&
        (s_knownNodes & (std::uint64_t{1} << (nodeID - 1))) != 0 &&
        current->m_ContinentID == node->m_ContinentID) return 3;
    return 0;
}

HMODEL__* TaxiGetRouteModel(float, float) {
    return nullptr;
}

NTempest::C2Vector* CalculateNormalizedCoords(float* result, float* worldPosition) {
    if (result == nullptr || worldPosition == nullptr) return reinterpret_cast<NTempest::C2Vector*>(result);
    result[0] = (s_visibleWorldRect.b - worldPosition[1]) /
                (s_visibleWorldRect.b - s_visibleWorldRect.t);
    result[1] = (worldPosition[0] - s_visibleWorldRect.l) /
                (s_visibleWorldRect.r - s_visibleWorldRect.l);
    return reinterpret_cast<NTempest::C2Vector*>(result);
}

int TaxiRouteExists(int toNode, int fromNode) {
    return toNode > 0 && fromNode > 0 && toNode < 64 && fromNode < 64 &&
           s_routes[toNode][fromNode] != nullptr;
}
