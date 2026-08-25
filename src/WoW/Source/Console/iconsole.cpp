#include "WoW/Source/Console/iconsole.h"

#include "WoW/Source/Console/consoleclient.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace {
std::vector<std::string> g_history;
std::string g_execBuffer;
}

int AddLineToExecFile(const char* line) {
    if (!line) return 0;
    if (_stricmp(line, "end") == 0) {
        if (g_execBuffer.empty()) return 1;
        std::ofstream file("WTF/config.wtf", std::ios::app);
        if (!file) return 0;
        file << g_execBuffer;
        g_execBuffer.clear();
        return 1;
    }
    g_execBuffer += line;
    g_execBuffer.push_back('\n');
    if (g_execBuffer.size() > 0x1FFF) g_execBuffer.resize(0x1FFF);
    return 0;
}

void AddToHistory(const char* line) {
    if (!line || !*line) return;
    g_history.emplace_back(line);
    if (g_history.size() > 32) g_history.erase(g_history.begin());
}

int _E1_298() { INFINITY_294 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E2_298() { return _E1_298(); }
void _E4_295() { TWO_PI_289 = 6.28318530717958647692F; }
int _E5_295() { _E4_295(); return 0; }
void _E7_295() { OO_TWO_PI_290 = 1.0F / TWO_PI_289; }
int _E8_295() { _E7_295(); return 0; }
