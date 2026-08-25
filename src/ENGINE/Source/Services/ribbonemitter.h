#pragma once
#include "ENGINE/Source/Services/camera.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include <vector>
class CTexture;
using HTEXTURE__ = CTexture;
struct CRibbonVertex {
  ~CRibbonVertex();
  NTempest::C3Vector pos{};
  NTempest::C2Vector texCoord{};
};
struct CRibbonMat {
  CRibbonMat();
  bool enableLighting = true, enableFog = true, enableDepthTest = true,
       enableDepthWrite = true, enableCulling = true;
  int alpha = 0;
};
std::vector<HTEXTURE__ *>
DuplicateTextureArray(const std::vector<HTEXTURE__ *> &textures);
class CRibbonEmitter {
public:
  CRibbonEmitter();
  ~CRibbonEmitter();
  CRibbonEmitter(const CRibbonEmitter &);
  CRibbonEmitter &operator=(const CRibbonEmitter &);
  void PrivCopy(const CRibbonEmitter &);
  void InitInterpDeltas();
  void InterpEdge(float age, float t, unsigned advance);
  void Advance(unsigned *pos, unsigned amount);
  void ConvertTexSlotToTexCoords();
  void CloseTextureHandles();
  void Initialize(float edgesPerSec, float life, const NTempest::CImVector *,
                  const std::vector<HTEXTURE__ *> &,
                  const std::vector<CRibbonMat> &,
                  const std::vector<unsigned> &, const NTempest::CRect *,
                  int rows, int cols);
  void SetPos(const NTempest::C44Matrix *, const NTempest::C3Vector *);
  void SetMats(const std::vector<CRibbonMat> &,
               const std::vector<HTEXTURE__ *> &,
               const std::vector<unsigned> &);
  unsigned ReplaceTexture(unsigned, HTEXTURE__ *);
  void MaterialDisableLight(int);
  void MaterialDisableFog(int);
  void SetColor(float, float, float);
  void SetAlpha(float);
  void SetEnabled(char);
  void SetTexSlot(unsigned);
  void SetAbove(float);
  void SetBelow(float);
  void SetGravity(float);
  void SingletonMgrUpdate(float, const NTempest::C3Vector *, int);
  void Update(float, int);
  int Render();
  int IsDead() const;
  unsigned AddRef();
  void DecRef();
  unsigned m_refCount = 1;
  std::vector<float> m_edges;
  unsigned m_writePos = 0, m_readPos = 0;
  float m_startTime = 0;
  NTempest::C3Vector m_prevPos{}, m_cameraPos{};
  NTempest::C3Vector m_prevVertical{0.0F, 0.0F, 1.0F},
      m_currVertical{0.0F, 0.0F, 1.0F};
  NTempest::C3Vector m_below0{}, m_below1{}, m_above0{}, m_above1{};
  std::vector<CRibbonVertex> m_gxVertices;
  std::vector<unsigned short> m_gxIndices;
  float m_ooLifeSpan = 1;
  NTempest::CRect m_texSlotBox{};
  float m_edgesPerSec = 0, m_edgeLifeSpan = 1;
  std::vector<CRibbonMat> m_materials;
  std::vector<HTEXTURE__ *> m_textures;
  std::vector<unsigned> m_replaces;
  NTempest::CImVector m_diffuseClr{0xFFFFFFFF};
  NTempest::CRect m_texBox{};
  unsigned m_rows = 1, m_cols = 1;
  bool m_posSet = false, m_initialized = false, m_enabled = true,
       m_updated = false, m_singletonUpdated = false;
  NTempest::C3Vector m_currPos{};
  unsigned m_texSlot = 0;
  float m_above = 0, m_below = 0, m_gravity = 0, m_edgeAccumulator = 0;
};
