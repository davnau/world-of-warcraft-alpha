#include "ENGINE/Source/Base/activity.h"
#include <array>
#include <chrono>
#include <mutex>

namespace { using Clock=std::chrono::steady_clock; struct Entry{Clock::time_point start{};double seconds=0;unsigned calls=0,depth=0;};std::array<Entry,ACTIVITY_COUNT>s;std::mutex m;bool suspended=false;Clock::time_point epoch=Clock::now(); }
void ActivityBegin(ACTIVITY a){std::lock_guard l(m);auto&e=s[static_cast<unsigned>(a)%s.size()];if(!e.depth++&&!suspended)e.start=Clock::now();++e.calls;}
void ActivityEnd(ACTIVITY a){std::lock_guard l(m);auto&e=s[static_cast<unsigned>(a)%s.size()];if(e.depth&&!--e.depth&&!suspended)e.seconds+=std::chrono::duration<double>(Clock::now()-e.start).count();}
double ActivityGetTime(ACTIVITY a){std::lock_guard l(m);auto&e=s[static_cast<unsigned>(a)%s.size()];return e.seconds+(e.depth&&!suspended?std::chrono::duration<double>(Clock::now()-e.start).count():0);}
unsigned ActivityGetCalls(ACTIVITY a){std::lock_guard l(m);return s[static_cast<unsigned>(a)%s.size()].calls;}
double ActivityGetTimePercent(ACTIVITY a){double total=std::chrono::duration<double>(Clock::now()-epoch).count();return total>0?ActivityGetTime(a)*100.0/total:0;}
void ActivitySuspend(){std::lock_guard l(m);if(suspended)return;auto now=Clock::now();for(auto&e:s)if(e.depth)e.seconds+=std::chrono::duration<double>(now-e.start).count();suspended=true;}
void ActivityResume(){std::lock_guard l(m);if(!suspended)return;auto now=Clock::now();for(auto&e:s)if(e.depth)e.start=now;suspended=false;}
void ActivityResetTimes(){std::lock_guard l(m);for(auto&e:s){e.seconds=0;e.calls=0;if(e.depth)e.start=Clock::now();}epoch=Clock::now();}
