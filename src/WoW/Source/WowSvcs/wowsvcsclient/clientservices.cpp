#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "ENGINE/Source/SRP/sha.h"
#include "ENGINE/Source/SRP/srp6.h"
#include "Storm/h/storm.h"
#include "WoW/Source/Console/consolevar.h"
#include "WoW/Source/Glue/cgluemgr.h"
#include "WoW/Source/client.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
constexpr NETMESSAGE CMSG_CHAR_CREATE = 54;
constexpr NETMESSAGE CMSG_CHAR_ENUM = 55;
constexpr NETMESSAGE CMSG_CHAR_DELETE = 56;
constexpr NETMESSAGE CMSG_PLAYER_LOGIN = 61;
constexpr NETMESSAGE SMSG_CHAR_CREATE = 58;
constexpr NETMESSAGE SMSG_CHAR_ENUM = 59;
constexpr NETMESSAGE SMSG_CHAR_DELETE = 60;
constexpr NETMESSAGE SMSG_CHARACTER_LOGIN_FAILED = 65;
constexpr NETMESSAGE SMSG_LOGOUT_RESPONSE = 76;
constexpr NETMESSAGE SMSG_LOGOUT_COMPLETE = 77;
constexpr NETMESSAGE SMSG_LOGOUT_CANCEL_ACK = 79;
constexpr NETMESSAGE SMSG_AUTH_CHALLENGE = 477;
constexpr NETMESSAGE CMSG_AUTH_SESSION = 478;
constexpr NETMESSAGE SMSG_AUTH_RESPONSE = 479;
constexpr int AUTH_OK = 12;
constexpr std::uint32_t CMSG_AUTH_SRP6_BEGIN = 0x33;
constexpr std::uint32_t CMSG_AUTH_SRP6_PROOF = 0x34;

std::unique_ptr<ClientConnection> g_clientConnection;
ClientConnection* s_currentConnection = nullptr;
char s_accountName[64]{};
bool s_accountNameValid = false;
CVar* s_realmListVar = nullptr;
CVar* g_realmNameVar = nullptr;
CVar* g_realmAddressVar = nullptr;
char s_redirectServer[32]{};
char s_statusText[256]{};
std::string s_pendingAccount;
std::string s_pendingPassword;

constexpr std::array<const char*, 66> s_errorCodeTokens{{
    "", "RESPONSE_DISCONNECTED", "RESPONSE_CANCELLED", "RESPONSE_DISCONNECTED",
    "RESPONSE_FAILED_TO_CONNECT", "RESPONSE_CONNECTED", "RESPONSE_DISCONNECTED",
    "CSTATUS_CONNECTING", "RESPONSE_NEGOTIATION_FAILED", "RESPONSE_UNKNOWN",
    "RESPONSE_UNKNOWN", "CSTATUS_AUTHENTICATING", "AUTH_OK", "AUTH_FAILED",
    "AUTH_REJECT", "AUTH_BAD_SERVER_PROOF", "AUTH_UNAVAILABLE", "AUTH_SYSTEM_ERROR",
    "AUTH_BILLING_ERROR", "AUTH_BILLING_EXPIRED", "AUTH_VERSION_MISMATCH",
    "AUTH_UNKNOWN_ACCOUNT", "AUTH_INCORRECT_PASSWORD", "AUTH_SESSION_EXPIRED",
    "AUTH_SERVER_SHUTTING_DOWN", "AUTH_ALREADY_LOGGING_IN",
    "AUTH_LOGIN_SERVER_NOT_FOUND", "AUTH_WAIT_QUEUE", "REALM_LIST_IN_PROGRESS",
    "REALM_LIST_SUCCESS", "REALM_LIST_FAILED", "REALM_LIST_INVALID",
    "REALM_LIST_REALM_NOT_FOUND", "ACCOUNT_CREATE_IN_PROGRESS",
    "ACCOUNT_CREATE_SUCCESS", "ACCOUNT_CREATE_FAILED", "CHAR_LIST_RETRIEVING",
    "CHAR_LIST_RETRIEVED", "CHAR_LIST_FAILED", "CHAR_CREATE_IN_PROGRESS",
    "CHAR_CREATE_SUCCESS", "CHAR_CREATE_ERROR", "CHAR_CREATE_FAILED",
    "CHAR_CREATE_NAME_IN_USE", "CHAR_CREATE_DISABLED", "CHAR_DELETE_IN_PROGRESS",
    "CHAR_DELETE_SUCCESS", "CHAR_DELETE_FAILED", "CHAR_LOGIN_IN_PROGRESS",
    "CHAR_LOGIN_SUCCESS", "CHAR_LOGIN_NO_WORLD", "CHAR_LOGIN_DUPLICATE_CHARACTER",
    "CHAR_LOGIN_NO_INSTANCES", "CHAR_LOGIN_FAILED", "CHAR_LOGIN_DISABLED",
    "CHAR_NAME_NO_NAME", "CHAR_NAME_TOO_SHORT", "CHAR_NAME_TOO_LONG",
    "CHAR_NAME_STARTS_WITH_GRAVE", "CHAR_NAME_TWO_GRAVES",
    "CHAR_NAME_INVALID_CHARACTER", "CHAR_NAME_MIXED_LANGUAGES",
    "CHAR_NAME_PROFANE", "CHAR_NAME_RESERVED", "CHAR_NAME_FAILURE",
    "CHAR_NAME_SUCCESS"
}};

// These indices are protocol values, not a compact list of only the operations
// implemented by this client. Keep the account-create range at 33..35 so all
// subsequent character results retain the values recovered from the original.
static_assert(std::string_view{s_errorCodeTokens[33]} == "ACCOUNT_CREATE_IN_PROGRESS");
static_assert(std::string_view{s_errorCodeTokens[36]} == "CHAR_LIST_RETRIEVING");
static_assert(std::string_view{s_errorCodeTokens[40]} == "CHAR_CREATE_SUCCESS");
static_assert(std::string_view{s_errorCodeTokens[43]} == "CHAR_CREATE_NAME_IN_USE");
static_assert(std::string_view{s_errorCodeTokens[46]} == "CHAR_DELETE_SUCCESS");
static_assert(std::string_view{s_errorCodeTokens[49]} == "CHAR_LOGIN_SUCCESS");
static_assert(std::string_view{s_errorCodeTokens[55]} == "CHAR_NAME_NO_NAME");
static_assert(std::string_view{s_errorCodeTokens[65]} == "CHAR_NAME_SUCCESS");

void CopyString(char* destination, std::size_t capacity, const char* source)
{
    if (!destination || capacity == 0)
        return;
    std::snprintf(destination, capacity, "%s", source ? source : "");
}

using ShaPiece = std::pair<const std::uint8_t*, std::size_t>;

std::array<std::uint8_t, 20> Sha1(std::initializer_list<ShaPiece> pieces)
{
    SHA1_CONTEXT context{};
    std::array<std::uint8_t, 20> digest{};
    SHA1_Init(&context);
    for (const ShaPiece& piece : pieces) {
        if (piece.first && piece.second)
            SHA1_Update(&context, piece.first, static_cast<unsigned>(piece.second));
    }
    SHA1_Final(digest.data(), &context);
    return digest;
}

std::string UpperAscii(const char* value)
{
    std::string result = value ? value : "";
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char character) {
        return static_cast<char>(std::toupper(character));
    });
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

bool ReceiveExact(SOCKET socketHandle, std::uint8_t* data, std::size_t bytes)
{
    while (bytes != 0) {
        const int received = recv(socketHandle, reinterpret_cast<char*>(data),
                                  static_cast<int>(std::min<std::size_t>(bytes, 0x7FFFFFFF)), 0);
        if (received <= 0)
            return false;
        data += received;
        bytes -= static_cast<std::size_t>(received);
    }
    return true;
}

bool SendLoginPacket(SOCKET socketHandle, std::uint32_t opcode,
                     const std::uint8_t* payload, std::size_t payloadBytes)
{
    if (payloadBytes > 0xFFFF - sizeof(opcode))
        return false;
    const std::uint16_t declared = static_cast<std::uint16_t>(sizeof(opcode) + payloadBytes);
    std::array<std::uint8_t, 6> header{{
        static_cast<std::uint8_t>(declared >> 8),
        static_cast<std::uint8_t>(declared & 0xFF),
        static_cast<std::uint8_t>(opcode & 0xFF),
        static_cast<std::uint8_t>((opcode >> 8) & 0xFF),
        static_cast<std::uint8_t>((opcode >> 16) & 0xFF),
        static_cast<std::uint8_t>((opcode >> 24) & 0xFF)}};
    return SendAll(socketHandle, header.data(), header.size()) &&
           (payloadBytes == 0 || SendAll(socketHandle, payload, payloadBytes));
}

bool ReceiveSrpPacket(SOCKET socketHandle, std::vector<std::uint8_t>& payload)
{
    std::array<std::uint8_t, 2> header{};
    if (!ReceiveExact(socketHandle, header.data(), header.size()))
        return false;
    const std::uint16_t bytes = static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(header[0]) << 8) | header[1]);
    if (bytes == 0 || bytes > 4096)
        return false;
    payload.resize(bytes);
    return ReceiveExact(socketHandle, payload.data(), payload.size());
}

SOCKET ConnectLoginServer(const char* hostName)
{
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    addrinfo* addresses = nullptr;
    if (getaddrinfo(hostName, "3724", &hints, &addresses) != 0)
        return INVALID_SOCKET;
    SOCKET result = INVALID_SOCKET;
    for (addrinfo* address = addresses; address; address = address->ai_next) {
        result = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (result != INVALID_SOCKET &&
            connect(result, address->ai_addr, static_cast<int>(address->ai_addrlen)) == 0)
            break;
        if (result != INVALID_SOCKET)
            closesocket(result);
        result = INVALID_SOCKET;
    }
    freeaddrinfo(addresses);
    if (result != INVALID_SOCKET) {
        const DWORD timeout = 3000;
        setsockopt(result, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&timeout), sizeof(timeout));
        setsockopt(result, SOL_SOCKET, SO_SNDTIMEO,
                   reinterpret_cast<const char*>(&timeout), sizeof(timeout));
    }
    return result;
}

void BigToFixed(const BigData& value, std::uint8_t* destination, unsigned bytes)
{
    std::memset(destination, 0, bytes);
    unsigned written = 0;
    SBigToBinaryBuffer(&value, destination, bytes, &written);
}

bool CalculateSrpProof(const char* account, const char* password,
                       const std::vector<std::uint8_t>& challenge,
                       std::array<std::uint8_t, 32>& clientPublic,
                       std::array<std::uint8_t, 20>& clientProof,
                       std::array<std::uint8_t, 40>& sessionKey)
{
    if (challenge.size() < 5 || challenge[0] != AUTH_OK || challenge[1] != 0)
        return false;
    std::size_t position = 2;
    const std::uint8_t generatorBytes = challenge[position++];
    if (generatorBytes == 0 || position + generatorBytes + 1 > challenge.size())
        return false;
    const std::uint8_t* generator = challenge.data() + position;
    position += generatorBytes;
    const std::uint8_t modulusBytes = challenge[position++];
    if (modulusBytes != 32 || position + modulusBytes + 64 > challenge.size())
        return false;
    const std::uint8_t* modulus = challenge.data() + position;
    position += modulusBytes;
    const std::uint8_t* salt = challenge.data() + position;
    position += 32;
    const std::uint8_t* serverPublic = challenge.data() + position;

    std::array<std::uint8_t, 32> privateKey{};
    SRP6_Random random(OsGetAsyncTimeMsPrecise());
    random.GenerateRandomBytes(privateKey.data(), static_cast<unsigned>(privateKey.size()));

    BigData n, g, a, publicA, publicB, x, u, gx, multiplier, multipliedGx;
    BigData base, temporary, ux, exponent, secret;
    SBigFromBinary(&n, modulus, modulusBytes);
    SBigFromBinary(&g, generator, generatorBytes);
    SBigFromBinary(&a, privateKey.data(), static_cast<unsigned>(privateKey.size()));
    SBigFromBinary(&publicB, serverPublic, 32);
    SBigPowMod(&publicA, &g, &a, &n);
    BigToFixed(publicA, clientPublic.data(), static_cast<unsigned>(clientPublic.size()));

    const std::string user = UpperAscii(account);
    const std::string pass = UpperAscii(password);
    const std::string identity = user + ":" + pass;
    const auto identityHash = Sha1({{
        reinterpret_cast<const std::uint8_t*>(identity.data()), identity.size()}});
    const auto xHash = Sha1({{salt, 32}, {identityHash.data(), identityHash.size()}});
    const auto uHash = Sha1({{clientPublic.data(), clientPublic.size()}, {serverPublic, 32}});
    SBigFromBinary(&x, xHash.data(), static_cast<unsigned>(xHash.size()));
    SBigFromBinary(&u, uHash.data(), static_cast<unsigned>(uHash.size()));
    SBigPowMod(&gx, &g, &x, &n);
    SBigFromUnsigned(&multiplier, 3);
    SBigMulMod(&multipliedGx, &multiplier, &gx, &n);
    if (SBigCompare(&publicB, &multipliedGx) >= 0) {
        SBigSub(&base, &publicB, &multipliedGx);
    } else {
        SBigAdd(&temporary, &publicB, &n);
        SBigSub(&base, &temporary, &multipliedGx);
    }
    SBigMul(&ux, &u, &x);
    SBigAdd(&exponent, &a, &ux);
    SBigPowMod(&secret, &base, &exponent, &n);
    std::array<std::uint8_t, 32> secretBytes{};
    BigToFixed(secret, secretBytes.data(), static_cast<unsigned>(secretBytes.size()));

    std::size_t secretOffset = 0;
    std::size_t secretLength = secretBytes.size();
    while (secretLength > 1 && secretBytes[secretOffset] == 0) {
        secretOffset += 2;
        secretLength -= 2;
    }
    std::vector<std::uint8_t> even;
    std::vector<std::uint8_t> odd;
    for (std::size_t index = 0; index < secretLength; ++index) {
        (index % 2 == 0 ? even : odd).push_back(secretBytes[secretOffset + index]);
    }
    const auto evenHash = Sha1({{even.data(), even.size()}});
    const auto oddHash = Sha1({{odd.data(), odd.size()}});
    for (std::size_t index = 0; index < evenHash.size(); ++index) {
        sessionKey[index * 2] = evenHash[index];
        sessionKey[index * 2 + 1] = oddHash[index];
    }

    constexpr std::array<std::uint8_t, 20> xorNg{{
        221, 123, 176, 58, 56, 172, 115, 17, 3, 152,
        124, 90, 80, 111, 202, 150, 108, 123, 194, 167}};
    const auto userHash = Sha1({{
        reinterpret_cast<const std::uint8_t*>(user.data()), user.size()}});
    clientProof = Sha1({
        {xorNg.data(), xorNg.size()}, {userHash.data(), userHash.size()},
        {salt, 32}, {clientPublic.data(), clientPublic.size()},
        {serverPublic, 32}, {sessionKey.data(), sessionKey.size()}});
    return true;
}

bool IsLocalRealmList(const char* value, std::string& host)
{
    if (!value || !*value)
        return false;
    host = value;
    if (const std::size_t colon = host.rfind(':'); colon != std::string::npos)
        host.resize(colon);
    return _stricmp(host.c_str(), "127.0.0.1") == 0 ||
           _stricmp(host.c_str(), "localhost") == 0;
}

bool BootstrapLocalLogin(LoginData& loginData, const char* requestedAccount,
                         const char* requestedPassword)
{
    if (loginData.m_account[0])
        return true;
    std::string host;
    if (!IsLocalRealmList(s_realmListVar ? s_realmListVar->StringValue() : nullptr, host))
        return false;

    // The recovered login service canonicalized the account before filling
    // LoginData.  The world-server proof hashes these bytes verbatim, so keep
    // that invariant in the local SRP bootstrap as well.
    const std::string canonicalAccount = UpperAscii(requestedAccount ? requestedAccount : "");
    const char* account = canonicalAccount.c_str();
    const char* password = requestedPassword ? requestedPassword : "";
    const std::size_t accountBytes = std::strlen(account);
    if (accountBytes == 0 || accountBytes > 64)
        return false;

    const SOCKET socketHandle = ConnectLoginServer(host.c_str());
    if (socketHandle == INVALID_SOCKET)
        return false;
    std::vector<std::uint8_t> begin(9 + accountBytes, 0);
    begin[8] = static_cast<std::uint8_t>(accountBytes);
    std::memcpy(begin.data() + 9, account, accountBytes);
    std::vector<std::uint8_t> challenge;
    bool success = SendLoginPacket(socketHandle, CMSG_AUTH_SRP6_BEGIN,
                                   begin.data(), begin.size()) &&
                   ReceiveSrpPacket(socketHandle, challenge);

    std::array<std::uint8_t, 32> clientPublic{};
    std::array<std::uint8_t, 20> clientProof{};
    std::array<std::uint8_t, 40> sessionKey{};
    success = success && CalculateSrpProof(account, password, challenge,
                                           clientPublic, clientProof, sessionKey);
    std::array<std::uint8_t, 52> proofPayload{};
    std::copy(clientPublic.begin(), clientPublic.end(), proofPayload.begin());
    std::copy(clientProof.begin(), clientProof.end(), proofPayload.begin() + clientPublic.size());
    std::vector<std::uint8_t> proofResponse;
    success = success && SendLoginPacket(socketHandle, CMSG_AUTH_SRP6_PROOF,
                                         proofPayload.data(), proofPayload.size()) &&
              ReceiveSrpPacket(socketHandle, proofResponse);
    if (success) {
        const auto expectedServerProof = Sha1({
            {clientPublic.data(), clientPublic.size()},
            {clientProof.data(), clientProof.size()},
            {sessionKey.data(), sessionKey.size()}});
        success = proofResponse.size() >= 22 && proofResponse[0] == AUTH_OK &&
                  proofResponse[1] == 1 &&
                  std::memcmp(proofResponse.data() + 2, expectedServerProof.data(),
                              expectedServerProof.size()) == 0;
    }
    shutdown(socketHandle, SD_BOTH);
    closesocket(socketHandle);
    if (!success)
        return false;

    CopyString(loginData.m_account, sizeof(loginData.m_account), account);
    loginData.m_loginServerID = 0;
    std::copy(sessionKey.begin(), sessionKey.end(), loginData.m_sessionKey);
    return true;
}

void RegisterServiceCVars()
{
    s_realmListVar = CVar::Lookup("realmList");
    if (!s_realmListVar)
        s_realmListVar = CVar::Register(nullptr, "realmList", 0,
                                        "wowrealms.battle.net", nullptr, 6, 0, nullptr);
    g_realmNameVar = CVar::Lookup("realmName");
    if (!g_realmNameVar)
        g_realmNameVar = CVar::Register(nullptr, "realmName", 0, "", nullptr, 6, 0, nullptr);
    g_realmAddressVar = CVar::Lookup("realmAddress");
    if (!g_realmAddressVar)
        g_realmAddressVar = CVar::Register(nullptr, "realmAddress", 0, "", nullptr, 6, 0, nullptr);
}

const char* StatusMessage(int errorCode, unsigned waitCount)
{
    const char* token = ClientServices_GetErrorToken(errorCode);
    const char* text = token && *token
        ? FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE) : nullptr;
    if (!text || !*text)
        text = token && *token ? token : "";
    if (errorCode == 27) {
        std::snprintf(s_statusText, sizeof(s_statusText), text, waitCount);
        return s_statusText;
    }
    return text;
}
}

ClientConnection::ClientConnection() = default;

ClientConnection::~ClientConnection()
{
    Destroy();
}

int ClientConnection::Initialize(const LoginData* loginData)
{
    if (!m_statusComplete)
        Cancel(1);
    m_statusCop = COP_INIT;
    if (!m_initialized && !NetClient::Initialize()) {
        m_statusCop = COP_NONE;
        return 0;
    }

    m_statusComplete = 1;
    m_statusResult = 1;
    m_errorCode = 0;
    m_initialized = 1;
    SetMessageHandler(SMSG_AUTH_CHALLENGE, ClientServices_MessageHandler, this);
    SetMessageHandler(SMSG_AUTH_RESPONSE, ClientServices_MessageHandler, this);
    SetMessageHandler(SMSG_CHAR_ENUM, ClientServices_MessageHandler, this);
    SetMessageHandler(SMSG_CHAR_CREATE, ClientServices_MessageHandler, this);
    SetMessageHandler(SMSG_CHARACTER_LOGIN_FAILED, ClientServices_MessageHandler, this);
    SetMessageHandler(SMSG_LOGOUT_COMPLETE, ClientServices_MessageHandler, this);
    SetMessageHandler(SMSG_LOGOUT_CANCEL_ACK, ClientServices_MessageHandler, this);
    SetMessageHandler(SMSG_LOGOUT_RESPONSE, ClientServices_MessageHandler, this);
    SetMessageHandler(SMSG_CHAR_DELETE, ClientServices_MessageHandler, this);
    m_loginData = loginData ? *loginData : LoginData{};
    return 1;
}

void ClientConnection::Destroy()
{
    if (!m_initialized)
        return;
    ClearMessageHandler(SMSG_AUTH_CHALLENGE);
    ClearMessageHandler(SMSG_AUTH_RESPONSE);
    ClearMessageHandler(SMSG_CHAR_ENUM);
    ClearMessageHandler(SMSG_CHAR_CREATE);
    ClearMessageHandler(SMSG_CHARACTER_LOGIN_FAILED);
    ClearMessageHandler(SMSG_LOGOUT_COMPLETE);
    ClearMessageHandler(SMSG_LOGOUT_CANCEL_ACK);
    ClearMessageHandler(SMSG_LOGOUT_RESPONSE);
    ClearMessageHandler(SMSG_CHAR_DELETE);
    if (!m_statusComplete)
        Cancel(1);
    if (m_connected)
        Disconnect();
    m_characterList.clear();
    m_realmList.clear();
    NetClient::Destroy();
    m_initialized = 0;
    m_statusCop = COP_NONE;
}

void ClientConnection::Cleanup()
{
    if (m_cleanup) {
        CLIENTSERVICESCLEANUP cleanup = m_cleanup;
        m_cleanup = nullptr;
        cleanup(this);
    }
}

void ClientConnection::Cancel(int errorCode)
{
    Cleanup();
    m_statusResult = 0;
    m_errorCode = errorCode;
    m_statusComplete = 1;
}

int ClientConnection::PollStatus(WOWCS_OPS* op, int* errorCode, int* result) const
{
    if (op) *op = m_statusCop;
    if (errorCode) *errorCode = m_errorCode;
    if (result) *result = m_statusResult;
    return m_statusComplete;
}

void ClientConnection::Connect()
{
    m_cleanup = nullptr;
    m_statusCop = COP_CONNECT;
    m_errorCode = 7;
    m_statusComplete = 0;
    if (m_connected) {
        Cleanup();
        m_statusResult = 1;
        m_statusComplete = 1;
        m_errorCode = 5;
        return;
    }

    if (!m_loginData.m_account[0] &&
        !BootstrapLocalLogin(m_loginData, s_pendingAccount.c_str(),
                             s_pendingPassword.c_str())) {
        s_pendingPassword.clear();
        m_statusResult = 0;
        m_errorCode = 13;
        m_statusComplete = 1;
        return;
    }
    s_pendingPassword.clear();
    if (m_loginData.m_account[0]) {
        CopyString(s_accountName, sizeof(s_accountName), m_loginData.m_account);
        s_accountNameValid = true;
    }
    m_realmList.clear();
    const char* serverAddress = s_realmListVar ? s_realmListVar->StringValue() : nullptr;
    ClientNetGetRealms(serverAddress, RealmEnum_InternalCallback, this);
}

int ClientConnection::Disconnect()
{
    if (!m_connected)
        return 0;
    NetClient::Disconnect();
    m_connected = 0;
    return 1;
}

void ClientConnection::ConnectToSelectedServer()
{
    const char* selectedName = g_realmNameVar ? g_realmNameVar->StringValue() : "";
    auto selected = std::find_if(m_realmList.begin(), m_realmList.end(),
        [selectedName](const REALM_INFO& realm) {
            return _stricmp(realm.name, selectedName ? selectedName : "") == 0;
        });

    // An unset selection uses the sole realm, which is equivalent to the
    // recovered first-login registry state after realm enumeration.
    if (selected == m_realmList.end() && m_realmList.size() == 1 &&
        (!selectedName || !*selectedName)) {
        selected = m_realmList.begin();
        if (g_realmNameVar)
            g_realmNameVar->Set(selected->name);
    }

    if (selected == m_realmList.end()) {
        Cleanup();
        m_statusResult = 0;
        m_errorCode = 32;
        m_statusComplete = 1;
        return;
    }

    CopyString(s_redirectServer, sizeof(s_redirectServer), selected->address);
    NetClient::Connect(selected->address);
}

void ClientConnection::RealmEnumCallback(CDataStore* data)
{
    if (!data) {
        Cleanup();
        m_errorCode = 30;
        m_statusResult = 0;
        m_statusComplete = 1;
        return;
    }

    std::uint8_t count = 0;
    data->Get(&count);
    m_realmList.clear();
    m_realmList.resize(count);
    for (std::uint8_t index = 0; index < count; ++index) {
        REALM_INFO& realm = m_realmList[index];
        realm.id = index;
        data->GetString(realm.name, sizeof(realm.name));
        data->GetString(realm.address, sizeof(realm.address));
        data->Get(&realm.players);
        if (data->Failed())
            break;
    }

    if (data->Failed() || !data->IsRead()) {
        m_realmList.clear();
        Cleanup();
        m_errorCode = 31;
        m_statusResult = 0;
        m_statusComplete = 1;
        return;
    }

    if (m_statusCop == COP_CONNECT) {
        ConnectToSelectedServer();
    } else {
        Cleanup();
        m_statusResult = 1;
        m_errorCode = 29;
        m_statusComplete = 1;
    }
}

int ClientConnection::HandleConnect()
{
    Cleanup();
    m_statusResult = 1;
    m_errorCode = 5;
    m_statusComplete = 1;
    m_connected = 1;
    std::fprintf(stderr,
                 "CHARSELECT_NET connected=1 cop=%d statusResult=%d error=%d\n",
                 static_cast<int>(m_statusCop), m_statusResult, m_errorCode);
    return NetClient::HandleConnect();
}

int ClientConnection::HandleDisconnect()
{
    std::fprintf(stderr,
                 "CHARSELECT_NET disconnect cop=%d statusComplete=%d statusResult=%d error=%d\n",
                 static_cast<int>(m_statusCop), m_statusComplete, m_statusResult, m_errorCode);
    Cleanup();
    m_statusComplete = 1;
    m_connected = 0;
    return NetClient::HandleDisconnect();
}

int ClientConnection::HandleCantConnect()
{
    Cleanup();
    m_statusResult = 0;
    m_errorCode = 4;
    m_statusComplete = 1;
    m_connected = 0;
    return NetClient::HandleCantConnect();
}

void ClientConnection::AccountLogin(const char* name, const char* password,
                                    int, WOW_LOCALE)
{
    if (!m_initialized || !m_statusComplete || !name || !password)
        return;
    m_cleanup = nullptr;
    m_statusCop = COP_AUTHENTICATE;
    m_errorCode = 11;
    m_statusComplete = 0;
    if (!m_connected)
        Cancel(4);
}

void ClientConnection::AccountLogin_Finish(int reason)
{
    Cleanup();
    m_errorCode = reason;
    m_statusComplete = 1;
    m_statusResult = reason == AUTH_OK;
}

void ClientConnection::AccountLogout()
{
    s_accountNameValid = false;
}

int ClientConnection::HandleAuthChallenge(NETMESSAGE msgId, unsigned, CDataStore* msg)
{
    if (msgId != SMSG_AUTH_CHALLENGE || !msg)
        return 0;
    std::uint32_t serverChallenge = 0;
    msg->Get(&serverChallenge);
    if (msg->Failed())
        return 0;

    const std::uint32_t clientChallenge = OsGetAsyncTimeMsPrecise();
    const std::uint32_t zero = 0;
    std::uint8_t digest[20]{};
    SHA1_CONTEXT context{};
    SHA1_Init(&context);
    SHA1_Update(&context, reinterpret_cast<const std::uint8_t*>(m_loginData.m_account),
                static_cast<unsigned>(std::strlen(m_loginData.m_account)));
    SHA1_Update(&context, reinterpret_cast<const std::uint8_t*>(&zero), sizeof(zero));
    SHA1_Update(&context, reinterpret_cast<const std::uint8_t*>(&clientChallenge), sizeof(clientChallenge));
    SHA1_Update(&context, reinterpret_cast<const std::uint8_t*>(&zero), sizeof(zero));
    SHA1_Update(&context, reinterpret_cast<const std::uint8_t*>(&serverChallenge), sizeof(serverChallenge));
    SHA1_Update(&context, m_loginData.m_sessionKey, sizeof(m_loginData.m_sessionKey));
    SHA1_Final(digest, &context);

    CDataStore response;
    response.Put<NETMESSAGE>(CMSG_AUTH_SESSION);
    response.Put<std::uint32_t>(3368);
    response.Put<std::uint32_t>(static_cast<std::uint32_t>(m_loginData.m_loginServerID));
    response.PutString(m_loginData.m_account);
    response.Put(clientChallenge);
    response.PutData(digest, sizeof(digest));
    response.Finalize();
    NetClient::Send(&response);
    return 1;
}

int ClientConnection::HandleAuthResponse(NETMESSAGE msgId, unsigned, CDataStore* msg)
{
    if (msgId != SMSG_AUTH_RESPONSE || !msg)
        return 0;
    std::uint8_t reason = 0;
    msg->Get(&reason);
    if (reason == 27) {
        msg->Get(&m_waitCount);
        m_errorCode = reason;
        m_statusCop = COP_WAIT_QUEUE;
        m_statusComplete = 0;
        CGlueMgr::UpdateWaitQueue(m_waitCount);
    } else {
        AccountLogin_Finish(reason);
    }
    return 1;
}

int __fastcall ClientServices_MessageHandler(void* param, NETMESSAGE msgId,
                                             unsigned time, CDataStore* msg)
{
    auto* client = static_cast<ClientConnection*>(param);
    if (!client)
        return 0;
    switch (msgId) {
    case SMSG_AUTH_CHALLENGE: return client->HandleAuthChallenge(msgId, time, msg);
    case SMSG_AUTH_RESPONSE: return client->HandleAuthResponse(msgId, time, msg);
    case SMSG_CHAR_CREATE: return client->HandleCharacterCreate(msgId, time, msg);
    case SMSG_CHAR_ENUM: return client->HandleCharEnum(msgId, time, msg);
    case SMSG_CHAR_DELETE: return client->HandleCharacterDelete(msgId, time, msg);
    case SMSG_CHARACTER_LOGIN_FAILED:
        return client->HandleCharacterLoginFailed(msgId, time, msg);
    default: return 0;
    }
}

void ClientServices_Initialize(const LoginData* loginData)
{
    RegisterServiceCVars();
    if (!g_clientConnection)
        g_clientConnection = std::make_unique<ClientConnection>();
    g_clientConnection->Initialize(loginData);
    s_currentConnection = g_clientConnection.get();
    if (g_clientConnection->m_loginData.m_account[0])
        ClientServices_SetAccountName(g_clientConnection->m_loginData.m_account);
}

void ClientServices_Destroy()
{
    s_currentConnection = nullptr;
    g_clientConnection.reset();
}

void ClientServices_Cleanup()
{
    if (s_currentConnection)
        s_currentConnection->Cleanup();
}

void ClientServices_Cancel()
{
    if (s_currentConnection)
        s_currentConnection->Cancel(2);
}

void ClientServices_Disconnected() {}

void ClientServices_Connect()
{
    if (!s_currentConnection)
        return;
    CGlueMgr::m_queueTime = {};
    CGlueMgr::m_queuePosition = {};
    CGlueMgr::m_estimatedWaitTime = 0;
    s_currentConnection->Connect();
}

int ClientServices_Disconnect()
{
    return s_currentConnection ? s_currentConnection->Disconnect() : 0;
}

int ClientServices_IsConnected()
{
    return s_currentConnection ? s_currentConnection->m_connected : 0;
}

int ClientServices_PollStatus(WOWCS_OPS* op, const char** msg,
                              int* result, int* errorCode)
{
    if (!s_currentConnection) {
        if (msg) *msg = "";
        if (result) *result = 0;
        if (errorCode) *errorCode = 1;
        if (op) *op = COP_NONE;
        return 1;
    }
    const int complete = s_currentConnection->PollStatus(op, errorCode, result);
    if (msg) {
        *msg = complete
            ? StatusMessage(errorCode ? *errorCode : 0, s_currentConnection->m_waitCount)
            : "";
    }
    return complete;
}

const char* ClientServices_GetErrorToken(int errorCode)
{
    if (errorCode <= 0 || errorCode >= static_cast<int>(s_errorCodeTokens.size()))
        return "";
    return s_errorCodeTokens[static_cast<std::size_t>(errorCode)];
}

unsigned ClientServices_GetWaitCount()
{
    return s_currentConnection ? s_currentConnection->m_waitCount : 0;
}

int ClientServices_ValidDisconnect(ClientConnection* client)
{
    return client == s_currentConnection;
}

void ClientServices_AccountLogin(const char* name, const char* password,
                                 int region, WOW_LOCALE locale)
{
    if (s_currentConnection)
        s_currentConnection->AccountLogin(name, password, region, locale);
}

void ClientServices_SetLoginCredentials(const char* accountName, const char* password)
{
    s_pendingAccount = accountName ? accountName : "";
    s_pendingPassword = password ? password : "";
    ClientServices_SetAccountName(s_pendingAccount.c_str());
}

void ClientServices_SetAccountName(const char* accountName)
{
    if (!accountName)
        return;
    CopyString(s_accountName, sizeof(s_accountName), accountName);
    s_accountNameValid = true;
}

const char* ClientServices_GetAccountName()
{
    return s_accountNameValid ? s_accountName : nullptr;
}

void ClientServices_GetRealmList()
{
    if (!s_currentConnection)
        return;
    s_currentConnection->m_realmList.clear();
    s_currentConnection->m_cleanup = nullptr;
    s_currentConnection->m_statusCop = COP_GET_REALMS;
    s_currentConnection->m_errorCode = 28;
    s_currentConnection->m_statusComplete = 0;
    ClientNetGetRealms(s_realmListVar ? s_realmListVar->StringValue() : nullptr,
                       RealmEnum_InternalCallback, s_currentConnection);
}

std::size_t ClientServices_GetRealmListCount()
{
    return s_currentConnection ? s_currentConnection->GetRealmListCount() : 0;
}

const REALM_INFO* ClientServices_GetRealmInfoByIndex(unsigned index)
{
    return s_currentConnection ? s_currentConnection->GetRealmInfoByIndex(index) : nullptr;
}

int ClientServices_EnumerateRealms(REALMENUMCALLBACK callback, void* param)
{
    return s_currentConnection ? s_currentConnection->EnumerateRealms(callback, param) : 0;
}

void ClientServices_SelectRealm(const char* realmName)
{
    if (g_realmNameVar && realmName)
        g_realmNameVar->Set(realmName);
}

const char* ClientServices_GetSelectedRealmName()
{
    return g_realmNameVar ? g_realmNameVar->StringValue() : "";
}

const char* ClientServices_GetSelectedRealmAddress()
{
    return s_redirectServer;
}

ClientConnection* ClientServices_GetCurrent() { return s_currentConnection; }

ClientConnection* ClientServices_SetCurrent(ClientConnection* client)
{
    ClientConnection* previous = s_currentConnection;
    s_currentConnection = client;
    return previous;
}

void ClientServices_PollEventQueue()
{
    if (s_currentConnection)
        s_currentConnection->PollEventQueue();
}

void __fastcall RealmEnum_InternalCallback(CDataStore* data, void* param)
{
    if (auto* client = static_cast<ClientConnection*>(param))
        client->RealmEnumCallback(data);
}

const REALM_INFO* ClientConnection::GetRealmInfoByIndex(unsigned index) const
{
    return index < m_realmList.size() ? &m_realmList[index] : nullptr;
}

const std::vector<REALM_INFO>& ClientConnection::GetRealmList() const { return m_realmList; }
std::size_t ClientConnection::GetRealmListCount() const { return m_realmList.size(); }
std::size_t ClientConnection::GetCharacterListCount() const { return m_characterList.size(); }

int ClientConnection::EnumerateRealms(REALMENUMCALLBACK callback, void* param)
{
    if (!callback)
        return 0;
    for (REALM_INFO& realm : m_realmList)
        callback(&realm, param);
    return static_cast<int>(m_realmList.size());
}

void ClientConnection::SetPlaying(int value) { m_playing = value; }
void ClientConnection::CharacterAbortLogout() {}
void ClientConnection::CharacterCreate(const CHARACTER_CREATE_INFO* info)
{
    std::fprintf(stderr, "CHARCREATE_SEND connected=%d name=%s race=%u class=%u sex=%u\n",
                 m_connected, info ? info->name : "<null>",
                 info ? info->raceID : 0, info ? info->classID : 0, info ? info->sexID : 0);
    m_cleanup = nullptr;
    m_statusCop = COP_CREATE_CHARACTER;
    m_errorCode = 39;
    m_statusComplete = 0;
    if (!m_connected || !info) {
        Cancel(4);
        return;
    }

    CDataStore netMsg;
    netMsg.Put<NETMESSAGE>(CMSG_CHAR_CREATE);
    netMsg.PutString(info->name);
    netMsg.Put(info->raceID);
    netMsg.Put(info->classID);
    netMsg.Put(info->sexID);
    netMsg.Put(info->skinID);
    netMsg.Put(info->faceID);
    netMsg.Put(info->hairStyleID);
    netMsg.Put(info->hairColorID);
    netMsg.Put(info->facialHairStyleID);
    netMsg.Put(info->outfitID);
    netMsg.Finalize();
    NetClient::Send(&netMsg);
}

void ClientConnection::CharacterDelete(std::uint64_t guid)
{
    m_cleanup = nullptr;
    m_statusCop = COP_DELETE_CHARACTER;
    m_errorCode = 45;
    m_statusComplete = 0;
    if (!m_connected || guid == 0) {
        Cancel(4);
        return;
    }

    CDataStore netMsg;
    netMsg.Put<NETMESSAGE>(CMSG_CHAR_DELETE);
    netMsg.Put(guid);
    netMsg.Finalize();
    NetClient::Send(&netMsg);
}
void ClientConnection::CharacterForceLogout() {}
void ClientConnection::CharacterLoggingOut() {}
void ClientConnection::CharacterLogin(std::uint64_t id)
{
    m_cleanup = nullptr;
    m_statusCop = COP_LOGIN_CHARACTER;
    m_errorCode = 48;
    m_statusComplete = 0;
    if (!m_connected) {
        Cancel(4);
        return;
    }

    CDataStore netMsg;
    netMsg.Put<NETMESSAGE>(CMSG_PLAYER_LOGIN);
    netMsg.Put(id);
    netMsg.Finalize();
    NetClient::Send(&netMsg);
    m_playing = 1;
    std::fprintf(stderr, "ENTERWORLD_SEND guid=%llu connected=%d\n",
                 static_cast<unsigned long long>(id), m_connected);
}
void ClientConnection::CharacterLogout() {}
void ClientConnection::CharacterRemoveFromGame() { m_inGame = 0; }
void ClientConnection::CharacterSetInGame(int state)
{
    Cleanup();
    m_statusResult = 1;
    m_statusComplete = 1;
    m_errorCode = 49;
    m_inGame = state;
}
int ClientConnection::EnumerateCharacters(CHARACTERENUMCALLBACK callback, void* param)
{
    if (!callback)
        return 0;
    const int count = static_cast<int>(m_characterList.size());
    for (CHARACTER_INFO& character : m_characterList)
        callback(&character, param);
    return count;
}

void ClientConnection::GetCharacterList()
{
    m_characterList.clear();
    m_cleanup = nullptr;
    m_statusCop = COP_GET_CHARACTERS;
    m_errorCode = 36;
    m_statusComplete = 0;
    std::fprintf(stderr, "CHARSELECT_ENUM send connected=%d\n", m_connected);
    if (!m_connected) {
        Cancel(4);
        std::fprintf(stderr, "CHARSELECT_ENUM cancelled error=%d\n", m_errorCode);
        return;
    }

    CDataStore netMsg;
    netMsg.Put<NETMESSAGE>(CMSG_CHAR_ENUM);
    netMsg.Finalize();
    NetClient::Send(&netMsg);
}

int ClientConnection::HandleCharEnum(NETMESSAGE msgId, unsigned, CDataStore* msg)
{
    if (msgId != SMSG_CHAR_ENUM || !msg)
        return 0;

    std::uint8_t count = 0;
    msg->Get(&count);
    std::fprintf(stderr,
                 "CHARSELECT_ENUM recv count=%u bytes=%u failed=%d\n",
                 static_cast<unsigned>(count), msg->m_size, msg->Failed());
    m_characterList.clear();
    m_characterList.resize(count);
    for (CHARACTER_INFO& character : m_characterList) {
        msg->Get(&character.guid);
        msg->GetString(character.name, sizeof(character.name));
        msg->Get(&character.raceID);
        msg->Get(&character.classID);
        msg->Get(&character.sexID);
        msg->Get(&character.skinID);
        msg->Get(&character.faceID);
        msg->Get(&character.hairStyleID);
        msg->Get(&character.hairColorID);
        msg->Get(&character.facialHairStyleID);
        msg->Get(&character.experienceLevel);
        msg->Get(&character.zoneID);
        msg->Get(&character.mapID);
        msg->Get(&character.position.x);
        msg->Get(&character.position.y);
        msg->Get(&character.position.z);
        msg->Get(&character.guildID);
        msg->Get(&character.petDisplayInfoID);
        msg->Get(&character.petExperienceLevel);
        msg->Get(&character.petCreatureFamilyID);
        for (std::size_t slot = 0; slot < std::size(character.inventoryItemDisplayID); ++slot) {
            std::uint8_t inventoryType = 0;
            msg->Get(&character.inventoryItemDisplayID[slot]);
            msg->Get(&inventoryType);
            character.inventoryItemType[slot] = inventoryType;
        }
        if (msg->Failed())
            break;
    }

    Cleanup();
    if (msg->Failed() || !msg->IsRead()) {
        m_characterList.clear();
        m_statusResult = 0;
        m_errorCode = 38;
    } else {
        m_statusResult = 1;
        m_errorCode = 37;
    }
    m_statusComplete = 1;
    std::fprintf(stderr,
                 "CHARSELECT_ENUM complete result=%d error=%d failed=%d read=%d records=%zu\n",
                 m_statusResult, m_errorCode, msg->Failed(), msg->IsRead(),
                 m_characterList.size());
    return 1;
}

int ClientConnection::HandleCharacterCreate(NETMESSAGE msgId, unsigned, CDataStore* msg)
{
    if (msgId != SMSG_CHAR_CREATE || !msg)
        return 0;
    std::uint8_t result = 0;
    msg->Get(&result);
    std::fprintf(stderr, "CHARCREATE_RESPONSE result=%u failed=%d read=%d\n",
                 result, msg->Failed(), msg->IsRead());
    Cleanup();
    m_errorCode = result;
    m_statusResult = result == 40;
    m_statusComplete = 1;
    return 1;
}

int ClientConnection::HandleCharacterDelete(NETMESSAGE msgId, unsigned, CDataStore* msg)
{
    if (msgId != SMSG_CHAR_DELETE || !msg)
        return 0;
    std::uint8_t result = 0;
    msg->Get(&result);
    Cleanup();
    if (!msg->Failed() && msg->IsRead() && result == 0) {
        m_statusResult = 1;
        m_errorCode = 46;
    } else {
        m_statusResult = 0;
        m_errorCode = 47;
    }
    m_statusComplete = 1;
    return 1;
}
int ClientConnection::HandleCharacterLoginFailed(NETMESSAGE msgId, unsigned,
                                                  CDataStore* msg)
{
    if (msgId != SMSG_CHARACTER_LOGIN_FAILED || !msg)
        return 0;
    std::uint8_t reason = 0;
    msg->Get(&reason);
    if (!m_isBot) {
        CharacterRemoveFromGame();
        if (this == s_currentConnection)
            ClientDestroyGame(1, 1, "1");
    }
    switch (reason) {
    case 1: Cancel(50); break;
    case 2: Cancel(51); break;
    case 3: Cancel(52); break;
    case 4: Cancel(54); break;
    default: Cancel(53); break;
    }
    std::fprintf(stderr, "ENTERWORLD_FAILED reason=%u error=%d\n", reason, m_errorCode);
    return 1;
}
void ClientConnection::HandleLogoutAbortAck() {}
void ClientConnection::HandleLogoutComplete() {}
void ClientConnection::HandleLogoutResponse() {}

void ClientServices_AccountLogout() { if (s_currentConnection) s_currentConnection->AccountLogout(); }
void ClientServices_AccountValidateName() {}
void ClientServices_CharacterAbortLogout() {}
void ClientServices_CharacterCreate(const CHARACTER_CREATE_INFO* info)
{
    if (s_currentConnection)
        s_currentConnection->CharacterCreate(info);
}
void ClientServices_CharacterDelete(std::uint64_t guid)
{
    if (s_currentConnection)
        s_currentConnection->CharacterDelete(guid);
}
void ClientServices_CharacterForceLogout() {}
void ClientServices_CharacterIsInGame() {}
void ClientServices_CharacterLoggingOut() {}
void ClientServices_CharacterLogin(std::uint64_t id, unsigned,
                                   NTempest::C3Vector)
{
    if (s_currentConnection)
        s_currentConnection->CharacterLogin(id);
}
void ClientServices_CharacterLogout() {}
void ClientServices_CharacterRemoveFromGame() {}
void ClientServices_CharacterSetInGame(int state)
{
    if (s_currentConnection)
        s_currentConnection->CharacterSetInGame(state);
}
int ClientServices_CharacterValidateName(const char* name)
{
    return 55 + static_cast<int>(ValidateCharacterName(
        static_cast<WOW_LOCALE>(CGlueMgr::m_locale), name));
}
void ClientServices_ClearMessageHandler(NETMESSAGE msgId)
{
    if (s_currentConnection)
        s_currentConnection->ClearMessageHandler(msgId);
}
int ClientServices_EnumerateCharacters(CHARACTERENUMCALLBACK callback, void* param)
{
    return s_currentConnection ? s_currentConnection->EnumerateCharacters(callback, param) : 0;
}
void ClientServices_Exit() {}
void ClientServices_GetCharacterList()
{
    if (s_currentConnection)
        s_currentConnection->GetCharacterList();
}
std::size_t ClientServices_GetCharacterListCount()
{
    return s_currentConnection ? s_currentConnection->GetCharacterListCount() : 0;
}
void ClientServices_GetNetStats() {}
void ClientServices_Report() {}
void ClientServices_ReportScreenshot() {}
void ClientServices_Send(CDataStore* msg)
{
    if (s_currentConnection)
        s_currentConnection->Send(msg);
}
void ClientServices_SetMessageHandler(NETMESSAGE msgId, NETMESSAGEHANDLER handler,
                                      void* param)
{
    if (s_currentConnection && handler)
        s_currentConnection->SetMessageHandler(msgId, handler, param);
}
void ConsoleCommand_Logout() {}
