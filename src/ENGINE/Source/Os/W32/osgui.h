#pragma once

#include "ENGINE/Source/Tempest/c2ivector.h"
#include "ENGINE/Source/Tempest/cirect.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "Storm/h/stpl.h"

#include <Windows.h>
#include <CommCtrl.h>
#include <vector>

using OsGuiHandle = void*;
using OsGuiTreeItem = void*;
struct OsGuiCallbackParams { int type; int subType; int code; void* user; };
using OsGuiCallback = void (__fastcall*)(const OsGuiCallbackParams*);
struct OsGuiCallbackInfo { OsGuiCallback function; void* userParam; };
struct OsGuiMenuHotkey { int keyID; int modKeyID; };
struct OsGuiTreeItemParams { OsGuiTreeItemParams(); int used; NTempest::CImVector color; void* user; };
struct OsGuiTVSelectionInfo { int numSelected; OsGuiTreeItem firstSelection; unsigned flags; };
class COsTreeView;
struct OsGuiTVDDInfo { COsTreeView* treeView; int action; OsGuiTreeItem dragItem; OsGuiTreeItem targItem; int targX; int targY; };
using OsGuiDragHandler = int (__fastcall*)(const OsGuiTVDDInfo*, void*);

class COsMenu {
public:
    COsMenu(); COsMenu(unsigned char id, const char* title); ~COsMenu();
    void Clear(); void RemoveItem(int); int GetNumItems() const; int GetHotkey(unsigned, OsGuiMenuHotkey*) const;
    void AddHotkey(int); void RemoveHotkey(int); void AppendHotkeyText(char*, const OsGuiMenuHotkey&) const;
    void AddTextItem(unsigned, const char*, OsGuiMenuHotkey* = nullptr); void AddSubMenu(int, const char*, COsMenu*); void AddSeparator(int);
    void EnableItem(unsigned, int); void CheckItem(unsigned, int); void SetItemText(unsigned, const char*);
    unsigned char mID; HMENU mMenuHandle; char mTitle[32]; TSGrowableArray<OsGuiMenuHotkey> mHotkeys;
};

class COsMenuBar {
public:
    explicit COsMenuBar(HWND window = nullptr); ~COsMenuBar(); void Set(TSGrowableArray<COsMenu*>*); void UpdateAccelerators(); void Refresh();
    TSGrowableArray<COsMenu*> mMenus; HWND mWindowHandle; HMENU mMenuBarHandle; HACCEL mAccelerators;
};

class COsDialog;
class COsControl {
public:
    COsControl(COsDialog*, int type, short id, unsigned flags); COsControl(HWND, int type, short id, unsigned flags); virtual ~COsControl();
    void Initialize(HWND, unsigned type, short id, unsigned flags); virtual void OnDestroy();
    void SetRedraw(int); void Refresh(int erase = 1); void SetCallback(OsGuiCallback, void*);
    virtual int OnEvent(int, int, int); int SendEvent(int, int); void SetFont(int); void SetInputFocus(); void LoseInputFocus(); bool HasInputFocus() const;
    void SetText(const char*); void GetText(char*, int) const; int GetTextLength() const; void GetTextSize(const char*, int*, int*) const; void GetTextSize(int*, int*) const;
    void Show(int); bool IsShowing() const; void Enable(int); bool IsEnabled() const; void SetPosition(int, int); void GetPosition(int*, int*, int = 0) const;
    virtual void SetSize(int, int); void GetSize(int*, int*) const; void SetTooltip(char*); void SetContextMenu(COsMenu*); virtual int OnContextMenu(int, int);
    virtual bool IsHandleFromControl(HWND) const; virtual int OnNotify(int, void*); virtual int OnCommand(unsigned); virtual int OnScroll(unsigned short);
    virtual int OnDraw(HDC, unsigned, RECT*); virtual HBRUSH OnSetColors(HDC); virtual int OnReturn(); virtual int OnEscape(); virtual int OnMouseDown(); virtual int OnMouseUp();
    virtual void OnMouseMove(int, int); virtual void OnSizeChange(); virtual void OnTextChange(); virtual int OnMouseWheel(int);
    virtual int CanDoClipboardAction(int); virtual int DoClipboardAction(int); virtual void SetValue(int); virtual int GetValue() const;
    unsigned mFlags; COsDialog* mDialog; short mID; int mType; HWND mHandle; OsGuiCallback mCallback; void* mCallbackParam; COsMenu* mContextMenu; int mContextMenuEnabled; int mRedrawLevel;
};

class COsDialog {
public:
    COsDialog(HWND, unsigned flags); virtual ~COsDialog();
    void ApplyModality(int); HWND GetParentWindow() const; HWND GetTooltips() const; void EnableTooltips(int);
    void AddControl(COsControl*); unsigned FindControl(COsControl*) const; void DeleteControl(COsControl*); void DetachControl(COsControl*); COsControl* FindControl(HWND) const;
    int ProcessMessage(MSG*); void CheckEvents(); void SetTrackMouse(int); bool IsMouseInside() const; void SetCallback(OsGuiCallback, void*);
    void BringToFront(); bool IsInFront() const; void SetInputFocus(); void Show(int); bool IsShowing() const; bool IsEnabled() const;
    void SetRedraw(int); void Refresh(int = 1); void SetPosition(int, int); void GetPosition(int*, int*, int = 0) const; void SetSize(int, int); void GetSize(int*, int*, bool = false) const;
    void SetMinSize(int, int); bool GetMinSize(int*, int*) const; void SetTitle(const char*);
    virtual int OnAccept(); virtual int OnCancel(); virtual int OnMouseDown(); virtual int OnMouseUp(); virtual int OnMouseLeave(); virtual int OnMouseMove(int, int);
    virtual int OnEvent(int, int, int); virtual int OnControlTab(); void SetContextMenu(COsMenu*); virtual int OnContextMenu(int, int);
    bool HasFlag(unsigned) const; int CanDoClipboardAction(int); int DoClipboardAction(int);
    HWND mHandle; HWND mTooltips; OsGuiCallback mCallback; void* mCallbackParam; TSGrowableArray<COsControl*> mControls; COsControl* mCancelButton;
    int mTrackMouse, mMouseInside, mNeedNewTrack, mTooltipsEnabled, mContextMenuEnabled; COsMenu* mContextMenu; unsigned mFlags; TSGrowableArray<HWND> mDisabledWindows; NTempest::C2iVector mMinSize;
};

class COsButton : public COsControl { public: COsButton(COsDialog*,short,unsigned); void SetDefaultButton(); void SetCancelButton(); void SetHighlight(int); };
class COsImageButton : public COsControl { public: COsImageButton(COsDialog*,short,unsigned); COsImageButton(HWND,short,unsigned); ~COsImageButton() override; void OnDestroy() override; void SetImage(int,int,void*); void SetHighlight(int); bool IsPushed() const; };
class COsStaticText : public COsControl { public: COsStaticText(COsDialog*,short,unsigned); COsStaticText(HWND,short,unsigned); void Initialize(); void SetJustification(int); void SetTextColor(const NTempest::CImVector&); HBRUSH OnSetColors(HDC) override; NTempest::CImVector mTextColor; };
class COsStaticImage : public COsControl { public: COsStaticImage(COsDialog*,short); ~COsStaticImage() override; void OnDestroy() override; void SetImage(int,int,void*); void ClearImage(); };
class COsEditBox : public COsControl { public: COsEditBox(COsDialog*,short,unsigned); COsEditBox(HWND,short,unsigned); void Initialize(); void UpdateSelection(); int OnReturn() override; void SetTextLimit(unsigned); void SelectAll(); unsigned GetSelectionSize() const; void EnableFilters(int); void SetFilter(unsigned,int); bool IsCharacterAllowed(char) const; int CanDoClipboardAction(int) override; int DoClipboardAction(int) override; int mFiltersEnabled; unsigned mFilters; int mSelSize; };
class COsPopupMenu : public COsControl { public: COsPopupMenu(COsDialog*,short); ~COsPopupMenu() override; void SetSize(int,int) override; void SetValue(int) override; int GetValue() const override; void ClearItems(); int GetNumItems() const; void InsertItem(const char*,int); void SetItemHeight(int); int GetItemHeight() const; void AdjustHeight(); void SetMaxHeight(int); void DeleteItem(int); void SetItemText(int,const char*); int mBaseHeight,mMaxHeight; };
class COsListBox : public COsControl { public: COsListBox(COsDialog*,short,unsigned); ~COsListBox() override; void SetValue(int) override; int GetValue() const override; void SelectItem(int,int); bool IsItemSelected(int) const; void SelectAll(int); void ClearItems(); int GetNumItems() const; void InsertItem(const char*,int); void DeleteItem(int); void SetItemText(int,const char*); void GetItemText(int,char*,int) const; int GetItemTextLength(int) const; void SetItemHeight(int); int GetItemHeight() const; int OnReturn() override; int OnContextMenu(int,int) override; };
class COsCheckbox : public COsControl { public: COsCheckbox(COsDialog*,short,unsigned); void SetValue(int) override; int GetValue() const override; bool HasValue() const; void ClearValue(); int OnEvent(int,int,int) override; void OnSizeChange() override; void OnTextChange() override; void SetMaxWidth(int); int mSettingSize,mMaxWidth; };
class COsProgressBar : public COsControl { public: COsProgressBar(COsDialog*,short,unsigned); void SetValue(int) override; int GetValue() const override; };
class COsSlider : public COsControl { public: COsSlider(COsDialog*,short,unsigned); void SetMinValue(int); void SetMaxValue(int); void SetValue(int) override; int GetValue() const override; };
class COsRadioButton : public COsControl { public: COsRadioButton(COsDialog*,short,unsigned); void SetValue(int) override; int GetValue() const override; };
class COsSpinButton : public COsControl { public: COsSpinButton(COsDialog*,short,unsigned); COsSpinButton(HWND,short,unsigned); void Initialize(); void OnSpinMouseUp(); void SetValueRange(int,int); void SetValue(int) override; int GetValue() const override; };
class COsTabControl : public COsControl { public: using COsControl::COsControl; void SetValue(int) override; int GetValue() const override; void InsertItem(char*,int); int GetNumItems() const; int OnControlTab(); };
class COsScrollBar : public COsControl { public: COsScrollBar(COsDialog*,short,unsigned); COsScrollBar(HWND,short,unsigned); void Initialize(); void SetRange(int,int); void SetPageSize(int); void UpdateRangeValues(); void SetValue(int) override; int GetValue() const override; int OnScroll(unsigned short) override; int OnMouseWheel(int) override; int mRealMin,mRealMax,mPageSize; };
class COsDivider : public COsControl { public: COsDivider(COsDialog*,short,unsigned); COsDivider(HWND,short,unsigned); ~COsDivider() override; void Initialize(); void UpdateCursor(); void OnDivMouseDown(); void OnDivMouseMove(int,int); void OnDivMouseUp(); void OnDivMouseLeave(); void SetPositionRange(int,int); int mMaxPos,mMinPos,mTracking,mDragging,mDragStartMouseX,mDragStartMouseY; NTempest::CiRect mDragStartPos; };
class COsTextButton : public COsControl { public: COsTextButton(COsDialog*,short); int OnDraw(HDC,unsigned,RECT*) override; NTempest::CImVector mActiveColor,mPushedColor,mGreyedColor; int mUnderline; };
class COsStaticBox : public COsControl { public: using COsControl::COsControl; void ClearTransparentRects(); void AddTransparentRect(const NTempest::CiRect&); int OnDraw(HDC,unsigned,RECT*) override; TSGrowableArray<NTempest::CiRect> mTransRect; };
class COsListView : public COsControl { public: COsListView(COsDialog*,short,unsigned); ~COsListView() override; void InsertColumn(int); void DeleteColumn(int); int GetNumColumns() const; void InsertRow(int); void DeleteRow(int); void ClearRows(); int GetNumRows() const; void SetRowColor(int,const NTempest::CImVector&); NTempest::CImVector GetRowColor(int) const; void SetItemText(int,int,char*); void GetItemText(int,int,char*,int) const; void SetColumnWidth(int,unsigned short); int GetColumnWidth(int) const; void SetColumnTitle(int,char*); void GetColumnTitle(int,char*,int) const; void SetColumnJustification(int,int); void EnsureRowVisible(int); void SetValue(int) override; int GetValue() const override; void OnSizeChange() override; void OnSelectionChange(); void OnColumnClick(int); int OnNotify(int,void*) override; int OnReturn() override; int mNumCols; std::vector<NTempest::CImVector> mRowColors; };
class COsToolBar : public COsControl { public: COsToolBar(COsDialog*,short,unsigned); COsToolBar(HWND,short,unsigned); ~COsToolBar() override; void InitializeToolBar(); void SetButtonSize(unsigned short,unsigned short); void GetButtonSize(int*,int*) const; void Clear(); void AddButton(int); void AddSeparator(int); void RemoveButton(int); int GetNumButtons() const; void SetButtonImage(int,int,int,HBITMAP); void SetButtonText(int,char*); void GetButtonText(int,char*,int) const; void EnableButton(int,int); void CheckButton(int,int); int OnCommand(unsigned) override; HIMAGELIST mImageList; };
using OsGuiTreeEnumProc = void (__fastcall*)(COsTreeView*,OsGuiTreeItem,void*);
class COsTreeView : public COsControl { public: COsTreeView(COsDialog*,short,unsigned); COsTreeView(HWND,short,unsigned); ~COsTreeView() override; void InitializeTreeView(); void SetBackgroundColor(const NTempest::CImVector&); void ClearItems(); void DeleteItem(OsGuiTreeItem); OsGuiTreeItem InsertItem(OsGuiTreeItem,OsGuiTreeItem,char*); void SetItemText(OsGuiTreeItem,char*); void GetItemText(OsGuiTreeItem,char*,int) const; void SetItemParam(OsGuiTreeItem,void*); void* GetItemParam(OsGuiTreeItem); void SetItemColor(OsGuiTreeItem,const NTempest::CImVector&); void ResetItemColor(OsGuiTreeItem); NTempest::CImVector GetItemColor(OsGuiTreeItem); OsGuiTreeItem GetItemParent(OsGuiTreeItem) const; int GetItemNumChildren(OsGuiTreeItem) const; OsGuiTreeItem GetItemChild(OsGuiTreeItem,int) const; void EnumerateItems(OsGuiTreeItem,OsGuiTreeEnumProc,void*); void EnumerateAllItems(OsGuiTreeEnumProc,void*); void SetItemImage(OsGuiTreeItem,int,int,HBITMAP); void ExpandItem(OsGuiTreeItem,int); bool IsItemExpanded(OsGuiTreeItem) const; void OnSizeChange() override; void OnExpandedItem(OsGuiTreeItem); void EnsureItemVisible(OsGuiTreeItem); OsGuiTreeItem GetSelectedItem() const; void SelectItem(OsGuiTreeItem,int); bool IsItemSelected(OsGuiTreeItem) const; void GetSelectionInfo(OsGuiTVSelectionInfo*) const; void SelectAll(int); void EditItem(OsGuiTreeItem); int OnMouseDown() override; int OnMouseUp() override; OsGuiTreeItem FindItemUnderCursor(); int OnReturn() override; int OnEscape() override; int OnNotify(int,void*) override; HWND GetEditControl() const; bool IsHandleFromControl(HWND) const override; void EnableDragDrop(int); void SetDragDropHandler(OsGuiDragHandler,void*); void CreateDragImage(OsGuiTreeItem); void DestroyDragImage(); NTempest::CiRect GetItemRect(OsGuiTreeItem) const; void RefreshItem(OsGuiTreeItem); OsGuiTreeItem GetFirstVisibleItem() const; void SetFirstVisibleItem(OsGuiTreeItem); int RunDragHandler(); void OnBeginDrag(OsGuiTreeItem,int,int); void OnMouseMove(int,int) override; void SetDropTarget(OsGuiTreeItem); void SetInsertionMark(OsGuiTreeItem,int); void OnEndDrag(); int OnBeginEdit(OsGuiTreeItem); int OnEndEdit(OsGuiTreeItem,char*); void SetTextLimit(int); void EnableFilters(int); void SetFilter(unsigned,int); bool IsCharacterAllowed(char) const; void SetCanEditFunction(int (__fastcall*)(void*,void*),void*); void SetExpandFunction(void (__fastcall*)(void*,void*),void*); void OnDeleteItem(OsGuiTreeItem); unsigned FindUnusedParams(); void InitParams(OsGuiTreeItem); OsGuiTreeItemParams* GetParams(OsGuiTreeItem); HIMAGELIST mImages; TSGrowableArray<int> mUnusedImageIDs; int mDragging; OsGuiTVDDInfo mDragInfo; HIMAGELIST mDragImage; OsGuiDragHandler mDragHandler; void* mDragHandlerParam; int (__fastcall *mCanEditFunc)(void*,void*); void* mCanEditParam; void (__fastcall *mExpandFunc)(void*,void*); void* mExpandParam; int mTextLimit,mFiltersEnabled; unsigned mFilters; TSGrowableArray<OsGuiTreeItemParams> mItemParams; };

class COsWindow {
public: explicit COsWindow(HWND); virtual ~COsWindow(); void SetMinSize(int,int); void GetMinSize(int*,int*) const; void SetCursor(int); void SetIcon(const char*); void SetInputFocus(); virtual void OnResize(); HWND mHandle; NTempest::C2iVector mMinSize; };

void OsGuiMenuSelect(unsigned char menuID, unsigned char itemID); void OsGuiInitialize(); void OsGuiDestroy(); void OsGuiSetApplicationInfo(HINSTANCE);
void OsGuiSetMenuCommandCallback(OsGuiCallback, void*); void OsGuiSetIdleCallback(OsGuiCallback, void*); int OsGuiProcessMessage(MSG*);
void OsGuiEnableTooltips(int); void OsGuiEnableMenuHotkeys(int); void OsGuiSetCursor(int); void OsGuiShowCursor(int); void OsGuiGetCursorPosition(int*, int*);
void OsGuiSetWindowTitle(HWND, const char*); void OsGuiSetWindowIcon(HWND, const char*); NTempest::CiRect OsGuiGetWindowRect(HWND, int); void OsGuiSetWindowRect(HWND, const NTempest::CiRect&);
void OsGuiBringWindowToFront(HWND); void OsGuiShowWindow(HWND, int); void OsGuiEnableWindow(HWND, int); int OsGuiWindowEnabled(HWND); HWND OsGuiGetWindow(int); void OsGuiSetGxWindow(HWND);
void OsGuiMaximizeWindow(HWND, int); int OsGuiWindowMaximized(HWND); void OsGuiMinimizeWindow(HWND, int); int OsGuiWindowMinimized(HWND);
NTempest::CiRect OsGuiGetWindowRestoredRect(HWND); void OsGuiSetWindowRestoredRect(HWND, const NTempest::CiRect&); int OsGuiWindowIsCursorInside(HWND, int); NTempest::CiRect OsGuiGetScreenBounds();
void OsGuiBeep(); int OsGuiMessageBox(HWND, int, const char*, const char*); int OsGuiIsModifierKeyDown(unsigned); void OsGuiGetHotkeyText(const OsGuiMenuHotkey&, char*, unsigned); NTempest::CImVector OsGuiGetColor(int);
LRESULT CALLBACK OsGuiWindowProc(HWND, UINT, WPARAM, LPARAM);
