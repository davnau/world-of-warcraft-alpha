#include "WoW/Source/UIUtil/cursor.h"

#include <cmath>
#include <limits>

CGCursor* g_cursor = nullptr;

namespace {
CURSORANIMATIONS s_cursorType = POINT_CURSOR;
CURSORANIMATIONS s_cursorMode = POINT_CURSOR;
HMODEL__* s_cursorModel = nullptr;
float INFINITY_138 = 0.0F;
float TWO_PI_138 = 0.0F;
float OO_TWO_PI_138 = 0.0F;
}

void CursorInitialize() {
    CursorDestroy();
    g_cursor = new CGCursor();
    s_cursorType = POINT_CURSOR;
    s_cursorMode = POINT_CURSOR;
}

void CursorDestroy() {
    delete g_cursor;
    g_cursor = nullptr;
    s_cursorModel = nullptr;
}

CGCursor::~CGCursor() {
    m_model = nullptr;
}

void CGCursor::Drop() {
    m_model = nullptr;
}

void CGCursor::SetItemType(CURSORITEMTYPE type) {
    if (type < CURSOR_EMPTY || type >= NUM_CURSOR_ITEM_TYPES) return;
    m_heldItem = type;
}

void CGCursor::Grab(HMODEL__* model) {
    Drop();
    m_model = model;
}

void CGCursor::SetArt(CModelCreate*) {
    m_model = nullptr;
}

void CGCursor::SetCursorAnim(CURSORANIMATIONS sequence) {
    if (sequence >= NUM_CURSOR_ANIMS) return;
    m_mouseOver = sequence;
}

void CGCursor::SetCursorMode(CURSORANIMATIONS sequence) {
    if (sequence >= NUM_CURSOR_ANIMS) return;
    m_cursorMode = sequence;
    if (m_mouseOver == NO_CURSOR) m_mouseOver = sequence;
}

void CGCursor::ResetCursor() {
    m_mouseOver = NO_CURSOR;
}

int CursorGrabMoney(HMODEL__* model) {
    if (!g_cursor || g_cursor->m_heldItem != CURSOR_EMPTY) return 0;
    g_cursor->SetItemType(CURSOR_MONEY);
    g_cursor->Grab(model);
    return 1;
}

int CursorGrabSpell(HMODEL__* model) {
    if (!g_cursor || g_cursor->m_heldItem != CURSOR_EMPTY) return 0;
    g_cursor->SetItemType(CURSOR_SPELL);
    g_cursor->Grab(model);
    return 1;
}

int CursorGrabMoney(unsigned, unsigned) {
    if (!g_cursor || g_cursor->m_heldItem != CURSOR_EMPTY) return 0;
    g_cursor->SetItemType(CURSOR_MONEY);
    return 1;
}

void CreateCursorIconModel(HTEXTURE__*) {
}

int CursorGrabSpell(CStatus* status) {
    if (!status || !g_cursor || g_cursor->m_heldItem != CURSOR_EMPTY) return 0;
    g_cursor->SetItemType(CURSOR_SPELL);
    return 1;
}

void CursorDropMoney() {
    if (g_cursor && g_cursor->m_heldItem == CURSOR_MONEY) {
        g_cursor->Drop();
        g_cursor->SetItemType(CURSOR_EMPTY);
    }
}

void CursorDropSpell() {
    if (g_cursor && g_cursor->m_heldItem == CURSOR_SPELL) {
        g_cursor->Drop();
        g_cursor->SetItemType(CURSOR_EMPTY);
    }
}

int __fastcall CursorHasSpell() {
    return g_cursor && g_cursor->m_heldItem == CURSOR_SPELL;
}

void __fastcall CursorModelSetSequence(CURSORANIMATIONS animation) {
    if (!g_cursor || animation >= NUM_CURSOR_ANIMS) return;
    s_cursorType = animation;
    if (s_cursorMode != CAST_CURSOR || animation != ATTACK_CURSOR)
        g_cursor->SetCursorAnim(animation);
}

CURSORANIMATIONS __fastcall CursorGetCursorType() { return s_cursorType; }
CURSORANIMATIONS __fastcall CursorGetCursorMode() { return s_cursorMode; }

void __fastcall CursorSetCursorMode(CURSORANIMATIONS mode) {
    if (!g_cursor || mode >= NUM_CURSOR_ANIMS) return;
    const auto previousMode = s_cursorMode;
    const auto previousType = s_cursorType;
    g_cursor->SetCursorMode(mode);
    s_cursorMode = mode;
    CursorResetCursor(0);
    if (previousType != previousMode && previousType != previousMode + ERROR_CURSORS)
        CursorModelSetSequence(previousType);
}

void CursorResetCursor(int force) {
    if (g_cursor && (s_cursorMode != CAST_CURSOR || force != 0)) {
        s_cursorType = s_cursorMode;
        g_cursor->ResetCursor();
    }
}

void __fastcall CursorSetHeldItem(std::uint64_t itemGuid) {
    if (!g_cursor) return;
    if (itemGuid == 0) {
        g_cursor->Drop();
        g_cursor->SetItemType(CURSOR_EMPTY);
        return;
    }
    g_cursor->SetItemType(CURSOR_ITEM);
}

void __fastcall CursorSetHeldVirtualItem(int itemDisplayInfo) {
    if (!g_cursor) return;
    if (itemDisplayInfo == 0) {
        g_cursor->Drop();
        g_cursor->SetItemType(CURSOR_EMPTY);
    } else {
        g_cursor->SetItemType(CURSOR_ITEM);
    }
}

int _E2_138() { return _E1_138(); }
int _E1_138() { INFINITY_138 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_138() { _E4_138(); return 0; }
void _E4_138() { TWO_PI_138 = 3.1415927F + 3.1415927F; }
int _E8_138() { _E7_138(); return 0; }
void _E7_138() { OO_TWO_PI_138 = 1.0F / TWO_PI_138; }

int _E28_62() { _E27_59(); return 0; }
void _E27_59() {}
int _E31_59() { _E30_57(); return 0; }
void _E30_57() {}
int _E34_60() { _E33_55(); return 0; }
void _E33_55() {}
int _E37_59() { _E36_54(); return 0; }
void _E36_54() {}
int _E40_60() { _E39_56(); return 0; }
void _E39_56() {}
int _E43_59() { _E42_55(); return 0; }
void _E42_55() {}
int _E46_57() { _E45_54(); return 0; }
void _E45_54() {}
int _E49_59() { _E48_59(); return 0; }
void _E48_59() {}
int _E52_58() { _E51_53(); return 0; }
void _E51_53() {}
int _E55_54() { _E54_58(); return 0; }
void _E54_58() {}
int _E58_53() { _E57_46(); return 0; }
void _E57_46() {}
int _E61_54() { _E60_54(); return 0; }
void _E60_54() {}
int _E68_20() { _E67_50(); return 0; }
void _E67_50() {}
int _E71_12() { _E70_42(); return 0; }
void _E70_42() {}
int _E74_9() { _E73_42(); return 0; }
void _E73_42() {}
int _E77_8() { _E76_41(); return 0; }
void _E76_41() {}
int _E80_12() { _E79_8(); return 0; }
void _E79_8() {}
