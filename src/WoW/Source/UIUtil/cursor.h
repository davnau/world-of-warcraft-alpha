#pragma once

#include <cstdint>

struct HMODEL__;
struct HTEXTURE__;
struct HOBJECT__;
struct CStatus;
struct CModelCreate;

enum CURSORITEMTYPE : int {
    CURSOR_EMPTY = 0,
    CURSOR_ITEM = 1,
    CURSOR_MONEY = 2,
    CURSOR_SPELL = 3,
    NUM_CURSOR_ITEM_TYPES = 4,
};

enum CURSORANIMATIONS : int {
    POINT_CURSOR = 0,
    CAST_CURSOR = 1,
    BUY_CURSOR = 2,
    ATTACK_CURSOR = 3,
    INTERACT_CURSOR = 4,
    SPEAK_CURSOR = 5,
    RANGED_CURSOR = 6,
    PICKUP_CURSOR = 7,
    TAXI_CURSOR = 8,
    ERROR_CURSORS = 9,
    POINT_ERROR_CURSOR = 9,
    CAST_ERROR_CURSOR = 10,
    BUY_ERROR_CURSOR = 11,
    ATTACK_ERROR_CURSOR = 12,
    INTERACT_ERROR_CURSOR = 13,
    SPEAK_ERROR_CURSOR = 14,
    RANGED_ERROR_CURSOR = 15,
    PICKUP_ERROR_CURSOR = 16,
    TAXI_ERROR_CURSOR = 17,
    NUM_CURSOR_ANIMS = 18,
    NO_CURSOR = 19,
};

class CGCursor {
public:
    HMODEL__* m_model = nullptr;
    CURSORITEMTYPE m_heldItem = CURSOR_EMPTY;
    CURSORANIMATIONS m_cursorMode = POINT_CURSOR;
    CURSORANIMATIONS m_mouseOver = NO_CURSOR;

    ~CGCursor();
    void Drop();
    void SetItemType(CURSORITEMTYPE type);
    void Grab(HMODEL__* model);
    void SetArt(CModelCreate* art);
    void SetCursorAnim(CURSORANIMATIONS sequence);
    void SetCursorMode(CURSORANIMATIONS sequence);
    void ResetCursor();
};

extern CGCursor* g_cursor;

void CursorInitialize();
void CursorDestroy();
int CursorGrabMoney(HMODEL__* model);
int CursorGrabSpell(HMODEL__* model);
int CursorGrabMoney(unsigned amount, unsigned iconFlags);
void CreateCursorIconModel(HTEXTURE__* texture);
int CursorGrabSpell(CStatus* status);
void CursorDropMoney();
void CursorDropSpell();
int __fastcall CursorHasSpell();
void __fastcall CursorModelSetSequence(CURSORANIMATIONS animation);
CURSORANIMATIONS __fastcall CursorGetCursorType();
CURSORANIMATIONS __fastcall CursorGetCursorMode();
void __fastcall CursorSetCursorMode(CURSORANIMATIONS mode);
void CursorResetCursor(int force);
void __fastcall CursorSetHeldItem(std::uint64_t itemGuid);
void __fastcall CursorSetHeldVirtualItem(int itemDisplayInfo);

int _E2_138();
int _E1_138();
int _E5_138();
void _E4_138();
int _E8_138();
void _E7_138();
int _E28_62(); void _E27_59();
int _E31_59(); void _E30_57();
int _E34_60(); void _E33_55();
int _E37_59(); void _E36_54();
int _E40_60(); void _E39_56();
int _E43_59(); void _E42_55();
int _E46_57(); void _E45_54();
int _E49_59(); void _E48_59();
int _E52_58(); void _E51_53();
int _E55_54(); void _E54_58();
int _E58_53(); void _E57_46();
int _E61_54(); void _E60_54();
int _E68_20(); void _E67_50();
int _E71_12(); void _E70_42();
int _E74_9(); void _E73_42();
int _E77_8(); void _E76_41();
int _E80_12(); void _E79_8();
