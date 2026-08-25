#pragma once

#include "WoW/Source/Net/netclient.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#ifdef SetPort
#undef SetPort
#endif

enum WOW_CONN_STATE : std::uint32_t {
    WOWC_UNINITIALIZED,
    WOWC_INITIALIZED,
    WOWC_CONNECTING,
    WOWC_CONNECTED,
    WOWC_DISCONNECTING,
    WOWC_DISCONNECTED,
    WOWC_LISTENING
};

struct WowConnectionStats {
    unsigned connectTime = 0;
    unsigned lastDataReceived = 0;
    std::uint64_t bytesSent = 0;
    std::uint64_t bytesReceived = 0;
};

class WowConnection {
public:
    struct SENDNODE {
        std::vector<std::uint8_t> data;
        std::size_t offset = 0;
        bool raw = false;
    };

    WowConnection(int sock, const NETCONNADDR* address, WowConnectionResponse* response);
    WowConnection(WowConnectionResponse* response = nullptr, void (__fastcall *threadInit)() = nullptr);
    ~WowConnection();

    static int CreateSocket();
    static void CloseSocket(int sock);
    static void DestroyOsNet();
    static bool IsDestroyed();
    static void RegisterSocket();

    void Init(WowConnectionResponse* response, void (__fastcall *threadInit)());
    int AddRef();
    int Release();
    void Disconnect();
    void DoDisconnect();
    SENDNODE* NewSendNode(void* data, int size, unsigned char raw);
    void FreeSendNode(SENDNODE* node);
    int Send(CDataStore* message);
    int SendRaw(unsigned char* data, int length);
    void CheckConnect();
    void CheckAccept();
    void DoWrites(int flags);
    void DoMessageReads();
    void DoStreamReads();
    void DoReads(int flags);
    void DoExceptions(int flags);
    void StartConnect();
    unsigned char Reconnect();
    unsigned char Listen(unsigned port);
    void StopListening();
    char* GetStringAddress(char* buffer, unsigned size);

    void SetResponse(WowConnectionResponse* response);
    void AcquireResponseRef();
    void ReleaseResponseRef();
    void SetState(WOW_CONN_STATE state);
    void SetType(WOWC_TYPE type);
    void RequestWriteNotification();
    bool GetLocal(NETADDR* address) const;
    unsigned GetAddr() const;
    unsigned short GetPort() const;
    static void SetPort(int port, unsigned short value);

    int m_refCount = 1;
    int m_sock = -1;
    int m_oldsock = -1;
    bool m_connectionFreed = false;
    WOW_CONN_STATE m_connState = WOWC_UNINITIALIZED;
    WowConnectionResponse* m_response = nullptr;
    unsigned m_needBytes = 0;
    std::vector<std::uint8_t> m_readBuffer;
    int m_readBytes = 0;
    int m_readBufferSize = 0;
    WowConnectionStats m_stats;
    unsigned m_haveSizeBytes = 0;
    unsigned short m_listenPort = 0;
    void (__fastcall *m_threadInit)() = nullptr;
    unsigned m_connectAddress = 0;
    unsigned short m_connectPort = 0;
    int m_connectRetryInterval = -1;
    unsigned m_retryConnection = 0;
    NETCONNADDR m_peer;
    unsigned m_bufferAutoSendSize = 0;
    int m_responseRef = 0;
    unsigned m_responseRefThread = 0;
    std::vector<SENDNODE*> m_sendList;
    int m_sendDepth = 0;
    int m_sendDepthBytes = 0;
    unsigned m_serviceFlags = 0;
    int m_serviceCount = 0;
    WOWC_TYPE m_type = WOWC_TYPE_STREAM;
    bool m_wantWriteNotification = false;
    mutable std::mutex m_lock;
    mutable std::mutex m_responseLock;
    mutable std::mutex m_outLock;
};

class WowConnectionNet {
public:
    WowConnectionNet() = default;
    ~WowConnectionNet();
    void Add(WowConnection* connection);
    void Remove(WowConnection* connection);
    void Start();
    void Stop();
    void Service(WowConnection* connection);
    void RunWorker(int id);
    void SignalWorker(WowConnection* connection, int flags);
    void Run();
    void Delete(WowConnection* connection);
    void PlatformInit(unsigned char useEngine);
    void PlatformDestroy();
    void PlatformAdd(WowConnection* connection);
    void PlatformRemove(WowConnection* connection);
    void PlatformChangeState(WowConnection* connection, WowConnection* oldState);
    void PlatformDestruct(WowConnection* connection);
    void PlatformWorkerReady();
    void PlatformRun(int* state = nullptr);

    std::vector<WowConnection*> m_connections;
    std::mutex m_connectionsLock;
    std::atomic<bool> m_stop{true};
    int m_numWorkers = 0;
    bool m_usingEngine = false;
};

void MakeSocketPipe(unsigned value);
int WinsockInit();
void WinsockDestroy();

extern float INFINITY_259;
extern float INFINITY_262;
extern float INFINITY_263;
int _E2_259(); int _E1_259(); int _E18_6(); void _E15_9(); int _E17_15(); void _E16_18();
int _E5_256(); void _E4_256(); int _E8_256(); void _E7_256();
int _E2_262(); int _E1_262(); int _E18_7(); void _E15_10(); int _E17_16(); void _E16_19();
int _E5_259(); void _E4_259(); int _E8_259(); void _E7_259();
int _E2_263(); int _E1_263(); int _E18_8(); void _E15_11(); int _E17_17(); void _E16_20();
int _E5_260(); void _E4_260(); int _E8_260(); void _E7_260();
