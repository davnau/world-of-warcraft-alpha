#include "ENGINE/Source/Services/ribbonemitter.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace {
unsigned char B(float v) {
  return static_cast<unsigned char>(std::clamp(v, 0.0F, 1.0F) * 255 + 0.5F);
}
} // namespace
CRibbonVertex::~CRibbonVertex() = default;
std::vector<HTEXTURE__ *>
DuplicateTextureArray(const std::vector<HTEXTURE__ *> &textures) {
  return textures;
}
CRibbonEmitter::CRibbonEmitter() = default;
CRibbonEmitter::~CRibbonEmitter() { CloseTextureHandles(); }
CRibbonEmitter::CRibbonEmitter(const CRibbonEmitter &o) { PrivCopy(o); }
CRibbonEmitter &CRibbonEmitter::operator=(const CRibbonEmitter &o) {
  if (this != &o)
    PrivCopy(o);
  return *this;
}
void CRibbonEmitter::PrivCopy(const CRibbonEmitter &o) {
  m_edges = o.m_edges;
  m_writePos = o.m_writePos;
  m_readPos = o.m_readPos;
  m_startTime = o.m_startTime;
  m_prevPos = o.m_prevPos;
  m_prevVertical = o.m_prevVertical;
  m_currVertical = o.m_currVertical;
  m_below0 = o.m_below0;
  m_below1 = o.m_below1;
  m_above0 = o.m_above0;
  m_above1 = o.m_above1;
  m_cameraPos = o.m_cameraPos;
  m_gxVertices = o.m_gxVertices;
  m_gxIndices = o.m_gxIndices;
  m_ooLifeSpan = o.m_ooLifeSpan;
  m_texSlotBox = o.m_texSlotBox;
  m_edgesPerSec = o.m_edgesPerSec;
  m_edgeLifeSpan = o.m_edgeLifeSpan;
  m_materials = o.m_materials;
  m_textures = o.m_textures;
  m_replaces = o.m_replaces;
  m_diffuseClr = o.m_diffuseClr;
  m_texBox = o.m_texBox;
  m_rows = o.m_rows;
  m_cols = o.m_cols;
  m_posSet = o.m_posSet;
  m_initialized = o.m_initialized;
  m_enabled = o.m_enabled;
  m_currPos = o.m_currPos;
  m_texSlot = o.m_texSlot;
  m_above = o.m_above;
  m_below = o.m_below;
  m_gravity = o.m_gravity;
  m_edgeAccumulator = o.m_edgeAccumulator;
}
void CRibbonEmitter::InitInterpDeltas() {
  auto travel = m_currPos - m_prevPos;
  auto view = m_cameraPos - m_currPos;
  auto vertical = NTempest::C3Vector::Cross(travel, view);
  if (vertical.SquaredMag() < 1.0e-8F)
    vertical = NTempest::C3Vector::Cross(view, {0.0F, 0.0F, 1.0F});
  if (vertical.SquaredMag() < 1.0e-8F)
    vertical = {0.0F, 1.0F, 0.0F};
  vertical.Normalize();
  if (!m_posSet)
    m_prevVertical = vertical;
  m_currVertical = vertical;
  m_below0 = m_prevPos - m_prevVertical * m_below;
  m_below1 = m_currPos - m_currVertical * m_below;
  m_above0 = m_prevPos + m_prevVertical * m_above;
  m_above1 = m_currPos + m_currVertical * m_above;
}
void CRibbonEmitter::InterpEdge(float age, float t, unsigned advance) {
  t = std::clamp(t, 0.0F, 1.0F);
  const float gravityOffset = 0.5F * m_gravity * age * age;
  auto below = m_below0 + (m_below1 - m_below0) * t;
  auto above = m_above0 + (m_above1 - m_above0) * t;
  below.z -= gravityOffset;
  above.z -= gravityOffset;
  const float u = m_texSlotBox.l +
                  (m_texSlotBox.r - m_texSlotBox.l) *
                      std::clamp(age * m_ooLifeSpan, 0.0F, 1.0F);
  m_gxVertices.push_back({above, {u, m_texSlotBox.t}});
  m_gxVertices.push_back({below, {u, m_texSlotBox.b}});
  if (advance)
    Advance(&m_writePos, advance);
}
void CRibbonEmitter::Advance(unsigned *p, unsigned n) {
  if (!p || m_edges.empty())
    return;
  *p = (*p + n) % static_cast<unsigned>(m_edges.size());
}
void CRibbonEmitter::ConvertTexSlotToTexCoords() {
  if (!m_cols || !m_rows)
    return;
  float w = 1.0F / m_cols, h = 1.0F / m_rows;
  unsigned x = m_texSlot % m_cols, y = m_texSlot / m_cols;
  m_texSlotBox = {x * w, y * h, (x + 1) * w, (y + 1) * h};
}
void CRibbonEmitter::CloseTextureHandles() { m_textures.clear(); }
void CRibbonEmitter::Initialize(float rate, float life,
                                const NTempest::CImVector *c,
                                const std::vector<HTEXTURE__ *> &t,
                                const std::vector<CRibbonMat> &m,
                                const std::vector<unsigned> &r,
                                const NTempest::CRect *b, int rows, int cols) {
  m_edgesPerSec = std::max(0.0F, rate);
  m_edgeLifeSpan = std::max(0.001F, life);
  m_ooLifeSpan = 1 / m_edgeLifeSpan;
  if (c)
    m_diffuseClr = *c;
  if (b)
    m_texBox = *b;
  m_rows = std::max(1, rows);
  m_cols = std::max(1, cols);
  SetMats(m, t, r);
  ConvertTexSlotToTexCoords();
  m_initialized = true;
}
void CRibbonEmitter::SetPos(const NTempest::C44Matrix *o,
                            const NTempest::C3Vector *c) {
  if (!o || !c)
    throw std::invalid_argument("ribbon position");
  m_prevPos = m_currPos;
  m_prevVertical = m_currVertical;
  m_currPos = {o->d0, o->d1, o->d2};
  m_cameraPos = *c;
  InitInterpDeltas();
  m_posSet = true;
}
void CRibbonEmitter::SetMats(const std::vector<CRibbonMat> &m,
                             const std::vector<HTEXTURE__ *> &t,
                             const std::vector<unsigned> &r) {
  m_materials = m;
  m_textures = t;
  m_replaces = r;
}
unsigned CRibbonEmitter::ReplaceTexture(unsigned id, HTEXTURE__ *t) {
  unsigned n = 0;
  for (unsigned i = 0; i < m_replaces.size() && i < m_textures.size(); ++i)
    if (m_replaces[i] == id) {
      m_textures[i] = t;
      ++n;
    }
  return n;
}
void CRibbonEmitter::MaterialDisableLight(int d) {
  for (auto &m : m_materials)
    m.enableLighting = !d;
}
void CRibbonEmitter::MaterialDisableFog(int d) {
  for (auto &m : m_materials)
    m.enableFog = !d;
}
void CRibbonEmitter::SetColor(float r, float g, float b) {
  m_diffuseClr.r = B(r);
  m_diffuseClr.g = B(g);
  m_diffuseClr.b = B(b);
}
void CRibbonEmitter::SetAlpha(float a) { m_diffuseClr.a = B(a); }
void CRibbonEmitter::SetEnabled(char e) { m_enabled = e != 0; }
void CRibbonEmitter::SetTexSlot(unsigned s) {
  if (s >= m_rows * m_cols)
    throw std::out_of_range("ribbon texture slot");
  m_texSlot = s;
  ConvertTexSlotToTexCoords();
}
void CRibbonEmitter::SetAbove(float v) {
  if (v < 0)
    throw std::invalid_argument("ribbon above");
  m_above = v;
}
void CRibbonEmitter::SetBelow(float v) {
  if (v < 0)
    throw std::invalid_argument("ribbon below");
  m_below = v;
}
void CRibbonEmitter::SetGravity(float v) { m_gravity = v; }
void CRibbonEmitter::SingletonMgrUpdate(float d, const NTempest::C3Vector *c,
                                        int suppress) {
  if (c)
    m_cameraPos = *c;
  if (!m_singletonUpdated) {
    Update(d, suppress);
    m_singletonUpdated = true;
  }
}
void CRibbonEmitter::Update(float d, int suppress) {
  for (float &a : m_edges)
    a += d;
  while (!m_edges.empty() && m_edges.front() >= m_edgeLifeSpan) {
    m_edges.erase(m_edges.begin());
    if (m_gxVertices.size() >= 2)
      m_gxVertices.erase(m_gxVertices.begin(), m_gxVertices.begin() + 2);
  }
  if (m_enabled && m_posSet && !suppress) {
    m_edgeAccumulator += m_edgesPerSec * d;
    const unsigned edgeCount = static_cast<unsigned>(m_edgeAccumulator);
    unsigned edge = 0;
    while (m_edgeAccumulator >= 1) {
      m_edgeAccumulator -= 1;
      m_edges.push_back(0);
      InterpEdge(0.0F, edgeCount ? float(++edge) / float(edgeCount) : 1.0F,
                 0);
    }
  }
  for (std::size_t i = 0; i < m_edges.size() && i * 2U + 1U < m_gxVertices.size(); ++i) {
    const float drop = m_gravity * m_edges[i] * d;
    m_gxVertices[i * 2U].pos.z -= drop;
    m_gxVertices[i * 2U + 1U].pos.z -= drop;
  }
  m_updated = true;
  m_singletonUpdated = false;
}
int CRibbonEmitter::Render() {
  m_gxIndices.resize(m_gxVertices.size());
  for (unsigned i = 0; i < m_gxIndices.size(); ++i)
    m_gxIndices[i] = static_cast<unsigned short>(i);
  return m_gxVertices.size() >= 4;
}
int CRibbonEmitter::IsDead() const { return !m_enabled && m_edges.empty(); }
unsigned CRibbonEmitter::AddRef() { return ++m_refCount; }
void CRibbonEmitter::DecRef() {
  if (!--m_refCount)
    delete this;
}
