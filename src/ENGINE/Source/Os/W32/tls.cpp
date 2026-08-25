#include "ENGINE/Source/Os/W32/tls.h"
DWORD __fastcall OsTlsAlloc(){return TlsAlloc();}void __fastcall OsTlsFree(DWORD i){TlsFree(i);}LPVOID __fastcall OsTlsGetValue(DWORD i){return TlsGetValue(i);}BOOL __fastcall OsTlsSetValue(DWORD i,void*v){return TlsSetValue(i,v);}
