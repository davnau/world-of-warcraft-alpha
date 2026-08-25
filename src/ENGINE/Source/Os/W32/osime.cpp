#include "ENGINE/Source/Os/W32/osime.h"
#include "ENGINE/Source/Os/W32/input.h"
#include <windows.h>
#include <imm.h>
#include <vector>
namespace{bool s_enabled=true;int Composition(DWORD type,char*b,unsigned n){HWND w=OsInputGetWindow();HIMC c=w?ImmGetContext(w):nullptr;if(!c)return 0;LONG bytes=ImmGetCompositionStringA(c,type,nullptr,0);if(bytes>0&&b&&n){LONG take=bytes<LONG(n-1)?bytes:LONG(n-1);ImmGetCompositionStringA(c,type,b,take);b[take]=0;}ImmReleaseContext(w,c);return int(bytes);}}
OS_IME_LANGUAGEMODE OsIMEGetLanguageMode(){LANGID l=LOWORD(GetKeyboardLayout(0));switch(PRIMARYLANGID(l)){case LANG_JAPANESE:return OS_IME_MODE_JAPANESE;case LANG_KOREAN:return OS_IME_MODE_KOREAN;case LANG_CHINESE:return OS_IME_MODE_CHINESE;default:return OS_IME_MODE_ROMAN;}}int OsIMEGetCompositionString(char*b,unsigned n){return Composition(GCS_COMPSTR,b,n);}int OsIMEGetCompositionResult(char*b,unsigned n){return Composition(GCS_RESULTSTR,b,n);}
int OsIMEGetClauseInfo(unsigned*clauseLeft,unsigned*clauseRight,unsigned*cursorPos){
 if(clauseLeft)*clauseLeft=0;if(clauseRight)*clauseRight=0;if(cursorPos)*cursorPos=0;
 HWND window=OsInputGetWindow();HIMC context=window?ImmGetContext(window):nullptr;if(!context)return 0;
 LONG cursor=ImmGetCompositionStringA(context,GCS_CURSORPOS,nullptr,0);LONG clauseBytes=ImmGetCompositionStringA(context,GCS_COMPCLAUSE,nullptr,0);
 if(cursor<0||clauseBytes<=0){ImmReleaseContext(window,context);return 0;}
 std::vector<DWORD>clauses((static_cast<unsigned>(clauseBytes)+sizeof(DWORD)-1)/sizeof(DWORD));
 LONG gotClauses=ImmGetCompositionStringA(context,GCS_COMPCLAUSE,clauses.data(),clauseBytes);
 if(gotClauses<=0||gotClauses==IMM_ERROR_GENERAL||gotClauses==IMM_ERROR_NODATA){ImmReleaseContext(window,context);return 0;}
 const unsigned clauseCount=static_cast<unsigned>(gotClauses)/sizeof(DWORD);if(clauseCount<2){ImmReleaseContext(window,context);return 0;}
 LONG stringBytes=ImmGetCompositionStringA(context,GCS_COMPSTR,nullptr,0);std::vector<char>string(stringBytes>0?static_cast<unsigned>(stringBytes):1U);
 if(stringBytes>0)ImmGetCompositionStringA(context,GCS_COMPSTR,string.data(),stringBytes);
 LONG attribBytes=ImmGetCompositionStringA(context,GCS_COMPATTR,nullptr,0);std::vector<BYTE>attrib(attribBytes>0?static_cast<unsigned>(attribBytes):0U);
 if(attribBytes>0)ImmGetCompositionStringA(context,GCS_COMPATTR,attrib.data(),attribBytes);
 ImmReleaseContext(window,context);
 unsigned current=0;for(unsigned i=0;i+1<clauseCount;++i)if(static_cast<DWORD>(cursor)>=clauses[i]&&static_cast<DWORD>(cursor)<clauses[i+1])current=i;
 for(unsigned i=0;i+1<clauseCount;++i)if(clauses[i]<attrib.size()&&attrib[clauses[i]]==ATTR_INPUT)current=i;
 const DWORD start=clauses[current],end=clauses[current+1];const UINT codePage=OsInputGetCodePage();
 auto wideLength=[&](DWORD bytes){bytes=(std::min)(bytes,static_cast<DWORD>(string.size()));return bytes?MultiByteToWideChar(codePage,MB_PRECOMPOSED,string.data(),bytes,nullptr,0):0;};
 if(clauseLeft)*clauseLeft=wideLength(start);if(clauseRight)*clauseRight=wideLength(start)+wideLength(end-start);if(cursorPos)*cursorPos=wideLength(static_cast<DWORD>(cursor));return 1;
}
int OsIMEGetCandidates(unsigned*listMask,unsigned*pageSize,unsigned*count,unsigned*selection,TSGrowableArray<OsIMECandidate>*out){
 if(out)out->SetCount(0);HWND window=OsInputGetWindow();HIMC context=window?ImmGetContext(window):nullptr;if(!context)return 0;
 const unsigned mask=listMask?*listMask:0;if(!mask){ImmReleaseContext(window,context);return 0;}DWORD listIndex=0;while(listIndex<32&&!(mask&(1U<<listIndex)))++listIndex;
 DWORD bytes=ImmGetCandidateListA(context,listIndex,nullptr,0);if(!bytes){ImmReleaseContext(window,context);return 0;}
 std::vector<unsigned char>storage(bytes);auto*list=reinterpret_cast<CANDIDATELIST*>(storage.data());if(!ImmGetCandidateListA(context,listIndex,list,bytes)||!list->dwPageSize){if(!list->dwPageSize)ImmNotifyIME(context,NI_CLOSECANDIDATE,listIndex,0);ImmReleaseContext(window,context);return 0;}
 DWORD desired=list->dwPageStart;if(list->dwSelection<desired){while(desired>list->dwPageSize&&list->dwSelection<desired)desired-=list->dwPageSize;if(list->dwSelection<desired)desired=0;}else while(list->dwSelection>=desired+list->dwPageSize)desired+=list->dwPageSize;
 if(desired!=list->dwPageStart){ImmNotifyIME(context,NI_CHANGECANDIDATELIST,listIndex,desired);ImmReleaseContext(window,context);return 0;}
 if(pageSize)*pageSize=list->dwPageSize;if(count)*count=list->dwCount;if(selection)*selection=list->dwSelection;
 if(out){out->SetCount(list->dwPageSize);const UINT codePage=OsInputGetCodePage();for(DWORD i=0;i<list->dwPageSize;++i){auto&candidate=(*out)[i];candidate.candidate[0]=0;const DWORD item=list->dwPageStart+i;if(item>=list->dwCount)continue;const char*source=reinterpret_cast<const char*>(list)+list->dwOffset[item];int wideCount=MultiByteToWideChar(codePage,MB_PRECOMPOSED,source,-1,nullptr,0);if(wideCount<=0)continue;std::vector<wchar_t>wide(static_cast<unsigned>(wideCount));MultiByteToWideChar(codePage,MB_PRECOMPOSED,source,-1,wide.data(),wideCount);WideCharToMultiByte(CP_UTF8,0,wide.data(),-1,candidate.candidate,sizeof(candidate.candidate),nullptr,nullptr);candidate.candidate[sizeof(candidate.candidate)-1]=0;}}
 ImmReleaseContext(window,context);return 1;
}void OsIMEEnable(int e){s_enabled=e!=0;HWND w=OsInputGetWindow();if(w)ImmAssociateContextEx(w,nullptr,e?IACE_DEFAULT:IACE_IGNORENOCONTEXT);}void OsIMEInitialize(){s_enabled=true;}void OsIMEDestroy(){s_enabled=false;}
