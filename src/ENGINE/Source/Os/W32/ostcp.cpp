#include "ENGINE/Source/Os/W32/ostcp.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <new>
#include <process.h>
#include <string>

namespace {
sockaddr_in *SockAddr(NETADDR *address) {
  return reinterpret_cast<sockaddr_in *>(address->data);
}
const sockaddr_in *SockAddr(const NETADDR *address) {
  return reinterpret_cast<const sockaddr_in *>(address->data);
}
OsNet::NETCONN *Conn(HNETCONN connection) {
  return dynamic_cast<OsNet::NETCONN *>(connection);
}
void FillAddress(SOCKET socket, bool peer, NETADDR *output) {
  std::memset(output, 0, sizeof(*output));
  int length = sizeof(sockaddr_in);
  auto *address = SockAddr(output);
  if ((peer
           ? getpeername(socket, reinterpret_cast<sockaddr *>(address), &length)
           : getsockname(socket, reinterpret_cast<sockaddr *>(address),
                         &length)) == SOCKET_ERROR)
    std::memset(output, 0, sizeof(*output));
}
} // namespace

namespace OsNet {
TCPNET *TCPNET::s_pnet = nullptr;

LONG LOCKEDLONG::Increment() { return InterlockedIncrement(&m_value); }
LONG LOCKEDLONG::Decrement() { return InterlockedDecrement(&m_value); }

NETSELSOCK::NETSELSOCK(SOCKET socket) : m_sock(socket) {}
NETSELSOCK::~NETSELSOCK() { Close(); }
bool NETSELSOCK::IsClosed() const { return m_sock == INVALID_SOCKET; }
void NETSELSOCK::Close() {
  if (m_sock != INVALID_SOCKET) {
    closesocket(m_sock);
    m_sock = INVALID_SOCKET;
  }
}
void NETSELSOCK::AddToSelectSets(NETSELECTSETS *) {}
void NETSELSOCK::Selected(SELECTSET) {}
NETSELSOCKPTR::NETSELSOCKPTR() : ptr(nullptr) {}
NETSELSOCKPTR::NETSELSOCKPTR(NETSELSOCK *value) : ptr(value) {}

NETSELECTSETS::NETSELECTSETS(TCPNET *net) : m_net(net) { Clear(); }
void NETSELECTSETS::Clear() {
  for (auto &set : m_sets)
    FD_ZERO(&set);
  m_sockets.clear();
}
void NETSELECTSETS::AddSelSock(NETSELSOCK *socket) {
  if (socket && !socket->IsClosed() &&
      std::find(m_sockets.begin(), m_sockets.end(), socket) == m_sockets.end())
    m_sockets.push_back(socket);
}
void NETSELECTSETS::AddToSet(NETSELSOCK *socket, SELECTSET set) {
  if (!socket || socket->IsClosed() || set < SELECTSET_FIRST ||
      set >= SELECT_MAX)
    return;
  AddSelSock(socket);
  FD_SET(socket->m_sock, &m_sets[set]);
}
int NETSELECTSETS::Select(unsigned long timeoutMs, long) {
  timeval timeout{static_cast<long>(timeoutMs / 1000),
                  static_cast<long>((timeoutMs % 1000) * 1000)};
  const int selected = select(0, &m_sets[SELECTSET_R], &m_sets[SELECTSET_W],
                              &m_sets[SELECTSET_E], &timeout);
  if (selected <= 0)
    return selected;
  const auto sockets = m_sockets;
  for (NETSELSOCK *socket : sockets) {
    if (!socket || socket->IsClosed())
      continue;
    if (FD_ISSET(socket->m_sock, &m_sets[SELECTSET_E]))
      socket->Selected(SELECTSET_E);
    else if (FD_ISSET(socket->m_sock, &m_sets[SELECTSET_R]))
      socket->Selected(SELECTSET_R);
    if (!socket->IsClosed() && FD_ISSET(socket->m_sock, &m_sets[SELECTSET_W]))
      socket->Selected(SELECTSET_W);
  }
  return selected;
}

OUTPUT::~OUTPUT() {
  delete[] m_data;
  if (m_completionEvent)
    CloseHandle(m_completionEvent);
}
INPUT::~INPUT() { delete[] m_buffer; }

NETCONN::NETCONN(TCPNET *net, SOCKET socket, NETEVENTPROC eventProc, void *user,
                 const NETCONNADDR *address)
    : NETSELSOCK(socket), m_refCount(1), m_connAddr{}, m_eventProc(eventProc),
      m_user(user), m_time(GetTickCount()), m_net(net),
      m_disconnectNoted(false) {
  if (address)
    m_connAddr = *address;
  else {
    FillAddress(socket, true, &m_connAddr.peerAddr);
    FillAddress(socket, false, &m_connAddr.selfAddr);
  }
}
NETCONN::~NETCONN() = default;
void NETCONN::IncIo() {}
void NETCONN::DecIo() {}
void NETCONN::CompleteWrite(NETOVERLAP *, unsigned) {}
void NETCONN::CompleteRead(NETOVERLAP *, unsigned) {}
void NETCONN::Send(const void *, unsigned) {}
bool NETCONN::SendSync(const void *, unsigned, unsigned *bytesSent, unsigned) {
  if (bytesSent)
    *bytesSent = 0;
  return false;
}
void NETCONN::CloseAndUnlock() { Close(); }
void NETCONN::Disconnect(int notify) {
  Close();
  if (notify && !m_disconnectNoted.exchange(true))
    NoteDisconnect();
}
void NETCONN::GetEventProcAndUser(NETEVENTPROC &eventProc, void *&user) {
  std::scoped_lock guard(m_lock);
  eventProc = m_eventProc;
  user = m_user;
}
void NETCONN::SetEventProcAndUser(NETEVENTPROC eventProc, void *user) {
  std::scoped_lock guard(m_lock);
  m_eventProc = eventProc;
  m_user = user;
}
void NETCONN::SetEventProc(NETEVENTPROC eventProc) {
  std::scoped_lock guard(m_lock);
  m_eventProc = eventProc;
}
void NETCONN::SetUser(void *user) {
  std::scoped_lock guard(m_lock);
  m_user = user;
}
int NETCONN::NoteCantConnect() {
  NETEVENTPROC proc;
  void *user;
  GetEventProcAndUser(proc, user);
  unsigned long used = 0;
  return proc ? proc(nullptr, &m_connAddr, NETNOTE_CANTCONNECT, user, nullptr,
                     0, &used)
              : 0;
}
int NETCONN::NoteConnect() {
  NETEVENTPROC proc;
  void *user;
  GetEventProcAndUser(proc, user);
  unsigned long used = 0;
  return proc
             ? proc(this, &m_connAddr, NETNOTE_CONNECT, user, nullptr, 0, &used)
             : 0;
}
int NETCONN::NoteDisconnect() {
  NETEVENTPROC proc;
  void *user;
  GetEventProcAndUser(proc, user);
  unsigned long used = 0;
  return proc ? proc(this, &m_connAddr, NETNOTE_DISCONNECT, user, nullptr, 0,
                     &used)
              : 0;
}
int NETCONN::NoteData(void *data, unsigned bytes, unsigned *processed,
                      const NETCONNADDR *address) {
  NETEVENTPROC proc;
  void *user;
  GetEventProcAndUser(proc, user);
  unsigned long local = processed ? *processed : 0;
  const int result = proc ? proc(this, address ? address : &m_connAddr,
                                 NETNOTE_DATA, user, data, bytes, &local)
                          : 0;
  if (processed)
    *processed = local;
  return result;
}
int NETCONN::NoteFileOperation(void *data, unsigned bytes, unsigned offsetLow,
                               unsigned offsetHigh, void *operationId,
                               NETNOTE note) {
  NETCONNADDR address{};
  address.peerAddr.file.pos =
      (static_cast<std::uint64_t>(offsetHigh) << 32) | offsetLow;
  address.peerAddr.file.operationId = operationId;
  NETEVENTPROC proc;
  void *user;
  GetEventProcAndUser(proc, user);
  unsigned long used = bytes;
  return proc ? proc(this, &address, note, user, data, bytes, &used) : 0;
}
void NETCONN::IncRef() { ++m_refCount; }
void NETCONN::DecRef() {
  if (--m_refCount == 0)
    delete this;
}
void NETCONN::ConnAddr(NETCONNADDR *address) {
  if (address)
    *address = m_connAddr;
}
void NETCONN::Close() { NETSELSOCK::Close(); }

void NETCONNFULL::SetNagle(int) {}
int NETCONNFULL::SetWindow(int) { return 0; }
void NETCONNFULL::SetRecvTimeout(unsigned) {}

LOOPCONN::INPUT::~INPUT() = default;
LOOPCONN::LOOPCONN(TCPNET *net, NETEVENTPROC proc, void *user,
                   const NETCONNADDR *address)
    : NETCONNFULL(net, INVALID_SOCKET, proc, user, address) {}
LOOPCONN::~LOOPCONN() {
  if (m_loopConn && m_loopConn->m_loopConn == this)
    m_loopConn->m_loopConn = nullptr;
}
void LOOPCONN::Connect() { NoteConnect(); }
void LOOPCONN::CompleteInput(INPUT *input) {
  if (input) {
    unsigned used = 0;
    NoteData(input->data.data(), static_cast<unsigned>(input->data.size()),
             &used);
    delete input;
  }
}
void LOOPCONN::EnqueueInput(const void *data, unsigned bytes) {
  auto *input = new INPUT;
  const auto *begin = static_cast<const unsigned char *>(data);
  input->data.assign(begin, begin + bytes);
  CompleteInput(input);
}
void LOOPCONN::Send(const void *data, unsigned bytes) {
  if (m_loopConn)
    m_loopConn->EnqueueInput(data, bytes);
}
bool LOOPCONN::SendSync(const void *data, unsigned bytes, unsigned *sent,
                        unsigned) {
  Send(data, bytes);
  if (sent)
    *sent = bytes;
  return m_loopConn != nullptr;
}
void LOOPCONN::CloseAndUnlock() { Close(); }
void LOOPCONN::Close() {
  if (m_loopConn) {
    m_loopConn->m_loopConn = nullptr;
    m_loopConn = nullptr;
  }
}
bool LOOPCONN::IsClosed() const { return m_loopConn == nullptr; }
void LOOPCONN::AddToSelectSets(NETSELECTSETS *) {}
void LOOPCONN::Selected(SELECTSET) {}

TCPCONN::TCPCONN(TCPNET *net, SOCKET socket, NETEVENTPROC proc, void *user,
                 const NETCONNADDR *address)
    : NETCONNFULL(net, socket, proc, user, address) {}
TCPCONN::~TCPCONN() = default;
void TCPCONN::CompleteWrite(NETOVERLAP *, unsigned bytes) {
  std::scoped_lock guard(m_lock);
  if (m_outputQueue.empty())
    return;
  auto &output = *m_outputQueue.front();
  output.m_bytes += bytes;
  if (output.m_bytes >= output.m_dataBytes) {
    output.m_state = OUTPUTSTATE_COMPLETED;
    if (output.m_completionEvent)
      SetEvent(output.m_completionEvent);
    m_outputQueue.pop_front();
  }
}
void TCPCONN::CompleteRead(NETOVERLAP *, unsigned bytes) {
  if (!bytes)
    Disconnect(1);
}
OUTPUT *TCPCONN::LockedEnqueue(const void *data, unsigned bytes) {
  if (!data || !bytes)
    return nullptr;
  auto *output = new OUTPUT;
  output->m_data = new unsigned char[bytes];
  std::memcpy(output->m_data, data, bytes);
  output->m_dataBytes = bytes;
  m_outputQueue.emplace_back(output);
  return output;
}
void TCPCONN::Send(const void *data, unsigned bytes) {
  if (!data || !bytes || IsClosed())
    return;
  {
    std::scoped_lock guard(m_lock);
    LockedEnqueue(data, bytes);
  }
  ContinueQueuedWrite();
}
bool TCPCONN::SendSync(const void *data, unsigned bytes, unsigned *bytesSent,
                       unsigned timeout) {
  if (bytesSent)
    *bytesSent = 0;
  if (IsClosed() || (!data && bytes))
    return false;
  const DWORD started = GetTickCount();
  unsigned total = 0;
  while (total < bytes) {
    int result =
        send(m_sock, static_cast<const char *>(data) + total, bytes - total, 0);
    if (result > 0) {
      total += result;
      continue;
    }
    const int error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK && error != WSAEINTR)
      break;
    const DWORD elapsed = GetTickCount() - started;
    if (timeout != INFINITE && elapsed >= timeout)
      break;
    fd_set writable;
    FD_ZERO(&writable);
    FD_SET(m_sock, &writable);
    timeval wait{};
    const DWORD remaining = timeout == INFINITE ? 100U : std::min<DWORD>(100U, timeout - elapsed);
    wait.tv_sec = remaining / 1000U;
    wait.tv_usec = (remaining % 1000U) * 1000U;
    if (select(0, nullptr, &writable, nullptr, &wait) == SOCKET_ERROR)
      break;
  }
  if (bytesSent)
    *bytesSent = total;
  return total == bytes;
}
void TCPCONN::CloseAndUnlock() { Close(); }
void TCPCONN::SetNagle(int enable) {
  BOOL noDelay = !enable;
  setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY,
             reinterpret_cast<const char *>(&noDelay), sizeof(noDelay));
}
int TCPCONN::SetWindow(int size) {
  int a = setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF,
                     reinterpret_cast<const char *>(&size), sizeof(size));
  int b = setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF,
                     reinterpret_cast<const char *>(&size), sizeof(size));
  return a == 0 && b == 0;
}
void TCPCONN::SetRecvTimeout(unsigned timeout) {
  DWORD value = timeout;
  setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO,
             reinterpret_cast<const char *>(&value), sizeof(value));
}
void TCPCONN::AddToSelectSets(NETSELECTSETS *sets) {
  sets->AddToSet(this, SELECTSET_R);
  sets->AddToSet(this, SELECTSET_E);
  std::scoped_lock guard(m_lock);
  if (!m_outputQueue.empty())
    sets->AddToSet(this, SELECTSET_W);
}
void TCPCONN::Selected(SELECTSET set) {
  if (set == SELECTSET_E) {
    Disconnect(1);
    return;
  }
  if (set == SELECTSET_W) {
    ContinueQueuedWrite();
    return;
  }
  unsigned char data[8192];
  int got = recv(m_sock, reinterpret_cast<char *>(data), sizeof(data), 0);
  if (got == 0) {
    Disconnect(1);
    return;
  }
  if (got < 0) {
    const int error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK && error != WSAEINTR)
      Disconnect(1);
    return;
  }
  m_pendingInput.insert(m_pendingInput.end(), data, data + got);
  unsigned used = 0;
  NoteData(m_pendingInput.data(), static_cast<unsigned>(m_pendingInput.size()), &used);
  used = std::min<unsigned>(used, static_cast<unsigned>(m_pendingInput.size()));
  m_pendingInput.erase(m_pendingInput.begin(), m_pendingInput.begin() + used);
}
void TCPCONN::ContinueQueuedWrite() {
  std::unique_lock guard(m_lock);
  bool failed = false;
  while (!m_outputQueue.empty() && !IsClosed()) {
    auto &output = *m_outputQueue.front();
    output.m_state = OUTPUTSTATE_WRITING;
    const int result = send(m_sock,
                            reinterpret_cast<const char *>(output.m_data) + output.m_bytes,
                            output.m_dataBytes - output.m_bytes, 0);
    if (result > 0) {
      output.m_bytes += static_cast<unsigned>(result);
      if (output.m_bytes == output.m_dataBytes) {
        output.m_state = OUTPUTSTATE_COMPLETED;
        if (output.m_completionEvent)
          SetEvent(output.m_completionEvent);
        m_outputQueue.pop_front();
        continue;
      }
    } else {
      const int error = WSAGetLastError();
      if (error != WSAEWOULDBLOCK && error != WSAEINTR)
        failed = true;
    }
    break;
  }
  guard.unlock();
  if (failed)
    Disconnect(1);
}
void TCPCONN::StartWriteAndLeaveLock() { ContinueQueuedWrite(); }
void TCPCONN::StartRead() {}
IOTCPCONN::IOTCPCONN(TCPNET *n, SOCKET s, NETEVENTPROC e, void *u,
                     const NETCONNADDR *a)
    : TCPCONN(n, s, e, u, a) {}
void IOTCPCONN::IncIo() { ++m_ioCount; IncRef(); }
void IOTCPCONN::DecIo() { if (m_ioCount > 0) { --m_ioCount; DecRef(); } }
void IOTCPCONN::AddToSelectSets(NETSELECTSETS *s) {
  TCPCONN::AddToSelectSets(s);
}
void IOTCPCONN::Selected(SELECTSET s) { TCPCONN::Selected(s); }
void IOTCPCONN::StartWriteAndLeaveLock() { TCPCONN::StartWriteAndLeaveLock(); }
void IOTCPCONN::StartRead() { TCPCONN::StartRead(); }
SLTCPCONN::SLTCPCONN(TCPNET *n, SOCKET s, NETEVENTPROC e, void *u,
                     const NETCONNADDR *a)
    : TCPCONN(n, s, e, u, a) {}
void SLTCPCONN::ContinueWrite() { ContinueQueuedWrite(); }
void SLTCPCONN::ContinueRead() { Selected(SELECTSET_R); }
void SLTCPCONN::AddToSelectSets(NETSELECTSETS *sets) {
  TCPCONN::AddToSelectSets(sets);
}
void SLTCPCONN::Selected(SELECTSET set) { TCPCONN::Selected(set); }
void SLTCPCONN::StartWriteAndLeaveLock() { ContinueQueuedWrite(); }
void SLTCPCONN::StartRead() {}

UDPCONN::UDPCONN(TCPNET *net, SOCKET socket, NETEVENTPROC proc, void *user,
                 const NETCONNADDR *address)
    : NETCONNLESS(net, socket, proc, user, address) {
  NoteConnect();
}
UDPCONN::~UDPCONN() = default;
void UDPCONN::SendTo(const void *data, unsigned bytes, unsigned count,
                     const NETADDR *addresses) {
  for (unsigned i = 0; i < count; ++i)
    sendto(m_sock, static_cast<const char *>(data), bytes, 0,
           reinterpret_cast<const sockaddr *>(SockAddr(&addresses[i])),
           sizeof(sockaddr_in));
}
void UDPCONN::CloseAndUnlock() { Close(); }
void UDPCONN::AddToSelectSets(NETSELECTSETS *sets) {
  sets->AddToSet(this, SELECTSET_R);
  sets->AddToSet(this, SELECTSET_E);
}
void UDPCONN::Selected(SELECTSET set) {
  if (set == SELECTSET_E) {
    Disconnect(1);
    return;
  }
  char data[65536];
  sockaddr_in from{};
  int len = sizeof(from);
  int got = recvfrom(m_sock, data, sizeof(data), 0,
                     reinterpret_cast<sockaddr *>(&from), &len);
  if (got > 0) {
    NETCONNADDR addr{};
    std::memcpy(addr.peerAddr.data, &from, sizeof(from));
    FillAddress(m_sock, false, &addr.selfAddr);
    unsigned used = 0;
    NoteData(data, got, &used, &addr);
  }
}

FILECONN::FILECONN(TCPNET *net, HANDLE file, NETEVENTPROC proc, void *user,
                   const NETCONNADDR *address)
    : NETCONN(net, INVALID_SOCKET, proc, user, address), m_file(file) {}
FILECONN::~FILECONN() { CloseAndUnlock(); }
bool FILECONN::IsClosed() const {
  return m_file == INVALID_HANDLE_VALUE || m_file == nullptr;
}
void FILECONN::IncIo() {}
void FILECONN::DecIo() {}
void FILECONN::CompleteWrite(NETOVERLAP *, unsigned) {}
void FILECONN::CompleteRead(NETOVERLAP *, unsigned) {}
OUTPUT *FILECONN::LockedEnqueue(std::uint64_t pos, const void *data,
                                unsigned bytes, void *operationId) {
  auto *out = new OUTPUT;
  out->m_pos = pos;
  out->m_data = new unsigned char[bytes];
  std::memcpy(out->m_data, data, bytes);
  out->m_dataBytes = bytes;
  out->m_overlap.m_overlapped.hEvent = operationId;
  return out;
}
int FILECONN::Write(std::uint64_t pos, const void *data, unsigned bytes,
                    void *operationId) {
  if (IsClosed())
    return 0;
  LARGE_INTEGER at{};
  at.QuadPart = pos;
  if (!SetFilePointerEx(m_file, at, nullptr, FILE_BEGIN))
    return 0;
  DWORD written = 0;
  BOOL ok = WriteFile(m_file, data, bytes, &written, nullptr);
  NoteFileOperation(
      const_cast<void *>(data), written, static_cast<unsigned>(pos),
      static_cast<unsigned>(pos >> 32), operationId, NETNOTE_FILEWRITE);
  return ok != FALSE;
}
int FILECONN::Read(std::uint64_t pos, void *buffer, unsigned bytes,
                   void *operationId) {
  if (IsClosed())
    return 0;
  LARGE_INTEGER at{};
  at.QuadPart = pos;
  if (!SetFilePointerEx(m_file, at, nullptr, FILE_BEGIN))
    return 0;
  DWORD read = 0;
  BOOL ok = ReadFile(m_file, buffer, bytes, &read, nullptr);
  NoteFileOperation(buffer, read, static_cast<unsigned>(pos),
                    static_cast<unsigned>(pos >> 32), operationId,
                    NETNOTE_FILEREAD);
  return ok != FALSE;
}
void FILECONN::CloseAndUnlock() {
  if (!IsClosed()) {
    CloseHandle(m_file);
    m_file = INVALID_HANDLE_VALUE;
  }
}
void FILECONN::StartWriteAndLeaveLock() {}
void FILECONN::StartRead() {}
void FILECONN::AddToSelectSets(NETSELECTSETS *) {}
void FILECONN::Selected(SELECTSET) {}
IOFILECONN::IOFILECONN(TCPNET *n, HANDLE f, NETEVENTPROC e, void *u,
                       const NETCONNADDR *a)
    : FILECONN(n, f, e, u, a) {}
void IOFILECONN::CloseAndUnlock() { FILECONN::CloseAndUnlock(); }
void IOFILECONN::StartWriteAndLeaveLock() {}
void IOFILECONN::StartRead() {}
SLFILECONN::SLFILECONN(TCPNET *n, HANDLE f, NETEVENTPROC e, void *u,
                       const NETCONNADDR *a)
    : FILECONN(n, f, e, u, a) {}
SLFILECONN::~SLFILECONN() = default;
unsigned __stdcall SLFILECONN::Thread(void *) { return 0; }
void SLFILECONN::CloseAndUnlock() { FILECONN::CloseAndUnlock(); }
void SLFILECONN::StartWriteAndLeaveLock() {}
void SLFILECONN::StartRead() {}

NETCONNECT::NETCONNECT(void *user, const void *data, unsigned bytes)
    : NETSELSOCK(), m_user(user) {
  if (data && bytes)
    m_data.assign(static_cast<const unsigned char *>(data),
                  static_cast<const unsigned char *>(data) + bytes);
}
NETCONNECT::~NETCONNECT() = default;
void NETCONNECT::NoteCantConnect(NETEVENTPROC proc,
                                 const NETCONNADDR *address) {
  unsigned long used = 0;
  if (proc)
    proc(nullptr, address, NETNOTE_CANTCONNECT, m_user, nullptr, 0, &used);
}
void NETCONNECT::Fail() {}
void NETCONNECT::Complete() {}
LOOPCONNECT::LOOPCONNECT(NETEVENTPROC src, NETEVENTPROC dst, void *user,
                         const void *data, unsigned bytes)
    : NETCONNECT(user, data, bytes), m_eventProcSrc(src), m_eventProcDst(dst) {}
void LOOPCONNECT::AddToSelectSets(NETSELECTSETS *) {}
void LOOPCONNECT::Selected(SELECTSET) {}
void LOOPCONNECT::Fail() {
  NETCONNADDR a{};
  NoteCantConnect(m_eventProcSrc, &a);
}
void LOOPCONNECT::Complete() {}
bool LOOPCONNECT::IsClosed() const { return false; }
TCPCONNECT::TCPCONNECT(unsigned node, unsigned short port, NETEVENTPROC proc,
                       void *user, const void *data, unsigned bytes)
    : NETCONNECT(user, data, bytes), m_nodeNumber(node), m_portAddr(port),
      m_eventProc(proc) {}
void TCPCONNECT::AddToSelectSets(NETSELECTSETS *sets) {
  sets->AddToSet(this, SELECTSET_W);
  sets->AddToSet(this, SELECTSET_E);
}
void TCPCONNECT::Selected(SELECTSET set) {
  set == SELECTSET_W ? Complete() : Fail();
}
void TCPCONNECT::Fail() {
  NETCONNADDR a{};
  TCPNET::MakeConnAddr(m_sock, m_nodeNumber, &a);
  NoteCantConnect(m_eventProc, &a);
}
void TCPCONNECT::Complete() {}
FILECONNECT::FILECONNECT(HANDLE file, NETEVENTPROC proc, void *user)
    : NETCONNECT(user, nullptr, 0), m_file(file), m_eventProc(proc) {}
void FILECONNECT::AddToSelectSets(NETSELECTSETS *) {}
void FILECONNECT::Selected(SELECTSET) {}
void FILECONNECT::Fail() {
  NETCONNADDR a{};
  NoteCantConnect(m_eventProc, &a);
}
void FILECONNECT::Complete() {}
bool FILECONNECT::IsClosed() const { return m_file == INVALID_HANDLE_VALUE; }

TCPLISTEN::TCPLISTEN(SOCKET socket, unsigned short port, NETEVENTPROC proc,
                     void *user, unsigned)
    : NETSELSOCK(socket), m_portAddr(port), m_eventProc(proc), m_user(user),
      m_enabled(true) {}
TCPLISTEN::~TCPLISTEN() = default;
int TCPLISTEN::Enable(int enable) {
  m_enabled = enable != 0;
  return 1;
}
void TCPLISTEN::Close() {
  m_enabled = false;
  NETSELSOCK::Close();
}
void TCPLISTEN::AddToSelectSets(NETSELECTSETS *sets) {
  if (m_enabled)
    sets->AddToSet(this, SELECTSET_R);
}
void TCPLISTEN::Selected(SELECTSET set) {
  if (set != SELECTSET_R)
    return;
  sockaddr_in peer{};
  int length = sizeof(peer);
  SOCKET accepted =
      accept(m_sock, reinterpret_cast<sockaddr *>(&peer), &length);
  if (accepted == INVALID_SOCKET)
    return;
  NETCONNADDR address{};
  std::memcpy(address.peerAddr.data, &peer, sizeof(peer));
  FillAddress(accepted, false, &address.selfAddr);
  auto *conn =
      new SLTCPCONN(TCPNET::s_pnet, accepted, m_eventProc, m_user, &address);
  if (TCPNET::s_pnet)
    TCPNET::s_pnet->LinkConn(conn, CONNLIST_TCP_CONNECTED);
  conn->NoteConnect();
}
TCPACCEPT::TCPACCEPT(TCPLISTEN *listen)
    : m_listen(listen), m_sock(INVALID_SOCKET) {}
TCPACCEPT::~TCPACCEPT() {
  if (m_sock != INVALID_SOCKET)
    closesocket(m_sock);
}
void TCPACCEPT::Init() {}
SOCKET TCPACCEPT::Complete(NETCONNADDR *address, int makeSocket) {
  if (makeSocket && m_listen) {
    sockaddr_in peer{};
    int len = sizeof(peer);
    m_sock =
        accept(m_listen->m_sock, reinterpret_cast<sockaddr *>(&peer), &len);
    if (address && m_sock != INVALID_SOCKET) {
      std::memcpy(address->peerAddr.data, &peer, sizeof(peer));
      FillAddress(m_sock, false, &address->selfAddr);
    }
  }
  SOCKET result = m_sock;
  m_sock = INVALID_SOCKET;
  return result;
}
TCPHOSTADDRINFO::TCPHOSTADDRINFO() : m_hostAddrProc(nullptr), m_user(nullptr) {}
TCPHOSTADDRINFO::~TCPHOSTADDRINFO() = default;
void TCPHOSTADDRINFO::Complete() {
  if (m_hostAddrProc)
    m_hostAddrProc(m_addrs.data(), static_cast<unsigned long>(m_addrs.size()),
                   m_user);
}

TCPNET::TCPNET() : m_refCount(1), m_winsockStarted(false) {
  m_wakeEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
}
TCPNET::~TCPNET() {
  PumpThreadsDestroy();
  TcpDestroy();
  WinsockDestroy();
  if (m_wakeEvent)
    CloseHandle(m_wakeEvent);
}
void TCPNET::IncludeDependantParts(unsigned *parts) {
  if (!parts)
    return;
  *parts |= 0x1U;
  if ((*parts & 0x2U) != 0)
    *parts |= 0x8U;
  if ((*parts & 0x6U) != 0)
    *parts |= 0x10U;
}
int TCPNET::Initialize(unsigned parts, unsigned pumpThreads) {
  if (s_pnet)
    return 1;
  auto *net = new (std::nothrow) TCPNET;
  if (!net)
    return 0;
  IncludeDependantParts(&parts);
  net->m_parts = parts;
  net->m_pumpThreadCount = pumpThreads;
  if (!net->BaseInitialize(parts) || !net->WinsockInitialize(parts) ||
      !net->IoInitialize(parts) || !net->TcpInitialize(parts)) {
    delete net;
    return 0;
  }
  s_pnet = net;
  net->m_refCount = 1;
  if (!net->PumpThreadsInitialize()) {
    s_pnet = nullptr;
    delete net;
    return 0;
  }
  return 1;
}
void TCPNET::Destroy(unsigned) {
  TCPNET *net = s_pnet;
  s_pnet = nullptr;
  if (net) {
    net->PumpThreadsDestroy();
    net->DecRef();
  }
}
bool TCPNET::IoCompletionPresent(unsigned *error) {
  HANDLE port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
  if (!port) {
    if (error)
      *error = GetLastError();
    return false;
  }
  SYSTEM_INFO info{};
  GetSystemInfo(&info);
  if (error)
    *error = 2U * static_cast<unsigned>(info.dwNumberOfProcessors ? info.dwNumberOfProcessors : 1U);
  CloseHandle(port);
  return true;
}
SOCKET TCPNET::CreateListenSocket(unsigned short port) {
  SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s == INVALID_SOCKET)
    return s;
  BOOL reuse = TRUE;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
             reinterpret_cast<const char *>(&reuse), sizeof(reuse));
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(s, reinterpret_cast<sockaddr *>(&address), sizeof(address)) ||
      listen(s, SOMAXCONN)) {
    closesocket(s);
    return INVALID_SOCKET;
  }
  return s;
}
void TCPNET::MakeConnAddr(SOCKET socket, unsigned node, NETCONNADDR *address) {
  if (!address)
    return;
  std::memset(address, 0, sizeof(*address));
  if (socket != INVALID_SOCKET) {
    FillAddress(socket, true, &address->peerAddr);
    FillAddress(socket, false, &address->selfAddr);
  } else
    OsNetAddrMake(node, 0, &address->peerAddr);
}
void TCPNET::Pump(unsigned timeout) {
  NETSELECTSETS sets(this);
  {
    std::scoped_lock guard(m_mutex);
    for (auto *listen : m_listeners)
      listen->AddToSelectSets(&sets);
    for (auto *conn : m_connections)
      if (conn && !conn->IsClosed())
        conn->AddToSelectSets(&sets);
  }
  sets.Select(timeout);
}
void TCPNET::IoPump(unsigned timeout) { Pump(timeout); }
unsigned __stdcall TCPNET::IoPumpThread(void *p) {
  auto *net = static_cast<TCPNET *>(p);
  while (!net->m_stopPump.load()) {
    net->Pump(50);
    if (net->m_wakeEvent && WaitForSingleObject(net->m_wakeEvent, 0) == WAIT_OBJECT_0)
      ResetEvent(net->m_wakeEvent);
  }
  return 0;
}
unsigned __stdcall TCPNET::SlPumpThread(void *p) { return IoPumpThread(p); }
unsigned __stdcall TCPNET::ListenThread(void *p) { return IoPumpThread(p); }
unsigned __stdcall TCPNET::UdpPumpThread(void *p) { return IoPumpThread(p); }
unsigned __stdcall TCPNET::BaseThread(void *p) { return IoPumpThread(p); }
unsigned __stdcall TCPNET::GetHostAddrsThread(void *) { return 0; }
int TCPNET::BaseInitialize(unsigned) { return 1; }
int TCPNET::WinsockInitialize(unsigned) {
  WSADATA data{};
  m_winsockStarted = WSAStartup(MAKEWORD(2, 2), &data) == 0;
  return m_winsockStarted;
}
int TCPNET::IoInitialize(unsigned) { return 1; }
int TCPNET::TcpInitialize(unsigned) { return 1; }
void TCPNET::BaseDestroy() {}
void TCPNET::WinsockDestroy() {
  if (m_winsockStarted) {
    WSACleanup();
    m_winsockStarted = false;
  }
}
void TCPNET::IoDestroy() {}
void TCPNET::TcpDestroy() {
  std::scoped_lock guard(m_mutex);
  for (auto *listener : m_listeners)
    delete listener;
  m_listeners.clear();
  for (auto *conn : m_connections)
    if (conn) {
      conn->Close();
      conn->DecRef();
    }
  m_connections.clear();
}
int TCPNET::PumpThreadsInitialize() {
  m_stopPump = false;
  for (unsigned i = 0; i < m_pumpThreadCount; ++i) {
    uintptr_t thread = _beginthreadex(nullptr, 0, IoPumpThread, this, 0, nullptr);
    if (!thread) {
      PumpThreadsDestroy();
      return 0;
    }
    m_pumpThreads.push_back(reinterpret_cast<HANDLE>(thread));
  }
  return 1;
}
void TCPNET::PumpThreadsDestroy() {
  m_stopPump = true;
  WakePumpThread();
  if (!m_pumpThreads.empty()) {
    WaitForMultipleObjects(static_cast<DWORD>(m_pumpThreads.size()), m_pumpThreads.data(), TRUE, 5000);
    for (HANDLE thread : m_pumpThreads)
      CloseHandle(thread);
    m_pumpThreads.clear();
  }
}
void TCPNET::WakePumpThread() {
  if (m_wakeEvent)
    SetEvent(m_wakeEvent);
}
void TCPNET::IncRef() { ++m_refCount; }
void TCPNET::DecRef() {
  if (--m_refCount == 0)
    delete this;
}
void TCPNET::LogWrite(const char *format, ...) {
  char text[1024];
  va_list args;
  va_start(args, format);
  vsnprintf_s(text, sizeof(text), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(text);
  OutputDebugStringA("\n");
}
void TCPNET::LogDump() {}
int TCPNET::TcpListen(unsigned short port, NETEVENTPROC proc, void *user) {
  if (!proc)
    return 0;
  SOCKET socket = CreateListenSocket(port);
  if (socket == INVALID_SOCKET)
    return 0;
  u_long nonblocking = 1;
  ioctlsocket(socket, FIONBIO, &nonblocking);
  auto *listener = new TCPLISTEN(socket, port, proc, user, 0);
  std::scoped_lock guard(m_mutex);
  m_listeners.push_back(listener);
  return 1;
}
void TCPNET::TcpListenEnable(unsigned short port, int enable) {
  std::scoped_lock guard(m_mutex);
  for (auto *listener : m_listeners)
    if (listener->m_portAddr == port)
      listener->Enable(enable);
}
void TCPNET::TcpConnect(unsigned node, unsigned short port, NETEVENTPROC proc,
                        void *user, const void *data, unsigned bytes) {
  NETCONNADDR addr{};
  OsNetAddrMake(node, port, &addr.peerAddr);
  SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s == INVALID_SOCKET) {
    unsigned long used = 0;
    if (proc)
      proc(nullptr, &addr, NETNOTE_CANTCONNECT, user, nullptr, 0, &used);
    return;
  }
  if (connect(s, reinterpret_cast<const sockaddr *>(SockAddr(&addr.peerAddr)),
              sizeof(sockaddr_in)) == SOCKET_ERROR) {
    closesocket(s);
    unsigned long used = 0;
    if (proc)
      proc(nullptr, &addr, NETNOTE_CANTCONNECT, user, nullptr, 0, &used);
    return;
  }
  FillAddress(s, false, &addr.selfAddr);
  u_long nonblocking = 1;
  ioctlsocket(s, FIONBIO, &nonblocking);
  auto *conn = new SLTCPCONN(this, s, proc, user, &addr);
  LinkConn(conn, CONNLIST_TCP_CONNECTED);
  if (data && bytes)
    conn->Send(data, bytes);
  conn->NoteConnect();
}
void TCPNET::UdpConnect(const NETADDR *bindAddress, unsigned short portMin,
                        unsigned short portMax, NETEVENTPROC proc, void *user) {
  SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  NETCONNADDR addr{};
  bool bound = false;
  for (unsigned port = portMin; port <= portMax && !bound; ++port) {
    sockaddr_in local = bindAddress ? *SockAddr(bindAddress) : sockaddr_in{};
    local.sin_family = AF_INET;
    local.sin_port = htons(static_cast<unsigned short>(port));
    if (!bindAddress)
      local.sin_addr.s_addr = htonl(INADDR_ANY);
    bound = bind(s, reinterpret_cast<sockaddr *>(&local), sizeof(local)) == 0;
  }
  if (!bound) {
    closesocket(s);
    unsigned long used = 0;
    if (proc)
      proc(nullptr, &addr, NETNOTE_CANTCONNECT, user, nullptr, 0, &used);
    return;
  }
  FillAddress(s, false, &addr.selfAddr);
  u_long mode = 1;
  ioctlsocket(s, FIONBIO, &mode);
  auto *conn = new UDPCONN(this, s, proc, user, &addr);
  LinkConn(conn, CONNLIST_UDP_CONNECTED);
}
void TCPNET::LoopConnect(NETEVENTPROC src, NETEVENTPROC dst, void *user,
                         const void *data, unsigned bytes) {
  NETCONNADDR a{};
  auto *left = new LOOPCONN(this, src, user, &a);
  auto *right = new LOOPCONN(this, dst, user, &a);
  left->m_loopConn = right;
  right->m_loopConn = left;
  LinkConn(left, CONNLIST_LOOP_CONNECTED);
  LinkConn(right, CONNLIST_LOOP_CONNECTED);
  left->Connect();
  right->Connect();
  if (data && bytes)
    left->Send(data, bytes);
}
void TCPNET::FileConnCreate(const char *fileName, NETEVENTPROC proc, void *user,
                            int readOnly) {
  DWORD access = GENERIC_READ | (readOnly ? 0 : GENERIC_WRITE);
  HANDLE file = CreateFileA(fileName, access, FILE_SHARE_READ, nullptr,
                            readOnly ? OPEN_EXISTING : OPEN_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, nullptr);
  NETCONNADDR a{};
  if (file == INVALID_HANDLE_VALUE) {
    unsigned long used = 0;
    if (proc)
      proc(nullptr, &a, NETNOTE_CANTCONNECT, user, nullptr, 0, &used);
    return;
  }
  auto *conn = new SLFILECONN(this, file, proc, user, &a);
  LinkConn(conn, CONNLIST_FILE_CONNECTED);
  conn->NoteConnect();
}
int TCPNET::GetHostAddrs(const char *names, unsigned short defaultPort,
                         HOSTADDRPROC proc, void *user) {
  if (!proc)
    return 0;
  std::vector<NETADDR> addresses;
  std::string list = names && *names ? names : "";
  if (list.empty()) {
    char local[256]{};
    gethostname(local, sizeof(local));
    list = local;
  }
  size_t begin = 0;
  while (begin <= list.size()) {
    size_t end = list.find_first_of(",; ", begin);
    std::string host = list.substr(begin, end - begin);
    if (!host.empty()) {
      addrinfo hints{};
      hints.ai_family = AF_INET;
      addrinfo *result = nullptr;
      if (getaddrinfo(host.c_str(), nullptr, &hints, &result) == 0) {
        for (auto *item = result; item; item = item->ai_next) {
          NETADDR address{};
          std::memcpy(address.data, item->ai_addr,
                      (std::min)(sizeof(address.data),
                                 static_cast<size_t>(item->ai_addrlen)));
          SockAddr(&address)->sin_port = htons(defaultPort);
          addresses.push_back(address);
        }
        freeaddrinfo(result);
      }
    }
    if (end == std::string::npos)
      break;
    begin = end + 1;
  }
  proc(addresses.data(), static_cast<unsigned long>(addresses.size()), user);
  return !addresses.empty();
}
void TCPNET::LinkConn(NETCONN *conn, CONNLIST) {
  if (!conn)
    return;
  conn->IncRef();
  std::scoped_lock guard(m_mutex);
  m_connections.push_back(conn);
}
LOOPCONN::INPUT *TCPNET::LoopAllocInput(unsigned bytes) {
  auto *input = new LOOPCONN::INPUT;
  input->data.reserve(bytes);
  return input;
}
void TCPNET::LoopFreeInput(LOOPCONN::INPUT *input) { delete input; }
void TCPNET::CompleteAcceptEx(TCPACCEPT *accept, int ok) {
  if (!accept)
    return;
  NETCONNADDR address{};
  SOCKET socket = accept->Complete(&address, ok);
  if (socket != INVALID_SOCKET && accept->m_listen)
    CompleteAccept(accept->m_listen, socket, &address);
}
void TCPNET::CompleteAccept(TCPLISTEN *listen, SOCKET socket,
                            const NETCONNADDR *address) {
  if (!listen || socket == INVALID_SOCKET)
    return;
  auto *conn =
      new SLTCPCONN(this, socket, listen->m_eventProc, listen->m_user, address);
  LinkConn(conn, CONNLIST_TCP_CONNECTED);
  conn->NoteConnect();
}
void TCPNET::LoopCompleteConnect(LOOPCONNECT *connect) {
  if (connect)
    connect->Complete();
}
void TCPNET::TcpCompleteConnect(TCPCONNECT *connect) {
  if (connect)
    connect->Complete();
}
void TCPNET::FileCompleteConnect(FILECONNECT *connect) {
  if (connect)
    connect->Complete();
}
void TCPNET::LoopConnectInit(LOOPCONNECT *connect) {
  LoopCompleteConnect(connect);
}
void TCPNET::TcpConnectInit(TCPCONNECT *connect) {
  if (connect)
    connect->Selected(SELECTSET_W);
}
void TCPNET::FileConnectInit(FILECONNECT *connect) {
  FileCompleteConnect(connect);
}
void TCPNET::LoopMakeConn(NETEVENTPROC src, NETEVENTPROC dst, void *user,
                          const void *data, unsigned bytes) {
  LoopConnect(src, dst, user, data, bytes);
}
void TCPNET::TcpMakeConn(SOCKET socket, NETEVENTPROC proc, void *user,
                         const NETCONNADDR *address, const void *data,
                         unsigned bytes) {
  if (socket == INVALID_SOCKET)
    return;
  auto *conn = new SLTCPCONN(this, socket, proc, user, address);
  LinkConn(conn, CONNLIST_TCP_CONNECTED);
  if (data && bytes)
    conn->Send(data, bytes);
  conn->NoteConnect();
}
void TCPNET::UdpMakeConn(SOCKET socket, NETEVENTPROC proc, void *user,
                         const NETCONNADDR *address) {
  if (socket == INVALID_SOCKET)
    return;
  auto *conn = new UDPCONN(this, socket, proc, user, address);
  LinkConn(conn, CONNLIST_UDP_CONNECTED);
}
void TCPNET::FileMakeConn(HANDLE file, NETEVENTPROC proc, void *user,
                          const NETCONNADDR *address) {
  if (file == INVALID_HANDLE_VALUE)
    return;
  auto *conn = new SLFILECONN(this, file, proc, user, address);
  LinkConn(conn, CONNLIST_FILE_CONNECTED);
  conn->NoteConnect();
}
TCPHOSTADDRINFO *TCPNET::LockedFindHostAddrInfo(unsigned) { return nullptr; }
} // namespace OsNet

int OsNetInitialize(unsigned long parts, unsigned long threads) {
  return OsNet::TCPNET::Initialize(parts, threads);
}
void OsNetDestroy(unsigned long parts) { OsNet::TCPNET::Destroy(parts); }
void OsNetPump(unsigned long timeout) {
  if (OsNet::TCPNET::s_pnet)
    OsNet::TCPNET::s_pnet->Pump(timeout);
  else
    Sleep(timeout);
}
HNETCONN OsNetConnCopyHandle(HNETCONN h) {
  if (auto *c = Conn(h))
    c->IncRef();
  return h;
}
void OsNetConnFreeHandle(HNETCONN h) {
  if (auto *c = Conn(h))
    c->DecRef();
}
void OsNetConnAddr(HNETCONN h, NETCONNADDR *a) {
  if (auto *c = Conn(h))
    c->ConnAddr(a);
  else if (a)
    std::memset(a, 0, sizeof(*a));
}
void OsNetConnClose(HNETCONN h) {
  if (auto *c = Conn(h))
    c->Close();
}
int OsNetConnIsClosed(HNETCONN h) {
  auto *c = Conn(h);
  return !c || c->IsClosed();
}
void OsNetConnSetEventProc(HNETCONN h, NETEVENTPROC p) {
  if (auto *c = Conn(h))
    c->SetEventProc(p);
}
void OsNetConnSetUser(HNETCONN h, void *u) {
  if (auto *c = Conn(h))
    c->SetUser(u);
}
void OsNetConnSetEventProcAndUser(HNETCONN h, NETEVENTPROC p, void *u) {
  if (auto *c = Conn(h))
    c->SetEventProcAndUser(p, u);
}
void OsLoopConnect(NETEVENTPROC a, NETEVENTPROC b, void *u, const void *d,
                   unsigned long n) {
  if (OsNet::TCPNET::s_pnet)
    OsNet::TCPNET::s_pnet->LoopConnect(a, b, u, d, n);
}
void OsLoopConnSend(HNETCONN h, const void *d, unsigned long n) {
  if (auto *c = Conn(h))
    c->Send(d, n);
}
int OsTcpListen(unsigned short p, NETEVENTPROC e, void *u) {
  return OsNet::TCPNET::s_pnet ? OsNet::TCPNET::s_pnet->TcpListen(p, e, u) : 0;
}
void OsTcpListenEnable(unsigned short p, int e) {
  if (OsNet::TCPNET::s_pnet)
    OsNet::TCPNET::s_pnet->TcpListenEnable(p, e);
}
void OsTcpConnect(unsigned long a, unsigned short p, NETEVENTPROC e, void *u,
                  const void *d, unsigned long n) {
  if (OsNet::TCPNET::s_pnet)
    OsNet::TCPNET::s_pnet->TcpConnect(a, p, e, u, d, n);
}
void OsTcpConnSend(HNETCONN h, const void *d, unsigned long n) {
  if (auto *c = Conn(h))
    c->Send(d, n);
}
int OsTcpConnSendSync(HNETCONN h, const void *d, unsigned long n,
                      unsigned long *s, unsigned long t) {
  unsigned sent = 0;
  bool ok = false;
  if (auto *c = Conn(h))
    ok = c->SendSync(d, n, &sent, t);
  if (s)
    *s = sent;
  return ok;
}
void OsTcpConnSetNagle(HNETCONN h, int e) {
  if (auto *c = dynamic_cast<OsNet::NETCONNFULL *>(Conn(h)))
    c->SetNagle(e);
}
int OsTcpConnSetWindow(HNETCONN h, int s) {
  if (auto *c = dynamic_cast<OsNet::NETCONNFULL *>(Conn(h)))
    return c->SetWindow(s);
  return 0;
}
void OsTcpConnSetRecvTimeout(HNETCONN h, unsigned long t) {
  if (auto *c = dynamic_cast<OsNet::NETCONNFULL *>(Conn(h)))
    c->SetRecvTimeout(t);
}
unsigned long OsTcpAddrLoop() { return 0; }
void OsUdpConnect(const NETADDR *a, unsigned short p, unsigned short x,
                  NETEVENTPROC e, void *u) {
  if (OsNet::TCPNET::s_pnet)
    OsNet::TCPNET::s_pnet->UdpConnect(a, p, x, e, u);
}
void OsUdpConnSendTo(HNETCONN h, const void *d, unsigned long n,
                     unsigned long c, const NETADDR *a) {
  if (auto *u = dynamic_cast<OsNet::UDPCONN *>(Conn(h)))
    u->SendTo(d, n, c, a);
}
void OsNetAddrMake(unsigned long address, unsigned short port, NETADDR *out) {
  if (!out)
    return;
  std::memset(out, 0, sizeof(*out));
  auto *a = SockAddr(out);
  a->sin_family = AF_INET;
  a->sin_port = htons(port);
  a->sin_addr.s_addr = address;
}
void OsNetAddrMakeBroadcast(unsigned short port, NETADDR *out) {
  OsNetAddrMake(INADDR_BROADCAST, port, out);
}
void OsNetAddrMakeFromStr(const char *s, unsigned short p, NETADDR *out) {
  IN_ADDR address{};
  address.s_addr = INADDR_NONE;
  if (s)
    InetPtonA(AF_INET, s, &address);
  OsNetAddrMake(address.s_addr, p, out);
}
int OsNetAddrLoopback(const NETADDR *a, const NETADDR *b) {
  if (!a || !b)
    return 0;
  auto x = SockAddr(a)->sin_addr.s_addr, y = SockAddr(b)->sin_addr.s_addr;
  return x == y || x == htonl(INADDR_LOOPBACK) || y == htonl(INADDR_LOOPBACK);
}
int OsNetAddrCompare(const NETADDR *a, const NETADDR *b, NETADDRDIFF *d) {
  NETADDRDIFF r = NETADDR_DIFF_EQUAL;
  if (!a || !b)
    r = NETADDR_DIFF_PROTOCOL;
  else if (SockAddr(a)->sin_family != SockAddr(b)->sin_family)
    r = NETADDR_DIFF_PROTOCOL;
  else if (SockAddr(a)->sin_port != SockAddr(b)->sin_port)
    r = NETADDR_DIFF_PORT;
  else if (SockAddr(a)->sin_addr.s_addr != SockAddr(b)->sin_addr.s_addr)
    r = NETADDR_DIFF_NETWORK;
  if (d)
    *d = r;
  return r == NETADDR_DIFF_EQUAL;
}
unsigned long OsNetAddrGetAddress(const NETADDR *a, unsigned short *p) {
  if (!a) {
    if (p)
      *p = 0;
    return 0;
  }
  if (p)
    *p = ntohs(SockAddr(a)->sin_port);
  return SockAddr(a)->sin_addr.s_addr;
}
char *OsNetAddrToStr(const NETADDR *a, char *out, unsigned long len) {
  if (!out || !len)
    return out;
  char ip[INET_ADDRSTRLEN]{};
  inet_ntop(AF_INET, &SockAddr(a)->sin_addr, ip, sizeof(ip));
  sprintf_s(out, len, "%s:%u", ip, ntohs(SockAddr(a)->sin_port));
  return out;
}
unsigned long OsNetAddrToHostOrder(unsigned long a) { return ntohl(a); }
unsigned long OsNetGetHostAddr(const char *name) {
  char local[256]{};
  if (!name) {
    gethostname(local, sizeof(local));
    name = local;
  }
  addrinfo hints{};
  hints.ai_family = AF_INET;
  addrinfo *result = nullptr;
  if (getaddrinfo(name, nullptr, &hints, &result))
    return 0;
  unsigned long address =
      reinterpret_cast<sockaddr_in *>(result->ai_addr)->sin_addr.s_addr;
  freeaddrinfo(result);
  return address;
}
int OsNetGetHostAddrs(const char *n, unsigned short p, HOSTADDRPROC c,
                      void *u) {
  return OsNet::TCPNET::s_pnet ? OsNet::TCPNET::s_pnet->GetHostAddrs(n, p, c, u)
                               : 0;
}
void OsFileConnCreate(const char *f, NETEVENTPROC e, void *u, int r) {
  if (OsNet::TCPNET::s_pnet)
    OsNet::TCPNET::s_pnet->FileConnCreate(f, e, u, r);
}
int OsFileConnRead(HNETCONN h, void *b, std::uint64_t p, unsigned long n,
                   void *o) {
  if (auto *f = dynamic_cast<OsNet::FILECONN *>(Conn(h)))
    return f->Read(p, b, n, o);
  return 0;
}
int OsFileConnWrite(HNETCONN h, const void *b, std::uint64_t p, unsigned long n,
                    void *o) {
  if (auto *f = dynamic_cast<OsNet::FILECONN *>(Conn(h)))
    return f->Write(p, b, n, o);
  return 0;
}
void OsFileConnClose(HNETCONN h) { OsNetConnClose(h); }
