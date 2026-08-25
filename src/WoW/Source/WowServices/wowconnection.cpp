#include "WoW/Source/WowServices/wowconnection.h"
#include "WoW/Source/Net/netinternal.h"

#include "ENGINE/Source/Os/W32/time.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>

#ifdef SetPort
#undef SetPort
#endif

float INFINITY_259 = 0.0F;
static bool s_destroyed = false;

int _E2_259() { return _E1_259(); }
int _E1_259() { INFINITY_259 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E18_6() { _E15_9(); return _E17_15(); }
void _E15_9() { if (++WowConnectionInitializer::count == 1) WowConnectionInitializer::Initialize(); }
int _E17_15() { return 0; }
void _E16_18() { if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0) WowConnectionInitializer::Destroy(); }

void WowConnection::RegisterSocket() {}
int WowConnection::CreateSocket() { return 1; }
void WowConnection::CloseSocket(int) {}

WowConnection::WowConnection(int sock, const NETCONNADDR* address, WowConnectionResponse* response)
    : m_sock(sock), m_response(response) {
    Init(response, nullptr);
    if (address) m_peer = *address;
    m_connState = WOWC_CONNECTED;
}

WowConnection::WowConnection(WowConnectionResponse* response, void (__fastcall *threadInit)()) {
    Init(response, threadInit);
}

WowConnection::~WowConnection() {
    for (SENDNODE* node : m_sendList) delete node;
    m_sendList.clear();
    m_readBuffer.clear();
    m_sock = -1;
}

void WowConnection::Init(WowConnectionResponse* response, void (__fastcall *threadInit)()) {
    m_refCount = 1;
    m_responseRef = 0;
    m_response = response;
    m_threadInit = threadInit;
    m_connectionFreed = false;
    m_needBytes = 0;
    m_haveSizeBytes = 0;
    m_readBytes = 0;
    m_readBufferSize = 0;
    m_connectRetryInterval = -1;
    m_connectAddress = 0;
    m_connectPort = 0;
    m_retryConnection = 0;
    m_stats = {};
    m_stats.connectTime = OsGetAsyncTimeMs();
    m_stats.lastDataReceived = m_stats.connectTime;
    m_sendDepth = 0;
    m_sendDepthBytes = 0;
    SetState(WOWC_INITIALIZED);
}

int WowConnection::AddRef() {
    return ++m_refCount;
}

int WowConnection::Release() {
    const int result = std::max(0, --m_refCount);
    if (result == 0) m_connectionFreed = true;
    return result;
}

void WowConnection::Disconnect() {
    std::lock_guard lock(m_lock);
    if (m_connState == WOWC_CONNECTED || m_connState == WOWC_CONNECTING)
        SetState(WOWC_DISCONNECTING);
}

void WowConnection::DoDisconnect() {
    std::lock_guard lock(m_lock);
    m_sock = -1;
    SetState(WOWC_DISCONNECTED);
    if (m_response) m_response->WCDisconnected(this, OsGetAsyncTimeMs(), &m_peer);
}

WowConnection::SENDNODE* WowConnection::NewSendNode(void* data, int size, unsigned char raw) {
    if (size < 0 || (size > 0 && data == nullptr)) return nullptr;
    auto* node = new SENDNODE;
    node->raw = raw != 0;
    node->data.resize(static_cast<std::size_t>(size));
    if (size) std::memcpy(node->data.data(), data, static_cast<std::size_t>(size));
    return node;
}

void WowConnection::FreeSendNode(SENDNODE* node) { delete node; }

int WowConnection::Send(CDataStore* message) {
    if (!message) return 0;
    return SendRaw(message->m_data, static_cast<int>(message->m_size));
}

int WowConnection::SendRaw(unsigned char* data, int length) {
    if (length < 0 || (length > 0 && !data)) return 0;
    std::lock_guard lock(m_outLock);
    auto* node = NewSendNode(data, length, 1);
    if (!node) return 0;
    m_sendDepthBytes += length;
    m_sendDepth = static_cast<int>(m_sendList.size() + 1);
    m_sendList.push_back(node);
    m_stats.bytesSent += static_cast<unsigned>(length);
    return 1;
}

void WowConnection::CheckConnect() { if (m_connState == WOWC_CONNECTING) StartConnect(); }
void WowConnection::CheckAccept() {}
void WowConnection::DoWrites(int) { m_sendDepth = 0; }
void WowConnection::DoMessageReads() {}
void WowConnection::DoStreamReads() {}
void WowConnection::DoReads(int flags) { if (flags) DoStreamReads(); }
void WowConnection::DoExceptions(int flags) { if (flags) DoDisconnect(); }
void WowConnection::StartConnect() {
    SetState(WOWC_CONNECTED);
    if (m_response) m_response->WCConnected(this, nullptr, OsGetAsyncTimeMs(), &m_peer);
}
unsigned char WowConnection::Reconnect() { StartConnect(); return 1; }
unsigned char WowConnection::Listen(unsigned port) { m_listenPort = static_cast<unsigned short>(port); SetState(WOWC_LISTENING); return 1; }
void WowConnection::StopListening() { if (m_connState == WOWC_LISTENING) SetState(WOWC_DISCONNECTED); }

char* WowConnection::GetStringAddress(char* buffer, unsigned size) {
    if (!buffer || size == 0) return buffer;
    std::snprintf(buffer, size, "%u:%u", m_peer.address, static_cast<unsigned>(m_peer.port));
    return buffer;
}

void WowConnection::DestroyOsNet() { s_destroyed = true; }
bool WowConnection::IsDestroyed() { return s_destroyed; }

void WowConnection::SetResponse(WowConnectionResponse* response) {
    std::lock_guard lock(m_responseLock);
    m_response = response;
}
void WowConnection::AcquireResponseRef() { std::lock_guard lock(m_responseLock); ++m_responseRef; }
void WowConnection::ReleaseResponseRef() { std::lock_guard lock(m_responseLock); if (m_responseRef > 0) --m_responseRef; }
void WowConnection::SetState(WOW_CONN_STATE state) { m_connState = state; }
void WowConnection::SetType(WOWC_TYPE type) { m_type = type; }
void WowConnection::RequestWriteNotification() { m_wantWriteNotification = true; }
bool WowConnection::GetLocal(NETADDR* address) const { if (!address) return false; *address = {}; return m_sock >= 0; }
unsigned WowConnection::GetAddr() const { return m_peer.address; }
unsigned short WowConnection::GetPort() const { return m_peer.port; }
void WowConnection::SetPort(int port, unsigned short value) { (void)port; (void)value; }

int _E5_256() { _E4_256(); return 0; }
void _E4_256() {}
int _E8_256() { _E7_256(); return 0; }
void _E7_256() {}
