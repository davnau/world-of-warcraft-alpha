#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ENGINE/Source/Services/texture.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Gx/gxapi/gxtexture.h"
#include "Storm/h/storm.h"
#include <Windows.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <unordered_map>
namespace { void CacheTexture(CTexture *, const char *, unsigned); void LogGxCache(); void LogTextures(); }
HASHKEY_TEXTUREFILE::~HASHKEY_TEXTUREFILE() = default;
CTextureItem::~CTextureItem() = default;
CTextureHash::CTextureHash() = default;
CTextureHash::~CTextureHash() = default;
CSolidTextureHash::CSolidTextureHash() = default;
CSolidTextureHash::~CSolidTextureHash() = default;
void UpdateTextureDefault(EGxTexCommand operation, unsigned, unsigned, unsigned,
                          unsigned mipLevel, void *user, unsigned *stride,
                          const void **texels) {
  auto *texture = static_cast<CTexture *>(user);
  if ((operation == GxTex_Lock || operation == GxTex_Latch) && texture && mipLevel < texture->mipBits.size()) {
    const auto &mip = texture->mipBits[mipLevel];
    if (stride) *stride = mip.width * 4U;
    if (texels) *texels = mip.bits.data();
  }
}
void UpdateBlpTextureAsync(EGxTexCommand operation, unsigned width, unsigned,
                           unsigned, unsigned mipLevel, void *user,
                           unsigned *stride, const void **texels) {
  auto *texture = static_cast<CTexture *>(user);
  if (!texture)
    return;
  if ((operation == GxTex_Lock || operation == GxTex_Latch) && mipLevel < texture->mipBits.size()) {
    texture->gxWidth = (std::max)(1U, width >> mipLevel);
    const auto &mip = texture->mipBits[mipLevel];
    if (stride) *stride = mip.width * 4U;
    if (texels) *texels = mip.bits.data();
  }
}
HTEXTURE__ *GetDefaultTexture() {
  static HTEXTURE__ *value = TextureCreateSolid(nullptr);
  return value;
}
int LoadBlpMips(const char *p, std::vector<MipBits> &m, unsigned *w,
                unsigned *h, EGxTexFormat *f) {
  std::unique_ptr<MipBits> x(TextureLoadImage(p, w, h, f, nullptr, nullptr));
  if (!x)
    return 0;
  m.push_back(std::move(*x));
  return 1;
}
int LoadTgaMips(const char *p, std::vector<MipBits> &m, unsigned *w,
                unsigned *h, EGxTexFormat *f) {
  return LoadBlpMips(p, m, w, h, f);
}
unsigned LoadPredrawnMips(const char *p, char *, MipBits *b) {
  std::unique_ptr<MipBits> x(
      TextureLoadImage(p, nullptr, nullptr, nullptr, nullptr, nullptr));
  if (!x)
    return 0;
  if (b)
    *b = std::move(*x);
  return 1;
}
void RequestImageDimensions(unsigned *w, unsigned *h, unsigned *m) {
  if (w)
    *w = (std::min)(*w, 2048U);
  if (h)
    *h = (std::min)(*h, 2048U);
  if (m)
    *m = 0;
}
void AsyncTextureHandler() {}
void AsyncTextureLoadImageCallback(void *p) {
  AsyncTextureLoadImageCreate(static_cast<CTexture *>(p));
}
int AsyncTextureLoadImageCreate(CTexture *t) {
  return t && TextureGetMips(t, 1);
}
void *TextureAllocImage(unsigned n) { return new std::uint8_t[n]{}; }
CGxTex *TextureAllocGxTex(CTexture *t) {
  return TextureGetGxTex(t, 1, nullptr);
}
void TextureFreeGxTex(CTexture *t) {
  if (t) {
    if (t->gxTex) GxTexDestroy(t->gxTex);
    t->gxTex = nullptr;
  }
}
void HashNewTexture(HTEXTURE__ *texture, const char *name, unsigned flags,
                    CStatus *) { CacheTexture(texture, name, flags); }
void FileError(const char *file, const char *message) {
  std::string text = "Texture error";
  if (file) text += " [" + std::string(file) + "]";
  if (message) text += ": " + std::string(message);
  text += "\n"; OutputDebugStringA(text.c_str());
}
int CreateTgaTexture(CTexture *t) {
  return t && TextureGetGxTex(t, 1, nullptr);
}
void UpdateTgaTexture(MipBits *mip, unsigned width, unsigned height,
                      unsigned levelsProvided, unsigned levelsDesired) {
  if (!mip || mip->bits.empty()) return;
  mip->width = width; mip->height = height;
  const std::size_t required = CalcLevelOffset(width, height, levelsDesired, 4);
  if (mip->bits.size() < required) mip->bits.resize(required);
  TextureGenerateMips(width, height, levelsProvided, levelsDesired, mip->bits.data());
}
int CreateBlpTexture(CTexture *t) { return CreateTgaTexture(t); }
void AsyncCreateBlpTextureCallback(void *p) {
  PumpBlpTextureAsync(static_cast<CTexture *>(p));
}
int PumpBlpTextureAsync(CTexture *t) { return CreateBlpTexture(t); }
void GetTextureFormats(int preferred, EGxTexFormat *d, EGxTexFormat *g) {
  if (d)
    *d = static_cast<EGxTexFormat>(preferred);
  if (g)
    *g = static_cast<EGxTexFormat>(preferred);
}
void FillInSolidTexture(CTexture *t, NTempest::CImVector c) {
  if (!t)
    return;
  if (t->mipBits.empty())
    t->mipBits.push_back({1, 1, {}});
  t->mipBits[0].bits = {c.b, c.g, c.r, c.a};
  t->opaque = c.a == 255;
}
HTEXTURE__ *GetTexture(const char *p, unsigned f) {
  return TextureCreate(p, nullptr, f, 0);
}
void TextureLogGxCache(void *) { LogGxCache(); }
void TextureLogTextures(void *) { LogTextures(); }
int TextureLogSortCallback(const void *a, const void *b) {
  auto l = *static_cast<CTexture *const *>(a),
       r = *static_cast<CTexture *const *>(b);
  return l->filename.compare(r->filename);
}
namespace {
std::mutex lock_;
std::unordered_map<std::string, CTexture *> cache;
std::vector<CTexture *> all;
unsigned maxSize_ = 2048;
std::string Lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return s;
}
void CacheTexture(CTexture *texture, const char *name, unsigned flags) {
  if (!texture || !name) return;
  std::lock_guard guard(lock_);
  texture->filename = name; texture->gxTexFlags = flags; texture->lastUsed = GetTickCount();
  cache[Lower(name) + "#" + std::to_string(flags)] = texture;
}
void LogGxCache() {
  char text[160];
  for (const auto *texture : all) if (texture && texture->gxTex) {
    sprintf_s(text, "GxTexture: %s %ux%u format=%d\n", texture->filename.c_str(), texture->gxWidth, texture->gxHeight, texture->gxTexFormat); OutputDebugStringA(text);
  }
}
void LogTextures() {
  std::vector<CTexture *> sorted = all;
  std::sort(sorted.begin(), sorted.end(), [](const CTexture *a,const CTexture *b){return a->filename < b->filename;});
  char text[200];
  for (const auto *texture : sorted) { sprintf_s(text, "Texture: %s %ux%u refs=%u\n", texture->filename.c_str(), texture->gxWidth, texture->gxHeight, texture->refCount); OutputDebugStringA(text); }
}
void ReadDims(const std::vector<std::uint8_t> &b, unsigned &w, unsigned &h) {
  if (b.size() > 18 && b[1] == 0 && (b[2] == 2 || b[2] == 10)) {
    w = b[12] | b[13] << 8;
    h = b[14] | b[15] << 8;
  } else if (b.size() > 20 && b[0] == 'B' && b[1] == 'L' && b[2] == 'P') {
    std::memcpy(&w, b.data() + 12, 4);
    std::memcpy(&h, b.data() + 16, 4);
  }
  if (!w)
    w = 1;
  if (!h)
    h = 1;
}

bool DecodeTga(const std::vector<std::uint8_t> &source, MipBits &mip,
               bool &isOpaque) {
  if (source.size() < 18 || (source[2] != 2 && source[2] != 10))
    return false;
  const unsigned width = source[12] | (unsigned(source[13]) << 8U);
  const unsigned height = source[14] | (unsigned(source[15]) << 8U);
  const unsigned bytesPerPixel = source[16] / 8U;
  if (!width || !height || (bytesPerPixel != 3 && bytesPerPixel != 4))
    return false;
  std::size_t input = 18U + source[0];
  mip = {width, height, std::vector<std::uint8_t>(std::size_t(width) * height * 4U)};
  std::size_t pixel = 0;
  auto writePixel = [&](const std::uint8_t *value) {
    const bool topOrigin = (source[17] & 0x20U) != 0;
    const unsigned x = static_cast<unsigned>(pixel % width);
    const unsigned sourceY = static_cast<unsigned>(pixel / width);
    const unsigned y = topOrigin ? sourceY : height - 1U - sourceY;
    auto *target = mip.bits.data() + (std::size_t(y) * width + x) * 4U;
    target[0] = value[0]; target[1] = value[1]; target[2] = value[2];
    target[3] = bytesPerPixel == 4 ? value[3] : 255U;
    isOpaque &= target[3] == 255U;
    ++pixel;
  };
  while (pixel < std::size_t(width) * height) {
    if (source[2] == 2) {
      if (input + bytesPerPixel > source.size()) return false;
      writePixel(source.data() + input); input += bytesPerPixel;
    } else {
      if (input >= source.size()) return false;
      const std::uint8_t packet = source[input++]; const unsigned count = (packet & 0x7FU) + 1U;
      if (packet & 0x80U) {
        if (input + bytesPerPixel > source.size()) return false;
        const auto *value = source.data() + input; input += bytesPerPixel;
        for (unsigned i = 0; i < count && pixel < std::size_t(width) * height; ++i) writePixel(value);
      } else {
        for (unsigned i = 0; i < count && pixel < std::size_t(width) * height; ++i) {
          if (input + bytesPerPixel > source.size()) return false;
          writePixel(source.data() + input); input += bytesPerPixel;
        }
      }
    }
  }
  return true;
}

unsigned ReadU32(const std::vector<std::uint8_t> &source, std::size_t offset) {
  unsigned value = 0; if (offset + 4U <= source.size()) std::memcpy(&value, source.data() + offset, 4U); return value;
}

bool DecodeBlpPalette(const std::vector<std::uint8_t> &source, MipBits &mip,
                      bool &isOpaque, unsigned &alphaDepth) {
  if (source.size() < 148U || source[0] != 'B' || source[1] != 'L' || source[2] != 'P') return false;
  const bool compact = source[3] == '2' ||
      ((source[8] == 1U || source[8] == 2U) && source[9] <= 8U && ReadU32(source, 20U) >= 148U);
  const unsigned width = ReadU32(source, 12), height = ReadU32(source, 16);
  const unsigned encoding = compact ? source[8] : ReadU32(source, 4);
  if (!width || !height || encoding != 1U) return false;
  alphaDepth = compact ? source[9] : (ReadU32(source, 8) & 0xFFU);
  const std::size_t offsetsBase = compact ? 20U : 28U;
  const std::size_t paletteBase = compact ? 148U : 156U;
  const unsigned offset = ReadU32(source, offsetsBase), size = ReadU32(source, offsetsBase + 64U);
  const std::size_t pixelCount = std::size_t(width) * height;
  if (paletteBase + 1024U > source.size() || offset > source.size() || size > source.size() - offset || size < pixelCount) return false;
  mip = {width, height, std::vector<std::uint8_t>(pixelCount * 4U)};
  const auto *indices = source.data() + offset; const auto *alpha = indices + pixelCount;
  for (std::size_t i = 0; i < pixelCount; ++i) {
    const auto *color = source.data() + paletteBase + std::size_t(indices[i]) * 4U;
    auto *target = mip.bits.data() + i * 4U; target[0]=color[0];target[1]=color[1];target[2]=color[2];
    std::uint8_t a = 255U;
    if (alphaDepth == 8 && alpha + i < source.data() + offset + size) a = alpha[i];
    else if (alphaDepth == 4 && alpha + i/2U < source.data() + offset + size) { const auto nibble=(alpha[i/2U] >> ((i&1U)*4U))&0xFU; a=static_cast<std::uint8_t>(nibble*17U); }
    else if (alphaDepth == 1 && alpha + i/8U < source.data() + offset + size) a=(alpha[i/8U]&(1U<<(i&7U)))?255U:0U;
    target[3]=a;isOpaque &= a==255U;
  }
  return true;
}

struct DxtColor {
  unsigned r, g, b, a;
};

DxtColor Expand565(std::uint16_t value) {
  return {((value >> 11U) & 31U) * 255U / 31U,
          ((value >> 5U) & 63U) * 255U / 63U,
          (value & 31U) * 255U / 31U, 255U};
}

bool DecodeBlpDxt(const std::vector<std::uint8_t>& source, MipBits& mip,
                  bool& isOpaque, unsigned& alphaDepth) {
  if (source.size() < 148U || source[0] != 'B' || source[1] != 'L' || source[2] != 'P')
    return false;
  const bool compact = source[3] == '2' ||
      ((source[8] == 1U || source[8] == 2U) && source[9] <= 8U && ReadU32(source, 20U) >= 148U);
  if (!compact || source[8] != 2U) return false;

  const unsigned width = ReadU32(source, 12U), height = ReadU32(source, 16U);
  const unsigned offset = ReadU32(source, 20U), size = ReadU32(source, 84U);
  alphaDepth = source[9];
  const unsigned alphaEncoding = source[10];
  const bool dxt1 = alphaDepth <= 1U;
  const bool dxt5 = !dxt1 && alphaEncoding == 7U;
  const unsigned blockBytes = dxt1 ? 8U : 16U;
  const unsigned blocksWide = (width + 3U) / 4U;
  const unsigned blocksHigh = (height + 3U) / 4U;
  const std::size_t required = static_cast<std::size_t>(blocksWide) * blocksHigh * blockBytes;
  if (!width || !height || offset > source.size() || size > source.size() - offset || required > size)
    return false;

  mip = {width, height, std::vector<std::uint8_t>(static_cast<std::size_t>(width) * height * 4U)};
  const std::uint8_t* input = source.data() + offset;
  for (unsigned blockY = 0; blockY < blocksHigh; ++blockY) {
    for (unsigned blockX = 0; blockX < blocksWide; ++blockX) {
      const std::uint8_t* block = input + (static_cast<std::size_t>(blockY) * blocksWide + blockX) * blockBytes;
      const std::uint8_t* colorBlock = block + (dxt1 ? 0U : 8U);
      const std::uint16_t color0 = static_cast<std::uint16_t>(colorBlock[0] | (colorBlock[1] << 8U));
      const std::uint16_t color1 = static_cast<std::uint16_t>(colorBlock[2] | (colorBlock[3] << 8U));
      std::array<DxtColor, 4> colors{Expand565(color0), Expand565(color1), {}, {}};
      if (color0 > color1 || !dxt1) {
        colors[2] = {(2U * colors[0].r + colors[1].r) / 3U,
                     (2U * colors[0].g + colors[1].g) / 3U,
                     (2U * colors[0].b + colors[1].b) / 3U, 255U};
        colors[3] = {(colors[0].r + 2U * colors[1].r) / 3U,
                     (colors[0].g + 2U * colors[1].g) / 3U,
                     (colors[0].b + 2U * colors[1].b) / 3U, 255U};
      } else {
        colors[2] = {(colors[0].r + colors[1].r) / 2U,
                     (colors[0].g + colors[1].g) / 2U,
                     (colors[0].b + colors[1].b) / 2U, 255U};
        colors[3] = {0U, 0U, 0U, 0U};
      }
      const std::uint32_t colorIndices = ReadU32(source,
          static_cast<std::size_t>(colorBlock - source.data()) + 4U);

      std::array<unsigned, 8> alphas{255U, 255U, 255U, 255U, 255U, 255U, 255U, 255U};
      std::uint64_t alphaIndices = 0;
      if (dxt5) {
        alphas[0] = block[0]; alphas[1] = block[1];
        for (unsigned byte = 0; byte < 6U; ++byte)
          alphaIndices |= static_cast<std::uint64_t>(block[2U + byte]) << (byte * 8U);
        if (alphas[0] > alphas[1]) {
          for (unsigned index = 1; index <= 6U; ++index)
            alphas[index + 1U] = ((7U - index) * alphas[0] + index * alphas[1]) / 7U;
        } else {
          for (unsigned index = 1; index <= 4U; ++index)
            alphas[index + 1U] = ((5U - index) * alphas[0] + index * alphas[1]) / 5U;
          alphas[6] = 0U; alphas[7] = 255U;
        }
      }

      for (unsigned y = 0; y < 4U && blockY * 4U + y < height; ++y) {
        for (unsigned x = 0; x < 4U && blockX * 4U + x < width; ++x) {
          const unsigned pixelIndex = y * 4U + x;
          DxtColor color = colors[(colorIndices >> (pixelIndex * 2U)) & 3U];
          if (!dxt1) {
            if (dxt5) color.a = alphas[(alphaIndices >> (pixelIndex * 3U)) & 7U];
            else color.a = ((block[pixelIndex / 2U] >> ((pixelIndex & 1U) * 4U)) & 15U) * 17U;
          }
          auto* output = mip.bits.data() +
              (static_cast<std::size_t>(blockY * 4U + y) * width + blockX * 4U + x) * 4U;
          output[0] = static_cast<std::uint8_t>(color.b);
          output[1] = static_cast<std::uint8_t>(color.g);
          output[2] = static_cast<std::uint8_t>(color.r);
          output[3] = static_cast<std::uint8_t>(color.a);
          isOpaque &= color.a == 255U;
        }
      }
    }
  }
  return true;
}
} // namespace
CTexture::CTexture() = default;
CTexture::~CTexture() { if (gxTex) GxTexDestroy(gxTex); }
const char *CTexture::GetObjectName() const { return filename.c_str(); }
void TextureInitialize(unsigned s) { maxSize_ = s; }
void TextureCacheFlush() {
  std::lock_guard g(lock_);
  cache.clear();
}
void TextureGxCacheFlush() {
  for (auto *t : all) {
    if (t->gxTex) GxTexDestroy(t->gxTex);
    t->gxTex = nullptr;
  }
}
void TextureCacheUpdate(CStatus *, int) {
  const unsigned now = GetTickCount();
  std::lock_guard guard(lock_);
  unsigned flushed = 0;
  for (auto *texture : all) {
    if (flushed == 4) break;
    if (texture && texture->gxTex && now - texture->lastUsed >= 30000U) {
      GxTexDestroy(texture->gxTex); texture->gxTex = nullptr; ++flushed;
    }
  }
}
int TextureIsUsed(HTEXTURE__ *t) { return t && t->refCount > 0; }
MipBits *TextureLoadImage(const char *p, unsigned *w, unsigned *h,
                          EGxTexFormat *f, int *opaque, CStatus *,
                          unsigned *a) {
  if (!p)
    return nullptr;
  std::vector<std::uint8_t> b;
  std::string resolved = p;
  if (std::filesystem::path(resolved).extension().empty()) resolved += ".blp";
  void *archiveData = nullptr;
  unsigned archiveBytes = 0;
  if (SFile::LoadFile(resolved.c_str(), &archiveData, &archiveBytes, 1, nullptr)) {
    const auto *begin = static_cast<const std::uint8_t *>(archiveData);
    b.assign(begin, begin + archiveBytes);
    SFile::Unload(archiveData);
  } else {
    std::ifstream file(resolved, std::ios::binary);
    if (!file) return nullptr;
    b.assign(std::istreambuf_iterator<char>(file), {});
  }
  auto *m = new MipBits;
  bool isOpaque = true; unsigned alphaDepth = 8;
  const bool decoded = DecodeTga(b, *m, isOpaque) ||
      DecodeBlpPalette(b, *m, isOpaque, alphaDepth) ||
      DecodeBlpDxt(b, *m, isOpaque, alphaDepth);
  if (!decoded) { ReadDims(b, m->width, m->height); m->bits = std::move(b); isOpaque = false; }
  if (w)
    *w = m->width;
  if (h)
    *h = m->height;
  if (f)
    *f = GxTex_Argb8888;
  if (opaque)
    *opaque = isOpaque ? 1 : 0;
  if (a)
    *a = alphaDepth;
  return m;
}
void TextureUnloadImage(void *p) { delete static_cast<MipBits *>(p); }
HTEXTURE__ *TextureCreate(const char *p, CStatus *s, unsigned flags,
                          int noCache) {
  if (!p)
    return nullptr;
  std::string key = Lower(p) + "#" + std::to_string(flags);
  std::lock_guard g(lock_);
  if (!noCache) {
    auto i = cache.find(key);
    if (i != cache.end()) {
      ++i->second->refCount;
      i->second->lastUsed = GetTickCount();
      return i->second;
    }
  }
  auto *t = new CTexture;
  t->filename = p;
  t->gxTexFlags = flags;
  t->lastUsed = GetTickCount();
  unsigned w, h;
  int op;
  std::unique_ptr<MipBits> m(
      TextureLoadImage(p, &w, &h, &t->dataFormat, &op, s, &t->alphaBits));
  if (m) {
    t->gxWidth = w;
    t->gxHeight = h;
    t->opaque = op != 0;
    t->mipBits.push_back(std::move(*m));
  } else
    t->gxWidth = t->gxHeight = 1;
  all.push_back(t);
  if (!noCache)
    cache[key] = t;
  return t;
}
HTEXTURE__ *TextureCreate(unsigned w, unsigned h, EGxTexFormat f,
                          unsigned flags) {
  auto *t = new CTexture;
  t->gxWidth = w;
  t->gxHeight = h;
  t->dataFormat = f;
  t->gxTexFormat = f;
  t->gxTexFlags = flags;
  t->lastUsed = GetTickCount();
  t->mipBits.push_back({w, h, std::vector<std::uint8_t>(w * h * 4)});
  all.push_back(t);
  return t;
}
HTEXTURE__ *TextureCreate(unsigned w, unsigned h, EGxTexFormat d,
                          EGxTexFormat g, unsigned f) {
  auto *t = TextureCreate(w, h, d, f);
  t->gxTexFormat = g;
  return t;
}
HTEXTURE__ *TextureCreate(CGxTex *x) {
  auto *t = new CTexture;
  t->gxTex = x;
  if (x) {
    t->gxWidth = x->width;
    t->gxHeight = x->height;
    t->gxTexFormat = static_cast<EGxTexFormat>(x->format);
  }
  all.push_back(t);
  return t;
}
HTEXTURE__ *TextureCreateSolid(const NTempest::CImVector *c, CStatus *) {
  auto *t = TextureCreate(1, 1, static_cast<EGxTexFormat>(0), 0);
  auto v = c ? *c : NTempest::CImVector(0xFF00FF00);
  t->mipBits[0].bits = {v.b, v.g, v.r, v.a};
  t->opaque = v.a == 255;
  return t;
}
void TextureRelease(HTEXTURE__ *t) {
  if (!t)
    return;
  std::lock_guard g(lock_);
  if (--t->refCount)
    return;
  std::erase_if(cache, [t](const auto &item) { return item.second == t; });
  all.erase(std::remove(all.begin(), all.end(), t), all.end());
  delete t;
}
CGxTex *TextureGetGxTex(HTEXTURE__ *t, int, CStatus *) {
  if (!t)
    return nullptr;
  if (!t->gxTex) {
    if (!GxTexCreate((std::max)(1U, t->gxWidth), (std::max)(1U, t->gxHeight),
                     GxTex_Argb8888, CGxTexFlags(t->gxTexFlags), t,
                     UpdateTextureDefault, &t->gxTex))
      return nullptr;
    GxTexUpdate(t->gxTex, nullptr, true);
  }
  t->lastUsed = GetTickCount();
  return t->gxTex;
}
MipBits *TextureGetMips(HTEXTURE__ *t, int) {
  return t && !t->mipBits.empty() ? t->mipBits.data() : nullptr;
}
int TextureIsOpaque(HTEXTURE__ *t) { return t && t->opaque; }
unsigned TextureCalcMipCount(unsigned w, unsigned h) {
  unsigned n = 1;
  while (w > 1 || h > 1) {
    w = (std::max)(1U, w / 2);
    h = (std::max)(1U, h / 2);
    ++n;
  }
  return n;
}
void TextureGenerateMips(unsigned width, unsigned height,
                         unsigned levelsProvided, unsigned levelsDesired,
                         void *levelBits) {
  if (!levelBits || !width || !height || levelsDesired <= levelsProvided)
    return;
  auto *pixels = static_cast<std::uint8_t *>(levelBits);
  unsigned sourceLevel = levelsProvided ? levelsProvided - 1U : 0U;
  for (unsigned level = (std::max)(1U, levelsProvided); level < levelsDesired;
       ++level) {
    const unsigned sourceWidth = (std::max)(1U, width >> sourceLevel);
    const unsigned sourceHeight = (std::max)(1U, height >> sourceLevel);
    const unsigned targetWidth = (std::max)(1U, width >> level);
    const unsigned targetHeight = (std::max)(1U, height >> level);
    const unsigned sourceOffset = CalcLevelOffset(width, height, sourceLevel, 4);
    const unsigned targetOffset = CalcLevelOffset(width, height, level, 4);
    for (unsigned y = 0; y < targetHeight; ++y)
      for (unsigned x = 0; x < targetWidth; ++x)
        for (unsigned channel = 0; channel < 4; ++channel) {
          unsigned sum = 0;
          for (unsigned dy = 0; dy < 2; ++dy)
            for (unsigned dx = 0; dx < 2; ++dx) {
              const unsigned sx = (std::min)(sourceWidth - 1U, x * 2U + dx);
              const unsigned sy = (std::min)(sourceHeight - 1U, y * 2U + dy);
              sum += pixels[sourceOffset + (sy * sourceWidth + sx) * 4U +
                            channel];
            }
          pixels[targetOffset + (y * targetWidth + x) * 4U + channel] =
              static_cast<std::uint8_t>((sum + 2U) / 4U);
        }
    sourceLevel = level;
  }
}
MipBits *TextureAllocMippedImg(unsigned w, unsigned h, unsigned) {
  return new MipBits{w, h, std::vector<std::uint8_t>(w * h * 4)};
}
unsigned CalcLevelSize(unsigned w, unsigned h, unsigned l, unsigned b) {
  return (std::max)(1U, w >> l) * (std::max)(1U, h >> l) * b;
}
unsigned CalcLevelOffset(unsigned w, unsigned h, unsigned l, unsigned b) {
  unsigned o = 0;
  for (unsigned i = 0; i < l; ++i)
    o += CalcLevelSize(w, h, i, b);
  return o;
}
void TextureCopyMippedImage(MipBits *d, const MipBits *s) {
  if (d && s)
    *d = *s;
}
void TextureFreeMippedImg(void *p) { delete static_cast<MipBits *>(p); }
int TextureDiscoverFileType(const char *p) {
  if (!p)
    return 0;
  auto e = Lower(std::filesystem::path(p).extension().string());
  return e == ".blp" ? 1 : e == ".tga" ? 2 : 0;
}
unsigned TexturePickAlternateFilename(const char *p, const char *e, int,
                                      char *out, unsigned n) {
  if (!p || !e || !out)
    return 0;
  auto x = std::filesystem::path(p).replace_extension(e).string();
  if (x.size() + 1 > n)
    return 0;
  strcpy_s(out, n, x.c_str());
  return std::filesystem::exists(x);
}
unsigned TextureGetUniqueID(HTEXTURE__ *t) {
  return static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(t));
}
void TextureGetDimensions(HTEXTURE__ *t, unsigned *w, unsigned *h) {
  if (w)
    *w = t ? t->gxWidth : 0;
  if (h)
    *h = t ? t->gxHeight : 0;
}
void TextureDestroy() {
  TextureCacheFlush();
  auto copy = all;
  for (auto *t : copy)
    delete t;
  all.clear();
}
const char *TextureGetFilename(HTEXTURE__ *t) {
  return t ? t->filename.c_str() : nullptr;
}
int TextureGetInfo(const char *p, unsigned *w, unsigned *h, EGxTexFormat *f,
                   int *o, CStatus *s) {
  std::unique_ptr<MipBits> m(TextureLoadImage(p, w, h, f, o, s));
  return m ? 1 : 0;
}
void AsyncTextureWait(HTEXTURE__ *t) {
  if (t && t->asyncObject)
    AsyncFileReadWait(t->asyncObject);
}
void RemoveExtension(char *p) {
  if (!p)
    return;
  char *x = std::strrchr(p, '.');
  if (x)
    *x = 0;
}
int IdentifyAndStripFileExtension(char *p, char **e) {
  if (!p)
    return 0;
  char *x = std::strrchr(p, '.');
  if (e)
    *e = x;
  if (!x)
    return 0;
  int type = TextureDiscoverFileType(p);
  *x = 0;
  return type;
}
void GenerateMipMask(char *out, const char *p) {
  if (!out)
    return;
  strcpy_s(out, 260, p ? p : "");
  RemoveExtension(out);
  strcat_s(out, 260, "_%d");
}
