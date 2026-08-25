#include "WoW/Source/Console/consolevar.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::vector<std::unique_ptr<CVar>> g_cvars;
std::string g_filename;
char* Duplicate(const char* value) { if(!value)return nullptr; const auto n=std::strlen(value)+1;auto*p=static_cast<char*>(std::malloc(n));if(p)std::memcpy(p,value,n);return p; }
void Replace(char*& destination,const char* value){std::free(destination);destination=Duplicate(value?value:"");}
}

CVar* s_errorsCvar = nullptr;
float INFINITY_295 = 0.0F;
float TWO_PI_290 = 0.0F;
float OO_TWO_PI_291 = 0.0F;

CVar::CVar() = default;
CVar::~CVar(){std::free(m_stringValue);std::free(m_defaultValue);std::free(m_resetValue);std::free(m_latchedValue);}
void CVar::Initialize(const char* fileName){
    g_filename=fileName?fileName:"";
    s_CreatePathDirectories("WTF");
    CVarLoadFile();
}
void CVar::Destroy(){CVarSaveFile();g_cvars.clear();s_errorsCvar=nullptr;}

CVar* CVar::Register(const char* help,const char* name,unsigned flags,const char* value,CVarCallback callback,unsigned category,unsigned char setCommand,void* arg){
    const char* key=help?help:name; if(!key)return nullptr;
    if(auto* existing=Lookup(key)){existing->m_flags|=flags|1U;existing->m_callback=callback;existing->m_arg=arg;existing->m_category=category;if(callback)callback(existing,existing->StringValue(),existing->StringValue(),arg);existing->Set(value?value:"",0,existing->m_resetValue==nullptr,existing->m_defaultValue==nullptr);return existing;}
    auto entry=std::make_unique<CVar>();std::strncpy(entry->m_name,key,sizeof(entry->m_name)-1);entry->m_hashval=0;for(const unsigned char c:std::string(key))entry->m_hashval=entry->m_hashval*33u+static_cast<unsigned>(std::tolower(c));entry->m_callback=callback;entry->m_arg=arg;entry->m_category=category;entry->m_flags=flags|1U;CVar* result=entry.get();g_cvars.push_back(std::move(entry));result->Set(value?value:"",1,setCommand?1:0,setCommand?0:1);return result;
}

CVar* CVar::Lookup(const char* name){if(!name)return nullptr;for(auto& item:g_cvars)if(SStrCmpI(item->m_name,name,0x7FFFFFFFu)==0)return item.get();return nullptr;}
unsigned char CVar::Set(const char* value,unsigned char setValue,unsigned char setReset,unsigned char setDefault){if(!value)return 0;if(setValue&&m_callback&&m_callback(this,m_stringValue?m_stringValue:"",value,m_arg)==0)return 1;if(setValue)++m_modified;if((m_flags&2)&&setValue){Replace(m_latchedValue,value);return 1;}InternalSet(value,setValue,setReset,setDefault);return 1;}
void CVar::Reset(){InternalSet(m_resetValue?m_resetValue:(m_defaultValue?m_defaultValue:""),1,0,0);}
void CVar::Default(){InternalSet(m_defaultValue?m_defaultValue:(m_resetValue?m_resetValue:""),1,0,0);}
unsigned char CVar::Update(){if(!(m_flags&2)||!m_latchedValue)return 0;InternalSet(m_latchedValue,1,0,0);std::free(m_latchedValue);m_latchedValue=nullptr;return 1;}
void CVar::InternalSet(const char* value,unsigned char setValue,unsigned char setReset,unsigned char setDefault){if(setValue){Replace(m_stringValue,value);m_intValue=std::strtol(value,nullptr,0);m_floatValue=std::strtof(value,nullptr);}if(setReset&&!m_resetValue)m_resetValue=Duplicate(value);if(setDefault&&!m_defaultValue)m_defaultValue=Duplicate(value);}

int __fastcall SetCommandHandler(const char*,char* commandLine){auto t=commandLine?commandLine:"";while(*t&&std::isspace(static_cast<unsigned char>(*t)))++t;auto* split=const_cast<char*>(std::strchr(t,' '));if(!split)return 1;*split++=0;while(*split&&std::isspace(static_cast<unsigned char>(*split)))++split;if(auto*v=CVar::Lookup(t))v->Set(split);else CVar::Register(t,t,0,split,nullptr,0,1,nullptr);return 1;}
int __fastcall CvarResetCommandHandler(const char*,char* commandLine){if(commandLine&&*commandLine){if(auto*v=CVar::Lookup(commandLine))v->Reset();}else for(auto&v:g_cvars)v->Reset();return 1;}
int __fastcall CvarDefaultCommandHandler(const char*,char* commandLine){if(commandLine&&*commandLine){if(auto*v=CVar::Lookup(commandLine))v->Default();}else for(auto&v:g_cvars)v->Default();return 1;}
int __fastcall CvarListCommandHandler(const char*,char*){for(const auto&v:g_cvars)ConsolePrintf("%s = %s",v->Name(),v->StringValue());return 1;}
int CVarLoadFile(){
    if(g_filename.empty())return 0;
    const std::filesystem::path path=std::filesystem::path("WTF")/g_filename;
    std::ifstream file(path);
    if(!file)return 0;
    std::string line;
    while(std::getline(file,line)){
        std::size_t cursor=0;
        while(cursor<line.size()&&std::isspace(static_cast<unsigned char>(line[cursor])))++cursor;
        if(cursor==line.size()||line[cursor]=='#'||line[cursor]==';')continue;
        if(line.size()-cursor<3||_strnicmp(line.c_str()+cursor,"SET",3)!=0)continue;
        cursor+=3;
        while(cursor<line.size()&&std::isspace(static_cast<unsigned char>(line[cursor])))++cursor;
        const std::size_t nameStart=cursor;
        while(cursor<line.size()&&!std::isspace(static_cast<unsigned char>(line[cursor])))++cursor;
        if(cursor==nameStart)continue;
        const std::string name=line.substr(nameStart,cursor-nameStart);
        while(cursor<line.size()&&std::isspace(static_cast<unsigned char>(line[cursor])))++cursor;
        std::string value;
        if(cursor<line.size()&&line[cursor]=='"'){
            const std::size_t valueStart=++cursor;
            const std::size_t quote=line.find_last_of('"');
            value=line.substr(valueStart,quote!=std::string::npos&&quote>=valueStart?quote-valueStart:std::string::npos);
        }else{
            value=line.substr(cursor);
            while(!value.empty()&&std::isspace(static_cast<unsigned char>(value.back())))value.pop_back();
        }
        if(CVar* variable=CVar::Lookup(name.c_str()))variable->Set(value.c_str(),1,1,0);
        else CVar::Register(name.c_str(),name.c_str(),1,value.c_str(),nullptr,5,1,nullptr);
    }
    return 1;
}
int CVarSaveFile(){
    if(g_filename.empty())return 0;
    const std::filesystem::path directory("WTF");
    std::error_code error;
    std::filesystem::create_directories(directory,error);
    std::ofstream file(directory/g_filename,std::ios::trunc);
    if(!file)return 0;
    for(const auto& variable:g_cvars){
        if((variable->m_flags&1U)!=0)
            file<<"SET "<<variable->m_name<<" \""<<variable->StringValue()<<"\"\n";
    }
    return file.good()?1:0;
}
int __fastcall CvarCommandHandler(const char* command,const char* arguments,const char* value){auto*v=CVar::Lookup(command);if(!v)return 0;if(value&&*value)v->Set(value);else ConsolePrintf("CVar \"%s\" is \"%s\"",arguments?arguments:command,v->StringValue());return 1;}
int s_CreatePathDirectories(const char* path){if(!path||!*path)return 0;std::error_code error;std::filesystem::create_directories(path,error);return !error?1:0;}

void _E13_10(){} int _E15_12(){return 0;} int _E16_22(){_E13_10();return _E15_12();} void _E14_15(){}
int _E1_295(){INFINITY_295=std::numeric_limits<float>::infinity();return 2139095040;} int _E2_295(){return _E1_295();}
void _E4_292(){TWO_PI_290=6.28318530717958647692F;} int _E5_292(){_E4_292();return 0;} void _E7_292(){OO_TWO_PI_291=1.0F/TWO_PI_290;} int _E8_292(){_E7_292();return 0;}
