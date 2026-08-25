#pragma once

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#include <atomic>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <vector>

union NETADDR {
  unsigned char data[16];
  struct {
    std::uint64_t pos;
    void *operationId;
  } file;
};
static_assert(sizeof(NETADDR) == 16);

struct NETCONNADDR {
  NETADDR peerAddr;
  NETADDR selfAddr;
};
enum NETNOTE : int {
  NETNOTE_CONNECT,
  NETNOTE_DATA,
  NETNOTE_DISCONNECT,
  NETNOTE_CANTCONNECT,
  NETNOTE_FILEWRITE,
  NETNOTE_FILEREAD
};
enum NETADDRDIFF : int {
  NETADDR_DIFF_EQUAL,
  NETADDR_DIFF_NODE,
  NETADDR_DIFF_NETWORK,
  NETADDR_DIFF_PORT,
  NETADDR_DIFF_PROTOCOL
};
struct HNETCONN__ {
  virtual ~HNETCONN__() = default;
};
using HNETCONN = HNETCONN__ *;
using NETEVENTPROC = int(__stdcall *)(HNETCONN, const NETCONNADDR *, NETNOTE,
                                      void *, const void *, unsigned long,
                                      unsigned long *);
using HOSTADDRPROC = void(__stdcall *)(const NETADDR *, unsigned long, void *);

namespace OsNet {
enum SELECTSET : int {
  SELECTSET_FIRST,
  SELECTSET_R = SELECTSET_FIRST,
  SELECTSET_W,
  SELECTSET_E,
  SELECT_MAX
};
enum CONNLIST : int {
  CONNLIST_LOOP_CONNECTED,
  CONNLIST_TCP_CONNECTED,
  CONNLIST_UDP_CONNECTED,
  CONNLIST_FILE_CONNECTED,
  CONNLISTS,
  CONNLIST_NONE = CONNLISTS
};
enum OVERLAPTYPE : int {
  OVERLAPTYPE_READ,
  OVERLAPTYPE_WRITE,
  OVERLAPTYPE_ACCEPT
};
enum OUTPUTSTATE : int {
  OUTPUTSTATE_WAITING,
  OUTPUTSTATE_WRITING,
  OUTPUTSTATE_COMPLETED
};

struct LOCKEDLONG {
  volatile LONG m_value = 0;
  LONG Increment();
  LONG Decrement();
};

struct NETOVERLAP {
  OVERLAPPED m_overlapped{};
  OVERLAPTYPE m_type = OVERLAPTYPE_READ;
};
class TCPNET;
class NETSELECTSETS;

class NETSELSOCK {
public:
  explicit NETSELSOCK(SOCKET socket = INVALID_SOCKET);
  virtual ~NETSELSOCK();
  virtual bool IsClosed() const;
  virtual void Close();
  virtual void AddToSelectSets(NETSELECTSETS *sets);
  virtual void Selected(SELECTSET selectedSet);
  SOCKET m_sock;
};

struct NETSELSOCKPTR {
  NETSELSOCKPTR();
  explicit NETSELSOCKPTR(NETSELSOCK *value);
  NETSELSOCK *ptr;
};
class NETSELECTSETS {
public:
  explicit NETSELECTSETS(TCPNET *net = nullptr);
  void Clear();
  void AddSelSock(NETSELSOCK *socket);
  void AddToSet(NETSELSOCK *socket, SELECTSET set);
  int Select(unsigned long timeoutMs, long socketCount = 0);
  TCPNET *m_net;

private:
  fd_set m_sets[SELECT_MAX];
  std::vector<NETSELSOCK *> m_sockets;
};

struct OUTPUT {
  NETOVERLAP m_overlap;
  OUTPUTSTATE m_state = OUTPUTSTATE_WAITING;
  std::uint64_t m_pos = 0;
  unsigned m_bytes = 0, m_dataBytes = 0;
  unsigned char *m_data = nullptr;
  HANDLE m_completionEvent = nullptr;
  ~OUTPUT();
};
struct INPUT {
  NETOVERLAP m_overlap;
  std::uint64_t m_pos = 0;
  void *m_operationId = nullptr;
  unsigned m_bytes = 0;
  unsigned char *m_buffer = nullptr;
  ~INPUT();
};

class NETCONN : public HNETCONN__, public NETSELSOCK {
public:
  NETCONN(TCPNET *net, SOCKET socket, NETEVENTPROC eventProc, void *user,
          const NETCONNADDR *address);
  ~NETCONN() override;
  virtual void IncIo();
  virtual void DecIo();
  virtual void CompleteWrite(NETOVERLAP *, unsigned);
  virtual void CompleteRead(NETOVERLAP *, unsigned);
  virtual void Send(const void *data, unsigned bytes);
  virtual bool SendSync(const void *data, unsigned bytes, unsigned *bytesSent,
                        unsigned timeout);
  virtual void CloseAndUnlock();
  void Disconnect(int notify);
  void GetEventProcAndUser(NETEVENTPROC &eventProc, void *&user);
  void SetEventProcAndUser(NETEVENTPROC eventProc, void *user);
  void SetEventProc(NETEVENTPROC);
  void SetUser(void *);
  int NoteCantConnect();
  int NoteConnect();
  int NoteDisconnect();
  int NoteData(void *data, unsigned bytes, unsigned *bytesProcessed,
               const NETCONNADDR *address = nullptr);
  int NoteFileOperation(void *data, unsigned bytes, unsigned offsetLow,
                        unsigned offsetHigh, void *operationId, NETNOTE note);
  void IncRef();
  void DecRef();
  void ConnAddr(NETCONNADDR *address);
  void Close() override;

protected:
  std::atomic<long> m_refCount;
  NETCONNADDR m_connAddr;
  NETEVENTPROC m_eventProc;
  void *m_user;
  std::mutex m_lock;
  unsigned m_time;
  TCPNET *m_net;
  std::atomic<bool> m_disconnectNoted;
};

class NETCONNFULL : public NETCONN {
public:
  using NETCONN::NETCONN;
  virtual void SetNagle(int);
  virtual int SetWindow(int);
  virtual void SetRecvTimeout(unsigned);
};
class NETCONNLESS : public NETCONN {
public:
  using NETCONN::NETCONN;
};

class LOOPCONN : public NETCONNFULL {
public:
  struct INPUT {
    std::vector<unsigned char> data;
    ~INPUT();
  };
  LOOPCONN(TCPNET *, NETEVENTPROC, void *, const NETCONNADDR *);
  ~LOOPCONN() override;
  void Connect();
  void CompleteInput(INPUT *);
  void EnqueueInput(const void *, unsigned);
  void Send(const void *, unsigned) override;
  bool SendSync(const void *, unsigned, unsigned *, unsigned) override;
  void CloseAndUnlock() override;
  void Close() override;
  bool IsClosed() const override;
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;
  LOOPCONN *m_loopConn = nullptr;
};

class TCPCONN : public NETCONNFULL {
public:
  TCPCONN(TCPNET *, SOCKET, NETEVENTPROC, void *, const NETCONNADDR *);
  ~TCPCONN() override;
  void CompleteWrite(NETOVERLAP *, unsigned) override;
  void CompleteRead(NETOVERLAP *, unsigned) override;
  OUTPUT *LockedEnqueue(const void *, unsigned);
  void Send(const void *, unsigned) override;
  bool SendSync(const void *, unsigned, unsigned *, unsigned) override;
  void CloseAndUnlock() override;
  void SetNagle(int) override;
  int SetWindow(int) override;
  void SetRecvTimeout(unsigned) override;
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;

protected:
  virtual void StartWriteAndLeaveLock();
  virtual void StartRead();
  void ContinueQueuedWrite();
  std::deque<std::unique_ptr<OUTPUT>> m_outputQueue;
  std::vector<unsigned char> m_pendingInput;
  std::atomic<long> m_ioCount{0};
};
class IOTCPCONN : public TCPCONN {
public:
  IOTCPCONN(TCPNET *, SOCKET, NETEVENTPROC, void *, const NETCONNADDR *);
  void IncIo() override;
  void DecIo() override;
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;

protected:
  void StartWriteAndLeaveLock() override;
  void StartRead() override;
};
class SLTCPCONN : public TCPCONN {
public:
  SLTCPCONN(TCPNET *, SOCKET, NETEVENTPROC, void *, const NETCONNADDR *);
  void ContinueWrite();
  void ContinueRead();
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;

protected:
  void StartWriteAndLeaveLock() override;
  void StartRead() override;
};

class UDPCONN : public NETCONNLESS {
public:
  UDPCONN(TCPNET *, SOCKET, NETEVENTPROC, void *, const NETCONNADDR *);
  ~UDPCONN() override;
  void SendTo(const void *, unsigned, unsigned, const NETADDR *);
  void CloseAndUnlock() override;
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;
};

class FILECONN : public NETCONN {
public:
  FILECONN(TCPNET *, HANDLE, NETEVENTPROC, void *, const NETCONNADDR *);
  ~FILECONN() override;
  bool IsClosed() const override;
  void IncIo() override;
  void DecIo() override;
  void CompleteWrite(NETOVERLAP *, unsigned) override;
  void CompleteRead(NETOVERLAP *, unsigned) override;
  OUTPUT *LockedEnqueue(std::uint64_t, const void *, unsigned, void *);
  int Write(std::uint64_t, const void *, unsigned, void *);
  int Read(std::uint64_t, void *, unsigned, void *);
  void CloseAndUnlock() override;
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;

protected:
  virtual void StartWriteAndLeaveLock();
  virtual void StartRead();
  HANDLE m_file;
};
class IOFILECONN : public FILECONN {
public:
  IOFILECONN(TCPNET *, HANDLE, NETEVENTPROC, void *, const NETCONNADDR *);
  void CloseAndUnlock() override;

protected:
  void StartWriteAndLeaveLock() override;
  void StartRead() override;
};
class SLFILECONN : public FILECONN {
public:
  SLFILECONN(TCPNET *, HANDLE, NETEVENTPROC, void *, const NETCONNADDR *);
  ~SLFILECONN() override;
  static unsigned __stdcall Thread(void *);
  void CloseAndUnlock() override;

protected:
  void StartWriteAndLeaveLock() override;
  void StartRead() override;
};

class NETCONNECT : public NETSELSOCK {
public:
  NETCONNECT(void *user, const void *data, unsigned bytes);
  ~NETCONNECT() override;
  void NoteCantConnect(NETEVENTPROC, const NETCONNADDR *);
  virtual void Fail();
  virtual void Complete();

protected:
  void *m_user;
  std::vector<unsigned char> m_data;
};
class LOOPCONNECT : public NETCONNECT {
public:
  LOOPCONNECT(NETEVENTPROC, NETEVENTPROC, void *, const void *, unsigned);
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;
  void Fail() override;
  void Complete() override;
  bool IsClosed() const override;
  NETEVENTPROC m_eventProcSrc, m_eventProcDst;
};
class TCPCONNECT : public NETCONNECT {
public:
  TCPCONNECT(unsigned, unsigned short, NETEVENTPROC, void *, const void *,
             unsigned);
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;
  void Fail() override;
  void Complete() override;
  unsigned m_nodeNumber;
  unsigned m_portAddr;
  NETEVENTPROC m_eventProc;
};
class FILECONNECT : public NETCONNECT {
public:
  FILECONNECT(HANDLE, NETEVENTPROC, void *);
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;
  void Fail() override;
  void Complete() override;
  bool IsClosed() const override;
  HANDLE m_file;
  NETEVENTPROC m_eventProc;
};

class TCPLISTEN : public NETSELSOCK {
public:
  TCPLISTEN(SOCKET, unsigned short, NETEVENTPROC, void *, unsigned);
  ~TCPLISTEN() override;
  int Enable(int);
  void Close() override;
  void AddToSelectSets(NETSELECTSETS *) override;
  void Selected(SELECTSET) override;
  unsigned m_portAddr;
  NETEVENTPROC m_eventProc;
  void *m_user;
  bool m_enabled;
};
class TCPACCEPT {
public:
  explicit TCPACCEPT(TCPLISTEN *);
  ~TCPACCEPT();
  void Init();
  SOCKET Complete(NETCONNADDR *, int);
  TCPLISTEN *m_listen;
  SOCKET m_sock;
};

class TCPHOSTADDRINFO {
public:
  TCPHOSTADDRINFO();
  ~TCPHOSTADDRINFO();
  void Complete();
  std::vector<NETADDR> m_addrs;
  HOSTADDRPROC m_hostAddrProc;
  void *m_user;
};

class TCPNET {
public:
  TCPNET();
  ~TCPNET();
  static int Initialize(unsigned parts, unsigned pumpThreads);
  static void Destroy(unsigned parts);
  static void IncludeDependantParts(unsigned *parts);
  static bool IoCompletionPresent(unsigned *error);
  static SOCKET CreateListenSocket(unsigned short);
  static void MakeConnAddr(SOCKET, unsigned, NETCONNADDR *);
  void Pump(unsigned timeout);
  void IoPump(unsigned timeout);
  static unsigned __stdcall IoPumpThread(void *);
  static unsigned __stdcall SlPumpThread(void *);
  static unsigned __stdcall ListenThread(void *);
  static unsigned __stdcall UdpPumpThread(void *);
  static unsigned __stdcall BaseThread(void *);
  static unsigned __stdcall GetHostAddrsThread(void *);
  int BaseInitialize(unsigned);
  int WinsockInitialize(unsigned);
  int IoInitialize(unsigned);
  int TcpInitialize(unsigned);
  void BaseDestroy();
  void WinsockDestroy();
  void IoDestroy();
  void TcpDestroy();
  int PumpThreadsInitialize();
  void PumpThreadsDestroy();
  void WakePumpThread();
  void IncRef();
  void DecRef();
  static void LogWrite(const char *, ...);
  static void LogDump();
  int TcpListen(unsigned short, NETEVENTPROC, void *);
  void TcpListenEnable(unsigned short, int);
  void TcpConnect(unsigned, unsigned short, NETEVENTPROC, void *, const void *,
                  unsigned);
  void UdpConnect(const NETADDR *, unsigned short, unsigned short, NETEVENTPROC,
                  void *);
  void LoopConnect(NETEVENTPROC, NETEVENTPROC, void *, const void *, unsigned);
  void FileConnCreate(const char *, NETEVENTPROC, void *, int);
  int GetHostAddrs(const char *, unsigned short, HOSTADDRPROC, void *);
  void LinkConn(NETCONN *, CONNLIST);
  void CompleteAcceptEx(TCPACCEPT *, int);
  void CompleteAccept(TCPLISTEN *, SOCKET, const NETCONNADDR *);
  void LoopCompleteConnect(LOOPCONNECT *);
  void TcpCompleteConnect(TCPCONNECT *);
  void FileCompleteConnect(FILECONNECT *);
  void LoopConnectInit(LOOPCONNECT *);
  void TcpConnectInit(TCPCONNECT *);
  void FileConnectInit(FILECONNECT *);
  void LoopMakeConn(NETEVENTPROC, NETEVENTPROC, void *, const void *, unsigned);
  void TcpMakeConn(SOCKET, NETEVENTPROC, void *, const NETCONNADDR *,
                   const void *, unsigned);
  void UdpMakeConn(SOCKET, NETEVENTPROC, void *, const NETCONNADDR *);
  void FileMakeConn(HANDLE, NETEVENTPROC, void *, const NETCONNADDR *);
  TCPHOSTADDRINFO *LockedFindHostAddrInfo(unsigned);
  LOOPCONN::INPUT *LoopAllocInput(unsigned);
  void LoopFreeInput(LOOPCONN::INPUT *);
  static TCPNET *s_pnet;

private:
  std::atomic<long> m_refCount;
  std::mutex m_mutex;
  std::vector<TCPLISTEN *> m_listeners;
  std::vector<NETCONN *> m_connections;
  bool m_winsockStarted;
  unsigned m_parts = 0;
  unsigned m_pumpThreadCount = 0;
  std::atomic<bool> m_stopPump{false};
  std::vector<HANDLE> m_pumpThreads;
  HANDLE m_wakeEvent = nullptr;
};
} // namespace OsNet

int OsNetInitialize(unsigned long parts, unsigned long pumpThreads);
void OsNetDestroy(unsigned long parts);
void OsNetPump(unsigned long timeout);
HNETCONN OsNetConnCopyHandle(HNETCONN);
void OsNetConnFreeHandle(HNETCONN);
void OsNetConnAddr(HNETCONN, NETCONNADDR *);
void OsNetConnClose(HNETCONN);
int OsNetConnIsClosed(HNETCONN);
void OsNetConnSetEventProc(HNETCONN, NETEVENTPROC);
void OsNetConnSetUser(HNETCONN, void *);
void OsNetConnSetEventProcAndUser(HNETCONN, NETEVENTPROC, void *);
void OsLoopConnect(NETEVENTPROC, NETEVENTPROC, void *, const void *,
                   unsigned long);
void OsLoopConnSend(HNETCONN, const void *, unsigned long);
int OsTcpListen(unsigned short, NETEVENTPROC, void *);
void OsTcpListenEnable(unsigned short, int);
void OsTcpConnect(unsigned long, unsigned short, NETEVENTPROC, void *,
                  const void *, unsigned long);
void OsTcpConnSend(HNETCONN, const void *, unsigned long);
int OsTcpConnSendSync(HNETCONN, const void *, unsigned long, unsigned long *,
                      unsigned long);
void OsTcpConnSetNagle(HNETCONN, int);
int OsTcpConnSetWindow(HNETCONN, int);
void OsTcpConnSetRecvTimeout(HNETCONN, unsigned long);
unsigned long OsTcpAddrLoop();
void OsUdpConnect(const NETADDR *, unsigned short, unsigned short, NETEVENTPROC,
                  void *);
void OsUdpConnSendTo(HNETCONN, const void *, unsigned long, unsigned long,
                     const NETADDR *);
void OsNetAddrMake(unsigned long, unsigned short, NETADDR *);
void OsNetAddrMakeBroadcast(unsigned short, NETADDR *);
void OsNetAddrMakeFromStr(const char *, unsigned short, NETADDR *);
int OsNetAddrLoopback(const NETADDR *, const NETADDR *);
int OsNetAddrCompare(const NETADDR *, const NETADDR *, NETADDRDIFF *);
unsigned long OsNetAddrGetAddress(const NETADDR *, unsigned short *);
char *OsNetAddrToStr(const NETADDR *, char *, unsigned long);
unsigned long OsNetAddrToHostOrder(unsigned long);
unsigned long OsNetGetHostAddr(const char *);
int OsNetGetHostAddrs(const char *, unsigned short, HOSTADDRPROC, void *);
void OsFileConnCreate(const char *, NETEVENTPROC, void *, int);
int OsFileConnRead(HNETCONN, void *, std::uint64_t, unsigned long, void *);
int OsFileConnWrite(HNETCONN, const void *, std::uint64_t, unsigned long,
                    void *);
void OsFileConnClose(HNETCONN);
