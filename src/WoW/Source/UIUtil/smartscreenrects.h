#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/crect.h"

class CLayoutFrame;

enum SCREENRECTGRIDS : int {
    SRECTGRID_NAMEPLATES = 0,
    SRECTGRID_WORLDTEXT = 1,
    NUM_SRECTGRIDS = 2,
};

enum TEST_DIRECTION : int {
    TEST_UP = 0,
    TEST_LEFT = 1,
    TEST_RIGHT = 2,
    TEST_DOWN = 3,
    NUM_TESTDIRECTIONS = 4,
    TEST_INVALID = -1,
};

struct BFSNODE {
    NTempest::CRect nodeRect{};
    TEST_DIRECTION dontTestDirection = TEST_INVALID;
};

extern float INFINITY_131;

void SmartScreenRectInitialize(const char* cvarName = nullptr);
void SmartScreenRectShutdown();
void SmartScreenRectClearAllGrids();
void SmartScreenRectGridPos(SCREENRECTGRIDS grid, NTempest::CRect& rect);
void SmartScreenRectGetGridPos(SCREENRECTGRIDS grid, CLayoutFrame* frameToPlace,
                               float totalWidth, float totalHeight,
                               CLayoutFrame* base, const NTempest::C2Vector& position,
                               int positionFromCenter);

int RectCollides(SCREENRECTGRIDS grid, const NTempest::CRect& rect,
                 TEST_DIRECTION direction, float* offset);
NTempest::CRect* TestUp(NTempest::CRect* result, SCREENRECTGRIDS grid,
                        NTempest::CRect rect);
NTempest::CRect* TestLeft(NTempest::CRect* result, SCREENRECTGRIDS grid,
                          NTempest::CRect rect);
NTempest::CRect* TestRight(NTempest::CRect* result, SCREENRECTGRIDS grid,
                           NTempest::CRect rect);
NTempest::CRect* TestDown(NTempest::CRect* result, SCREENRECTGRIDS grid,
                          NTempest::CRect rect);

void MarkRect(const NTempest::CRect& rect, SCREENRECTGRIDS grid);
bool CheckRect(const NTempest::CRect& rect, int allowEmpty);
NTempest::CRect* FindFreeRect(SCREENRECTGRIDS grid, NTempest::CRect* result,
                              const NTempest::CRect& rect);
NTempest::C2Vector* FindNextAvailableRect(SCREENRECTGRIDS grid,
                                          NTempest::C2Vector* result,
                                          const NTempest::CRect& rect,
                                          int* repositioned);
int RectOutsideBorder(NTempest::CRect* clipped, int writeClipped,
                      NTempest::CRect rect);
unsigned SelectNewSearchPattern(char hitFlags);
int CalculateMaxTraversals(const NTempest::CRect& rect);
void ClipRect(NTempest::CRect& rect);

void CleanupActiveBFSNodes();
BFSNODE* GetBFSNode();

int _E2_131(); int _E1_131();
int _E177(); unsigned* _E174_0(); int _E176_0(); void _E175_0();
int _E182_0(); unsigned _E179(); int _E181(); void _E180();
int _E187_4(); unsigned _E184_3(); int _E186_5(); void _E185_5();
int _E5_131(); void _E4_131(); int _E8_131(); void _E7_131();
char* TSBaseArray_CRect_MemFileName(); int TSBaseArray_CRect_MemLineNo();
void TSLinkedNode_BFSNODE_Destroy(BFSNODE* node);
void TSLink_BFSNODE_NextLink(); void TSLink_BFSNODE_Unlink();
void TSFixedArray_CRect_ReallocData(unsigned count);
int _E28_55(); void _E27_52(); int _E31_52(); void _E30_50();
int _E34_53(); void _E33_48(); int _E37_52(); void _E36_47();
int _E40_53(); void _E39_49(); int _E43_52(); void _E42_48();
int _E46_50(); void _E45_47(); int _E49_52(); void _E48_52();
int _E52_51(); void _E51_46(); int _E55_47(); void _E54_51();
int _E58_46(); void _E57_39(); int _E61_47(); void _E60_47();
int _E68_13(); void _E67_43(); int _E71_5(); void _E70_35();
int _E74_2(); void _E73_35(); int _E77_1(); void _E76_34();
int _E80_5(); void _E79_1();
