#pragma once

#include <cstdint>

class CGPlayer_C {
public:
    static void GMIdle();
    static std::uint64_t GetRealActivePlayer();
    static void InstallGMHandlers();
    static void SetRealActivePlayer(std::uint64_t guid);
    static void StartGhosting(const char* name);
    static void StartGhosting(std::uint64_t guid);
    static void StopGhosting();
    static void UninstallGMHandlers();
};

int OnGMEvent(unsigned int messageID, unsigned int eventTime, class CDataStore* msg);
void MaybeSendGhostRequest();
bool GhostRequestPending();
std::uint64_t GhostTarget();

// Recovered symbol declarations for objectclient/playergm_c.cpp
