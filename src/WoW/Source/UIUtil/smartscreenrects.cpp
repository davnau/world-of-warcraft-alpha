#include "WoW/Source/UIUtil/smartscreenrects.h"

#include "ENGINE/Source/Frame/frame.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <cstring>
#include <limits>
#include <vector>

float INFINITY_131 = 0.0F;

namespace {
constexpr float kViewWidth = 0.80000001F;
constexpr float kViewHeight = 0.60000002F;
constexpr float kViewBottom = 0.018750001F;
constexpr float kViewTop = kViewHeight - 0.037500001F;
constexpr float kEpsilon = 0.000099999997F;

struct GridState {
    std::vector<NTempest::CRect> rects;
};

GridState s_gridRectList[NUM_SRECTGRIDS];
std::deque<BFSNODE> s_freeBFSNodes;
std::deque<BFSNODE> s_activeBFSNodes;
const TEST_DIRECTION s_testDirections[9][4] = {
    {TEST_UP, TEST_LEFT, TEST_INVALID, TEST_INVALID},
    {TEST_UP, TEST_RIGHT, TEST_INVALID, TEST_INVALID},
    {TEST_DOWN, TEST_LEFT, TEST_INVALID, TEST_INVALID},
    {TEST_DOWN, TEST_RIGHT, TEST_INVALID, TEST_INVALID},
    {TEST_LEFT, TEST_UP, TEST_INVALID, TEST_INVALID},
    {TEST_RIGHT, TEST_UP, TEST_INVALID, TEST_INVALID},
    {TEST_LEFT, TEST_DOWN, TEST_INVALID, TEST_INVALID},
    {TEST_RIGHT, TEST_DOWN, TEST_INVALID, TEST_INVALID},
    {TEST_UP, TEST_DOWN, TEST_INVALID, TEST_INVALID},
};

bool ValidGrid(SCREENRECTGRIDS grid) {
    return static_cast<int>(grid) >= 0 && static_cast<int>(grid) < NUM_SRECTGRIDS;
}

bool Intersects(const NTempest::CRect& left, const NTempest::CRect& right) {
    return left.b > right.t && left.t < right.b && left.l < right.r && left.r > right.l;
}

NTempest::CRect MoveForDirection(const NTempest::CRect& rect,
                                 TEST_DIRECTION direction, float offset) {
    NTempest::CRect result = rect;
    switch (direction) {
    case TEST_UP: result.t += offset; result.b += offset; break;
    case TEST_LEFT: result.l -= offset; result.r -= offset; break;
    case TEST_RIGHT: result.l += offset; result.r += offset; break;
    case TEST_DOWN: result.t -= offset; result.b -= offset; break;
    default: break;
    }
    return result;
}
}

int RectCollides(SCREENRECTGRIDS grid, const NTempest::CRect& rect,
                 TEST_DIRECTION direction, float* offset) {
    if (offset == nullptr || !ValidGrid(grid))
        return 0;
    *offset = 0.0F;
    for (const NTempest::CRect& marked : s_gridRectList[grid].rects) {
        if (!Intersects(rect, marked))
            continue;
        switch (direction) {
        case TEST_UP: *offset = marked.r - rect.l; break;
        case TEST_LEFT: *offset = rect.b - marked.t; break;
        case TEST_RIGHT: *offset = marked.b - rect.t; break;
        case TEST_DOWN: *offset = rect.r - marked.l; break;
        default: return 0;
        }
        return 1;
    }
    return 0;
}

NTempest::CRect* TestUp(NTempest::CRect* result, SCREENRECTGRIDS grid,
                        NTempest::CRect rect) {
    float offset = 0.0F;
    if (RectCollides(grid, rect, TEST_UP, &offset)) {
        rect.t += offset;
        rect.b += offset;
    }
    if (result != nullptr) *result = rect;
    return result;
}

NTempest::CRect* TestLeft(NTempest::CRect* result, SCREENRECTGRIDS grid,
                          NTempest::CRect rect) {
    float offset = 0.0F;
    if (RectCollides(grid, rect, TEST_LEFT, &offset)) {
        rect.l -= offset;
        rect.r -= offset;
    }
    if (result != nullptr) *result = rect;
    return result;
}

NTempest::CRect* TestRight(NTempest::CRect* result, SCREENRECTGRIDS grid,
                           NTempest::CRect rect) {
    float offset = 0.0F;
    if (RectCollides(grid, rect, TEST_RIGHT, &offset)) {
        rect.l += offset;
        rect.r += offset;
    }
    if (result != nullptr) *result = rect;
    return result;
}

NTempest::CRect* TestDown(NTempest::CRect* result, SCREENRECTGRIDS grid,
                          NTempest::CRect rect) {
    float offset = 0.0F;
    if (RectCollides(grid, rect, TEST_DOWN, &offset)) {
        rect.t -= offset;
        rect.b -= offset;
    }
    if (result != nullptr) *result = rect;
    return result;
}

void SmartScreenRectInitialize(const char*) {
    _E1_131();
    SmartScreenRectClearAllGrids();
}

void SmartScreenRectShutdown() {
    s_activeBFSNodes.clear();
    s_freeBFSNodes.clear();
    for (GridState& grid : s_gridRectList)
        grid.rects.clear();
}

void SmartScreenRectClearAllGrids() {
    for (GridState& grid : s_gridRectList)
        grid.rects.clear();
}

bool CheckRect(const NTempest::CRect& rect, int allowEmpty) {
    const bool validCoordinates = allowEmpty != 0 ||
        (rect.t >= 0.0F && rect.l >= 0.0F && rect.b >= 0.0F && rect.r >= 0.0F &&
         rect.t <= kViewHeight && rect.l <= kViewWidth &&
         rect.b <= kViewHeight && rect.r <= kViewWidth);
    return validCoordinates && rect.t >= rect.b && rect.r >= rect.l;
}

void ClipRect(NTempest::CRect& rect) {
    const float height = rect.t - rect.b;
    const float width = rect.r - rect.l;
    if (rect.t >= kViewHeight) {
        rect.t = kViewHeight;
        rect.b = kViewHeight - height;
    }
    if (rect.b < 0.0F) {
        rect.b = 0.0F;
        rect.t = height;
    }
    if (rect.l < 0.0F) {
        rect.l = 0.0F;
        rect.r = width;
    }
    if (rect.r > kViewWidth) {
        rect.r = kViewWidth;
        rect.l = kViewWidth - width;
    }
}

int RectOutsideBorder(NTempest::CRect* clipped, int writeClipped,
                      NTempest::CRect rect) {
    int flags = 0;
    if (rect.l < 0.0F) flags |= 1;
    if (rect.r > kViewWidth) flags |= 2;
    if (rect.b < kViewBottom) flags |= 8;
    if (rect.t > kViewTop) flags |= 4;
    if (writeClipped != 0 && clipped != nullptr) {
        if ((flags & 0xC) != 0) {
            if ((flags & 4) != 0) {
                rect.t = kViewTop;
                rect.b = kViewTop - (rect.t - rect.b);
            } else {
                rect.b = kViewBottom;
                rect.t = kViewBottom + (rect.t - rect.b);
            }
        }
        if ((flags & 3) != 0) {
            if ((flags & 1) != 0) {
                rect.l = 0.0F;
                rect.r = rect.l + (rect.r - rect.l);
            } else {
                rect.r = kViewWidth;
                rect.l = rect.r - (rect.r - rect.l);
            }
        }
        *clipped = rect;
    }
    return flags;
}

unsigned SelectNewSearchPattern(char hitFlags) {
    if ((hitFlags & 1) != 0)
        return (hitFlags & 4) != 0 ? 8u : static_cast<unsigned>((hitFlags & 8 | 0x10) >> 2);
    if ((hitFlags & 4) != 0)
        return (hitFlags & 2) != 0 ? 7u : 2u;
    if ((hitFlags & 2) != 0)
        return (hitFlags & 8) != 0 ? 5u : 3u;
    return (hitFlags & 8) != 0 ? 1u : 0u;
}

int CalculateMaxTraversals(const NTempest::CRect& rect) {
    const float height = rect.t - rect.b;
    const float width = rect.r - rect.l;
    if (height <= 0.0F || width <= 0.0F)
        return 0;
    return (static_cast<int>(kViewHeight / height) + 1) *
           (static_cast<int>(kViewWidth / width) + 1);
}

void MarkRect(const NTempest::CRect& rect, SCREENRECTGRIDS grid) {
    if (!ValidGrid(grid) || !CheckRect(rect, 0))
        return;
    s_gridRectList[grid].rects.push_back(rect);
}

BFSNODE* GetBFSNode() {
    if (!s_freeBFSNodes.empty()) {
        s_activeBFSNodes.push_back(s_freeBFSNodes.front());
        s_freeBFSNodes.pop_front();
    } else {
        s_activeBFSNodes.emplace_back();
    }
    s_activeBFSNodes.back().dontTestDirection = TEST_INVALID;
    return &s_activeBFSNodes.back();
}

void CleanupActiveBFSNodes() {
    while (!s_activeBFSNodes.empty()) {
        s_freeBFSNodes.push_back(s_activeBFSNodes.front());
        s_activeBFSNodes.pop_front();
    }
}

NTempest::CRect* FindFreeRect(SCREENRECTGRIDS grid, NTempest::CRect* result,
                              const NTempest::CRect& rect) {
    if (result == nullptr || !ValidGrid(grid))
        return result;
    NTempest::CRect initial = rect;
    RectOutsideBorder(&initial, 1, initial);
    std::deque<NTempest::CRect> pending;
    pending.push_back(initial);
    const int maxTraversals = (std::max)(1, CalculateMaxTraversals(rect));
    int traversals = 0;
    while (!pending.empty() && traversals++ < maxTraversals) {
        const NTempest::CRect current = pending.front();
        pending.pop_front();
        bool collision = false;
        for (const NTempest::CRect& marked : s_gridRectList[grid].rects) {
            if (Intersects(current, marked)) {
                collision = true;
                break;
            }
        }
        if (!collision) {
            *result = current;
            CleanupActiveBFSNodes();
            return result;
        }
        for (TEST_DIRECTION direction : {TEST_UP, TEST_LEFT, TEST_RIGHT, TEST_DOWN}) {
            float offset = 0.0F;
            if (!RectCollides(grid, current, direction, &offset) || offset <= 0.0F)
                continue;
            NTempest::CRect next = MoveForDirection(current, direction, offset + 0.0001F);
            if (RectOutsideBorder(nullptr, 0, next) == 0)
                pending.push_back(next);
        }
    }
    *result = rect;
    CleanupActiveBFSNodes();
    return result;
}

NTempest::C2Vector* FindNextAvailableRect(SCREENRECTGRIDS grid,
                                          NTempest::C2Vector* result,
                                          const NTempest::CRect& rect,
                                          int* repositioned) {
    if (result == nullptr)
        return result;
    NTempest::CRect freeRect;
    FindFreeRect(grid, &freeRect, rect);
    if (repositioned != nullptr)
        *repositioned = std::memcmp(&freeRect, &rect, sizeof(rect)) != 0;
    result->x = (freeRect.l + freeRect.r) * 0.5F;
    result->y = freeRect.t;
    return result;
}

void SmartScreenRectGridPos(SCREENRECTGRIDS grid, NTempest::CRect& rect) {
    const float width = std::fabs(rect.r - rect.l);
    const float height = std::fabs(rect.t - rect.b);
    ClipRect(rect);
    NTempest::C2Vector position;
    int repositioned = 0;
    FindNextAvailableRect(grid, &position, rect, &repositioned);
    position.x = std::clamp(position.x, width * 0.5F, kViewWidth - width * 0.5F);
    position.y = std::clamp(position.y, height * 0.5F, kViewHeight - height * 0.5F);
    rect = {position.x - width * 0.5F, position.y,
            position.x + width * 0.5F, position.y - height};
    ClipRect(rect);
    MarkRect(rect, grid);
}

void SmartScreenRectGetGridPos(SCREENRECTGRIDS grid, CLayoutFrame* frameToPlace,
                               float totalWidth, float totalHeight,
                               CLayoutFrame* base, const NTempest::C2Vector& position,
                               int positionFromCenter) {
    if (frameToPlace == nullptr || base == nullptr || !ValidGrid(grid))
        return;
    NTempest::CRect baseRect;
    if (!base->GetRect(&baseRect))
        return;
    NTempest::CRect rect{position.x - totalWidth * 0.5F, position.y,
                         position.x + totalWidth * 0.5F, position.y - totalHeight};
    SmartScreenRectGridPos(grid, rect);
    NTempest::C2Vector center{(rect.l + rect.r) * 0.5F, rect.t};
    frameToPlace->ClearAllPoints();
    frameToPlace->SetPoint(positionFromCenter != 0 ? FRAMEPOINT_CENTER : FRAMEPOINT_TOP,
                           base, FRAMEPOINT_BOTTOMLEFT, center.x, center.y);
    frameToPlace->Resize();
}

int _E2_131() { return _E1_131(); }
int _E1_131() { INFINITY_131 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E177() { _E174_0(); return _E176_0(); }
unsigned* _E174_0() { SmartScreenRectClearAllGrids(); return nullptr; }
int _E176_0() { return 0; }
void _E175_0() { SmartScreenRectShutdown(); }
int _E182_0() { _E179(); return _E181(); }
unsigned _E179() { return 0; }
int _E181() { return 0; }
void _E180() { CleanupActiveBFSNodes(); }
int _E187_4() { _E184_3(); return _E186_5(); }
unsigned _E184_3() { return 0; }
int _E186_5() { return 0; }
void _E185_5() { CleanupActiveBFSNodes(); }
int _E5_131() { _E4_131(); return 0; }
void _E4_131() { }
int _E8_131() { _E7_131(); return 0; }
void _E7_131() { }
char* TSBaseArray_CRect_MemFileName() { return const_cast<char*>("SmartScreenRects.cpp"); }
int TSBaseArray_CRect_MemLineNo() { return 0; }
void TSLinkedNode_BFSNODE_Destroy(BFSNODE*) {}
void TSLink_BFSNODE_NextLink() {}
void TSLink_BFSNODE_Unlink() {}
void TSFixedArray_CRect_ReallocData(unsigned) {}

#define SCREENRECT_THUNK_PAIR(thunk, body) \
    int thunk() { body(); return 0; } \
    void body()
SCREENRECT_THUNK_PAIR(_E28_55, _E27_52) { }
SCREENRECT_THUNK_PAIR(_E31_52, _E30_50) { }
SCREENRECT_THUNK_PAIR(_E34_53, _E33_48) { }
SCREENRECT_THUNK_PAIR(_E37_52, _E36_47) { }
SCREENRECT_THUNK_PAIR(_E40_53, _E39_49) { }
SCREENRECT_THUNK_PAIR(_E43_52, _E42_48) { }
SCREENRECT_THUNK_PAIR(_E46_50, _E45_47) { }
SCREENRECT_THUNK_PAIR(_E49_52, _E48_52) { }
SCREENRECT_THUNK_PAIR(_E52_51, _E51_46) { }
SCREENRECT_THUNK_PAIR(_E55_47, _E54_51) { }
SCREENRECT_THUNK_PAIR(_E58_46, _E57_39) { }
SCREENRECT_THUNK_PAIR(_E61_47, _E60_47) { }
SCREENRECT_THUNK_PAIR(_E68_13, _E67_43) { }
SCREENRECT_THUNK_PAIR(_E71_5, _E70_35) { }
SCREENRECT_THUNK_PAIR(_E74_2, _E73_35) { }
SCREENRECT_THUNK_PAIR(_E77_1, _E76_34) { }
SCREENRECT_THUNK_PAIR(_E80_5, _E79_1) { }
#undef SCREENRECT_THUNK_PAIR
