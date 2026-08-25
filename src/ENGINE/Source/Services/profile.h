#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

using unreal = double;

namespace ProfileInternal {
struct KEYVALUE { std::vector<std::string> values; };
struct SECTION { std::unordered_map<std::string, KEYVALUE> keyTable; };
struct PROFILE { std::unordered_map<std::string, SECTION> sectionTable; };
struct STRINGBLOCK {
    unsigned m_refCount = 1, m_dataSize = 0, m_dataUsed = 0;
    static STRINGBLOCK* AllocBlock(unsigned);
    static char* AllocString(std::vector<STRINGBLOCK*>&, const char*, int inSitu = 0);
    static void FreeString(std::vector<STRINGBLOCK*>&, char*);
};

PROFILE* IReadFile(const char* path);
int IReadBuffer(PROFILE*, const void* buffer, unsigned bufferBytes);
void TokenizeStringValues(PROFILE*, const char* sectionName, const char* keyName, char* value);
int ISetValue(PROFILE*, const char* sectionName, const char* keyName, const char* value, int clear, int inSitu = 0);
int IWriteFile(PROFILE*, const char* path);
void WriteFileBuffer(PROFILE*, std::string& buffer);
void WriteSection(std::string&, const std::string&, const SECTION&);
void WriteLine(std::string&, const char* format, ...);
void WriteKey(std::string&, const std::string&, const KEYVALUE&);
const char* IGetValue(PROFILE*, const char* sectionName, const char* keyName, unsigned index);
KEYVALUE* GetKeyValue(PROFILE*, const char* sectionName, const char* keyName);
int PrfStrToInt(const char*);
unsigned IGetNumValues(PROFILE*, const char* sectionName, const char* keyName);
}

using HPROFILE = ProfileInternal::PROFILE*;
using PROFILE_KEY_CALLBACK = int (__fastcall*)(const char*, const char*, void*);
using PROFILE_SECTION_CALLBACK = int (__fastcall*)(const char*, void*);

HPROFILE ProfileCreate();
int ProfileReadFile(HPROFILE, const char* path);
int ProfileReadBuffer(HPROFILE, const void* buffer, unsigned bufferBytes);
int ProfileWriteFile(HPROFILE, const char* path);
int ProfileAddValue(HPROFILE,const char*,const char*,bool);
int ProfileAddValue(HPROFILE,const char*,const char*,int);
int ProfileAddValue(HPROFILE,const char*,const char*,std::int64_t);
int ProfileAddValue(HPROFILE,const char*,const char*,float);
int ProfileAddValue(HPROFILE,const char*,const char*,const unreal*);
int ProfileAddValue(HPROFILE,const char*,const char*,const char*);
int ProfileSetValue(HPROFILE,const char*,const char*,bool);
int ProfileSetValue(HPROFILE,const char*,const char*,int);
int ProfileSetValue(HPROFILE,const char*,const char*,std::int64_t);
int ProfileSetValue(HPROFILE,const char*,const char*,float);
int ProfileSetValue(HPROFILE,const char*,const char*,const unreal*);
int ProfileSetValue(HPROFILE,const char*,const char*,const char*);
int ProfileGetValue(HPROFILE,const char*,const char*,bool*,unsigned=0);
int ProfileGetValue(HPROFILE,const char*,const char*,int*,unsigned=0);
int ProfileGetValue(HPROFILE,const char*,const char*,std::int64_t*,unsigned=0);
int ProfileGetValue(HPROFILE,const char*,const char*,float*,unsigned=0);
int ProfileGetValue(HPROFILE,const char*,const char*,unreal*,unsigned=0);
int ProfileGetValue(HPROFILE,const char*,const char*,char*,unsigned,unsigned=0);
const char* ProfileGetValueNoCopy(HPROFILE,const char*,const char*,unsigned=0);
unsigned ProfileGetNumValues(HPROFILE,const char*,const char*);
int ProfileGetValueIndex(HPROFILE,const char*,const char*,const char*);
void ProfileEnumKeys(HPROFILE,const char*,PROFILE_KEY_CALLBACK,void*);
void ProfileEnumSections(HPROFILE,PROFILE_SECTION_CALLBACK,void*);
int ProfileSectionExists(HPROFILE,const char*);
void ProfileClose(HPROFILE);
