#pragma once

#include "WoW/Source/Net/netclient.h"

#include <cstdint>
#include <deque>
#include <mutex>
#include <vector>

struct NETEVENTQUEUENODE {
    EVENTID m_eventId = EVENT_ID_IDLE;
    unsigned m_timeReceived = 0;
    std::vector<std::uint8_t> m_data;
};

class NETEVENTQUEUE {
public:
    explicit NETEVENTQUEUE(NetClient* client = nullptr);
    ~NETEVENTQUEUE();

    void Poll();
    void AddEvent(EVENTID eventId, void* connection, NetClient* client,
                  const void* data, unsigned bytes);
    std::size_t Size() const;

    NetClient* m_client = nullptr;

private:
    mutable std::mutex m_critsect;
    std::deque<NETEVENTQUEUENODE> m_eventQueue;
};

struct WowConnectionInitializer {
    static int count;
    static void Initialize();
    static void Destroy();
};

extern float INFINITY_141;
extern float TWO_PI_141;
extern float OO_TWO_PI_141;
extern float MAX_DUEL_DISTANCE_SQUARED_72;
extern float MAX_TRADE_DISTANCE_SQUARED_72;
extern float MAX_SHOP_DISTANCE_SQUARED_72;
extern float MAX_INSPECT_DISTANCE_SQUARED_72;
extern float MAX_LOOT_DISTANCE_SQUARED_72;
extern float MAX_BIND_DISTANCE_SQUARED_72;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_72;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_72;
extern float MAX_SHARE_DISTANCE_SQUARED_72;
extern float SECONDS_PER_HEARTBEAT_72;
extern float SPELL_FOCUS_DISTANCE_SQUARED_71;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_72;

int _E2_141(); int _E1_141(); int _E5_141(); void _E4_141();
int _E8_141(); void _E7_141(); int _E18_4(); void _E15_8();
int _E17_11(); void _E16_15(); int _E21_20(); void _E20_37();
int _E24_22(); void _E23_39(); int _E27_62(); void _E26_35();
int _E30_60(); void _E29_34(); int _E33_58(); void _E32_32();
int _E36_57(); void _E35_32(); int _E39_59(); void _E38_32();
int _E42_58(); void _E41_30(); int _E45_57(); void _E44_32();
int _E48_62(); void _E47_34(); int _E51_56(); void _E50_31();
int _E54_61(); void _E53_29();
