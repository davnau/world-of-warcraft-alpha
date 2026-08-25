#include "ENGINE/Source/Base/rcstring.h"
#include <mutex>
#include <unordered_map>

namespace{std::mutex m;std::unordered_map<std::string,std::weak_ptr<CStringRep>> strings;}
CStringRep::CStringRep(const char*s):m_string(s?s:""){}void CStringRep::DecrRef(){}int CStringRep::IsString(const char*s)const{return m_string==(s?s:"");}bool CStringRep::operator==(const CStringRep&r)const{return m_string==r.m_string;}
CStringManager*CStringManager::Get(){static CStringManager manager;return &manager;}void CStringManager::DestroyManager(){std::lock_guard l(m);strings.clear();}CStringManager::~CStringManager()=default;
std::shared_ptr<CStringRep>CStringManager::Find(const char*s){std::lock_guard l(m);auto i=strings.find(s?s:"");return i==strings.end()?nullptr:i->second.lock();}
std::shared_ptr<CStringRep>CStringManager::Add(const char*s){std::lock_guard l(m);std::string key=s?s:"";if(auto p=strings[key].lock())return p;auto p=std::make_shared<CStringRep>(key.c_str());strings[key]=p;return p;}
RCString::RCString(const char*s):m_rep(CStringManager::Get()->Add(s)){}RCString::~RCString()=default;void RCString::Copy(const RCString&s){m_rep=s.m_rep;}void RCString::Copy(const char*s){m_rep=CStringManager::Get()->Add(s);}void RCString::Free(){m_rep.reset();}
bool RCString::operator==(const RCString&s)const{return GetString()==s.GetString()||std::string(GetString())==s.GetString();}bool RCString::operator==(const char*s)const{return std::string(GetString())==(s?s:"");}const char*RCString::GetString()const{return m_rep?m_rep->m_string.c_str():"";}RCString::operator const char*()const{return GetString();}
RCString RCString::Cat(const char*s)const{std::string r=GetString();r+=s?s:"";return RCString(r.c_str());}RCString&RCString::operator+=(const RCString&s){return *this+=s.GetString();}RCString&RCString::operator+=(const char*s){*this=Cat(s);return*this;}RCString RCString::SubString(unsigned b,unsigned n)const{return RCString(m_rep?m_rep->m_string.substr(b,n).c_str():"");}RCString RCString::Get(const char*s){return RCString(s);}
