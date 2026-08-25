#include "WoW/Source/Object/objectclient/playergm_c.h"

#include "ENGINE/Source/Base/cdatastore.h"

#include <cstring>

namespace {
std::uint64_t s_realActivePlayer = 0;
std::uint64_t s_ghostTarget = 0;
std::uint64_t s_ghostTargetRequested = 0;
char s_ghostNameRequested[256]{};
bool s_ghostRequestPending = false;
bool s_handlersInstalled = false;
}

void CGPlayer_C::SetRealActivePlayer(std::uint64_t guid) {
    s_realActivePlayer = guid;
}

std::uint64_t CGPlayer_C::GetRealActivePlayer() { return s_realActivePlayer; }

void CGPlayer_C::InstallGMHandlers() { s_handlersInstalled = true; }
void CGPlayer_C::UninstallGMHandlers() { s_handlersInstalled = false; }

void CGPlayer_C::StartGhosting(const char* name) {
    s_ghostTarget = 0;
    s_ghostTargetRequested = 0;
    std::strncpy(s_ghostNameRequested, name ? name : "",
                 sizeof(s_ghostNameRequested) - 1);
    s_ghostNameRequested[sizeof(s_ghostNameRequested) - 1] = 0;
    s_ghostRequestPending = true;
}

void CGPlayer_C::StartGhosting(std::uint64_t guid) {
    s_ghostTarget = 0;
    s_ghostTargetRequested = guid;
    s_ghostNameRequested[0] = 0;
    s_ghostRequestPending = true;
}

void CGPlayer_C::StopGhosting() {
    s_ghostTarget = 0;
    s_ghostTargetRequested = 0;
    s_ghostNameRequested[0] = 0;
    s_ghostRequestPending = false;
}

int OnGMEvent(unsigned int messageID, unsigned int, CDataStore* msg) {
    // 470 is the ghost response opcode in the recovered client.  The other
    // GM messages carry a one-byte success result and are acknowledged here.
    if (messageID == 470) {
        s_ghostRequestPending = false;
        if (msg) msg->Get(&s_ghostTarget);
        return 1;
    }
    if (msg) {
        std::uint8_t success = 0;
        msg->Get(&success);
    }
    return 1;
}

void MaybeSendGhostRequest() {
    if (s_ghostRequestPending) return;
    if (s_ghostTargetRequested)
        CGPlayer_C::StartGhosting(s_ghostTargetRequested);
    else if (s_ghostNameRequested[0])
        CGPlayer_C::StartGhosting(s_ghostNameRequested);
}

void CGPlayer_C::GMIdle() {
    // The full world-frame teleport/movement integration calls this hook once
    // the target object is available.  The state machine itself is preserved.
    if (s_ghostTarget == 0) MaybeSendGhostRequest();
}

bool GhostRequestPending() { return s_ghostRequestPending; }
std::uint64_t GhostTarget() { return s_ghostTarget; }
