namespace BattleNet.Protocol;

public enum AuthCode : byte
{
    AuthOk = 0x0C,
    AuthFailed = 0x0D,
    AuthReject = 0x0E,
    AuthBadServerProof = 0x0F,
    AuthUnavailable = 0x10,
    AuthSystemError = 0x11,
    AuthBillingError = 0x12,
    AuthBillingExpired = 0x13,
    AuthVersionMismatch = 0x14,
    AuthUnknownAccount = 0x15,
    AuthIncorrectPassword = 0x16,
    AuthSessionExpired = 0x17,
    AuthServerShuttingDown = 0x18,
    AuthAlreadyLoggingIn = 0x19,
    AuthLoginServerNotFound = 0x1A,
    AuthWaitQueue = 0x1B
}

public enum CharacterLoginCode : byte
{
    InProgress = 0x30,
    Success = 0x31,
    NoWorld = 0x32,
    DuplicateCharacter = 0x33,
    NoInstances = 0x34,
    Failed = 0x35,
    Disabled = 0x36
}

public enum CharacterCreateCode : byte
{
    InProgress = 0x27,
    Success = 0x28,
    Error = 0x29,
    Failed = 0x2A,
    NameInUse = 0x2B,
    Disabled = 0x2C
}

public enum CharacterDeleteCode : byte
{
    InProgress = 0x2D,
    Success = 0x2E,
    Failed = 0x2F
}

public enum Srp6ResponseType : byte
{
    AuthChallenge = 0,
    AuthProof = 1,
    AuthRecode = 2,
    AuthRecodeProof = 3
}

public enum UpdateServerOpcode : byte
{
    ServerFileList = 0x00,
    ServerReady = 0x07,
    ServerRsyncMeta = 0x08,
    ServerRsyncDelta = 0x09,
    ServerError = 0x0A,
    ServerFileData = 0x0B,
    ServerFileFinalize = 0x0C,
    ServerDeltaBlock = 0x10,
    ServerFileRequest = 0x11,
    ServerEnd = 0x13,
    ClientRequestFileMd5 = 0x02,
    ClientRequestFile = 0x06,
    ClientSetFileset = 0x12,
    ClientHello = 0x14,
    ClientSignature = 0xF0,
    ClientSignatureLegacy = 0x69,
    ServerHello = 0x15,
    ServerHelloAck = 0x14
}
