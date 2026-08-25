#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "ENGINE/Source/Event/event.h"

#include <array>
#include <cstdint>
#include <vector>

class NETEVENTQUEUE;
class NetClient;
class NetClientRedirect;
class WowConnection;

using NETMESSAGE = unsigned;
using NETMESSAGEHANDLER = int (__fastcall *)(void*, NETMESSAGE, unsigned, CDataStore*);

struct NETCONNADDR {
    std::uint32_t address = 0;
    std::uint16_t port = 0;
};

struct NETADDR {
    std::uint32_t address = 0;
    std::uint16_t port = 0;
};

enum NETSTATE : std::uint32_t {
    NS_UNINITIALIZED,
    NS_INITIALIZED,
    NS_REDIRECT_CONNECTING,
    NS_CONNECTING,
    NS_CONNECTED,
    NS_DISCONNECTING
};

enum WOWC_TYPE : std::uint32_t { WOWC_TYPE_STREAM, WOWC_TYPE_DATAGRAM };

class WowConnectionResponse {
public:
    virtual ~WowConnectionResponse() = default;
    virtual void WCGlobalLock() {}
    virtual void WCGlobalUnlock() {}
    virtual void WCDataReady(WowConnection*, WowConnection*, unsigned char*, unsigned char*) {}
    virtual void WCWriteReady(WowConnection*) {}
    virtual void WCConnected(WowConnection*, WowConnection*, unsigned, const NETCONNADDR*) {}
    virtual void WCDisconnected(WowConnection*, unsigned, const NETCONNADDR*) {}
    virtual void WCCantConnect(WowConnection*, unsigned, const NETCONNADDR*) {}
};

class NetClient : public WowConnectionResponse {
public:
    NetClient();
    ~NetClient() override;

    int Initialize();
    void Destroy();
    int DelayedDelete();
    void CancelRedirect();
    void Connect(const char* hostName);
    int Connect(const char* hostName, std::uint16_t port);
    void Disconnect();
    void Send(CDataStore* message);
    void SetMessageHandler(NETMESSAGE message, NETMESSAGEHANDLER handler, void* userData = nullptr);
    void ClearMessageHandler(NETMESSAGE message);
    void ProcessMessage(unsigned timeStamp, CDataStore* message);
    virtual int HandleData(unsigned timeReceived, unsigned char* data, unsigned size);
    virtual int HandleConnect();
    virtual int HandleDisconnect();
    virtual int HandleCantConnect();
    void PongHandler(CDataStore* message);
    void Ping();
    void DisplayNetworkStats();
    void GetNetStats(float* bandwidthIn, float* bandwidthOut, unsigned* latency);
    void HandleIdle();
    void HandleEvent(EVENTID eventId, const void* data, unsigned bytes);
    void HandleDestroy(bool fromQueue);
    void PushObjMgr();
    void PopObjMgr();
    void PollEventQueue();
    unsigned GetAddr();

    void WCMessageReady(WowConnection*, unsigned timeStamp, CDataStore* message);
    void WCConnected(WowConnection*, WowConnection*, unsigned timeStamp, const NETCONNADDR*);
    void WCDisconnected(WowConnection*, unsigned timeStamp, const NETCONNADDR*);
    void WCCantConnect(WowConnection*, unsigned timeStamp, const NETCONNADDR*);

    NETSTATE m_netState = NS_UNINITIALIZED;
    NETEVENTQUEUE* m_netEventQueue = nullptr;
    WowConnection* m_serverConnection = nullptr;
    int m_refCount = 0;
    bool m_deleteMe = false;
    bool m_pingSent = false;
    unsigned m_pingSequence = 0;
    std::uint64_t m_bytesSent = 0;
    std::uint64_t m_bytesReceived = 0;
    unsigned m_connectedTimestamp = 0;
    unsigned m_latencyStart = 0;
    unsigned m_latencyEnd = 0;
    std::array<unsigned, 16> m_latency{};
    void* m_objMgr = nullptr;
    void* m_saveObjMgr = nullptr;
    NetClientRedirect* m_redirect = nullptr;
    unsigned m_redirectBytesRead = 0;
    std::array<char, 1024> m_redirectHostPort{};
    std::array<NETMESSAGEHANDLER, 512> m_handlers{};
    std::array<void*, 512> m_handlerParams{};
    std::uint32_t m_lastAddress = 0;

    // Socket backing for the recovered WowConnection stream.  The original
    // class stored this state in m_serverConnection; keeping it here lets the
    // reconstructed client preserve NetClient's public behavior without
    // exposing WinSock types through the header.
    std::uintptr_t m_socketHandle = ~std::uintptr_t{0};
    std::vector<std::uint8_t> m_receiveBuffer;
    bool m_winsockStarted = false;

    static int s_clientCount;
};

class NetClientRedirect : public WowConnectionResponse {
public:
    explicit NetClientRedirect(NetClient* owner = nullptr) : m_owner(owner) {}
    ~NetClientRedirect() override = default;

    void WCMessageReady(WowConnection*, WowConnection*, CDataStore*) {}
    void WCConnected(WowConnection*, WowConnection*, unsigned, const NETCONNADDR*) {}
    void WCDisconnected(WowConnection*, unsigned, const NETCONNADDR*);
    void WCCantConnect(WowConnection*, unsigned, const NETCONNADDR*);
    void WCDataReady(WowConnection*, unsigned, unsigned char*, unsigned);

    NetClient* m_owner = nullptr;
    WowConnection* m_conn = nullptr;
};

void LogStats();
void InitializePropContext(unsigned context);
void ClientNetGetRealms(const char* serverAddress,
                        void (__fastcall *callback)(CDataStore*, void*),
                        void* userData);
int __stdcall GetRealmsEventHandler(void*, const NETCONNADDR*, int, char*, unsigned char*, unsigned, unsigned*);

extern float INFINITY_140;
extern float TWO_PI_140;
extern float OO_TWO_PI_140;
extern float MAX_DUEL_DISTANCE_SQUARED_71;
extern float MAX_TRADE_DISTANCE_SQUARED_71;
extern float MAX_SHOP_DISTANCE_SQUARED_71;
extern float MAX_INSPECT_DISTANCE_SQUARED_71;
extern float MAX_LOOT_DISTANCE_SQUARED_71;
extern float MAX_BIND_DISTANCE_SQUARED_71;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_71;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_71;
extern float MAX_SHARE_DISTANCE_SQUARED_71;
extern float SECONDS_PER_HEARTBEAT_71;
extern float SPELL_FOCUS_DISTANCE_SQUARED_70;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_71;

int _E2_140(); int _E1_140(); int _E5_140(); void _E4_140();
int _E8_140(); void _E7_140(); int _E18_3(); void _E15_7();
int _E17_10(); void _E16_14(); int _E21_19(); void _E20_36();
int _E24_21(); void _E23_38(); int _E27_61(); void _E26_34();
int _E30_59(); void _E29_33(); int _E33_57(); void _E32_31();
int _E36_56(); void _E35_31(); int _E39_58(); void _E38_31();
int _E42_57(); void _E41_29(); int _E45_56(); void _E44_31();
int _E48_61(); void _E47_33(); int _E51_55(); void _E50_30();
int _E54_60(); void _E53_28(); int _E77_10(); unsigned _E74_11();
int _E76_43(); void _E75_35();
