#pragma once

#include "Storm/h/sapibase.h"
#include "Storm/h/stpl.h"

#include <cstddef>
#include <cstdint>

struct SDIRENT {
    char d_name[MAX_PATH];
};

struct SDIR {
    char name[MAX_PATH];
    HANDLE handle;
    WIN32_FIND_DATAA findData;
    SDIRENT dirent;
};

struct SOVERLAPPED {
    unsigned int Offset;
    SEvent* hEvent;
};

struct RECTF;

class SFile {
public:
    SFILE_TYPE m_type;
    void* m_fileptr;
    SArchive* m_archive;
    char* m_filename;
    char* m_actualname;
    unsigned int m_size;
    std::uint8_t* m_zbuffer;
    z_stream_s* m_zstream;
    unsigned int m_curOffset;
    SCritSect m_lock;
    void* m_hsfile;
    ZipFileFCB* m_zipFile;
    MD5 m_md5;
    int m_haveMD5;
    int m_closeAfterLoad;
    unsigned int m_asyncCount;

    explicit SFile(SFILE_TYPE type);
    ~SFile();

    static SDIR* __stdcall OpenDir(const char* name);
    static SDIRENT* __stdcall ReadDir(SDIR* dir);
    static void __stdcall CloseDir(SDIR* dir);
    static void __stdcall EnableHash(unsigned char enable);
    static void __stdcall RebuildHash();
    void DoAsyncRead();
    static unsigned int __stdcall ReadProc(void* parameter);
    static void InitializeReadThread();
    static void __fastcall QueueReadRequest(SFile* file, void* buffer, unsigned int bytesToRead, SOVERLAPPED* overlapped);
    BOOL __fastcall DoZRead(void* buffer, unsigned int bytesToRead, unsigned int* bytesRead);
    static unsigned int __stdcall Open(const char* filename, SFile** file);
    static int __stdcall OpenEx(SArchive* archive, const char* filename, unsigned int flags, SFile** file);
    static BOOL __stdcall Read(SFile* file, void* buffer, unsigned int bytesToRead, unsigned int* bytesRead, SOVERLAPPED* overlapped, void* asyncParam);
    static unsigned int __stdcall LoadFile(const char* filename, void** buffer, unsigned int* bytes, unsigned int extraBytes, SOVERLAPPED* overlapped);
    static int __stdcall Load(SArchive* archive, const char* filename, void** buffer, unsigned int* bytes, unsigned int extraBytes, unsigned int flags, SOVERLAPPED* overlapped);
    static int __stdcall Unload(void* buffer);
    static unsigned int __stdcall Close(SFile* file);
    static int __stdcall GetActualFileName(SFile* file, char* buffer, unsigned int bufferChars);
    static unsigned int __stdcall GetFileSize(SFile* file, unsigned int* fileSizeHigh);
    static int __stdcall GetBasePath(char* buffer, unsigned int bufferChars);
    static int __stdcall SetBasePath(const char* path);
    static int __stdcall SetDataPath(const char* path);
    static int __stdcall SetDataPathAlternate(const char* path);
    static int __stdcall FileExists(const char* filename);
    static DWORD __stdcall SetFilePointer(SFile* file, int distanceToMove, int* distanceToMoveHigh, DWORD moveMethod);
    static int __stdcall EnableDirectAccess(unsigned int access);
    static void __stdcall DisableSFileCheckDisk();
    static void __stdcall CreateOverlapped(SOVERLAPPED* overlapped);
    static void __stdcall DestroyOverlapped(SOVERLAPPED* overlapped);
    static void __stdcall ResetOverlapped(SOVERLAPPED* overlapped);
    static void __stdcall WaitOverlapped(SOVERLAPPED* overlapped);
    static BOOL __stdcall PollOverlapped(SOVERLAPPED* overlapped);
    static void Destroy();
    static int __stdcall OpenArchive(const char* archiveName, int priority, char flags, SArchive** handle);
    static int __stdcall CloseArchive(SArchive* archive);
    static int __stdcall GetMD5(SFile* file, MD5* sum);
    static int __stdcall List(SArchive* archive, int (__fastcall* callback)(char*, void*), void* parameter);
};

static_assert(sizeof(SFile) == 96, "SFile layout must remain PDB-compatible on x86.");

class __declspec(align(8)) Sha1 {
public:
    enum { DIGEST = 20, DATA_SIZE = 64, DATA_MASK = 63 };

    std::uint64_t m_size;
    std::uint32_t m_hash[5];
    std::uint8_t m_data[DATA_SIZE];

    void Initialize();
    void Append(const void* data, unsigned long size);
    void Finalize(std::uint8_t* hash);

private:
    static void __fastcall Pump(std::uint32_t* hash, const std::uint8_t* data);
};

static_assert(sizeof(Sha1) == 96, "Sha1 layout must remain PDB-compatible.");

class BigBuffer {
public:
    TSGrowableArray<unsigned int> m_data;
    unsigned int m_offset;

    BigBuffer();
    ~BigBuffer();
    BigBuffer& operator=(const BigBuffer& other);
    unsigned int& operator[](unsigned int index);
    const unsigned int& operator[](unsigned int index) const;
    void GrowToFit(unsigned int index);
    bool IsUsed(unsigned int index) const;
    void SetCount(unsigned int count);
    unsigned int Count() const;
    void Trim();
    void SetOffset(unsigned int offset);
    void Clear();
};

class BigStack {
public:
    enum { SIZE = 16 };
    BigBuffer m_buffer[SIZE];
    unsigned int m_used;

    BigStack();
    ~BigStack();
    BigBuffer* Alloc(unsigned int* count);
    void Free(unsigned int count);
    BigBuffer* MakeDistinct(BigBuffer* original, int required);
    void UnmakeDistinct(BigBuffer* original, BigBuffer* distinct);
};

class BigData {
public:
    BigBuffer m_primary;
    BigStack m_stack;
    TSGrowableArray_<unsigned char, 1396853063U, 102U> m_output;

    BigData();
    ~BigData();
    BigData* Primary();
    const BigData* Primary() const;
    BigStack* Stack();
    TSGrowableArray_<unsigned char, 1396853063U, 102U>* Output() const;
};

#if defined(_M_IX86)
static_assert(sizeof(BigBuffer) == 24, "BigBuffer x86 PDB layout mismatch.");
static_assert(sizeof(BigStack) == 388, "BigStack x86 PDB layout mismatch.");
static_assert(sizeof(BigData) == 432, "BigData x86 PDB layout mismatch.");
#endif

void __stdcall SBigAdd(BigData* a, const BigData* b, const BigData* c);
void __stdcall SBigAnd(BigData* a, const BigData* b, const BigData* c);
void __stdcall SBigBitLen(BigData* a, unsigned int* bits);
int __stdcall SBigCompare(const BigData* b, const BigData* c);
void __stdcall SBigCopy(BigData* a, const BigData* b);
void __stdcall SBigDec(BigData* a, const BigData* b);
void __stdcall SBigDel(BigData* number);
void __stdcall SBigDiv(BigData* a, const BigData* b, const BigData* c);
void __stdcall SBigFindPrime(BigData* a, unsigned int bits, const BigData* lower, const BigData* upper);
void __stdcall SBigFromBinary(BigData* number, const void* data, unsigned int bytes);
void __stdcall SBigFromStr(BigData* number, const char* string);
void __stdcall SBigFromStream(BigData* number, const void* data, unsigned int maxBytes, unsigned int* bytes);
void __stdcall SBigFromUnsigned(BigData* number, unsigned int value);
void __stdcall SBigGcd(BigData* a, const BigData* b, const BigData* c);
void __stdcall SBigInc(BigData* a, const BigData* b);
void __stdcall SBigInvMod(BigData* a, const BigData* b, const BigData* modulus);
BOOL __stdcall SBigIsEven(const BigData* a);
int __stdcall SBigIsOdd(const BigData* a);
unsigned int __stdcall SBigIsOne(const BigData* a);
int __stdcall SBigIsPrime(const BigData* a);
BOOL __stdcall SBigIsZero(const BigData* a);
void __stdcall SBigMod(BigData* a, const BigData* b, const BigData* modulus);
void __stdcall SBigMul(BigData* a, const BigData* b, const BigData* c);
void __stdcall SBigMulMod(BigData* a, const BigData* b, const BigData* c, const BigData* modulus);
void __stdcall SBigNew(BigData** number);
void __stdcall SBigNot(BigData* a, const BigData* b);
void __stdcall SBigOr(BigData* a, const BigData* b, const BigData* c);
void __stdcall SBigPow(BigData* a, const BigData* b, unsigned int exponent);
void __stdcall SBigPowMod(BigData* a, const BigData* b, const BigData* exponent, const BigData* modulus);
void __stdcall SBigRand(BigData* a, const BigData* limit, BigData* seed);
void __stdcall SBigSet2Exp(BigData* a, unsigned int exponent);
void __stdcall SBigSetOne(BigData* a);
void __stdcall SBigSetZero(BigData* a);
void __stdcall SBigShl(BigData* a, const BigData* b, unsigned int bits);
void __stdcall SBigShr(BigData* a, const BigData* b, unsigned int bits);
void __stdcall SBigSquare(BigData* a, const BigData* b);
void __stdcall SBigSub(BigData* a, const BigData* b, const BigData* c);
void __stdcall SBigToBinaryArray(const BigData* number, TSGrowableArray<unsigned char>* array, int append);
void __stdcall SBigToBinaryBuffer(const BigData* number, unsigned char* data, unsigned int maxBytes, unsigned int* bytes);
void __stdcall SBigToBinaryPtr(const BigData* number, const void** data, unsigned int* bytes);
void __stdcall SBigToStrArray(const BigData* number, TSGrowableArray<char>* array, int append);
void __stdcall SBigToStrBuffer(const BigData* number, char* string, unsigned int chars);
void __stdcall SBigToStrPtr(const BigData* number, const char** string);
void __stdcall SBigToStreamArray(const BigData* number, TSGrowableArray<unsigned char>* array, int append);
void __stdcall SBigToStreamBuffer(const BigData* number, unsigned char* data, unsigned int maxBytes, unsigned int* bytes);
void __stdcall SBigToStreamPtr(const BigData* number, const void** data, unsigned int* bytes);
void __stdcall SBigToUnsigned(const BigData* number, unsigned int* value);
void __stdcall SBigXor(BigData* a, const BigData* b, const BigData* c);

struct StormOptions {
    unsigned int serrleaksilentwarning;
    unsigned int wavechunksize;
    unsigned int smemleaksilentwarning;
    unsigned int alignstreamingwavedata;
    unsigned int echotooutputdebugstring;
    unsigned int serrsuppresslogs;
    unsigned int crcenabled;
    unsigned int orderedprintfenabled;
};

extern StormOptions g_opt;

struct I64ToStringRequest {
    std::int64_t value;
    char* destination;
    unsigned int destinationSize;
};

unsigned long __fastcall CrcBuffer(const void* buffer, unsigned long len, unsigned long* crc, unsigned long stage);
unsigned long __stdcall SCrcBuffer(const void* buffer, unsigned long len, unsigned long* crc, unsigned long stage);

DWORD __stdcall SErrGetLastError();
void __stdcall SErrSetLastError(DWORD errorcode);
void SErrSetLogCallback(void* callback);
void SErrDestroy();

const char* __fastcall SStrChr(const char* string, char ch);
char* __fastcall SStrChr(char* string, char ch);
const char* __fastcall SStrChrR(const char* string, char ch);
char* __fastcall SStrChrR(char* string, char ch);
int __stdcall SStrCmp(const char* string1, const char* string2, std::size_t maxchars);
int __stdcall SStrCmpI(const char* string1, const char* string2, std::size_t maxchars);
unsigned long __stdcall SStrLen(const char* string);
unsigned long __stdcall SStrLen(const wchar_t* string);
void __stdcall SStrUpper(char* string);
void __stdcall SStrLower(char* string);

int __stdcall SUniConvertUTF16to8Len(const std::uint16_t* src, unsigned long srcMaxChars, unsigned long* srcChars);
int __stdcall SUniConvertUTF16to8(char* dst, unsigned long dstMaxChars, const std::uint16_t* src, unsigned long srcMaxChars, unsigned long* dstChars, unsigned long* srcChars);
int __stdcall SUniConvertUTF8to16Len(const char* src, unsigned long srcMaxChars, unsigned long* srcChars);

char* __cdecl vsnoprintf(char* out, int outSize, const char* format, char* argumentList);
char* __cdecl vsoprintf(char* out, const char* format, char* argumentList);
char* snoprintf(char* out, int outSize, const char* format, ...);
char* soprintf(char* out, const char* format, ...);
int __fastcall ParseFormatSpecifier(const char** size, unsigned long* argumentSize, int* orderingPtr);
void RemoveOrderingFromFormatSpecifier(char* format);
void FixUpLongLongFormatSpecifier(char* format);

void IncrementAllocCount();
void IncrementFreeCount();
int __stdcall StormGetOption(int optname, unsigned int optval, unsigned int* optlen);
int __stdcall StormSetOption(int optname, int* optval, unsigned int optlen);

int __stdcall SRegDeleteValue(const char* keyname, const char* valuename, unsigned int flags);
int __stdcall SRegDeleteKey(const char* keyname, unsigned int flags);
int __stdcall SRegGetBaseKey(unsigned int flags, char* buffer, unsigned int bufferChars);
int __stdcall SRegLoadData(const char* keyname, const char* valuename, unsigned int flags, void* buffer, unsigned int bufferSize, unsigned int* bytesRead);
int __stdcall SRegLoadString(const char* keyname, const char* valuename, unsigned int flags, char* buffer, unsigned int bufferSize);
int __stdcall SRegLoadValue(const char* keyname, const char* valuename, unsigned int flags, unsigned int* value);
int __stdcall SRegSaveData(const char* keyname, const char* valuename, unsigned int flags, const void* data, unsigned int dataBytes);
int __stdcall SRegSaveString(const char* keyname, const char* valuename, unsigned int flags, const char* string);
int __stdcall SRegSaveValue(const char* keyname, const char* valuename, unsigned int flags, unsigned int value);
int __stdcall SRegEnumKey(const char* baseKeyName, unsigned int flags, DWORD subKeyIndex, char* keyNameBuffer, unsigned int bufferChars);
int __stdcall SRegGetNumSubKeys(const char* keyName, unsigned int flags, unsigned int* numSubKeys);

void __stdcall SRgnClear(HLOCKEDRGN__* handle);
void __stdcall SRgnCombineRectf(HLOCKEDRGN__* handle, const RECTF* rect, void* param, int combineMode);
void __stdcall SRgnCombineRecti(HSRGN__* handle, const RECT* rect, void* param, int combineMode);
void __stdcall SRgnCreate(HSRGN__** handle, unsigned int reserved);
void __stdcall SRgnDelete(HSRGN__* handle);
void __stdcall SRgnDestroy();
void __stdcall SRgnDuplicate(HLOCKEDRGN__* originalHandle, HSRGN__** handle, unsigned int reserved);
void __stdcall SRgnGetBoundingRectf(HLOCKEDRGN__* handle, RECTF* rect);
void __stdcall SRgnGetBoundingRecti(HSRGN__* handle, RECT* rect);
void __stdcall SRgnGetRectParamsf(HSRGN__* handle, const RECTF* rect, unsigned int* numParams, void** buffer);
void __stdcall SRgnGetRectParamsi(HSRGN__* handle, const RECT* rect, unsigned int* numParams, void** buffer);
void __stdcall SRgnGetRectsf(HLOCKEDRGN__* handle, unsigned int* numRects, RECTF* buffer);
void __stdcall SRgnGetRectsi(HSRGN__* handle, unsigned int* numRects, RECT* buffer);
int __stdcall SRgnIsPointInRegionf(HLOCKEDRGN__* handle, float x, float y);
int __stdcall SRgnIsPointInRegioni(HSRGN__* handle, int x, int y);
int __stdcall SRgnIsRectInRegionf(HSRGN__* handle, const RECTF* rect);
int __stdcall SRgnIsRectInRegioni(HSRGN__* handle, const RECT* rect);
void __stdcall SRgnOffsetf(HLOCKEDRGN__* handle, float xOffset, float yOffset);
void __stdcall SRgnOffseti(HSRGN__* handle, int xOffset, int yOffset);

void StormRtlInitialize();
void StormRtlDestroy();
void __stdcall SLogInitialize();
void __stdcall SLogDestroy();
int __stdcall SMemInitialize();
int __stdcall SMemDestroy();
int __stdcall SMemDumpStateEx(char* destination);
int __stdcall SMemMarkAllHeapsEx(char* marker);

int ISMemGetAllocator(void** destination);
int ISMemGenerateReport(char* destination);
int ISMemMarkAllHeaps(char* marker);
int ISStrI64ToString(I64ToStringRequest* request);
int StormCallService(int selector, ...);

HMODULE __stdcall StormGetInstance();
void SOutputDebugString(const char* format, ...);

long __fastcall SInterlockedExchangePointer(long* exchange, long value);
void* __fastcall SInterlockedCompareExchangePointer(void** exchange, void* value, void* comparand);
long __fastcall SInterlockedIncrement(long* value);
long __fastcall SInterlockedDecrement(long* value);
long __fastcall SInterlockedExchangeAdd(long* value, long delta);
long __fastcall SInterlockedExchangeSub(long* value, long delta);
long __fastcall SInterlockedExchange(long* value, long replacement);
long __fastcall SInterlockedCompareExchange(long* value, long replacement, long comparand);

void __fastcall SServerInitialize();
void __fastcall SServerDestroy();
int __fastcall STryEnterCriticalSection(void* opaqueData);
