#include "Storm/h/SEvt.h"

#include <algorithm>
#include <map>
#include <mutex>
#include <utility>
#include <vector>

namespace {

struct HandlerRecord {
    unsigned int id;
    unsigned int sequence;
    SEVT_HANDLER handler;
};

struct TypeState {
    unsigned int sequence = 0;
    std::vector<std::vector<HandlerRecord>> states{1};
};

std::recursive_mutex s_eventLock;
std::map<std::pair<unsigned int, unsigned int>, TypeState> s_types;
std::vector<void*> s_breakCommands;

bool ConsumeBreak(void* data) {
    const auto found = std::find(s_breakCommands.begin(), s_breakCommands.end(), data);
    if (found == s_breakCommands.end()) return false;
    s_breakCommands.erase(found);
    return true;
}

} // namespace

int __stdcall SEvtBreakHandlerChain(void* data) {
    std::lock_guard lock(s_eventLock);
    s_breakCommands.push_back(data);
    return 1;
}

int __stdcall SEvtDestroy() {
    std::lock_guard lock(s_eventLock);
    s_types.clear();
    s_breakCommands.clear();
    return 1;
}

int __stdcall SEvtDispatch(unsigned int type, unsigned int subtype, unsigned int id, void* data) {
    std::vector<HandlerRecord> handlers;
    {
        std::lock_guard lock(s_eventLock);
        if (ConsumeBreak(data)) return 0;
        const auto found = s_types.find({type, subtype});
        if (found == s_types.end()) return 0;
        for (const HandlerRecord& record : found->second.states.back()) {
            if (record.id == id) handlers.push_back(record);
        }
        std::sort(handlers.begin(), handlers.end(), [](const auto& left, const auto& right) {
            return left.sequence > right.sequence;
        });
    }
    int dispatched = 0;
    for (const HandlerRecord& record : handlers) {
        {
            std::lock_guard lock(s_eventLock);
            if (ConsumeBreak(data)) break;
            const auto found = s_types.find({type, subtype});
            if (found == s_types.end()) break;
            const auto& current = found->second.states.back();
            const bool registered = std::any_of(current.begin(), current.end(), [&](const HandlerRecord& item) {
                return item.id == id && item.sequence == record.sequence && item.handler == record.handler;
            });
            if (!registered) continue;
        }
        record.handler(data);
        dispatched = 1;
    }
    {
        std::lock_guard lock(s_eventLock);
        s_breakCommands.erase(std::remove(s_breakCommands.begin(), s_breakCommands.end(), data), s_breakCommands.end());
    }
    return dispatched;
}

int __stdcall SEvtPopState(unsigned int type, unsigned int subtype) {
    std::lock_guard lock(s_eventLock);
    const auto found = s_types.find({type, subtype});
    if (found == s_types.end()) return 0;
    if (found->second.states.size() > 1) found->second.states.pop_back();
    else s_types.erase(found);
    return 1;
}

int __stdcall SEvtPushState(unsigned int type, unsigned int subtype) {
    std::lock_guard lock(s_eventLock);
    const auto found = s_types.find({type, subtype});
    if (found == s_types.end()) return 0;
    found->second.states.push_back(found->second.states.back());
    return 1;
}

int __stdcall SEvtRegisterHandler(unsigned int type, unsigned int subtype, unsigned int id,
    void* flags, SEVT_HANDLER handler) {
    if (flags || !handler) return 0;
    std::lock_guard lock(s_eventLock);
    TypeState& state = s_types[{type, subtype}];
    state.states.back().push_back(HandlerRecord{id, ++state.sequence, handler});
    return 1;
}

int __stdcall SEvtUnregisterHandler(unsigned int type, unsigned int subtype, unsigned int id,
    SEVT_HANDLER handler) {
    std::lock_guard lock(s_eventLock);
    const auto found = s_types.find({type, subtype});
    if (found == s_types.end()) return 0;
    auto& records = found->second.states.back();
    const auto oldSize = records.size();
    records.erase(std::remove_if(records.begin(), records.end(), [&](const HandlerRecord& record) {
        return record.id == id && (!handler || record.handler == handler);
    }), records.end());
    return records.size() != oldSize;
}

int __stdcall SEvtUnregisterType(unsigned int type, unsigned int subtype) {
    std::lock_guard lock(s_eventLock);
    return s_types.erase({type, subtype}) ? 1 : 0;
}
