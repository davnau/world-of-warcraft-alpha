#include "WoW/Source/Console/consolecommand.h"

#include "ENGINE/Source/Event/event.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace {
std::vector<std::string> g_history;
std::size_t g_historyCursor = 0;
std::vector<CONSOLECOMMAND> g_defaults;
std::ofstream g_execFile;
std::string g_wtfPath = "WTF";
CONSOLECOMMAND g_parsed;
std::string g_parsedName;
}

float INFINITY_296 = 0.0F;
float TWO_PI_291 = 0.0F;
float OO_TWO_PI_292 = 0.0F;

int __fastcall CreateWTFFilePath(char* path,unsigned capacity){if(!path||!capacity)return 0;std::snprintf(path,capacity,"%s\\%s",g_wtfPath.c_str(),"config.wtf");return 1;}
int ValidateFileName(const char* fileName){return fileName&&*fileName&&!std::strstr(fileName,"..")&&!std::strchr(fileName,'\\')&&!std::strchr(fileName,'/');}

int __fastcall ConsoleCommand_RunExec(const char*,const char* commandLine){if(!commandLine)return 0;std::ifstream file(commandLine);std::string line;while(std::getline(file,line)){auto tokens=line.find_first_of(" \t");ConsoleCommandExecute(tokens==std::string::npos?line.c_str():line.substr(0,tokens).c_str(),tokens==std::string::npos?nullptr:line.c_str()+tokens+1);}return 1;}

CONSOLECOMMAND* __fastcall ParseCommand(const char** command,std::uint32_t* flags,const char** arguments){if(!command||!*command)return nullptr;const char* begin=*command;while(*begin&&std::isspace(static_cast<unsigned char>(*begin)))++begin;const char* end=begin;while(*end&&!std::isspace(static_cast<unsigned char>(*end)))++end;g_parsedName.assign(begin,end);g_parsed={g_parsedName.c_str(),CLIENT_COMMAND_CATEGORY_DEBUG,nullptr,nullptr};if(flags)*flags=0;if(arguments)*arguments=*end?end+1:end;*command=end;return &g_parsed;}
unsigned __fastcall ConsoleCommandHistoryDepth(){return static_cast<unsigned>(g_history.size());}
char* __fastcall ConsoleCommandHistory(char direction){static std::string current;if(g_history.empty())return nullptr;if(direction<0){if(g_historyCursor>0)--g_historyCursor;}else if(direction>0&&g_historyCursor+1<g_history.size())++g_historyCursor;current=g_history[g_historyCursor];return current.data();}
int __fastcall ConsoleCommandComplete(const char**,const char**,int){return 0;}
void ConsoleCommandWriteHelp(const char* command){ConsolePrintf("Help for %s",command?command:"console");}
int __fastcall ConsoleCommand_Help(const char*,char* commandLine){ConsoleCommandWriteHelp(commandLine);return 1;}
void ConsoleCommandRegisterDefault(ClientCommandFunction function){g_defaults.push_back({nullptr,CLIENT_COMMAND_CATEGORY_DEBUG,function,nullptr});}
void ConsoleCommandInitialize(){g_history.clear();g_historyCursor=0;}
int ConsoleCommand_Quit(){EventPostClose();return 1;}
int ConsoleCommand_Ver(const char* command){ConsolePrintf("%s: recovered WoW client",command?command:"version");return 1;}
int ConsoleCommand_CreateExec(const char*,const char* fileName){if(!ValidateFileName(fileName))return 0;g_execFile.open(fileName,std::ios::out|std::ios::trunc);return g_execFile.good();}
int __fastcall ConsoleCommand_AppendExec(const char*,const char* fileName){if(!ValidateFileName(fileName))return 0;g_execFile.open(fileName,std::ios::out|std::ios::app);return g_execFile.good();}
int ConsoleCommand_CloseExec(){if(g_execFile.is_open())g_execFile.close();return 1;}
int __fastcall ConsoleCommand_TypeExec(const char*,const char* fileName){if(!ValidateFileName(fileName))return 0;std::ifstream in(fileName);std::string line;while(std::getline(in,line))ConsolePrintf("%s",line.c_str());return 1;}
int ConsoleCommand_DirWtf(){ConsolePrintf("%s",g_wtfPath.c_str());return 1;}
void ConsoleCommandDestroy(){ConsoleCommand_CloseExec();g_defaults.clear();g_history.clear();}

void _E4_293(){TWO_PI_291=6.28318530717958647692F;} int _E5_293(){_E4_293();return 0;} void _E7_293(){OO_TWO_PI_292=1.0F/TWO_PI_291;} int _E8_293(){_E7_293();return 0;}
int _E1_296(){INFINITY_296=std::numeric_limits<float>::infinity();return 2139095040;} int _E2_296(){return _E1_296();}
void _E13_11(){} int _E15_13(){return 0;} int _E16_23(){_E13_11();return _E15_13();} void _E14_16(){}
