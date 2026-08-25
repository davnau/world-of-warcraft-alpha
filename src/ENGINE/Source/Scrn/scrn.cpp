#include "ENGINE/Source/Scrn/scrn.h"

namespace {
int s_consoleInitialized = 0;
}

void ScrnDestroy()
{
    ILayerDestroy();
    IPerfDestroy();
    IStockDestroy();
    s_consoleInitialized = 0;
}

void ScrnInitialize(int consoleInitialized)
{
    ILayerInitialize();
    s_consoleInitialized = consoleInitialized;
    IPerfInitialize();
    IStockInitialize();
}

void ScrnPaint()
{
    ILayerPaint();
}

int _E1_44()
{
    return 0x7F800000;
}

int _E2_44()
{
    return _E1_44();
}

void _E4_44() {}
int _E5_44() { _E4_44(); return 0; }
void _E7_44() {}
int _E8_44() { _E7_44(); return 0; }

