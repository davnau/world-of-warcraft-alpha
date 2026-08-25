#include "ENGINE/Source/Gx/gxapi/gxformat.h"

CGxFormat::CGxFormat() noexcept
    : apiSpecificModeID(0),
      hwTnL(0),
      fixLag(0),
      window(0),
      depthFormat(Fmt_Ds160),
      size(),
      colorFormat(Fmt_Rgb565),
      refreshRate(0),
      vsync(0),
      pos()
{
}

CGxFormat::CGxFormat(
    unsigned char windowValue,
    const NTempest::C2iVector& sizeValue,
    Format colorFormatValue,
    Format depthFormatValue,
    unsigned refreshRateValue,
    unsigned char vsyncValue,
    unsigned char hwTnLValue,
    unsigned char fixLagValue) noexcept
    : apiSpecificModeID(0),
      hwTnL(hwTnLValue),
      fixLag(fixLagValue),
      window(windowValue),
      depthFormat(depthFormatValue),
      size(sizeValue),
      colorFormat(colorFormatValue),
      refreshRate(refreshRateValue),
      vsync(vsyncValue),
      pos()
{
}
