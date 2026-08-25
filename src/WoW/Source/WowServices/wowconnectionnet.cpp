#include "WoW/Source/WowServices/wowconnection.h"
#include "WoW/Source/Net/netinternal.h"

#include <algorithm>
#include <limits>

float INFINITY_263 = 0.0F;

int _E2_263() { return _E1_263(); }
int _E1_263() { INFINITY_263 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E18_8() { _E15_11(); return _E17_17(); }
void _E15_11() { if (++WowConnectionInitializer::count == 1) WowConnectionInitializer::Initialize(); }
int _E17_17() { return 0; }
void _E16_20() { if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0) WowConnectionInitializer::Destroy(); }

void WowConnectionNet::Add(WowConnection* connection) {
    if (!connection) return;
    std::lock_guard lock(m_connectionsLock);
    if (std::find(m_connections.begin(), m_connections.end(), connection) == m_connections.end())
        m_connections.push_back(connection);
    PlatformAdd(connection);
}
void WowConnectionNet::Remove(WowConnection* connection) {
    std::lock_guard lock(m_connectionsLock);
    auto it = std::remove(m_connections.begin(), m_connections.end(), connection);
    m_connections.erase(it, m_connections.end());
    PlatformRemove(connection);
}
void WowConnectionNet::Start() { m_stop = false; if (m_numWorkers == 0) m_numWorkers = 1; }
void WowConnectionNet::Stop() { m_stop = true; PlatformWorkerReady(); }
void WowConnectionNet::Service(WowConnection* connection) { if (connection) connection->DoReads(static_cast<int>(connection->m_serviceFlags)); }
void WowConnectionNet::RunWorker(int id) { (void)id; Run(); }
void WowConnectionNet::SignalWorker(WowConnection* connection, int flags) { if (connection) connection->m_serviceFlags = static_cast<unsigned>(flags); }
void WowConnectionNet::Run() { PlatformRun(nullptr); }
void WowConnectionNet::Delete(WowConnection* connection) { Remove(connection); }
int _E5_260() { _E4_260(); return 0; }
void _E4_260() {}
int _E8_260() { _E7_260(); return 0; }
void _E7_260() {}
