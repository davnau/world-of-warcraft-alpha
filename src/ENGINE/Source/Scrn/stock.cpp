#include "ENGINE/Source/Scrn/stock.h"

#include "ENGINE/Source/Os/W32/path.h"
#include "Storm/h/storm.h"

#include <array>

namespace {

std::array<HTEXTFONT, SCRNSTOCKOBJECTS> s_stockObjects{};
constexpr std::array<float, SCRNSTOCKOBJECTS> s_stockObjectHeights{0.034F, 0.026F};

bool ValidStock(SCRNSTOCK stockID)
{
    return static_cast<unsigned>(stockID) < SCRNSTOCKOBJECTS;
}

} // namespace

void IStockDestroy()
{
    for (HTEXTFONT& font : s_stockObjects) {
        if (font != nullptr)
            TextBlockReleaseFont(font);
        font = nullptr;
    }
}

void IStockInitialize()
{
    IStockDestroy();
    char fontFile[260]{};
    OsBuildFontFilePath(fontFile, "FRIZQT__.TTF", sizeof(fontFile));
    ScrnSetStockFont(STOCK_SYSFONT, fontFile);
    ScrnSetStockFont(STOCK_PERFFONT, fontFile);
}

HTEXTFONT __fastcall ScrnGetStockFont(SCRNSTOCK stockID)
{
    if (!ValidStock(stockID)) {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return nullptr;
    }
    return s_stockObjects[static_cast<unsigned>(stockID)];
}

void ScrnSetStockFont(SCRNSTOCK stockID, const char* fontTexturePath)
{
    if (!ValidStock(stockID) || fontTexturePath == nullptr || *fontTexturePath == '\0') {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return;
    }
    CreateSysFont(stockID, fontTexturePath);
}

void CreateSysFont(SCRNSTOCK stockID, const char* fontFilePath)
{
    if (!ValidStock(stockID) || fontFilePath == nullptr || *fontFilePath == '\0') {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return;
    }
    HTEXTFONT& destination = s_stockObjects[static_cast<unsigned>(stockID)];
    if (destination != nullptr)
        TextBlockReleaseFont(destination);
    destination = TextBlockGenerateFont(fontFilePath, 0,
        static_cast<float>(NDCToDDCHeight(s_stockObjectHeights[static_cast<unsigned>(stockID)])));
    if (destination == nullptr)
        SErrSetLastError(ERROR_OUTOFMEMORY);
}

float __fastcall ScrnGetStockFontHeight(SCRNSTOCK stockID)
{
    if (!ValidStock(stockID)) {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0.0F;
    }
    return s_stockObjectHeights[static_cast<unsigned>(stockID)];
}

// The original object files contain compiler-generated initialization thunks
// for the Tempest constants.  Keep their recovered symbol names represented by
// small, deterministic helpers while letting normal C++ initialization handle
// the actual storage in this reconstruction.
int _E1_46() { return 0x7F800000; }
int _E2_46() { return _E1_46(); }
void _E4_46() {}
int _E5_46() { _E4_46(); return 0; }
void _E7_46() {}
int _E8_46() { _E7_46(); return 0; }
