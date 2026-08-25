#include "ENGINE/Source/Base/cmdline.h"

#include <cstdlib>
#include <string>
#include <unordered_map>
#include <windows.h>

namespace { std::unordered_map<unsigned,std::string> s_values; unsigned Hash(const std::string& s) { unsigned h=0; for(char c:s) h=(h*33)^static_cast<unsigned char>(c|0x20); return h; } }
int CmdLineProcess(const char* line) {
    s_values.clear(); if (!line) return 0; std::string input(line), token;
    for (std::size_t i=0;i<=input.size();) { while(i<input.size() && input[i]==' ') ++i; if(i>=input.size()) break; bool q=input[i]=='\"'; if(q) ++i; std::size_t b=i; while(i<input.size() && (q?input[i]!='\"':input[i]!=' ')) ++i; token=input.substr(b,i-b); if(q&&i<input.size()) ++i;
        if (!token.empty() && (token[0]=='-' || token[0]=='/')) token.erase(0,1); auto eq=token.find('='); std::string key=token.substr(0,eq); std::string val=eq==std::string::npos?"1":token.substr(eq+1); s_values[Hash(key)]=val;
    } return static_cast<int>(s_values.size());
}
int CmdLineProcess() { return CmdLineProcess(GetCommandLineA()); }
int CmdLineGetBool(unsigned id) { auto i=s_values.find(id); if(i==s_values.end()) return 0; return i->second.empty() || i->second=="1" || _stricmp(i->second.c_str(),"true")==0; }
char* CmdLineGetString(unsigned id) { auto i=s_values.find(id); return i==s_values.end()?nullptr:i->second.data(); }
unsigned CmdLineGetUint(unsigned id) { char* s=CmdLineGetString(id); return s?static_cast<unsigned>(std::strtoul(s,nullptr,0)):0; }
