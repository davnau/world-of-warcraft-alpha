#pragma once

#include "WoW/Source/Glue/charselectinfo.h"
#include "WoW/Source/Net/netclient.h"
#include "WoW/Source/Game/validatename.h"

#include <cstddef>
#include <cstdint>
#include <vector>

enum WOWCS_OPS : std::int32_t {
    COP_NONE = 0,
    COP_INIT = 1,
    COP_CONNECT = 2,
    COP_AUTHENTICATE = 3,
    COP_CREATE_ACCOUNT = 4,
    COP_CREATE_CHARACTER = 5,
    COP_GET_CHARACTERS = 6,
    COP_DELETE_CHARACTER = 7,
    COP_LOGIN_CHARACTER = 8,
    COP_GET_REALMS = 9,
    COP_WAIT_QUEUE = 10,
};

struct LoginData {
    char m_account[64]{};
    std::int32_t m_loginServerID = 0;
    std::uint8_t m_sessionKey[40]{};
};

struct REALM_INFO {
    std::uint8_t id = 0;
    char name[256]{};
    char address[32]{};
    std::uint32_t players = 0;
};

struct CHARACTER_CREATE_INFO {
    char name[48]{};
    std::uint8_t raceID = 0;
    std::uint8_t classID = 0;
    std::uint8_t sexID = 0;
    std::uint8_t skinID = 0;
    std::uint8_t faceID = 0;
    std::uint8_t hairStyleID = 0;
    std::uint8_t hairColorID = 0;
    std::uint8_t facialHairStyleID = 0;
    std::uint8_t outfitID = 0;
};

using CLIENTSERVICESCLEANUP = void (__fastcall *)(class ClientConnection*);
using REALMENUMCALLBACK = void (__fastcall *)(REALM_INFO*, void*);
using CHARACTERENUMCALLBACK = void (__fastcall *)(CHARACTER_INFO*, void*);

class ClientConnection : public NetClient {
public:
    ClientConnection();
    ~ClientConnection() override;

    int Initialize(const LoginData* loginData = nullptr);
    void Destroy();
    void Cleanup();
    void Cancel(int errorCode);
    int PollStatus(WOWCS_OPS* op, int* errorCode, int* result) const;

    void Connect();
    int Disconnect();
    void ConnectToSelectedServer();
    void RealmEnumCallback(CDataStore* data);

    int HandleConnect() override;
    int HandleDisconnect() override;
    int HandleCantConnect() override;

    void AccountLogin(const char* name, const char* password,
                      int region, WOW_LOCALE locale);
    void AccountLogin_Finish(int reason);
    void AccountLogout();
    int HandleAuthChallenge(NETMESSAGE msgId, unsigned time, CDataStore* msg);
    int HandleAuthResponse(NETMESSAGE msgId, unsigned time, CDataStore* msg);

    void CharacterAbortLogout();
    void CharacterCreate(const CHARACTER_CREATE_INFO* info);
    void CharacterDelete(std::uint64_t guid);
    void CharacterForceLogout();
    void CharacterLoggingOut();
    void CharacterLogin(std::uint64_t id);
    void CharacterLogout();
    void CharacterRemoveFromGame();
    void CharacterSetInGame(int state);
    int EnumerateCharacters(CHARACTERENUMCALLBACK callback, void* param);
    void GetCharacterList();
    std::size_t GetCharacterListCount() const;
    const REALM_INFO* GetRealmInfoByIndex(unsigned index) const;
    const std::vector<REALM_INFO>& GetRealmList() const;
    std::size_t GetRealmListCount() const;
    int HandleCharEnum(NETMESSAGE msgId, unsigned time, CDataStore* msg);
    int HandleCharacterCreate(NETMESSAGE msgId, unsigned time, CDataStore* msg);
    int HandleCharacterDelete(NETMESSAGE msgId, unsigned time, CDataStore* msg);
    int HandleCharacterLoginFailed(NETMESSAGE msgId, unsigned time, CDataStore* msg);
    void HandleLogoutAbortAck();
    void HandleLogoutComplete();
    void HandleLogoutResponse();
    int EnumerateRealms(REALMENUMCALLBACK callback, void* param);
    void SetPlaying(int value);

    int m_initialized = 0;
    int m_connected = 0;
    int m_playing = 0;
    int m_statusComplete = 1;
    int m_statusResult = 1;
    WOWCS_OPS m_statusCop = COP_NONE;
    int m_errorCode = 0;
    int m_inGame = 0;
    std::uint8_t m_exitAfterLogout = 0;
    std::uint8_t m_loggingOut = 0;
    LoginData m_loginData{};
    std::vector<CHARACTER_INFO> m_characterList;
    std::vector<REALM_INFO> m_realmList;
    int m_isBot = 0;
    unsigned m_waitCount = 0;
    CLIENTSERVICESCLEANUP m_cleanup = nullptr;
};

int __fastcall ClientServices_MessageHandler(void* param, NETMESSAGE msgId,
                                             unsigned time, CDataStore* msg);
void ClientServices_Initialize(const LoginData* loginData = nullptr);
void ClientServices_Destroy();
void ClientServices_Cleanup();
void ClientServices_Cancel();
void ClientServices_Disconnected();
void ClientServices_Connect();
int ClientServices_Disconnect();
int ClientServices_IsConnected();
int ClientServices_PollStatus(WOWCS_OPS* op, const char** msg,
                              int* result, int* errorCode);
const char* ClientServices_GetErrorToken(int errorCode);
unsigned ClientServices_GetWaitCount();
int ClientServices_ValidDisconnect(ClientConnection* client);

void ClientServices_AccountLogin(const char* name, const char* password,
                                 int region, WOW_LOCALE locale);
void ClientServices_SetLoginCredentials(const char* accountName, const char* password);
void ClientServices_SetAccountName(const char* accountName);
const char* ClientServices_GetAccountName();

void ClientServices_GetRealmList();
std::size_t ClientServices_GetRealmListCount();
const REALM_INFO* ClientServices_GetRealmInfoByIndex(unsigned index);
int ClientServices_EnumerateRealms(REALMENUMCALLBACK callback, void* param);
void ClientServices_SelectRealm(const char* realmName);
const char* ClientServices_GetSelectedRealmName();
const char* ClientServices_GetSelectedRealmAddress();

ClientConnection* ClientServices_GetCurrent();
ClientConnection* ClientServices_SetCurrent(ClientConnection* client);
void ClientServices_PollEventQueue();

void ClientServices_AccountLogout();
void ClientServices_AccountValidateName();
void ClientServices_CharacterAbortLogout();
void ClientServices_CharacterCreate(const CHARACTER_CREATE_INFO* info);
void ClientServices_CharacterDelete(std::uint64_t guid);
void ClientServices_CharacterForceLogout();
void ClientServices_CharacterIsInGame();
void ClientServices_CharacterLoggingOut();
void ClientServices_CharacterLogin(std::uint64_t id, unsigned continentID,
                                   NTempest::C3Vector position);
void ClientServices_CharacterLogout();
void ClientServices_CharacterRemoveFromGame();
void ClientServices_CharacterSetInGame(int state);
int ClientServices_CharacterValidateName(const char* name);
void ClientServices_ClearMessageHandler(NETMESSAGE msgId);
int ClientServices_EnumerateCharacters(CHARACTERENUMCALLBACK callback, void* param);
void ClientServices_Exit();
void ClientServices_GetCharacterList();
std::size_t ClientServices_GetCharacterListCount();
void ClientServices_GetNetStats();
void ClientServices_Report();
void ClientServices_ReportScreenshot();
void ClientServices_Send(CDataStore* msg);
void ClientServices_SetMessageHandler(NETMESSAGE msgId, NETMESSAGEHANDLER handler,
                                      void* param = nullptr);
void ConsoleCommand_Logout();
void __fastcall RealmEnum_InternalCallback(CDataStore* data, void* param);
