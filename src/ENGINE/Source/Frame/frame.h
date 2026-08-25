#pragma once

// Shared typed contract for the recovered Frame subsystem.
//
// The recovered translation units were exported as Hex-Rays listings and did
// not contain usable C++ declarations.  This header keeps the original class
// and method names while expressing the state that the listings operate on in
// normal C++ types.  The small containers here intentionally model the old
// intrusive lists as ordinary ownership-aware vectors; callers still see the
// same Frame API and the same lifetime rules.

#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Gx/gxapi/gxbase.h"
#include "ENGINE/Source/Services/textblock.h"
#include "ENGINE/Source/Tempest/caabox.h"
#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "ENGINE/Source/Tempest/crect.h"
#include "ENGINE/Source/Tempest/c3vector.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cctype>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

struct XMLAttribute {
    std::string name;
    std::string value;
};

class XMLNode {
public:
    std::string m_name;
    std::string m_value;
    std::vector<XMLAttribute> m_attributes;
    XMLNode* m_child = nullptr;
    XMLNode* m_next = nullptr;

    const char* GetAttributeByName(const char* attribute) const
    {
        if (!attribute) return nullptr;
        for (const auto& item : m_attributes) {
            if (item.name.size() != std::char_traits<char>::length(attribute)) continue;
            bool equal = true;
            for (std::size_t i = 0; i < item.name.size(); ++i) {
                const auto left = static_cast<unsigned char>(item.name[i]);
                const auto right = static_cast<unsigned char>(attribute[i]);
                if (std::tolower(left) != std::tolower(right)) {
                    equal = false;
                    break;
                }
            }
            if (equal) return item.value.c_str();
        }
        return nullptr;
    }
};

class CStatus;
class CTexture;
class CBackdropGenerator;
class CLayoutFrame;
class CSimpleFrame;
class CSimpleTop;
class CSimpleRegion;
class CSimpleTexture;
class CSimpleFontString;
class CSimpleHyperlinkButton;
class CSimpleMessageFrame;
class CSimpleMessageScrollFrame;
class CSimpleEditBox;
class CSimpleSlider;
class CSimpleStatusBar;
class CSimpleModel;
class CSimpleHyperlinkedFrame;
struct CModelCreate;
struct CModel;
struct HOBJECT__;
struct HCAMERA__;
struct lua_State;
struct HMODEL__;
using HMODEL = HMODEL__*;

enum FRAMEPOINT {
    FRAMEPOINT_TOPLEFT,
    FRAMEPOINT_TOP,
    FRAMEPOINT_TOPRIGHT,
    FRAMEPOINT_LEFT,
    FRAMEPOINT_CENTER,
    FRAMEPOINT_RIGHT,
    FRAMEPOINT_BOTTOMLEFT,
    FRAMEPOINT_BOTTOM,
    FRAMEPOINT_BOTTOMRIGHT,
    FRAMEPOINT_MAX
};

enum CSimpleButtonState {
    BUTTONSTATE_DISABLED,
    BUTTONSTATE_NORMAL,
    BUTTONSTATE_PUSHED,
    NUM_BUTTONSTATES
};

enum CSimpleSliderOrientation { SLIDER_HORIZONTAL, SLIDER_VERTICAL };

enum SIMPLE_EVENT : unsigned {
    SIMPLE_EVENT_NONE = 0,
    SIMPLE_EVENT_MOUSE = 1u << 0,
    SIMPLE_EVENT_KEYBOARD = 1u << 1,
    SIMPLE_EVENT_CHAR = 1u << 2,
    SIMPLE_EVENT_UPDATE = 1u << 3,
    SIMPLE_EVENT_RENDER = 1u << 4,
    SIMPLE_EVENT_ALL = 0xffffffffu
};

enum FRAME_STRATA : int {
    FRAME_STRATA_BACKGROUND,
    FRAME_STRATA_LOW,
    FRAME_STRATA_MEDIUM,
    FRAME_STRATA_HIGH,
    FRAME_STRATA_DIALOG,
    FRAME_STRATA_FULLSCREEN,
    FRAME_STRATA_FULLSCREEN_DIALOG
};

struct FrameScript_Method {
    const char* name = nullptr;
    int (*function)(lua_State*) = nullptr;
};

using FrameScriptDispatcher = int (*)(lua_State*, const char* functionName);
void FrameScript_SetDispatcher(FrameScriptDispatcher dispatcher);
int FrameScript_Invoke(lua_State* state, const char* functionName);
int FrameScript_DispatchBuiltin(lua_State* state, const char* functionName);

class FrameScript_Object {
public:
    FrameScript_Object();
    virtual ~FrameScript_Object();

    void RegisterScriptObject(const char* name);
    void UnregisterScriptObject(const char* name);
    int RegisterScriptEvent(const char* eventName);
    void UnregisterScriptEvent(const char* eventName);
    void UnregisterAllScriptEvents();
    void SetEventScript(int& slot, const char* body, const char* debugName = nullptr);
    void OnScriptEvent(const char* eventName);
    void OnScriptEvent(const char* eventName, const char* format, va_list arguments);

    static void FillScriptMethodTable(std::vector<FrameScript_Method>& table,
                                      std::initializer_list<FrameScript_Method> methods);
    static void EmptyScriptMethodTable(std::vector<FrameScript_Method>& table);
    static int LookupScriptMethod(lua_State* state);
    static int LookupScriptMethod(lua_State* state, const char* name,
                                  const std::vector<FrameScript_Method>& table);
    virtual int LookupScriptMethod(lua_State* state, const char* name);

    int m_onEvent = 0;
    int m_luaRegistered = 0;
    int m_luaObjectRef = -2;
    std::string m_scriptObjectName;
    std::vector<unsigned> m_scriptEvents;
};

class CFramePoint {
public:
    static constexpr float UNDEFINED = std::numeric_limits<float>::infinity();
    virtual ~CFramePoint() = default;
    virtual CLayoutFrame* GetRelative() const;
    virtual double X(float) const { return UNDEFINED; }
    virtual double Y(float) const { return UNDEFINED; }
};

class CLayoutFrame {
public:
    struct FRAMENODE {
        CLayoutFrame* frame = nullptr;
        FRAMENODE* next = nullptr;
        unsigned dependency = 0;
    };

    NTempest::CRect m_rect{};
    bool m_attachmentOrigin = false;
    float m_layoutScale = 1.0f;
    float m_width = 0.0f;
    float m_height = 0.0f;
    unsigned m_layoutGuard = 0;
    std::array<CFramePoint*, FRAMEPOINT_MAX> m_points{};
    std::array<bool, FRAMEPOINT_MAX> m_pointSet{};
    bool m_resizePending = false;
    bool m_deferredResize = false;
    bool m_resizeInProgress = false;
    std::vector<FRAMENODE> m_resizeList;

    CLayoutFrame();
    virtual ~CLayoutFrame();

    virtual CLayoutFrame* GetLayoutParent() const;
    virtual bool GetRect(NTempest::CRect* rect) const;
    virtual bool IsAttachmentOrigin() const { return m_attachmentOrigin; }
    virtual void LoadXML(const XMLNode*, CStatus*);
    virtual void OnFrameSizeChanged();
    virtual void OnFrameResize();

    static CLayoutFrame* GetLayoutFrameByName(CLayoutFrame*, const char* context, const char* name);
    double GetFirstPointX(const FRAMEPOINT* pointarray, int elements) const;
    double GetFirstPointY(const FRAMEPOINT* pointarray, int elements) const;
    double CenterX();
    double CenterY();
    double Left();
    double Top();
    double Right();
    double Bottom();

    void FreePoints();
    void Clear(CLayoutFrame* relative, bool doResize = true);
    void ClearAllPoints(bool doResize = true);
    void SetPoint(FRAMEPOINT point, CLayoutFrame* relative, FRAMEPOINT relativePoint,
                  float x, float y);
    void SetPoint(FRAMEPOINT point, CFramePoint* framePoint);
    void SetAllPoints(CLayoutFrame* relative);
    void CalculateRect();
    void RegisterResize(CLayoutFrame* dependency);
    void UnregisterResize(CLayoutFrame* dependency);
    bool IsResizeDependency(const CLayoutFrame* dependency) const;
    void SetDeferredResize(bool deferred);
    void Resize();
    bool IsResizePending() const;
    void SetRect(const NTempest::CRect& rect);
    void SetLayoutScale(float scale);
    void SetWidth(float width);
    void SetHeight(float height);
    virtual float GetHeight() const;
    virtual float GetWidth() const;
    void FlattenFrame();
    void ScaleBy(float scale);
    void DragBy(float x, float y);
    bool PtInFrameRect(float x, float y) const;
    void CageMouseInFrame(float* x, float* y) const;
    void DestroyLayout();
    static void ResizePending();
    void RemoveFromResizeList();
    static void ClearResizePendingList();
};

class CFramePointAbsolute final : public CFramePoint {
public:
    NTempest::C2Vector m_point{};
    double X(float scale) const override;
    double Y(float scale) const override;
};

class CFramePointRelative final : public CFramePoint {
public:
    CLayoutFrame* m_relative = nullptr;
    FRAMEPOINT m_framePoint = FRAMEPOINT_TOPLEFT;
    NTempest::C2Vector m_offset{};
    CLayoutFrame* GetRelative() const override;
    double X(float scale) const override;
    double Y(float scale) const override;
};

class CSimpleRegion : public FrameScript_Object, public CLayoutFrame {
public:
    CSimpleFrame* m_frame = nullptr;
    std::string m_name;
    unsigned m_registryContext = 0;
    bool m_visible = true;
    float m_alpha = 1.0f;
    NTempest::CImVector m_vertexColor{0xffffffffu};

    explicit CSimpleRegion(CSimpleFrame* parent = nullptr);
    ~CSimpleRegion() override;
    CLayoutFrame* GetLayoutParent() const override;
    const char* GetName() const;
    void SetName(const char* name);
    void Show();
    void Hide();
    bool IsVisible() const;
    void SetAlpha(float alpha);
    float GetAlpha() const;
    void SetVertexColor(const NTempest::CImVector& color);
    const NTempest::CImVector& GetVertexColor() const;
    void OnGxColorChanged();
    void SetFrame(CSimpleFrame* frame);
    void OnRegionChanged();
    void SetPoint(FRAMEPOINT point, CLayoutFrame* relative, FRAMEPOINT relativePoint,
                  float x, float y) { CLayoutFrame::SetPoint(point, relative, relativePoint, x, y); }
    void ClearAllPoints() { CLayoutFrame::ClearAllPoints(); }
};

struct CSimpleBatchedTexture {
    CSimpleTexture* texture = nullptr;
    NTempest::CRect rect{};
    float alpha = 1.0f;
};

struct RENDERCALLBACKNODE {
    std::function<void()> callback;
};

class CRenderBatch {
public:
    unsigned m_count = 0;
    std::vector<CSimpleBatchedTexture> m_texturelist;
    std::vector<CSimpleFontString*> m_strings;
    std::vector<RENDERCALLBACKNODE> m_callbacks;

    CRenderBatch();
    ~CRenderBatch();
    void QueueTexture(CSimpleTexture* texture, const NTempest::CRect* rect = nullptr);
    void QueueFontString(CSimpleFontString* string);
    void QueueCallback(std::function<void()> callback);
    void Finish();
    void Clear();
};

class CSimpleRender {
public:
    static void DrawBatch(CRenderBatch* batch);
};

void SortByTexture(CRenderBatch* batch);
void AnimFinishedCallback(CSimpleModel* model);

class CSimpleTexture : public CSimpleRegion {
public:
    std::string m_texture;
    CTexture* m_textureHandle = nullptr;
    EGxBlend m_alphamode = EGxBlend::GxBlend_Alpha;
    std::array<NTempest::C3Vector, 4> m_position{};
    std::array<NTempest::C2Vector, 4> m_texCoord{};
    bool m_TexCoordModifiesPosition = false;

    explicit CSimpleTexture(CSimpleFrame* parent = nullptr);
    ~CSimpleTexture() override;
    CLayoutFrame* GetLayoutFrameByName(const char* context, const char* name);
    void PreLoadXML(const XMLNode*, CStatus*);
    void LoadXML(const XMLNode*, CStatus*);
    void PostLoadXML(const XMLNode*, CStatus*);
    void AddToRegistry(const char* context = nullptr);
    bool SetTexture(const char* texture, int flags = 0);
    void SetTexture(void* texture);
    void SetTexture(const NTempest::CImVector* color);
    void SetBlendMode(EGxBlend mode);
    void SetTexCoord(const NTempest::C2Vector* coords);
    void SetTexCoord(float left, float right, float top, float bottom);
    void TexCorrectRect(NTempest::CRect* rect) const;
    void SetPosition(const NTempest::C3Vector* positions);
    void OnFrameSizeChanged() override;
    void Draw(CRenderBatch* batch = nullptr);
    void ClearFromSimpleRegistry();
    const char* GetName() const;
    CGxTex* GetTexture() const;
    float GetWidth() const override;
    float GetHeight() const override;
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CSimpleFontStringAttributes {
public:
    int m_flags = 0;
    std::string m_font;
    float m_fontHeight = 12.0f;
    unsigned m_fontFlags = 0;
    float m_spacing = 0.0f;
    unsigned m_styleFlags = 0;
    NTempest::CImVector m_color{0xffffffffu};
    NTempest::CImVector m_shadowColor{};
    NTempest::C2Vector m_shadowOffset{};
    CSimpleFontStringAttributes();
    ~CSimpleFontStringAttributes();
    void UpdateString(CSimpleFontString* string) const;
    CSimpleFontStringAttributes& operator=(const CSimpleFontStringAttributes& other);
    void CopyFlags(const CSimpleFontStringAttributes& other);
};

class CSimpleFontString : public CSimpleRegion {
public:
    std::string m_font;
    HTEXTFONT m_fontHandle = nullptr;
    HTEXTBLOCK m_string = nullptr;
    float m_fontHeight = 0.0f;
    unsigned m_fontFlags = 0;
    int m_textMaxSize = 0;
    std::string m_text;
    float m_spacing = 0.0f;
    float m_cachedWidth = 0.0f;
    float m_cachedHeight = 0.0f;
    bool m_autoWidth = true;
    bool m_autoHeight = true;
    NTempest::CImVector m_shadowColor{};
    NTempest::C2Vector m_shadowOffset{};
    NTempest::C2Vector m_justificationOffset{};
    int m_alphaGradientStart = -1;
    int m_alphaGradientLength = 0;
    unsigned m_styleFlags = 658;

    explicit CSimpleFontString(CSimpleFrame* parent = nullptr);
    ~CSimpleFontString() override;
    CLayoutFrame* GetLayoutFrameByName(const char* context, const char* name);
    void PreLoadXML(const XMLNode*, CStatus*);
    void LoadXML(const XMLNode*, CStatus*);
    void PostLoadXML(const XMLNode*, CStatus*);
    void AddToRegistry(const char* context = nullptr);
    void SetFont(const char* font, float height, unsigned flags = 0);
    void SetTextHeight(float height);
    void SetTextLength(int length);
    void SetText(const char* text);
    const char* GetText() const;
    void SetTextColor(const NTempest::CImVector& color);
    void OnGxColorChanged();
    void SetSpacing(float spacing);
    void AddShadow(const NTempest::CImVector& color, const NTempest::C2Vector& offset);
    void RemoveShadow();
    void SetAlphaGradient(int start, int length);
    void UpdateString();
    float GetStringWidth() const;
    float GetWidth() const override;
    float GetStringHeight() const;
    float GetHeight() const override;
    float GetTextWidth() const;
    void WrapText(float width);
    int GetNumCharsWithinWidth(float width) const;
    int GetNumCharsWithinWidthFromEnd(float width) const;
    void SetJustificationOffset(const NTempest::C2Vector& offset);
    void Draw(CRenderBatch* batch = nullptr);
    void ClearFromSimpleRegistry();
    const char* GetName() const;
    void SetLayoutScale(float scale);
    void OnFrameSizeChanged() override;
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CBackdropGenerator {
public:
    CSimpleTexture* m_backgroundTexture = nullptr;
    CSimpleTexture* m_leftTexture = nullptr;
    CSimpleTexture* m_rightTexture = nullptr;
    CSimpleTexture* m_topTexture = nullptr;
    CSimpleTexture* m_bottomTexture = nullptr;
    CSimpleTexture* m_topLeftTexture = nullptr;
    CSimpleTexture* m_topRightTexture = nullptr;
    CSimpleTexture* m_bottomLeftTexture = nullptr;
    CSimpleTexture* m_bottomRightTexture = nullptr;
    std::string m_background;
    std::string m_border;
    unsigned m_pieces = 0;
    int m_tileBackground = 0;
    int m_blendAll = 0;
    float m_cornerSize = 0.0f;
    float m_backgroundSize = 0.0f;
    float m_topInset = 0.0f;
    float m_bottomInset = 0.0f;
    float m_leftInset = 0.0f;
    float m_rightInset = 0.0f;
    NTempest::CImVector m_color{0xffffffffu};
    NTempest::CImVector m_borderColor{0xffffffffu};

    CBackdropGenerator();
    ~CBackdropGenerator();
    void LoadXML(const XMLNode*, CStatus*);
    void SetOutput(CSimpleFrame* output);
    void Generate(const NTempest::CRect* rect);
    void SetVertexColor(const NTempest::CImVector* color);
    void GetVertexColor(NTempest::CImVector* color) const;
    void SetBorderVertexColor(const NTempest::CImVector* color);
    void GetBorderVertexColor(NTempest::CImVector* color) const;
};

class CSimpleFrame : public FrameScript_Object, public CLayoutFrame {
public:
    CSimpleTop* m_top = nullptr;
    CSimpleFrame* m_parent = nullptr;
    CSimpleFrame* m_tooltip = nullptr;
    unsigned m_frameRegContext = 0;
    unsigned m_flags = 0;
    int m_id = 0;
    std::string m_frameName;
    int m_strata = FRAME_STRATA_MEDIUM;
    int m_level = 0;
    unsigned char m_alpha = 255;
    unsigned m_eventmask = 0;
    bool m_shown = true;
    bool m_visible = false;
    NTempest::CRect m_hitRect{};
    NTempest::CRect m_hitOffset{};
    bool m_highlightLocked = false;
    unsigned m_lookForDrag = 0;
    bool m_mouseDown = false;
    bool m_dragging = false;
    MOUSEBUTTON m_dragButton = MOUSE_BUTTON_NONE;
    NTempest::C2Vector m_clickPoint{};
    bool m_loading = false;
    int m_onLoad = 0, m_onSizeChanged = 0, m_onUpdate = 0, m_onShow = 0, m_onHide = 0;
    int m_onEnter = 0, m_onLeave = 0, m_onMouseDown = 0, m_onMouseUp = 0;
    int m_onMouseWheel = 0, m_onDragStart = 0, m_onDragStop = 0, m_onReceiveDrag = 0;
    int m_onChar = 0, m_onKeyDown = 0, m_onKeyUp = 0;
    std::array<bool, 5> m_drawenabled{true, true, true, true, false};
    std::unique_ptr<CBackdropGenerator> m_backdrop;
    unsigned m_batchDirty = 0x1fu;
    std::vector<CSimpleRegion*> m_regions;
    std::array<std::vector<CSimpleRegion*>, 5> m_drawlayers;
    std::array<CRenderBatch, 5> m_batch;
    std::vector<CSimpleFrame*> m_children;

    explicit CSimpleFrame(CSimpleFrame* parent = nullptr);
    ~CSimpleFrame() override;
    CLayoutFrame* GetLayoutParent() const override;
    CSimpleFrame* GetToplevelFrame() const;
    bool IsAncestor(const CSimpleFrame* frame) const;
    const char* GetName() const { return m_frameName.c_str(); }
    void SetName(const char* name) { m_frameName = name ? name : ""; }
    void PreLoadXML(const XMLNode*, CStatus*);
    void LoadXML(const XMLNode*, CStatus*) override;
    void LoadXML_Layers(const XMLNode*, CStatus*);
    virtual void LoadXML_Scripts(const XMLNode*, CStatus*);
    void PostLoadXML(const XMLNode*, CStatus*);
    void DelayedDelete();
    CLayoutFrame* GetLayoutFrameByName(const char* context, const char* name);
    void SetFrameFlag(unsigned flag, bool set);
    void SetBeingScrolled(bool value);
    void SetFrameStrata(int strata);
    void SetFrameLevel(int level);
    void Raise();
    void Lower();
    void SetBackdrop(CBackdropGenerator* backdrop);
    void SetHighlight(bool highlight);
    int SetHighlight(CStatus* texture, EGxBlend blendMode);
    int SetHighlight(CSimpleTexture* texture, EGxBlend blendMode);
    void SetAlpha(float alpha);
    void EnableDrawLayer(unsigned layer);
    void DisableDrawLayer(unsigned layer);
    void RegisterRegion(CSimpleRegion* region, unsigned layer = 0);
    void UnregisterRegion(CSimpleRegion* region);
    void AddFrameRegion(CSimpleRegion* region, unsigned layer = 0);
    void RemoveFrameRegion(CSimpleRegion* region);
    void NotifyDrawLayerChanged(CSimpleRegion* region);
    void NotifyDrawLayersChanged();
    void AddToFrameRegistry(const char* context = nullptr);
    void ClearFromSimpleRegistry();
    CSimpleFrame* ParentFrame() const { return m_parent; }
    void ParentFrame(CSimpleFrame* frame);
    void UnparentFrame();
    void ClearChildrenFromSimpleRegistry();
    void SetParent(CSimpleFrame* parent);
    void SetDeferredResize(bool deferred);
    void SetLayoutScale(float scale);
    void HideThis();
    void ShowThis();
    void EnableEvent(unsigned eventMask, unsigned flags = 0xffffffffu);
    void DisableEvent(unsigned eventMask);
    void RegisterForEvents(unsigned events) { EnableEvent(events); }
    void RegisterForEvents();
    void UnregisterForEvents(unsigned events) { DisableEvent(events); }
    void UnregisterForEvents();
    bool TestHitRect(float x, float y) const;
    void SetHitRect(const NTempest::CRect& rect);
    void SetHitRectInsets(const NTempest::CRect& insets);
    NTempest::CRect GetHitRect() const;
    virtual void OnLayerShow();
    virtual void OnLayerHide();
    virtual void OnLayerUpdate(float elapsedSec);
    virtual void OnLayerTrackUpdate(float elapsedSec);
    void OnUpdateBatch(unsigned layer);
    virtual void OnFrameRender(CRenderBatch* batch, unsigned layer);
    virtual void OnFrameRender();
    void OnFrameSizeChanged() override;
    void OnFrameSizeChanged(const NTempest::CRect* rect);
    void OnFrameSizeChanged(float width, float height);
    virtual void OnLayerCursorEnter();
    virtual void OnLayerCursorExit();
    virtual int OnLayerChar(CCharEvent* event);
    virtual int OnLayerKeyDown(CKeyEvent* event);
    virtual int OnLayerKeyDownRepeat(CKeyEvent* event);
    virtual int OnLayerKeyUp(CKeyEvent* event);
    virtual int OnLayerMouseDown(CMouseEvent* event);
    virtual int OnLayerMouseUp(CMouseEvent* event);
    virtual int OnLayerMouseMoveRelative(CMouseEvent* event);
    virtual int OnLayerMouseWheel(CMouseEvent* event);
    virtual void OnDragStart(MOUSEBUTTON button);
    virtual void OnDragStop(MOUSEBUTTON button);
    virtual void OnReceiveDrag();
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CSimpleTitleRegion : public CLayoutFrame {
public:
    CLayoutFrame* m_parent = nullptr;
    CLayoutFrame* GetLayoutParent() const override;
};

class CSimpleButton : public CSimpleFrame {
public:
    CSimpleButtonState m_state = BUTTONSTATE_DISABLED;
    bool m_stateLocked = false;
    unsigned m_clickAction = 256;
    CSimpleFontString* m_disabledText = nullptr;
    CSimpleFontString* m_text = nullptr;
    CSimpleFontString* m_highlightText = nullptr;
    NTempest::C2Vector m_pressedOffset{0.001f, -0.001f};
    std::array<CSimpleTexture*, 3> m_textures{};
    CSimpleTexture* m_activeTexture = nullptr;
    int m_onClick = 0;
    std::function<void(CSimpleButton&)> m_clickHandler;
    std::function<void(CSimpleButton&, bool)> m_trackHandler;

    explicit CSimpleButton(CSimpleFrame* parent = nullptr);
    ~CSimpleButton() override;
    virtual void LockHighlight(int lock);
    void LoadXML(const XMLNode*, CStatus*) override;
    void LoadXML_Scripts(const XMLNode*, CStatus*) override;
    void SetText(CSimpleFontString* text);
    void SetDisabledText(CSimpleFontString* text);
    void SetHighlightText(CSimpleFontString* text);
    void SetTextString(const char* text);
    void SetDisabledTextString(const char* text);
    void SetHighlightTextString(const char* text);
    void SetTextColor(const NTempest::CImVector& color);
    void SetDisabledTextColor(const NTempest::CImVector& color);
    void SetHighlightTextColor(const NTempest::CImVector& color);
    void SetPressedOffset(const NTempest::C2Vector& offset);
    bool SetStateTexture(CSimpleButtonState state, const char* texture);
    void SetStateTexture(CSimpleButtonState state, CSimpleTexture* texture);
    virtual void Enable(int enabled);
    void OnLayerHide() override;
    int OnLayerMouseDown(CMouseEvent* event) override;
    int OnLayerMouseUp(CMouseEvent* event) override;
    void OnDragStart(MOUSEBUTTON button) override;
    void OnLayerCursorEnter() override;
    void OnLayerCursorExit() override;
    virtual void OnClick(MOUSEBUTTON button);
    void SetClickAction(unsigned action);
    void RegisterClick(std::function<void(CSimpleButton&)> handler);
    void RegisterTrack(std::function<void(CSimpleButton&, bool)> handler);
    virtual void SetButtonState(CSimpleButtonState state, bool lock = false);
    void UpdateTextState(CSimpleButtonState state);
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

struct CSimpleButtonClickEvent : CEvent {
    MOUSEBUTTON button = MOUSE_BUTTON_NONE;
};

class CSimpleCheckbox : public CSimpleButton {
public:
    bool m_checked = false;
    CSimpleTexture* m_checkedTexture = nullptr;
    CSimpleTexture* m_disabledTexture = nullptr;
    explicit CSimpleCheckbox(CSimpleFrame* parent = nullptr);
    ~CSimpleCheckbox() override;
    void LoadXML(const XMLNode*, CStatus*) override;
    void SetCheckedTexture(const char* texture);
    void SetCheckedTexture(CSimpleTexture* texture);
    void SetDisabledCheckedTexture(const char* texture);
    void SetDisabledCheckedTexture(CSimpleTexture* texture);
    void Enable(int enabled) override;
    void SetChecked(int state, int force = 0);
    void OnClick(MOUSEBUTTON click) override;
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CSimpleEditBox : public CSimpleFrame {
public:
    std::string m_text;
    int m_textLengthMax = 0;
    int m_textLettersMax = 0;
    int m_cursorPos = 0;
    int m_highlightLeft = 0;
    int m_highlightRight = 0;
    bool m_password = false;
    bool m_multiline = false;
    bool m_autoFocus = true;
    bool m_clauseHighlightVisible = false;
    bool m_candidatesVisible = false;
    bool m_cursorVisible = false;
    unsigned m_imeInputMode = 0;
    int m_visiblePos = 0;
    int m_visibleLen = 0;
    std::vector<std::string> m_history;
    int m_curHistory = -1;
    NTempest::CRect m_editTextInset{};
    CSimpleFontString* m_string = nullptr;
    CSimpleTexture* m_cursor = nullptr;
    float m_cursorBlinkSpeed = 0.5f;
    float m_blinkElapsedTime = 0.0f;
    std::array<int, 6> m_actions{};
    int m_onEnterPressed = 0;
    int m_onEscapePressed = 0;
    int m_onSpacePressed = 0;
    int m_onTabPressed = 0;
    int m_onTextChanged = 0;
    int m_onTextSet = 0;
    bool m_textChangedPending = false;

    explicit CSimpleEditBox(CSimpleFrame* parent = nullptr);
    ~CSimpleEditBox() override;
    void LoadXML(const XMLNode*, CStatus*) override;
    void LoadXML_Scripts(const XMLNode*, CStatus*) override;
    void SetMultiLine(int multiline);
    void SetAutoFocus(int autoFocus);
    void SetEditTextInsets(const NTempest::CRect& insets);
    void OnLayerShow() override;
    void OnLayerHide() override;
    void OnLayerUpdate(float elapsedSec) override;
    void OnLayerTrackUpdate(float elapsedSec) override;
    void OnFrameSizeChanged() override;
    int OnLayerChar(CCharEvent* event) override;
    void OnLayerIme(CImeEvent* event);
    int OnLayerKeyDown(CKeyEvent* event) override;
    int OnLayerKeyDownRepeat(CKeyEvent* event) override;
    int OnLayerKeyUp(CKeyEvent* event) override;
    int OnLayerMouseDown(CMouseEvent* event) override;
    int OnLayerMouseUp(CMouseEvent* event) override;
    void CreateClauseHighlight();
    void CreateCandidatesFrame();
    void UpdateLanguageIndicator();
    void UpdateClauseInfo();
    void PopulateCandidates();
    void DispatchAction(unsigned action);
    void UpdateSizes();
    void UpdateTextInfo();
    int GetNumToLen(int number) const;
    int GetLenToNum(int length) const;
    int NextCharOffset(int offset) const;
    int PrevCharOffset(int offset) const;
    int GetOffsetToLine(int offset) const;
    void GrowText(int size);
    void SetText(const char* text, bool dispatch = true);
    void Insert(int offset, const char* text);
    void Insert(const char* text);
    void Delete(int count);
    void DeleteForward();
    void DeleteForwardWord();
    void DeleteBackward();
    void DeleteBackwardWord();
    void DeleteToStart();
    void DeleteToEnd();
    void DeleteText(int left, int right);
    void DeleteSubstring(int left, int right);
    void Move(int offset, bool extend = false);
    void MoveForward(bool extend = false);
    void MoveForwardWord(bool extend = false);
    void MoveBackward(bool extend = false);
    void MoveBackwardWord(bool extend = false);
    void MoveToStart(bool extend = false);
    void MoveToEnd(bool extend = false);
    void MoveLine(int direction);
    void MoveForwardLine();
    void MoveBackwardLine();
    void HighlightText(int left, int right);
    void StartHighlight();
    void ExtendHighlight(int offset);
    void DeleteHighlight();
    void SetHistoryLines(const std::vector<std::string>& history);
    void AddHistoryLine(const char* line);
    void ForwardHistory();
    void BackwardHistory();
    int ConvertCoordinateToIndex(float x, float y) const;
    void MakeTextVisible();
    void UpdateVisibleText();
    void UpdateVisibleHighlight();
    void UpdateVisibleCursor();
    void UpdateHighlightArea();
    void CopyToClipboard() const;
    void PasteFromClipboard();
    void SetFont(const char* font, float height);
    void ShowCandidates();
    void HideCandidates();
    void SetKeyboardFocus();
    void ClearKeyboardFocus();
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CSimpleHTML : public CSimpleFrame {
public:
    std::string m_text;
    std::vector<CSimpleRegion*> m_content;
    explicit CSimpleHTML(CSimpleFrame* parent = nullptr);
    ~CSimpleHTML() override;
    void LoadXML(const XMLNode*, CStatus*) override;
    void SetText(const char* text);
    void ClearContent();
    void ParseBODY(std::string_view body);
    void ParseP(std::string_view paragraph);
    void ParseIMG(std::string_view image);
    void AddText(std::string_view text);
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CSimpleHyperlinkButton : public CSimpleButton {
public:
    std::string m_hyperlink;
    CSimpleHyperlinkedFrame* m_owner = nullptr;
    explicit CSimpleHyperlinkButton(CSimpleFrame* parent = nullptr);
    ~CSimpleHyperlinkButton() override;
    void SetHyperlink(const char* hyperlink);
    void OnLayerCursorEnter() override;
    void OnLayerCursorExit() override;
    void OnClick(MOUSEBUTTON button);
};

class CSimpleHyperlinkedFrame : public CSimpleFrame {
public:
    std::vector<std::unique_ptr<CSimpleHyperlinkButton>> m_hyperlinks;
    explicit CSimpleHyperlinkedFrame(CSimpleFrame* parent = nullptr);
    ~CSimpleHyperlinkedFrame() override;
    void LoadXML_Scripts(const XMLNode*, CStatus*) override;
    void OnHyperlinkEnter(CSimpleHyperlinkButton* button);
    void OnHyperlinkLeave(CSimpleHyperlinkButton* button);
    void OnHyperlinkClick(CSimpleHyperlinkButton* button);
    CSimpleHyperlinkButton* CreateHyperlinkButton(const char* hyperlink);
    void ReleaseHyperlinkButton(CSimpleHyperlinkButton* button);
};

struct MessageData {
    std::string text;
    NTempest::CImVector color{0xffffffffu};
    float timeVisible = 0.0f;
    bool permanent = false;
    ~MessageData();
};

struct CSimpleMessageFrameLineNode : TRefCnt {
    NTempest::CImVector color{0xffffffffu};
    CSimpleFontString* string = nullptr;
    float timeLeft = 0.0f;
    float fadeLeft = 0.0f;
    bool permanent = false;
    bool isVisible = false;
    CSimpleMessageFrameLineNode();
    ~CSimpleMessageFrameLineNode() override;
};

struct CSimpleMessageFrameLine {
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    CSimpleMessageFrameLineNode* stringNode = nullptr;
    CSimpleMessageFrameLine();
    ~CSimpleMessageFrameLine();
    CSimpleMessageFrameLine(const CSimpleMessageFrameLine&) = delete;
    CSimpleMessageFrameLine& operator=(const CSimpleMessageFrameLine&) = delete;
    CSimpleMessageFrameLine(CSimpleMessageFrameLine&& other) noexcept;
    CSimpleMessageFrameLine& operator=(CSimpleMessageFrameLine&& other) noexcept;
};

class CSimpleMessageFrame : public CSimpleFrame {
public:
    enum SimpleMessageFrameInsertMode { INSERT_AT_TOP, INSERT_AT_BOTTOM };
    unsigned m_rows = 0;
    unsigned m_numVisible = 0;
    NTempest::CRect m_messageFrameArea{};
    NTempest::CRect m_messageFrameInset{};
    int m_textMaxSize = 0;
    CSimpleFontStringAttributes m_attrib;
    float m_fadeDuration = 0.0f;
    SimpleMessageFrameInsertMode m_insertMode = INSERT_AT_BOTTOM;
    std::vector<MessageData> m_pendingMessages;
    std::vector<CSimpleMessageFrameLine> m_lines;
    explicit CSimpleMessageFrame(CSimpleFrame* parent = nullptr);
    ~CSimpleMessageFrame() override;
    void LoadXML(const XMLNode*, CStatus*) override;
    void SetMessageFrameInsets(const NTempest::CRect& inset);
    void SetTextLength(int length);
    void SetInsertMode(SimpleMessageFrameInsertMode mode);
    void AddMessage(const char* text, const NTempest::CImVector& color, float timeVisible, bool permanent);
    void Clear();
    void ClearPending();
    void OnFrameSizeChanged() override;
    void OnLayerUpdate(float elapsedSec) override;
    void AddPendingMessage(const MessageData& message);
    void ScrollMessages(int lines);
    void HideLineNode(CSimpleMessageFrameLineNode* node);
    void ShowLineNode(CSimpleMessageFrameLineNode* node);
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

struct CSimpleMessageScrollFrameLine {
    std::string string;
    CSimpleFontStringAttributes attrib;
    bool isVisible = true;
    float timeLeft = 0.0f;
    float fadeLeft = 0.0f;
    ~CSimpleMessageScrollFrameLine();
};

struct CSimpleMessageScrollFrameDisplayNode : TRefCnt {
    CSimpleFontString* string = nullptr;
    CSimpleMessageScrollFrameLine* line = nullptr;
    CSimpleFontStringAttributes attrib;
    CSimpleMessageScrollFrameDisplayNode();
    ~CSimpleMessageScrollFrameDisplayNode() override;
    CSimpleMessageScrollFrameDisplayNode(const CSimpleMessageScrollFrameDisplayNode&) = delete;
    CSimpleMessageScrollFrameDisplayNode& operator=(const CSimpleMessageScrollFrameDisplayNode&) = delete;
    CSimpleMessageScrollFrameDisplayNode(CSimpleMessageScrollFrameDisplayNode&& other) noexcept;
    CSimpleMessageScrollFrameDisplayNode& operator=(CSimpleMessageScrollFrameDisplayNode&& other) noexcept;
};

class CSimpleMessageScrollFrame : public CSimpleHyperlinkedFrame {
public:
    int m_numMessages = 0, m_maxMessages = 0, m_currentLine = 0, m_currentScroll = 0;
    int m_numDisplayed = 0, m_atTop = 1, m_atBottom = 1, m_textMaxSize = 0;
    CSimpleFontStringAttributes m_attrib;
    int m_fading = 0;
    float m_fadeDuration = 0.0f, m_timeVisible = 0.0f;
    NTempest::CRect m_messageFrameArea{}, m_messageFrameInset{};
    std::vector<CSimpleMessageScrollFrameLine> m_lines;
    std::vector<CSimpleMessageScrollFrameDisplayNode> m_displayNodes;
    explicit CSimpleMessageScrollFrame(CSimpleFrame* parent = nullptr);
    ~CSimpleMessageScrollFrame() override;
    void LoadXML(const XMLNode*, CStatus*) override;
    void SetMaxLines(int lines);
    void SetMessageFrameInsets(const NTempest::CRect& inset);
    void SetTextLength(int length);
    void SetFading(int fading);
    void SetTimeVisible(float time);
    void AddMessage(const char* text, const CSimpleFontStringAttributes* attrib = nullptr,
                    float timeVisible = 0.0f);
    void AddMultiLine(const char* text, const CSimpleFontStringAttributes* attrib = nullptr,
                      float timeVisible = 0.0f);
    void Clear();
    void OnFrameSizeChanged() override;
    void OnLayerUpdate(float elapsedSec) override;
    void UpdateDisplay();
    void ScrollMessages(int lines);
    void PageUp();
    void PageDown();
    void ScrollUp();
    void ScrollDown();
    void ScrollToTop();
    void ScrollToBottom();
    void UpdateNode(CSimpleMessageScrollFrameDisplayNode* node);
    void RefreshMessages();
    void RefreshHyperlinks();
    bool IsAtTop() const;
    bool IsAtBottom() const;
    void SetCurrentScroll(int scroll);
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CSimpleModel : public CSimpleFrame {
public:
    HMODEL m_model = nullptr;
    std::shared_ptr<CModel> m_modelOwner;
    HCAMERA__* m_camera = nullptr;
    CGxLight m_light{};
    NTempest::C3Vector m_position{};
    float m_facing = 0.0f, m_scale = 1.0f;
    unsigned m_flags = 0;
    NTempest::CImVector m_fogColor{};
    float m_fogNear = 0.0f, m_fogFar = 0.0f;
    NTempest::CAaBox m_cachedExtents{};
    unsigned m_cameraIndex = 0;
    unsigned m_sequenceIndex = 0;
    float m_sequenceTime = 0.0f;
    bool m_fogEnabled = false;
    int m_onUpdateModel = 0, m_onAnimFinished = 0;
    explicit CSimpleModel(CSimpleFrame* parent = nullptr);
    ~CSimpleModel() override;
    void LoadXML(const XMLNode*, CStatus*) override;
    void LoadXML_Scripts(const XMLNode*, CStatus*) override;
    void SetModel(HMODEL model);
    void SetModel(const char* sourcefile);
    void SetModel(const char* sourcefile, const CModelCreate* createData,
                  CStatus* status = nullptr);
    void SetModel(HOBJECT__* object, HMODEL model);
    void ClearModel();
    void SetCameraInternal(HOBJECT__* object, HCAMERA__* camera);
    void FinishLoadingModel(HMODEL model);
    void SetCamera(HCAMERA__* camera);
    void SetCameraByIndex(unsigned index);
    void SetLight(const CGxLight* light);
    void SetSequence(unsigned index);
    int SetSequenceTime(unsigned index, float timeOffset);
    int AdvanceTime(float elapsedSec);
    void ReplaceTexture(unsigned materialID, CStatus* textureName);
    bool ModelJustLoaded() const;
    void SetPosition(const NTempest::C3Vector& position);
    void SetFacing(float facing);
    void SetScale(float scale);
    void SetFogColor(const NTempest::CImVector& color);
    void SetFogRange(float nearPlane, float farPlane);
    void OnLayerUpdate(float elapsedSec) override;
    void OnFrameRender(CRenderBatch* batch, unsigned layer) override;
    void CalculateBounds();
    float GetWidth() const override;
    float GetHeight() const override;
    void SetAlpha(float alpha);
    void UpdateModel(HMODEL model);
    void RenderModel();
    const NTempest::CImVector* GetFogColor() const;
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CSimpleScrollFrame : public CSimpleFrame {
public:
    CSimpleFrame* m_scrollChild = nullptr;
    NTempest::CRect m_scrollChildRect{};
    float m_horizontalScroll = 0.0f, m_verticalScroll = 0.0f;
    float m_horizontalRange = 0.0f, m_verticalRange = 0.0f;
    explicit CSimpleScrollFrame(CSimpleFrame* parent = nullptr);
    ~CSimpleScrollFrame() override;
    void LoadXML(const XMLNode*, CStatus*) override;
    void LoadXML_Scripts(const XMLNode*, CStatus*) override;
    void SetScrollChild(CSimpleFrame* child);
    void SetHorizontalScroll(float value);
    void SetVerticalScroll(float value);
    float GetHorizontalScroll() const { return m_horizontalScroll; }
    float GetVerticalScroll() const { return m_verticalScroll; }
    float GetHorizontalScrollRange() const { return m_horizontalRange; }
    float GetVerticalScrollRange() const { return m_verticalRange; }
    void UpdateScrollChildRect();
    void UpdateScrollChild();
    void OnLayerUpdate(float elapsedSec) override;
    void OnFrameRender(CRenderBatch* batch, unsigned layer) override;
    void OnFrameSizeChanged() override;
    void RenderScrollChild();
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

NTempest::CRect GetScrollChildRect(const CSimpleScrollFrame* frame);

class CSimpleSlider : public CSimpleFrame {
public:
    CSimpleTexture* m_thumbTexture = nullptr;
    CSimpleSliderOrientation m_orientation = SLIDER_HORIZONTAL;
    float m_minValue = 0.0f, m_maxValue = 1.0f, m_value = 0.0f, m_valueStep = 0.0f;
    bool m_draggingThumb = false;
    explicit CSimpleSlider(CSimpleFrame* parent = nullptr);
    ~CSimpleSlider() override;
    void LoadXML(const XMLNode*, CStatus*) override;
    void LoadXML_Scripts(const XMLNode*, CStatus*) override;
    void SetThumbTexture(CSimpleTexture* texture);
    void SetOrientation(CSimpleSliderOrientation orientation);
    void SetMinMaxValues(float min, float max);
    void SetValue(float value);
    void SetValueStep(float step);
    float GetMinValue() const { return m_minValue; }
    float GetMaxValue() const { return m_maxValue; }
    float GetValue() const { return m_value; }
    float GetValueStep() const { return m_valueStep; }
    void OnLayerUpdate(float elapsedSec) override;
    void OnLayerTrackUpdate(float elapsedSec) override;
    void OnFrameSizeChanged() override;
    int OnLayerMouseDown(CMouseEvent* event) override;
    int OnLayerMouseUp(CMouseEvent* event) override;
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

class CSimpleStatusBar : public CSimpleFrame {
public:
    bool m_changed = false, m_rangeSet = false, m_valueSet = false;
    float m_minValue = 0.0f, m_maxValue = 1.0f, m_value = 0.0f;
    CSimpleTexture* m_barTexture = nullptr;
    int m_onValueChanged = 0;
    explicit CSimpleStatusBar(CSimpleFrame* parent = nullptr);
    ~CSimpleStatusBar() override;
    void SetStatusBarColor(const NTempest::CImVector& color);
    void LoadXML(const XMLNode*, CStatus*) override;
    void LoadXML_Scripts(const XMLNode*, CStatus*) override;
    bool SetBarTexture(const char* texture, int layer = 0);
    void SetBarTexture(CSimpleTexture* texture, unsigned layer = 0);
    void SetMinMaxValues(float min, float max);
    void SetValue(float value);
    double GetAnimValue() const;
    void OnLayerUpdate(float elapsedSec) override;
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();
    int LookupScriptMethod(lua_State* state, const char* name);
};

struct CFrameStrataNode;

class CFrameStrata {
public:
    std::vector<CFrameStrataNode*> nodes;
    ~CFrameStrata();
    bool FrameOccluded(const CSimpleFrame* frame) const;
    void CheckOcclusion();
};

struct CFrameStrataNode {
    CSimpleFrame* frame = nullptr;
    std::array<CRenderBatch, 5> batches;
    void BuildBatches();
};

class CSimpleTop : public CLayoutFrame {
public:
    static CSimpleTop* s_instance;
    std::vector<CSimpleFrame*> m_frames;
    CSimpleFrame* m_mouseFocus = nullptr;
    CSimpleFrame* m_mouseCapture = nullptr;
    CSimpleFrame* m_keyboardFocus = nullptr;
    CSimpleFrame* m_keyboardFallback = nullptr;
    std::array<CSimpleFrame*, KEY_LAST> m_keyCapture{};
    CSimpleFrame* m_moveFrame = nullptr;
    MOUSEBUTTON m_moveButton = MOUSE_BUTTON_NONE;
    NTempest::C2Vector m_cursor{};
    bool m_eventsEnabled = false;
    std::uint32_t m_screenLayer = 0;
    std::uint32_t m_cursorLayer = 0;

    CSimpleTop();
    ~CSimpleTop();
    void EnumerateFrames(int (*callback)(CSimpleFrame*, void*), void* param);
    void ValidateDeletedFrame(CSimpleFrame* frame);
    void RegisterFrame(CSimpleFrame* frame);
    void UnregisterFrame(CSimpleFrame* frame);
    void NotifyFrameMovedOrResized(CSimpleFrame* frame);
    void NotifyFrameLayerChanged(CSimpleFrame* frame);
    void RegisterForEvent(CSimpleFrame* frame, unsigned eventId);
    void UnregisterForEvent(CSimpleFrame* frame, unsigned eventId);
    void RegisterForDelete(CSimpleFrame* frame);
    void SetCursor(float x, float y);
    void RaiseFrame(CSimpleFrame* frame);
    void LowerFrame(CSimpleFrame* frame);
    void StartMoveOrResizeFrame(CSimpleFrame* frame, MOUSEBUTTON button);
    void MoveOrResizeFrame(float x, float y);
    void StopMoveOrResizeFrame();
    void OnLayerUpdate(float elapsedSec);
    void OnLayerRender();
    void EnableEvents();
    void DisableEvents();
    void DrawCursor();
    int OnChar(CCharEvent* event);
    void OnIme(CImeEvent* event);
    int OnKeyDown(CKeyEvent* event);
    int OnKeyUp(CKeyEvent* event);
    int OnKeyDownRepeat(CKeyEvent* event);
    void OnMouseMove(CMouseEvent* event);
    void OnMouseMoveRelative(CMouseEvent* event);
    void OnMouseDown(CMouseEvent* event);
    void OnMouseUp(CMouseEvent* event);
    int OnMouseWheel(CMouseEvent* event);
    void OnDisplaySizeChanged(int width, int height);
};

// Registry API preserved from SimpleFrameRegistry.cpp.
struct SIMPLEFRAMEREGHASH;
struct SIMPLETEXTUREREGHASH;
struct SIMPLEFONTSTRINGREGHASH;

SIMPLEFRAMEREGHASH* GetSimpleFrameHash(char* context, unsigned nameHash,
                                       int create, int* alreadyExisted);
SIMPLETEXTUREREGHASH* GetSimpleTextureHash(char* context, unsigned nameHash,
                                           int create, int* alreadyExisted);
SIMPLEFONTSTRINGREGHASH* GetSimpleFontStringHash(char* context, unsigned nameHash,
                                                 int create, int* alreadyExisted);

void SimpleFrameRegistryAddEntry(CSimpleFrame* object, const char* context, const char* name);
void SimpleTextureRegistryAddEntry(CSimpleTexture* object, const char* context, const char* name);
void SimpleFontStringRegistryAddEntry(CSimpleFontString* object, const char* context, const char* name);
void SimpleFrameRegistryRemoveEntry(CSimpleFrame* object);
void SimpleTextureRegistryRemoveEntry(CSimpleTexture* object);
void SimpleFontStringRegistryRemoveEntry(CSimpleFontString* object);
CSimpleFrame* SimpleFrameRegistryGetEntry(const char* context, const char* name);
CSimpleTexture* SimpleTextureRegistryGetEntry(const char* context, const char* name);
CSimpleFontString* SimpleFontStringRegistryGetEntry(const char* context, const char* name);
void SimpleFrameRegistryClear();

// Script entry points are intentionally kept as free functions, matching the
// recovered export names.  The runtime bridge can bind them to Lua later.
int CSimpleButton_Enable(lua_State*);
int CSimpleButton_Disable(lua_State*);
int CSimpleButton_IsEnabled(lua_State*);
int CSimpleButton_GetButtonState(lua_State*);
int CSimpleButton_SetButtonState(lua_State*);
int CSimpleButton_SetText(lua_State*);
int CSimpleButton_SetTextColor(lua_State*);
int CSimpleButton_SetNormalTexture(lua_State*);
int CSimpleButton_SetPushedTexture(lua_State*);
int CSimpleButton_SetDisabledTexture(lua_State*);
int CSimpleButton_SetHighlightTexture(lua_State*);
int CSimpleButton_GetText(lua_State*);
int CSimpleButton_GetTextWidth(lua_State*);
int CSimpleButton_GetTextHeight(lua_State*);
int CSimpleButton_RegisterForClicks(lua_State*);
int CSimpleButton_Click(lua_State*);
int CSimpleButton_LockHighlight(lua_State*);
int CSimpleButton_UnlockHighlight(lua_State*);
const char* ButtonStateToString(CSimpleButtonState state);
bool StringToButtonState(const char* text, CSimpleButtonState* state);

int CSimpleCheckbox_SetChecked(lua_State*);
int CSimpleCheckbox_GetChecked(lua_State*);
int CSimpleCheckbox_SetCheckedTexture(lua_State*);
int CSimpleCheckbox_SetDisabledCheckedTexture(lua_State*);

int CSimpleEditBox_Insert(lua_State*);
int CSimpleEditBox_SetText(lua_State*);
int CSimpleEditBox_GetText(lua_State*);
int CSimpleEditBox_AddHistoryLine(lua_State*);
int CSimpleEditBox_SetTextInsets(lua_State*);
int CSimpleEditBox_SetTextColor(lua_State*);
int CSimpleEditBox_SetFocus(lua_State*);
int CSimpleEditBox_ClearFocus(lua_State*);

int CSimpleFrame_GetParent(lua_State*);
int CSimpleFrame_GetName(lua_State*);
int CSimpleFrame_GetFrameLevel(lua_State*);
int CSimpleFrame_SetFrameLevel(lua_State*);
int CSimpleFrame_RegisterEvent(lua_State*);
int CSimpleFrame_UnregisterEvent(lua_State*);
int CSimpleFrame_SetAlpha(lua_State*);
int CSimpleFrame_GetAlpha(lua_State*);
int CSimpleFrame_SetID(lua_State*);
int CSimpleFrame_GetID(lua_State*);
int CSimpleFrame_EnableDrawLayer(lua_State*);
int CSimpleFrame_DisableDrawLayer(lua_State*);
int CSimpleFrame_Show(lua_State*);
int CSimpleFrame_Hide(lua_State*);
int CSimpleFrame_IsVisible(lua_State*);
int CSimpleFrame_IsShown(lua_State*);
int CSimpleFrame_Raise(lua_State*);
int CSimpleFrame_Lower(lua_State*);
int CSimpleFrame_GetCenter(lua_State*);
int CSimpleFrame_GetWidth(lua_State*);
int CSimpleFrame_SetWidth(lua_State*);
int CSimpleFrame_GetHeight(lua_State*);
int CSimpleFrame_SetHeight(lua_State*);
int CSimpleFrame_SetPoint(lua_State*);
int CSimpleFrame_SetAllPoints(lua_State*);
int CSimpleFrame_ClearAllPoints(lua_State*);
int CSimpleFrame_RegisterForDrag(lua_State*);
int CSimpleFrame_EnableMouse(lua_State*);
int CSimpleFrame_EnableKeyboard(lua_State*);
int CSimpleFrame_SetBackdropColor(lua_State*);
int CSimpleFrame_SetBackdropBorderColor(lua_State*);

int CSimpleHTML_SetText(lua_State*);
int CSimpleHTML_SetTextColor(lua_State*);
int CSimpleMessageFrame_AddMessage(lua_State*);

int CSimpleScrollFrame_SetHorizontalScroll(lua_State*);
int CSimpleScrollFrame_SetVerticalScroll(lua_State*);
int CSimpleScrollFrame_GetHorizontalScroll(lua_State*);
int CSimpleScrollFrame_GetVerticalScroll(lua_State*);
int CSimpleScrollFrame_GetHorizontalScrollRange(lua_State*);
int CSimpleScrollFrame_GetVerticalScrollRange(lua_State*);
int CSimpleScrollFrame_UpdateScrollChildRect(lua_State*);

int CSimpleSlider_SetMinMaxValues(lua_State*);
int CSimpleSlider_GetMinMaxValues(lua_State*);
int CSimpleSlider_SetValue(lua_State*);
int CSimpleSlider_GetValue(lua_State*);
int CSimpleSlider_SetValueStep(lua_State*);
int CSimpleSlider_GetValueStep(lua_State*);

int CSimpleStatusBar_SetMinMaxValues(lua_State*);
int CSimpleStatusBar_GetMinMaxValues(lua_State*);
int CSimpleStatusBar_SetValue(lua_State*);
int CSimpleStatusBar_GetValue(lua_State*);
int CSimpleStatusBar_SetStatusBarColor(lua_State*);

int CSimpleTexture_GetAlpha(lua_State*);
int CSimpleTexture_GetName(lua_State*);
int CSimpleTexture_SetVertexColor(lua_State*);
int CSimpleTexture_SetAlpha(lua_State*);
int CSimpleTexture_Show(lua_State*);
int CSimpleTexture_Hide(lua_State*);
int CSimpleTexture_IsVisible(lua_State*);
int CSimpleTexture_SetTexture(lua_State*);
int CSimpleTexture_SetTexCoord(lua_State*);
int CSimpleTexture_SetPoint(lua_State*);
int CSimpleTexture_ClearAllPoints(lua_State*);
int CSimpleTexture_GetWidth(lua_State*);
int CSimpleTexture_SetWidth(lua_State*);
int CSimpleTexture_GetHeight(lua_State*);
int CSimpleTexture_SetHeight(lua_State*);
int CSimpleFontString_GetName(lua_State*);
int CSimpleFontString_SetVertexColor(lua_State*);
int CSimpleFontString_SetAlpha(lua_State*);
int CSimpleFontString_SetAlphaGradient(lua_State*);
int CSimpleFontString_Show(lua_State*);
int CSimpleFontString_Hide(lua_State*);
int CSimpleFontString_IsVisible(lua_State*);
int CSimpleFontString_SetText(lua_State*);
int CSimpleFontString_GetText(lua_State*);
int CSimpleFontString_SetTextColor(lua_State*);
int CSimpleFontString_SetTextHeight(lua_State*);
int CSimpleFontString_SetWidth(lua_State*);
int CSimpleFontString_GetWidth(lua_State*);
int CSimpleFontString_SetHeight(lua_State*);
int CSimpleFontString_GetHeight(lua_State*);
int CSimpleFontString_SetPoint(lua_State*);
int CSimpleFontString_ClearAllPoints(lua_State*);
int CSimpleFontString_SetJustifyH(lua_State*);
int CSimpleFontString_SetJustifyV(lua_State*);
