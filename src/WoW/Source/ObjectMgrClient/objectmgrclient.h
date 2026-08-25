#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "ENGINE/Source/Tempest/c3vector.h"

#include <array>
#include <cstdint>
#include <vector>

struct ClntObjMgr;

struct CLNT_VISIBLE_OBJECT {
    std::uint64_t guid = 0;
    std::uint64_t ownerGuid = 0;
    std::uint8_t objectType = 0;
    std::uint32_t entryId = 0;
    std::uint32_t displayId = 0;
    std::uint32_t level = 0;
    std::uint32_t experience = 0;
    std::uint32_t nextLevelExperience = 0;
    std::uint32_t faction = 0;
    std::uint32_t health = 0;
    std::array<std::uint32_t, 4> power{};
    std::uint32_t maxHealth = 0;
    std::array<std::uint32_t, 4> maxPower{};
    std::uint32_t unitBytes0 = 0;
    std::uint32_t corpseBytes1 = 0;
    std::uint32_t corpseBytes2 = 0;
    std::uint8_t race = 0;
    std::uint8_t classId = 0;
    std::uint8_t sex = 0;
    std::uint8_t powerType = 0;
    float scale = 1.0F;
    NTempest::C3Vector position{};
    float facing = 0.0F;
    bool moving = false;
    float movementSpeed = 0.0F;
    float movementElapsed = 0.0F;
    float movementDuration = 0.0F;
    std::vector<NTempest::C3Vector> movementPath;
};

struct CLNT_ITEM_OBJECT {
    std::uint64_t guid = 0;
    std::uint64_t ownerGuid = 0;
    std::uint64_t containedGuid = 0;
    std::uint8_t objectType = 0;
    std::uint32_t entryId = 0;
    std::uint32_t stackCount = 0;
    std::uint32_t flags = 0;
};

void ClntObjMgrInitializeShared();
ClntObjMgr* ClntObjMgrCreate(int playerType, void* clientPtr);
void ClntObjMgrSetCurrent(ClntObjMgr* manager);
ClntObjMgr* ClntObjMgrGetCurrent();
void ClntObjMgrSetNet(void* net);
void* ClntObjMgrGetNet();
void ClntObjMgrInitialize();
void ClntObjMgrSetMapID(unsigned mapID);
unsigned ClntObjMgrGetMapID();
void ClntObjMgrSetActivePlayer(std::uint64_t guid);
std::uint64_t ClntObjMgrGetActivePlayer();
int ClntObjMgrGetPlayerType();
const NTempest::C3Vector& ClntObjMgrGetActivePosition();
float ClntObjMgrGetActiveFacing();
void ClntObjMgrSetActivePosition(const NTempest::C3Vector& position);
void ClntObjMgrSetActiveFacing(float facing);
const std::vector<CLNT_VISIBLE_OBJECT>& ClntObjMgrGetVisibleObjects();
const CLNT_VISIBLE_OBJECT* ClntObjMgrGetVisibleObject(std::uint64_t guid);
const CLNT_ITEM_OBJECT* ClntObjMgrGetItem(std::uint64_t guid);
std::uint64_t ClntObjMgrGetInventoryGuid(unsigned slot);
const CLNT_ITEM_OBJECT* ClntObjMgrGetInventoryItem(unsigned slot);
int ClntObjMgrFindInventorySlot(std::uint32_t itemEntry);
void ClntObjMgrUpdateVisibleObjects(float elapsedSec);
void ClntObjMgrPollNameQueries();

int __fastcall ObjectUpdateHandler(void* param, unsigned msgId, unsigned eventTime,
                                   CDataStore* message);
int __fastcall ObjectDestroyHandler(void* param, unsigned msgId,
                                    unsigned eventTime, CDataStore* message);
int __fastcall ObjectCompressedUpdateHandler(void* param, unsigned msgId,
                                             unsigned eventTime,
                                             CDataStore* message);
int __fastcall OnMonsterMoveEvent(void* param, unsigned msgId,
                                  unsigned eventTime, CDataStore* message);

class CClientMoveUpdate {
public:
    CClientMoveUpdate();
};

class CMirrorHandler {
public:
    ~CMirrorHandler();
};

class CMoveSpline {
public:
    CMoveSpline();
};

class CMovementStatus {
public:
    CMovementStatus();
    ~CMovementStatus();
};
