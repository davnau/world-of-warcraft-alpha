#include "ENGINE/Source/Base/status.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

CStatus*GetGlobalStatusObj(){static CStatus s;return &s;}
CStatus::~CStatus(){Clear();}
char*FormatStatusMessage(const char*f,va_list a){va_list c;va_copy(c,a);int n=std::vsnprintf(nullptr,0,f,c);va_end(c);if(n<0)return nullptr;auto*p=static_cast<char*>(std::malloc(static_cast<size_t>(n)+1));if(p)std::vsnprintf(p,n+1,f,a);return p;}
void CStatus::Prepend(const char*t,STATUS_TYPE s){m_entries.push_front({t?t:"",s});}
void CStatus::Add(STATUS_TYPE s,const char*f,...){va_list a;va_start(a,f);char*p=FormatStatusMessage(f,a);va_end(a);m_entries.push_back({p?p:"",s});std::free(p);}
void CStatus::Add(const CStatus*s){if(s)m_entries.insert(m_entries.end(),s->m_entries.begin(),s->m_entries.end());}
void CStatus::Display() const{for(const auto&e:m_entries)std::fprintf(stderr,"%s\n",e.text.c_str());}
int CStatus::IsEmpty()const{return m_entries.empty();}void CStatus::Clear(){m_entries.clear();}
int CStatus::GetErrorStrLen(STATUS_TYPE min)const{int n=0;for(auto&e:m_entries)if(e.severity>=min)n+=static_cast<int>(e.text.size()+2);return n? n-2:0;}
void CStatus::GetErrorStr(char*b,unsigned cap,STATUS_TYPE min)const{if(!b||!cap)return;std::string s;for(auto&e:m_entries)if(e.severity>=min){if(!s.empty())s+="\r\n";s+=e.text;}std::memcpy(b,s.data(),std::min<std::size_t>(s.size(),cap-1));b[std::min<std::size_t>(s.size(),cap-1)]=0;}
char*CStatus::GetErrorStrAlloc(STATUS_TYPE min)const{int n=GetErrorStrLen(min);auto*p=static_cast<char*>(std::malloc(n+1));if(p)GetErrorStr(p,n+1,min);return p;}
STATUS_TYPE CStatus::GetHighestSeverity()const{STATUS_TYPE r=STATUS_INFO;for(auto&e:m_entries)if(e.severity>r)r=e.severity;return r;}
