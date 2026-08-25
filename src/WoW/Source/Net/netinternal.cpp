#include "WoW/Source/Net/netinternal.h"

#include "ENGINE/Source/Os/W32/time.h"

#include <limits>

float INFINITY_141 = 0.0F;
float TWO_PI_141 = 0.0F;
float OO_TWO_PI_141 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_72 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_72 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_72 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_72 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_72 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_72 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_72 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_72 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_72 = 0.0F;
float SECONDS_PER_HEARTBEAT_72 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_71 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_72 = 0.0F;
int WowConnectionInitializer::count = 0;

void WowConnectionInitializer::Initialize() {}
void WowConnectionInitializer::Destroy() {}

NETEVENTQUEUE::NETEVENTQUEUE(NetClient* client) : m_client(client) {}

NETEVENTQUEUE::~NETEVENTQUEUE() {
    std::lock_guard lock(m_critsect);
    m_eventQueue.clear();
}

void NETEVENTQUEUE::AddEvent(EVENTID eventId, void*, NetClient* client,
                             const void* data, unsigned bytes) {
    std::lock_guard lock(m_critsect);
    NETEVENTQUEUENODE node;
    node.m_eventId = eventId;
    node.m_timeReceived = OsGetAsyncTimeMs();
    if (data != nullptr && bytes != 0) {
        const auto* begin = static_cast<const std::uint8_t*>(data);
        node.m_data.assign(begin, begin + bytes);
    }
    if (client != nullptr)
        m_client = client;
    m_eventQueue.push_back(std::move(node));
}

std::size_t NETEVENTQUEUE::Size() const {
    std::lock_guard lock(m_critsect);
    return m_eventQueue.size();
}

void NETEVENTQUEUE::Poll() {
    std::deque<NETEVENTQUEUENODE> events;
    {
        std::lock_guard lock(m_critsect);
        events.swap(m_eventQueue);
    }
    bool deleted = false;
    for (const NETEVENTQUEUENODE& event : events) {
        if (m_client == nullptr || m_client->m_deleteMe)
            break;
        const void* data = event.m_data.empty() ? nullptr : event.m_data.data();
        m_client->HandleEvent(event.m_eventId, data,
                              static_cast<unsigned>(event.m_data.size()));
        if (m_client->m_deleteMe) {
            deleted = true;
            break;
        }
    }
    if (!deleted && m_client != nullptr)
        m_client->HandleIdle();
    if (deleted && m_client != nullptr)
        m_client->HandleDestroy(true);
}

int _E2_141() { return _E1_141(); }
int _E1_141() { INFINITY_141 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_141() { _E4_141(); return 0; }
void _E4_141() { TWO_PI_141 = 3.1415927F + 3.1415927F; }
int _E8_141() { _E7_141(); return 0; }
void _E7_141() { OO_TWO_PI_141 = 1.0F / TWO_PI_141; }

#define NET_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
NET_CONST_PAIR(_E21_20, _E20_37, MAX_DUEL_DISTANCE_SQUARED_72, 100.0F)
NET_CONST_PAIR(_E24_22, _E23_39, MAX_TRADE_DISTANCE_SQUARED_72, 123.45679F)
NET_CONST_PAIR(_E27_62, _E26_35, MAX_SHOP_DISTANCE_SQUARED_72, 30.864195F)
NET_CONST_PAIR(_E30_60, _E29_34, MAX_INSPECT_DISTANCE_SQUARED_72, 30.864195F)
NET_CONST_PAIR(_E33_58, _E32_32, MAX_LOOT_DISTANCE_SQUARED_72, 25.0F)
NET_CONST_PAIR(_E36_57, _E35_32, MAX_BIND_DISTANCE_SQUARED_72, 100.0F)
NET_CONST_PAIR(_E39_59, _E38_32, MAX_SITCHAIR_DISTANCE_SQUARED_72, 0.25F)
NET_CONST_PAIR(_E42_58, _E41_30, MAX_SITCHAIRUSE_DISTANCE_SQUARED_72, 9.0F)
NET_CONST_PAIR(_E45_57, _E44_32, MAX_SHARE_DISTANCE_SQUARED_72, 10000.0F)
NET_CONST_PAIR(_E48_62, _E47_34, SECONDS_PER_HEARTBEAT_72, 5.0F)
NET_CONST_PAIR(_E51_56, _E50_31, SPELL_FOCUS_DISTANCE_SQUARED_71, 2500.0F)
NET_CONST_PAIR(_E54_61, _E53_29, MAX_OBJ_INTEREST_RADIUS_SQUARED_72, 10000.0F)
#undef NET_CONST_PAIR

int _E18_4() { _E15_8(); return _E17_11(); }
void _E15_8() { if (++WowConnectionInitializer::count == 1) WowConnectionInitializer::Initialize(); }
int _E17_11() { return 0; }
void _E16_15() { if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0) WowConnectionInitializer::Destroy(); }
