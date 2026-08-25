#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "WoW/Source/Net/netclient.h"

#include "WoW/Source/Net/netinternal.h"
#include "ENGINE/Source/Os/W32/time.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

float INFINITY_140 = 0.0F;
float TWO_PI_140 = 0.0F;
float OO_TWO_PI_140 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_71 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_71 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_71 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_71 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_71 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_71 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_71 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_71 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_71 = 0.0F;
float SECONDS_PER_HEARTBEAT_71 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_70 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_71 = 0.0F;

int NetClient::s_clientCount = 0;

namespace {
constexpr std::uintptr_t INVALID_SOCKET_HANDLE = ~std::uintptr_t{0};

SOCKET ToSocket(std::uintptr_t handle)
{
    return handle == INVALID_SOCKET_HANDLE ? INVALID_SOCKET
                                           : static_cast<SOCKET>(handle);
}

void CloseClientSocket(NetClient* client)
{
    if (!client)
        return;
    const SOCKET socketHandle = ToSocket(client->m_socketHandle);
    if (socketHandle != INVALID_SOCKET) {
        shutdown(socketHandle, SD_BOTH);
        closesocket(socketHandle);
    }
    client->m_socketHandle = INVALID_SOCKET_HANDLE;
    client->m_receiveBuffer.clear();
}

SOCKET ConnectSocket(const char* hostName, std::uint16_t port,
                     std::uint32_t* resolvedAddress = nullptr)
{
    if (!hostName || !*hostName || port == 0)
        return INVALID_SOCKET;

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    addrinfo* addresses = nullptr;
    const std::string service = std::to_string(port);
    if (getaddrinfo(hostName, service.c_str(), &hints, &addresses) != 0)
        return INVALID_SOCKET;

    SOCKET result = INVALID_SOCKET;
    for (addrinfo* address = addresses; address; address = address->ai_next) {
        SOCKET candidate = socket(address->ai_family, address->ai_socktype,
                                  address->ai_protocol);
        if (candidate == INVALID_SOCKET)
            continue;

        u_long nonblocking = 1;
        ioctlsocket(candidate, FIONBIO, &nonblocking);
        const int connectResult = connect(candidate, address->ai_addr,
                                          static_cast<int>(address->ai_addrlen));
        if (connectResult == 0 || WSAGetLastError() == WSAEWOULDBLOCK) {
            fd_set writeSet;
            FD_ZERO(&writeSet);
            FD_SET(candidate, &writeSet);
            timeval timeout{3, 0};
            if (select(0, nullptr, &writeSet, nullptr, &timeout) > 0) {
                int socketError = 0;
                int errorSize = sizeof(socketError);
                getsockopt(candidate, SOL_SOCKET, SO_ERROR,
                           reinterpret_cast<char*>(&socketError), &errorSize);
                if (socketError == 0) {
                    nonblocking = 0;
                    ioctlsocket(candidate, FIONBIO, &nonblocking);
                    if (resolvedAddress && address->ai_family == AF_INET) {
                        const auto* ipv4 = reinterpret_cast<const sockaddr_in*>(address->ai_addr);
                        *resolvedAddress = ipv4->sin_addr.s_addr;
                    }
                    result = candidate;
                    break;
                }
            }
        }
        closesocket(candidate);
    }
    freeaddrinfo(addresses);
    return result;
}

bool SendAll(SOCKET socketHandle, const std::uint8_t* data, std::size_t bytes)
{
    while (bytes != 0) {
        const int sent = send(socketHandle, reinterpret_cast<const char*>(data),
                              static_cast<int>(std::min<std::size_t>(bytes, 0x7FFFFFFF)), 0);
        if (sent <= 0)
            return false;
        data += sent;
        bytes -= static_cast<std::size_t>(sent);
    }
    return true;
}

bool ReadRedirect(SOCKET socketHandle, std::array<char, 1024>& hostPort,
                  unsigned& bytesRead)
{
    bytesRead = 0;
    const DWORD timeout = 3000;
    setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO,
               reinterpret_cast<const char*>(&timeout), sizeof(timeout));
    while (bytesRead + 1 < hostPort.size()) {
        const int received = recv(socketHandle, hostPort.data() + bytesRead,
                                  static_cast<int>(hostPort.size() - bytesRead - 1), 0);
        if (received <= 0)
            break;
        bytesRead += static_cast<unsigned>(received);
        if (std::memchr(hostPort.data(), 0, bytesRead) != nullptr)
            break;
    }
    hostPort[std::min<std::size_t>(bytesRead, hostPort.size() - 1)] = 0;
    return bytesRead != 0 && hostPort[0] != 0;
}

void QueueSocketFailure(NetClient* client)
{
    if (!client || !client->m_netEventQueue)
        return;
    const EVENTID eventId = client->m_netState == NS_CONNECTED
        ? EVENT_ID_NET_DISCONNECT : EVENT_ID_NET_CANTCONNECT;
    client->m_netEventQueue->AddEvent(eventId, nullptr, client, nullptr, 0);
}

void PumpWorldSocket(NetClient* client)
{
    if (!client)
        return;
    const SOCKET socketHandle = ToSocket(client->m_socketHandle);
    if (socketHandle == INVALID_SOCKET ||
        (client->m_netState != NS_CONNECTING && client->m_netState != NS_CONNECTED))
        return;

    bool socketClosed = false;
    std::array<std::uint8_t, 8192> input{};
    for (;;) {
        const int received = recv(socketHandle, reinterpret_cast<char*>(input.data()),
                                  static_cast<int>(input.size()), 0);
        if (received > 0) {
            client->m_receiveBuffer.insert(client->m_receiveBuffer.end(),
                                           input.begin(), input.begin() + received);
            continue;
        }
        if (received == 0) {
            closesocket(socketHandle);
            client->m_socketHandle = INVALID_SOCKET_HANDLE;
            socketClosed = true;
        } else {
            const int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                closesocket(socketHandle);
                client->m_socketHandle = INVALID_SOCKET_HANDLE;
                socketClosed = true;
            }
        }
        break;
    }

    while (client->m_receiveBuffer.size() >= 2) {
        const std::uint16_t declared = static_cast<std::uint16_t>(
            (static_cast<std::uint16_t>(client->m_receiveBuffer[0]) << 8) |
             static_cast<std::uint16_t>(client->m_receiveBuffer[1]));
        if (declared < sizeof(NETMESSAGE) || declared > 0x8000) {
            QueueSocketFailure(client);
            CloseClientSocket(client);
            return;
        }
        const std::size_t frameBytes = static_cast<std::size_t>(declared) + 2;
        if (client->m_receiveBuffer.size() < frameBytes)
            break;
        if (client->m_netEventQueue) {
            client->m_netEventQueue->AddEvent(EVENT_ID_NET_DATA, nullptr, client,
                                               client->m_receiveBuffer.data() + 2,
                                               declared);
        }
        client->m_receiveBuffer.erase(client->m_receiveBuffer.begin(),
                                      client->m_receiveBuffer.begin() + frameBytes);
    }
    if (socketClosed) {
        QueueSocketFailure(client);
        client->m_receiveBuffer.clear();
    }
}
}

int _E2_140() { return _E1_140(); }
int _E1_140() { INFINITY_140 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_140() { _E4_140(); return 0; }
void _E4_140() { TWO_PI_140 = 6.2831853071795864769F; }
int _E8_140() { return _E7_140(), 0; }
void _E7_140() { OO_TWO_PI_140 = 1.0F / TWO_PI_140; }
int _E18_3() { _E15_7(); return _E17_10(); }
void _E15_7() { if (++WowConnectionInitializer::count == 1) WowConnectionInitializer::Initialize(); }
int _E17_10() { return 0; }
void _E16_14() { if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0) WowConnectionInitializer::Destroy(); }

#define NET_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
NET_CONST_PAIR(_E21_19, _E20_36, MAX_DUEL_DISTANCE_SQUARED_71, 100.0F)
NET_CONST_PAIR(_E24_21, _E23_38, MAX_TRADE_DISTANCE_SQUARED_71, 123.45679F)
NET_CONST_PAIR(_E27_61, _E26_34, MAX_SHOP_DISTANCE_SQUARED_71, 30.864195F)
NET_CONST_PAIR(_E30_59, _E29_33, MAX_INSPECT_DISTANCE_SQUARED_71, 30.864195F)
NET_CONST_PAIR(_E33_57, _E32_31, MAX_LOOT_DISTANCE_SQUARED_71, 25.0F)
NET_CONST_PAIR(_E36_56, _E35_31, MAX_BIND_DISTANCE_SQUARED_71, 100.0F)
NET_CONST_PAIR(_E39_58, _E38_31, MAX_SITCHAIR_DISTANCE_SQUARED_71, 0.25F)
NET_CONST_PAIR(_E42_57, _E41_29, MAX_SITCHAIRUSE_DISTANCE_SQUARED_71, 9.0F)
NET_CONST_PAIR(_E45_56, _E44_31, MAX_SHARE_DISTANCE_SQUARED_71, 10000.0F)
NET_CONST_PAIR(_E48_61, _E47_33, SECONDS_PER_HEARTBEAT_71, 5.0F)
NET_CONST_PAIR(_E51_55, _E50_30, SPELL_FOCUS_DISTANCE_SQUARED_70, 2500.0F)
NET_CONST_PAIR(_E54_60, _E53_28, MAX_OBJ_INTEREST_RADIUS_SQUARED_71, 10000.0F)
#undef NET_CONST_PAIR

int _E77_10() { return _E74_11(), _E76_43(); }
unsigned _E74_11() { return 0; }
int _E76_43() { return 0; }
void _E75_35() {}

NetClient::NetClient() {
    m_handlers.fill(nullptr);
    m_handlerParams.fill(nullptr);
}

NetClient::~NetClient() {
    Destroy();
    delete m_redirect;
    m_redirect = nullptr;
}

int NetClient::Initialize() {
    if (m_netState != NS_UNINITIALIZED)
        return 1;
    if (++s_clientCount == 1)
        WowConnectionInitializer::Initialize();
    WSADATA winsock{};
    if (WSAStartup(MAKEWORD(2, 2), &winsock) != 0) {
        if (s_clientCount > 0 && --s_clientCount == 0)
            WowConnectionInitializer::Destroy();
        return 0;
    }
    m_winsockStarted = true;
    m_netEventQueue = new NETEVENTQUEUE(this);
    m_netState = NS_INITIALIZED;
    m_connectedTimestamp = OsGetAsyncTimeMs();
    return m_netEventQueue != nullptr;
}

void NetClient::Destroy() {
    if (m_netState == NS_UNINITIALIZED)
        return;
    Disconnect();
    delete m_netEventQueue;
    m_netEventQueue = nullptr;
    m_handlers.fill(nullptr);
    m_handlerParams.fill(nullptr);
    m_serverConnection = nullptr;
    m_netState = NS_UNINITIALIZED;
    if (m_winsockStarted) {
        WSACleanup();
        m_winsockStarted = false;
    }
    if (s_clientCount > 0 && --s_clientCount == 0)
        WowConnectionInitializer::Destroy();
}

int NetClient::DelayedDelete() {
    if (m_netState == NS_UNINITIALIZED)
        return 0;
    Disconnect();
    m_deleteMe = true;
    if (m_netEventQueue)
        m_netEventQueue->AddEvent(EVENT_ID_NET_DESTROY, nullptr, this, nullptr, 0);
    return 1;
}

void NetClient::CancelRedirect() {
    if (m_netState == NS_REDIRECT_CONNECTING) {
        m_redirectBytesRead = 0;
        m_netState = NS_INITIALIZED;
    }
}

void NetClient::Connect(const char* hostName) {
    if (!hostName)
        return;
    CancelRedirect();
    if (m_netState != NS_INITIALIZED)
        return;
    m_redirectBytesRead = 0;
    std::array<char, 1024> proxyHost{};
    const std::size_t hostLength = std::min(std::strlen(hostName), proxyHost.size() - 1);
    std::copy_n(hostName, hostLength, proxyHost.data());
    proxyHost[hostLength] = 0;
    char* colon = std::strrchr(proxyHost.data(), ':');
    std::uint16_t port = 9090;
    if (colon) {
        port = static_cast<std::uint16_t>(std::clamp(std::strtoul(colon + 1, nullptr, 10), 1UL, 65535UL));
        *colon = 0;
    }
    m_netState = NS_REDIRECT_CONNECTING;
    const SOCKET redirectSocket = ConnectSocket(proxyHost.data(), port);
    if (redirectSocket == INVALID_SOCKET ||
        !ReadRedirect(redirectSocket, m_redirectHostPort, m_redirectBytesRead)) {
        if (redirectSocket != INVALID_SOCKET)
            closesocket(redirectSocket);
        m_netState = NS_CONNECTING;
        QueueSocketFailure(this);
        return;
    }
    closesocket(redirectSocket);

    char* redirectColon = std::strrchr(m_redirectHostPort.data(), ':');
    if (!redirectColon) {
        m_netState = NS_CONNECTING;
        QueueSocketFailure(this);
        return;
    }
    *redirectColon = 0;
    const unsigned long redirectPort = std::strtoul(redirectColon + 1, nullptr, 10);
    if (redirectPort == 0 || redirectPort > 65535 || m_redirectHostPort[0] == 0) {
        m_netState = NS_CONNECTING;
        QueueSocketFailure(this);
        return;
    }
    m_netState = NS_INITIALIZED;
    Connect(m_redirectHostPort.data(), static_cast<std::uint16_t>(redirectPort));
}

int NetClient::Connect(const char* hostName, std::uint16_t port) {
    if (!hostName || !*hostName || m_netState != NS_INITIALIZED)
        return 0;
    CloseClientSocket(this);
    m_netState = NS_CONNECTING;
    m_lastAddress = 0;
    const SOCKET socketHandle = ConnectSocket(hostName, port, &m_lastAddress);
    if (socketHandle == INVALID_SOCKET) {
        QueueSocketFailure(this);
        return 0;
    }
    u_long nonblocking = 1;
    ioctlsocket(socketHandle, FIONBIO, &nonblocking);
    m_socketHandle = static_cast<std::uintptr_t>(socketHandle);
    if (m_netEventQueue)
        m_netEventQueue->AddEvent(EVENT_ID_NET_CONNECT, nullptr, this, nullptr, 0);
    return 1;
}

void NetClient::Disconnect() {
    CancelRedirect();
    const bool notify = m_netState == NS_CONNECTED || m_netState == NS_CONNECTING;
    if (m_netState == NS_CONNECTED)
        m_netState = NS_DISCONNECTING;
    CloseClientSocket(this);
    if (notify && m_netEventQueue)
        m_netEventQueue->AddEvent(EVENT_ID_NET_DISCONNECT, nullptr, this, nullptr, 0);
    if (m_netState != NS_UNINITIALIZED)
        m_netState = NS_INITIALIZED;
}

void NetClient::Send(CDataStore* message) {
    if (!message || m_netState != NS_CONNECTED || message->m_size < sizeof(NETMESSAGE) ||
        message->m_size > 0xFFFF)
        return;
    const SOCKET socketHandle = ToSocket(m_socketHandle);
    if (socketHandle == INVALID_SOCKET)
        return;
    const std::uint16_t size = static_cast<std::uint16_t>(message->m_size);
    const std::array<std::uint8_t, 2> header{{
        static_cast<std::uint8_t>(size >> 8), static_cast<std::uint8_t>(size & 0xFF)}};
    if (!SendAll(socketHandle, header.data(), header.size()) ||
        !SendAll(socketHandle, message->m_data, message->m_size)) {
        QueueSocketFailure(this);
        CloseClientSocket(this);
        return;
    }
    m_bytesSent += message->m_size;

    NETMESSAGE id = 0;
    std::memcpy(&id, message->m_data, sizeof(id));
    if (id == 0x95)
        std::fprintf(stderr, "CHAT_SOCKET_SEND opcode=0x%X bytes=%u\n", id, message->m_size);
}

void NetClient::SetMessageHandler(NETMESSAGE message, NETMESSAGEHANDLER handler, void* userData) {
    if (message < m_handlers.size()) {
        m_handlers[message] = handler;
        m_handlerParams[message] = userData;
    }
}

void NetClient::ClearMessageHandler(NETMESSAGE message) {
    if (message < m_handlers.size()) {
        m_handlers[message] = nullptr;
        m_handlerParams[message] = nullptr;
    }
}

void NetClient::ProcessMessage(unsigned timeStamp, CDataStore* message) {
    if (!message)
        return;
    NETMESSAGE id = 0;
    message->Get(&id);
    if (id == 0x96)
        std::fprintf(stderr, "CHAT_SOCKET_RECV opcode=0x%X bytes=%u\n", id, message->m_size);
    if (id < m_handlers.size() && m_handlers[id])
        m_handlers[id](m_handlerParams[id], id, timeStamp, message);
    else
        message->Reset();
}

int NetClient::HandleData(unsigned timeReceived, unsigned char* data, unsigned size) {
    if (m_netState != NS_CONNECTED && m_netState != NS_DISCONNECTING)
        return 0;
    m_bytesReceived += size;
    if (m_netState == NS_CONNECTED && data) {
        CDataStore message(data, size);
        ProcessMessage(timeReceived, &message);
    }
    return 1;
}

int NetClient::HandleConnect() {
    m_netState = NS_CONNECTED;
    return 1;
}

int NetClient::HandleDisconnect() {
    m_netState = NS_INITIALIZED;
    return 1;
}

int NetClient::HandleCantConnect() {
    m_netState = NS_INITIALIZED;
    return 1;
}

void NetClient::PongHandler(CDataStore* message) {
    if (!message)
        return;
    unsigned sequence = 0;
    message->Get(&sequence);
    if (sequence == m_pingSequence) {
        m_latency[m_latencyEnd] = OsGetAsyncTimeMsPrecise() - m_connectedTimestamp;
        m_latencyEnd = (m_latencyEnd + 1) % m_latency.size();
        if (m_latencyEnd == m_latencyStart)
            m_latencyStart = (m_latencyStart + 1) % m_latency.size();
    }
}

void NetClient::Ping() {
    CDataStore message;
    message.Put<NETMESSAGE>(461);
    message.Put(++m_pingSequence);
    m_pingSent = true;
    Send(&message);
}

void NetClient::DisplayNetworkStats() {
    float in = 0.0F, out = 0.0F; unsigned latency = 0;
    GetNetStats(&in, &out, &latency);
}

void NetClient::GetNetStats(float* bandwidthIn, float* bandwidthOut, unsigned* latency) {
    const float seconds = std::max(0.001F, (OsGetAsyncTimeMs() - m_connectedTimestamp) * 0.001F);
    if (bandwidthIn) *bandwidthIn = static_cast<float>(m_bytesReceived) / 1024.0F / seconds;
    if (bandwidthOut) *bandwidthOut = static_cast<float>(m_bytesSent) / 1024.0F / seconds;
    unsigned total = 0, count = 0;
    for (unsigned i = m_latencyStart; i != m_latencyEnd; i = (i + 1) % m_latency.size()) {
        total += m_latency[i]; ++count;
    }
    if (latency) *latency = count ? total / count : 0;
}

void NetClient::HandleIdle() {
    if (!m_pingSent || OsGetAsyncTimeMs() - m_connectedTimestamp >= 30000)
        Ping();
}

void NetClient::HandleEvent(EVENTID eventId, const void* data, unsigned bytes) {
    switch (eventId) {
    case EVENT_ID_NET_DATA:
        HandleData(OsGetAsyncTimeMs(), const_cast<unsigned char*>(static_cast<const unsigned char*>(data)), bytes);
        break;
    case EVENT_ID_NET_CONNECT: HandleConnect(); break;
    case EVENT_ID_NET_DISCONNECT: HandleDisconnect(); break;
    case EVENT_ID_NET_CANTCONNECT: HandleCantConnect(); break;
    case EVENT_ID_NET_DESTROY: m_deleteMe = true; break;
    default: break;
    }
}

void NetClient::HandleDestroy(bool fromQueue) {
    (void)fromQueue;
    m_deleteMe = true;
}

void NetClient::PushObjMgr() { if (m_objMgr) std::swap(m_objMgr, m_saveObjMgr); }
void NetClient::PopObjMgr() { if (m_saveObjMgr) std::swap(m_objMgr, m_saveObjMgr); }
void NetClient::PollEventQueue() {
    PumpWorldSocket(this);
    if (m_netEventQueue)
        m_netEventQueue->Poll();
}
unsigned NetClient::GetAddr() { return m_lastAddress; }

void NetClient::WCMessageReady(WowConnection* connection, unsigned timeStamp, CDataStore* message) {
    (void)connection;
    ProcessMessage(timeStamp, message);
}
void NetClient::WCConnected(WowConnection*, WowConnection*, unsigned timeStamp, const NETCONNADDR* address) {
    m_connectedTimestamp = timeStamp ? timeStamp : OsGetAsyncTimeMs();
    m_netState = NS_CONNECTING;
    if (address) m_lastAddress = address->address;
    HandleConnect();
}
void NetClient::WCDisconnected(WowConnection*, unsigned, const NETCONNADDR*) { HandleDisconnect(); }
void NetClient::WCCantConnect(WowConnection*, unsigned, const NETCONNADDR*) { HandleCantConnect(); }

void NetClientRedirect::WCDisconnected(WowConnection*, unsigned, const NETCONNADDR*) {
    if (!m_owner) return;
    m_owner->m_netState = NS_INITIALIZED;
    if (m_owner->m_netEventQueue)
        m_owner->m_netEventQueue->AddEvent(EVENT_ID_NET_CANTCONNECT, nullptr, m_owner, nullptr, 0);
}
void NetClientRedirect::WCCantConnect(WowConnection*, unsigned, const NETCONNADDR*) {
    WCDisconnected(nullptr, 0, nullptr);
}
void NetClientRedirect::WCDataReady(WowConnection*, unsigned, unsigned char* data, unsigned bytes) {
    if (!m_owner || !data) return;
    const unsigned available = static_cast<unsigned>(m_owner->m_redirectHostPort.size()) - m_owner->m_redirectBytesRead;
    const unsigned copyBytes = std::min(bytes, available);
    std::memcpy(m_owner->m_redirectHostPort.data() + m_owner->m_redirectBytesRead, data, copyBytes);
    m_owner->m_redirectBytesRead += copyBytes;
}

void LogStats() {}
void InitializePropContext(unsigned) {}

void ClientNetGetRealms(const char* serverAddress,
                        void (__fastcall *callback)(CDataStore*, void*),
                        void* userData) {
    if (!serverAddress || !*serverAddress || !callback) {
        if (callback) callback(nullptr, userData);
        return;
    }

    std::array<char, 1024> host{};
    const std::size_t length = std::min(std::strlen(serverAddress), host.size() - 1);
    std::memcpy(host.data(), serverAddress, length);
    unsigned short port = 0x238C;
    if (char* colon = std::strrchr(host.data(), ':')) {
        const unsigned long parsed = std::strtoul(colon + 1, nullptr, 10);
        if (parsed > 0 && parsed <= 65535) {
            port = static_cast<unsigned short>(parsed);
            *colon = 0;
        }
    }

    WSADATA winsock{};
    if (WSAStartup(MAKEWORD(2, 2), &winsock) != 0) {
        callback(nullptr, userData);
        return;
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    addrinfo* addresses = nullptr;
    const std::string service = std::to_string(port);
    SOCKET socketHandle = INVALID_SOCKET;
    if (getaddrinfo(host.data(), service.c_str(), &hints, &addresses) == 0) {
        for (addrinfo* address = addresses; address; address = address->ai_next) {
            socketHandle = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
            if (socketHandle == INVALID_SOCKET)
                continue;
            u_long nonblocking = 1;
            ioctlsocket(socketHandle, FIONBIO, &nonblocking);
            const int connectResult = connect(socketHandle, address->ai_addr,
                                              static_cast<int>(address->ai_addrlen));
            if (connectResult == 0 || WSAGetLastError() == WSAEWOULDBLOCK) {
                fd_set writeSet;
                FD_ZERO(&writeSet);
                FD_SET(socketHandle, &writeSet);
                timeval timeout{2, 0};
                if (select(0, nullptr, &writeSet, nullptr, &timeout) > 0) {
                    int socketError = 0;
                    int errorSize = sizeof(socketError);
                    getsockopt(socketHandle, SOL_SOCKET, SO_ERROR,
                               reinterpret_cast<char*>(&socketError), &errorSize);
                    if (socketError == 0)
                        break;
                }
            }
            closesocket(socketHandle);
            socketHandle = INVALID_SOCKET;
        }
        freeaddrinfo(addresses);
    }

    if (socketHandle == INVALID_SOCKET) {
        WSACleanup();
        callback(nullptr, userData);
        return;
    }

    u_long blocking = 0;
    ioctlsocket(socketHandle, FIONBIO, &blocking);
    const DWORD receiveTimeout = 3000;
    setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO,
               reinterpret_cast<const char*>(&receiveTimeout), sizeof(receiveTimeout));
    CDataStore message;
    std::array<unsigned char, 4096> data{};
    for (;;) {
        const int bytes = recv(socketHandle, reinterpret_cast<char*>(data.data()),
                               static_cast<int>(data.size()), 0);
        if (bytes <= 0)
            break;
        message.PutData(data.data(), static_cast<unsigned>(bytes));
    }
    closesocket(socketHandle);
    WSACleanup();
    if (message.m_size == 0) {
        callback(nullptr, userData);
        return;
    }
    message.Finalize();
    callback(&message, userData);
}
int __stdcall GetRealmsEventHandler(void*, const NETCONNADDR*, int note, char* user, unsigned char* data, unsigned bytes, unsigned* processed) {
    if (processed) *processed = bytes;
    if (note == 1 && user && data && bytes) {
        auto* message = reinterpret_cast<CDataStore*>(user);
        message->PutData(data, bytes);
    }
    return note >= 0 ? 1 : 0;
}
