#include "WoW/Source/WowServices/wowconnection.h"
#include "WoW/Source/Net/netinternal.h"

#include <limits>

float INFINITY_262 = 0.0F;

int _E2_262() { return _E1_262(); }
int _E1_262() { INFINITY_262 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E18_7() { _E15_10(); return _E17_16(); }
void _E15_10() { if (++WowConnectionInitializer::count == 1) WowConnectionInitializer::Initialize(); }
int _E17_16() { return 0; }
void _E16_19() { if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0) WowConnectionInitializer::Destroy(); }

void WowConnectionNet::PlatformInit(unsigned char useEngine) { m_usingEngine = useEngine != 0; }
int WinsockInit() { return 0; }
void WowConnectionNet::PlatformDestroy() { m_usingEngine = false; }
void WinsockDestroy() {}
void WowConnectionNet::PlatformAdd(WowConnection*) { PlatformWorkerReady(); }
void WowConnectionNet::PlatformRemove(WowConnection*) { PlatformWorkerReady(); }
void WowConnectionNet::PlatformChangeState(WowConnection*, WowConnection*) { PlatformWorkerReady(); }
void WowConnectionNet::PlatformDestruct(WowConnection*) {}
void WowConnectionNet::PlatformWorkerReady() {}
void WowConnectionNet::PlatformRun(int* state) { if (state) *state = m_stop ? 1 : 0; }
void MakeSocketPipe(unsigned) {}

int _E5_259() { _E4_259(); return 0; }
void _E4_259() {}
int _E8_259() { _E7_259(); return 0; }
void _E7_259() {}
