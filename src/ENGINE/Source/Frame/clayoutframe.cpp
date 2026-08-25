#include "ENGINE/Source/Frame/clayoutframe.h"
#include "ENGINE/Source/Base/status.h"
#include "ENGINE/Source/Frame/framexml.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {
std::vector<CLayoutFrame*> g_resizePending;

double SynthesizeSide(double center, double opposite, double size)
{
    if (center != CFramePoint::UNDEFINED) {
        if (opposite != CFramePoint::UNDEFINED)
            return center - opposite + center;
        if (size != 0.0)
            return center + size * 0.5;
    }
    if (opposite != CFramePoint::UNDEFINED && size != 0.0)
        return opposite + size;
    return CFramePoint::UNDEFINED;
}

double SynthesizeCenter(double side1, double side2, double size)
{
    if (side1 != CFramePoint::UNDEFINED && side2 != CFramePoint::UNDEFINED)
        return (side1 + side2) * 0.5;
    if (side1 != CFramePoint::UNDEFINED && size != 0.0)
        return side1 + size * 0.5;
    if (side2 != CFramePoint::UNDEFINED && size != 0.0)
        return side2 - size * 0.5;
    return CFramePoint::UNDEFINED;
}

const XMLNode* ChildByName(const XMLNode* node, const char* name)
{
    if (!node || !name) return nullptr;
    for (const XMLNode* child = node->m_child; child; child = child->m_next)
        if (_stricmp(child->m_name.c_str(), name) == 0) return child;
    return nullptr;
}

bool StringToFramePoint(const char* value, FRAMEPOINT& point)
{
    static constexpr const char* names[FRAMEPOINT_MAX] = {
        "TOPLEFT", "TOP", "TOPRIGHT", "LEFT", "CENTER", "RIGHT",
        "BOTTOMLEFT", "BOTTOM", "BOTTOMRIGHT"
    };
    if (!value || !*value) return false;
    for (unsigned index = 0; index < FRAMEPOINT_MAX; ++index) {
        if (_stricmp(value, names[index]) == 0) {
            point = static_cast<FRAMEPOINT>(index);
            return true;
        }
    }
    return false;
}

bool LoadXMLDimensions(const XMLNode* node, float& x, float& y, CStatus* status)
{
    x = 0.0f;
    y = 0.0f;
    if (!node || !node->m_child) {
        if (status && node) status->Add(STATUS_WARNING, "No child node in %s element", node->m_name.c_str());
        return false;
    }
    const XMLNode* dimension = node->m_child;
    float scale = 1.0f;
    if (_stricmp(dimension->m_name.c_str(), "AbsDimension") == 0)
        scale = 0.0009765625f * 0.80000001f;
    else if (_stricmp(dimension->m_name.c_str(), "RelDimension") != 0) {
        if (status) status->Add(STATUS_WARNING, "Unknown child node in %s element: %s",
                                node->m_name.c_str(), dimension->m_name.c_str());
        return false;
    }
    if (const char* value = dimension->GetAttributeByName("x"); value && *value)
        x = std::strtof(value, nullptr) * scale;
    if (const char* value = dimension->GetAttributeByName("y"); value && *value)
        y = std::strtof(value, nullptr) * scale;
    return true;
}

CLayoutFrame* ResolveRelativeFrame(CLayoutFrame* frame, CLayoutFrame* parent, const char* name)
{
    if (!name || !*name) return parent;
    if (_stricmp(name, "$parent") == 0) return parent;

    std::string resolved = name;
    if (_strnicmp(name, "$parent", 7) == 0) {
        auto* parentFrame = dynamic_cast<CSimpleFrame*>(parent);
        while (parentFrame && (!parentFrame->GetName() || !*parentFrame->GetName()))
            parentFrame = parentFrame->ParentFrame();
        if (!parentFrame) return nullptr;
        resolved = parentFrame->GetName();
        resolved += name + 7;
    }

    // Frames are registered after their complete XML subtree is loaded.  A
    // region or child frame can therefore name an ancestor before that
    // ancestor has reached PostLoadXML.  The recovered resolver searches the
    // live parent chain before consulting the global registries.
    for (CLayoutFrame* current = frame; current; current = current->GetLayoutParent()) {
        if (auto* currentFrame = dynamic_cast<CSimpleFrame*>(current)) {
            const char* currentName = currentFrame->GetName();
            if (currentName && _stricmp(currentName, resolved.c_str()) == 0)
                return current;
        } else if (auto* currentRegion = dynamic_cast<CSimpleRegion*>(current)) {
            const char* currentName = currentRegion->GetName();
            if (currentName && _stricmp(currentName, resolved.c_str()) == 0)
                return current;
        }
    }
    return CLayoutFrame::GetLayoutFrameByName(frame, nullptr, resolved.c_str());
}
}

CLayoutFrame::CLayoutFrame()
{
    m_points.fill(nullptr);
    m_pointSet.fill(false);
}

CLayoutFrame::~CLayoutFrame()
{
    DestroyLayout();
}

CLayoutFrame* CFramePoint::GetRelative() const
{
    return nullptr;
}

double CFramePointAbsolute::X(float scale) const
{
    (void)scale;
    return static_cast<double>(m_point.x);
}

double CFramePointAbsolute::Y(float scale) const
{
    (void)scale;
    return static_cast<double>(m_point.y);
}

CLayoutFrame* CFramePointRelative::GetRelative() const
{
    return m_relative;
}

void CLayoutFrame::LoadXML(const XMLNode* node, CStatus* status)
{
    if (!node) return;

    if (const char* inherited = node->GetAttributeByName("inherits"); inherited && *inherited) {
        std::string names = inherited;
        std::size_t begin = 0;
        while (begin <= names.size()) {
            const std::size_t comma = names.find(',', begin);
            std::string name = names.substr(
                begin, comma == std::string::npos ? std::string::npos : comma - begin);
            const auto first = name.find_first_not_of(" \t\r\n");
            const auto last = name.find_last_not_of(" \t\r\n");
            name = first == std::string::npos ? std::string() : name.substr(first, last - first + 1);
            if (!name.empty()) {
                if (const XMLNode* base = FrameXML_FindHashNode(name.c_str()))
                    CLayoutFrame::LoadXML(base, status);
                else if (status)
                    status->Add(STATUS_WARNING, "Couldn't find inherited layout '%s'", name.c_str());
            }
            if (comma == std::string::npos) break;
            begin = comma + 1;
        }
    }

    if (const XMLNode* size = ChildByName(node, "Size")) {
        float width = 0.0f, height = 0.0f;
        if (LoadXMLDimensions(size, width, height, status)) {
            SetWidth(width);
            SetHeight(height);
        }
    }

    CLayoutFrame* parent = GetLayoutParent();
    const char* allPointsValue = node->GetAttributeByName("setAllPoints");
    const bool setAllPoints = allPointsValue && _stricmp(allPointsValue, "true") == 0;
    const XMLNode* anchors = ChildByName(node, "Anchors");
    if (anchors) {
        if (setAllPoints && status)
            status->Add(STATUS_WARNING, "setAllPoints specified together with Anchors");
        for (const XMLNode* anchor = anchors->m_child; anchor; anchor = anchor->m_next) {
            if (_stricmp(anchor->m_name.c_str(), "Anchor") != 0) continue;
            FRAMEPOINT point = FRAMEPOINT_TOPLEFT;
            const char* pointName = anchor->GetAttributeByName("point");
            if (!StringToFramePoint(pointName, point)) {
                if (status) status->Add(STATUS_WARNING, "Invalid anchor point: %s", pointName ? pointName : "");
                continue;
            }
            FRAMEPOINT relativePoint = point;
            const char* relativePointName = anchor->GetAttributeByName("relativePoint");
            if (relativePointName && *relativePointName && !StringToFramePoint(relativePointName, relativePoint)) {
                if (status) status->Add(STATUS_WARNING, "Invalid relative anchor point: %s", relativePointName);
                continue;
            }
            const char* relativeName = anchor->GetAttributeByName("relativeTo");
            CLayoutFrame* relative = ResolveRelativeFrame(this, parent, relativeName);
            if (relativeName && *relativeName && !relative) {
                if (status) status->Add(STATUS_WARNING, "Couldn't find relative frame '%s'", relativeName);
                continue;
            }
            float x = 0.0f, y = 0.0f;
            if (const XMLNode* offset = ChildByName(anchor, "Offset"))
                LoadXMLDimensions(offset, x, y, status);
            SetPoint(point, relative, relativePoint, x, y);
        }
        Resize();
    } else if (setAllPoints && parent) {
        SetAllPoints(parent);
    }
}

void CLayoutFrame::OnFrameSizeChanged()
{
}

void CLayoutFrame::OnFrameResize()
{
}

bool CLayoutFrame::GetRect(NTempest::CRect* rect) const
{
    if (!rect) return false;
    *rect = m_rect;
    return true;
}

CLayoutFrame* CLayoutFrame::GetLayoutFrameByName(CLayoutFrame*, const char* context, const char* name)
{
    if (auto* frame = SimpleFrameRegistryGetEntry(context, name))
        return static_cast<CLayoutFrame*>(frame);
    if (auto* string = SimpleFontStringRegistryGetEntry(context, name))
        return static_cast<CLayoutFrame*>(string);
    if (auto* texture = SimpleTextureRegistryGetEntry(context, name))
        return static_cast<CLayoutFrame*>(texture);
    return nullptr;
}

double CLayoutFrame::GetFirstPointX(const FRAMEPOINT* points, int elements) const
{
    if (!points || elements <= 0) return CFramePoint::UNDEFINED;
    for (int i = 0; i < elements; ++i) {
        const auto point = points[i];
        if (point < FRAMEPOINT_MAX && m_points[point]) {
            const auto value = m_points[point]->X(m_layoutScale);
            if (value != CFramePoint::UNDEFINED) return value;
        }
    }
    return CFramePoint::UNDEFINED;
}

double CLayoutFrame::GetFirstPointY(const FRAMEPOINT* points, int elements) const
{
    if (!points || elements <= 0) return CFramePoint::UNDEFINED;
    for (int i = 0; i < elements; ++i) {
        const auto point = points[i];
        if (point < FRAMEPOINT_MAX && m_points[point]) {
            const auto value = m_points[point]->Y(m_layoutScale);
            if (value != CFramePoint::UNDEFINED) return value;
        }
    }
    return CFramePoint::UNDEFINED;
}

double CLayoutFrame::CenterX()
{
    if (m_layoutGuard & 0x10U) return CFramePoint::UNDEFINED;
    m_layoutGuard |= 0x10U;
    const FRAMEPOINT points[] = {FRAMEPOINT_CENTER, FRAMEPOINT_TOP, FRAMEPOINT_BOTTOM};
    auto result = GetFirstPointX(points, 3);
    if (result == CFramePoint::UNDEFINED)
        result = SynthesizeCenter(Left(), Right(), static_cast<double>(GetWidth()) * m_layoutScale);
    m_layoutGuard &= ~0x10U;
    return result;
}

double CLayoutFrame::CenterY()
{
    if (m_layoutGuard & 0x20U) return CFramePoint::UNDEFINED;
    m_layoutGuard |= 0x20U;
    const FRAMEPOINT points[] = {FRAMEPOINT_CENTER, FRAMEPOINT_LEFT, FRAMEPOINT_RIGHT};
    auto result = GetFirstPointY(points, 3);
    if (result == CFramePoint::UNDEFINED)
        result = SynthesizeCenter(Bottom(), Top(), static_cast<double>(GetHeight()) * m_layoutScale);
    m_layoutGuard &= ~0x20U;
    return result;
}

double CLayoutFrame::Left()
{
    if (m_layoutGuard & 0x01U) return CFramePoint::UNDEFINED;
    m_layoutGuard |= 0x01U;
    const FRAMEPOINT points[] = {FRAMEPOINT_TOPLEFT, FRAMEPOINT_LEFT, FRAMEPOINT_BOTTOMLEFT};
    auto result = GetFirstPointX(points, 3);
    if (result == CFramePoint::UNDEFINED)
        result = SynthesizeSide(CenterX(), Right(), -static_cast<double>(GetWidth()) * m_layoutScale);
    m_layoutGuard &= ~0x01U;
    return result;
}

double CLayoutFrame::Top()
{
    if (m_layoutGuard & 0x02U) return CFramePoint::UNDEFINED;
    m_layoutGuard |= 0x02U;
    const FRAMEPOINT points[] = {FRAMEPOINT_TOPLEFT, FRAMEPOINT_TOP, FRAMEPOINT_TOPRIGHT};
    auto result = GetFirstPointY(points, 3);
    if (result == CFramePoint::UNDEFINED)
        result = SynthesizeSide(CenterY(), Bottom(), static_cast<double>(GetHeight()) * m_layoutScale);
    m_layoutGuard &= ~0x02U;
    return result;
}

double CLayoutFrame::Right()
{
    if (m_layoutGuard & 0x04U) return CFramePoint::UNDEFINED;
    m_layoutGuard |= 0x04U;
    const FRAMEPOINT points[] = {FRAMEPOINT_TOPRIGHT, FRAMEPOINT_RIGHT, FRAMEPOINT_BOTTOMRIGHT};
    auto result = GetFirstPointX(points, 3);
    if (result == CFramePoint::UNDEFINED)
        result = SynthesizeSide(CenterX(), Left(), static_cast<double>(GetWidth()) * m_layoutScale);
    m_layoutGuard &= ~0x04U;
    return result;
}

double CLayoutFrame::Bottom()
{
    if (m_layoutGuard & 0x08U) return CFramePoint::UNDEFINED;
    m_layoutGuard |= 0x08U;
    const FRAMEPOINT points[] = {FRAMEPOINT_BOTTOMLEFT, FRAMEPOINT_BOTTOM, FRAMEPOINT_BOTTOMRIGHT};
    auto result = GetFirstPointY(points, 3);
    if (result == CFramePoint::UNDEFINED)
        result = SynthesizeSide(CenterY(), Top(), -static_cast<double>(GetHeight()) * m_layoutScale);
    m_layoutGuard &= ~0x08U;
    return result;
}

void CLayoutFrame::FreePoints()
{
    for (auto*& point : m_points) {
        if (point) {
            if (auto* relative = point->GetRelative())
                relative->UnregisterResize(this);
        }
        delete point;
        point = nullptr;
    }
    m_pointSet.fill(false);
}

void CLayoutFrame::Clear(CLayoutFrame* relative, bool doResize)
{
    for (std::size_t index = 0; index < m_points.size(); ++index) {
        CFramePoint*& point = m_points[index];
        if (point && point->GetRelative() == relative) {
            delete point;
            point = nullptr;
            m_pointSet[index] = false;
        }
    }
    if (doResize)
        Resize();
}

void CLayoutFrame::ClearAllPoints(bool doResize)
{
    FreePoints();
    if (doResize)
        Resize();
}

void CLayoutFrame::SetPoint(FRAMEPOINT point, CLayoutFrame* relative,
                            FRAMEPOINT relativePoint, float x, float y)
{
    if (point >= FRAMEPOINT_MAX || !relative || relative == this) return;
    if (m_points[point]) {
        if (auto* oldRelative = m_points[point]->GetRelative())
            oldRelative->UnregisterResize(this);
    }
    delete m_points[point];
    auto* value = new CFramePointRelative;
    value->m_relative = relative;
    value->m_framePoint = relativePoint;
    value->m_offset = {x, y};
    m_points[point] = value;
    m_pointSet[point] = true;
    relative->RegisterResize(this);
    Resize();
}

void CLayoutFrame::SetPoint(FRAMEPOINT point, CFramePoint* framePoint)
{
    if (point >= FRAMEPOINT_MAX) {
        delete framePoint;
        return;
    }
    if (m_points[point]) {
        if (auto* oldRelative = m_points[point]->GetRelative())
            oldRelative->UnregisterResize(this);
    }
    delete m_points[point];
    m_points[point] = framePoint;
    m_pointSet[point] = framePoint != nullptr;
    if (framePoint) {
        if (auto* relative = framePoint->GetRelative())
            relative->RegisterResize(this);
    }
    Resize();
}

void CLayoutFrame::SetAllPoints(CLayoutFrame* relative)
{
    SetPoint(FRAMEPOINT_TOPLEFT, relative, FRAMEPOINT_TOPLEFT, 0.0f, 0.0f);
    SetPoint(FRAMEPOINT_BOTTOMRIGHT, relative, FRAMEPOINT_BOTTOMRIGHT, 0.0f, 0.0f);
    SetPoint(FRAMEPOINT_TOPRIGHT, relative, FRAMEPOINT_TOPRIGHT, 0.0f, 0.0f);
    SetPoint(FRAMEPOINT_BOTTOMLEFT, relative, FRAMEPOINT_BOTTOMLEFT, 0.0f, 0.0f);
}

void CLayoutFrame::CalculateRect()
{
    const auto left = Left();
    const auto bottom = Bottom();
    const auto right = Right();
    const auto top = Top();
    if (left == CFramePoint::UNDEFINED || bottom == CFramePoint::UNDEFINED ||
        right == CFramePoint::UNDEFINED || top == CFramePoint::UNDEFINED)
        return;
    m_rect.l = static_cast<float>(left);
    m_rect.t = static_cast<float>(bottom);
    m_rect.r = static_cast<float>(right);
    m_rect.b = static_cast<float>(top);
}

void CLayoutFrame::RegisterResize(CLayoutFrame* dependent)
{
    if (!dependent || dependent == this || IsResizeDependency(dependent)) return;
    const auto found = std::find_if(m_resizeList.begin(), m_resizeList.end(),
        [dependent](const FRAMENODE& node) { return node.frame == dependent; });
    if (found == m_resizeList.end())
        m_resizeList.push_back({dependent, nullptr, 0x0FU});
    else
        found->dependency |= 0x0FU;
}

void CLayoutFrame::UnregisterResize(CLayoutFrame* dependent)
{
    std::erase_if(m_resizeList,
        [dependent](const FRAMENODE& node) { return node.frame == dependent; });
}

bool CLayoutFrame::IsResizeDependency(const CLayoutFrame* dependency) const
{
    if (!dependency) return false;
    for (const auto* point : m_points) {
        if (!point) continue;
        const auto* relative = point->GetRelative();
        if (relative && (relative == dependency || relative->IsResizeDependency(dependency)))
            return true;
    }
    return false;
}

void CLayoutFrame::SetDeferredResize(bool deferred)
{
    m_deferredResize = deferred;
    if (!deferred) Resize();
}

void CLayoutFrame::Resize()
{
    if (m_deferredResize) {
        if (std::find(g_resizePending.begin(), g_resizePending.end(), this) == g_resizePending.end())
            g_resizePending.push_back(this);
        m_resizePending = true;
        return;
    }
    if (m_resizeInProgress) return;
    m_resizeInProgress = true;
    const auto oldRect = m_rect;
    CalculateRect();
    m_resizePending = false;
    OnFrameResize();
    if (oldRect.l != m_rect.l || oldRect.r != m_rect.r ||
        oldRect.t != m_rect.t || oldRect.b != m_rect.b)
        OnFrameSizeChanged();
    const auto dependents = m_resizeList;
    m_resizeInProgress = false;
    for (const auto& node : dependents)
        if (node.frame) node.frame->Resize();
}

bool CLayoutFrame::IsResizePending() const { return m_resizePending; }

void CLayoutFrame::SetRect(const NTempest::CRect& rect)
{
    const auto oldRect = m_rect;
    m_rect = rect;
    if (oldRect.l != m_rect.l || oldRect.r != m_rect.r ||
        oldRect.t != m_rect.t || oldRect.b != m_rect.b)
        OnFrameSizeChanged();
    const auto dependents = m_resizeList;
    for (const auto& node : dependents)
        if (node.frame) node.frame->Resize();
}

void CLayoutFrame::SetLayoutScale(float scale)
{
    m_layoutScale = std::max(0.0001f, scale);
    Resize();
}

void CLayoutFrame::SetWidth(float width)
{
    m_width = width;
    Resize();
}

void CLayoutFrame::SetHeight(float height)
{
    m_height = height;
    Resize();
}

float CLayoutFrame::GetHeight() const { return m_height; }
float CLayoutFrame::GetWidth() const { return m_width; }

void CLayoutFrame::FlattenFrame()
{
    for (auto*& point : m_points) {
        if (auto* relative = dynamic_cast<CFramePointRelative*>(point)) {
            auto* absolute = new CFramePointAbsolute;
            absolute->m_point = {static_cast<float>(point->X(m_layoutScale)),
                                 static_cast<float>(point->Y(m_layoutScale))};
            delete relative;
            point = absolute;
        }
    }
}

void CLayoutFrame::ScaleBy(float scale)
{
    m_rect.l *= scale; m_rect.r *= scale; m_rect.t *= scale; m_rect.b *= scale;
    for (auto* point : m_points) {
        if (auto* absolute = dynamic_cast<CFramePointAbsolute*>(point))
            absolute->m_point = scale * absolute->m_point;
        if (auto* relative = dynamic_cast<CFramePointRelative*>(point))
            relative->m_offset = scale * relative->m_offset;
    }
    OnFrameSizeChanged();
}

void CLayoutFrame::DragBy(float x, float y)
{
    m_rect.l += x; m_rect.r += x; m_rect.t += y; m_rect.b += y;
    for (auto* point : m_points) {
        if (auto* absolute = dynamic_cast<CFramePointAbsolute*>(point)) {
            absolute->m_point.x += x;
            absolute->m_point.y += y;
        }
    }
    OnFrameSizeChanged();
}

bool CLayoutFrame::PtInFrameRect(float x, float y) const
{
    return x >= m_rect.l && x <= m_rect.r && y >= m_rect.t && y <= m_rect.b;
}

void CLayoutFrame::CageMouseInFrame(float* x, float* y) const
{
    if (x) *x = std::clamp(*x, m_rect.l, m_rect.r);
    if (y) *y = std::clamp(*y, m_rect.t, m_rect.b);
}

void CLayoutFrame::DestroyLayout()
{
    // Match the original ownership order. A frame owns the resize-list nodes,
    // while each dependent owns the CFramePoint objects that refer back to it.
    // Remove those points before destroying the list so dependents cannot keep
    // stale relative-frame pointers after this object has been freed.
    FreePoints();
    for (const FRAMENODE& node : m_resizeList) {
        if (node.frame)
            node.frame->Clear(this, true);
    }
    m_resizeList.clear();
    RemoveFromResizeList();
}

void CLayoutFrame::ResizePending()
{
    auto pending = std::move(g_resizePending);
    g_resizePending.clear();
    for (auto* frame : pending) if (frame) frame->Resize();
}

void CLayoutFrame::RemoveFromResizeList()
{
    g_resizePending.erase(std::remove(g_resizePending.begin(), g_resizePending.end(), this), g_resizePending.end());
    m_resizePending = false;
}

void CLayoutFrame::ClearResizePendingList()
{
    for (auto* frame : g_resizePending) if (frame) frame->m_resizePending = false;
    g_resizePending.clear();
}
