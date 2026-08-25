#include "ENGINE/Source/Os/W32/ossystem.h"
#include <intrin.h>
#include <shellapi.h>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <vector>
namespace { int s_sleepInBackground=1;unsigned s_backgroundSleepMs=10;std::mutex s_objectLock;std::vector<HANDLE>s_objects; }
int __cdecl IOsGetProcessorFeatures(unsigned char vendor[16],unsigned int*standard,unsigned int*extended){int regs[4]{};__cpuid(regs,0);if(vendor){std::memcpy(vendor,regs+1,4);std::memcpy(vendor+4,regs+3,4);std::memcpy(vendor+8,regs+2,4);vendor[12]=0;}__cpuid(regs,1);if(standard)*standard=static_cast<unsigned>(regs[3]);__cpuid(regs,0x80000000);if(extended){if(static_cast<unsigned>(regs[0])>=0x80000001U){__cpuid(regs,0x80000001);*extended=static_cast<unsigned>(regs[3]);}else *extended=0;}return 1;}
unsigned OsGetProcessorFeaturesEx(){unsigned stdf=0,extf=0;IOsGetProcessorFeatures(nullptr,&stdf,&extf);unsigned result=0;if(stdf&(1U<<23))result|=OS_PROCESSOR_MMX;if(stdf&(1U<<25))result|=OS_PROCESSOR_SSE;if(stdf&(1U<<26))result|=OS_PROCESSOR_SSE2;if(extf&(1U<<31))result|=OS_PROCESSOR_3DNOW;return result;}unsigned __fastcall OsGetProcessorFeatures(int){static unsigned value=OsGetProcessorFeaturesEx();return value;}unsigned __fastcall OsGetProcessorCount(){SYSTEM_INFO info{};GetSystemInfo(&info);return info.dwNumberOfProcessors;}void OsSleep(DWORD ms){Sleep(ms);}int __fastcall OsSleepInBackground(){return s_sleepInBackground;}void OsSetSleepInBackground(int e){s_sleepInBackground=e!=0;}unsigned __fastcall OsGetBackgroundSleepMs(){return s_backgroundSleepMs;}void OsSetBackgroundSleepMs(unsigned m){s_backgroundSleepMs=m;}void __cdecl OsPause(){Sleep(0);}
int __fastcall OsGetVersion(){OSVERSIONINFOEXW info{sizeof(info)};
#pragma warning(push)
#pragma warning(disable:4996)
GetVersionExW(reinterpret_cast<OSVERSIONINFOW*>(&info));
#pragma warning(pop)
return (int(info.dwMajorVersion)<<16)|int(info.dwMinorVersion);}void __fastcall OsGetVersionString(char*b,unsigned n){if(!b||!n)return;std::snprintf(b,n,"Windows %u.%u",unsigned(OsGetVersion()>>16),unsigned(OsGetVersion()&0xFFFF));}BOOL __fastcall OsGetComputerName(char*b,unsigned*l){return GetComputerNameA(b,reinterpret_cast<DWORD*>(l));}BOOL __fastcall OsGetUserName(char*b,unsigned*l){return GetUserNameA(b,reinterpret_cast<DWORD*>(l));}unsigned __fastcall OsGetPhysicalMemory(){MEMORYSTATUSEX m{sizeof(m)};return GlobalMemoryStatusEx(&m)?unsigned(m.ullTotalPhys/(1024*1024)):0;}
void OsSystemObjectCreate(const char*n){if(!n)return;HANDLE h=CreateMutexA(nullptr,FALSE,n);if(h){std::lock_guard lock(s_objectLock);s_objects.push_back(h);}}HANDLE OsSystemObjectExists(const char*n){return n?OpenMutexA(SYNCHRONIZE,FALSE,n):nullptr;}BOOL OsLaunchURL(const char*u){return u&&reinterpret_cast<INT_PTR>(ShellExecuteA(nullptr,"open",u,nullptr,nullptr,SW_SHOWNORMAL))>32;}void OsClearFP(){_clearfp();}int __fastcall OsGetCurrentThreadPriority(){return GetThreadPriority(GetCurrentThread());}void OsSetCurrentThreadPriority(int p){SetThreadPriority(GetCurrentThread(),p);}
