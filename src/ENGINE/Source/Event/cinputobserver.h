#pragma once
#include "ENGINE/Source/Event/observer.h"

class CInputObserver : public CObserver {
public:
    static CInputObserver* GetInputObserver();
    static int __fastcall OnChar(const void*,void*);static int __fastcall OnKeyDown(const void*,void*);static int __fastcall OnKeyRepeat(const void*,void*);static int __fastcall OnKeyUp(const void*,void*);static int __fastcall OnMouseDown(const void*,void*);static int __fastcall OnMouseUp(const void*,void*);static int __fastcall OnMouseMove(const void*,void*);static int __fastcall OnMouseWheel(const void*,void*);static int __fastcall OnMouseMoveRelative(const void*,void*);static int __fastcall OnMouseModeChanged(const void*,void*);static int __fastcall OnIme(const void*,void*);static int __fastcall OnWindowSize(const void*,void*);static int __fastcall OnWindowFocus(const void*,void*);
    static void RegisterKeyDown(CObserver*);static void UnregisterKeyDown(CObserver*);static void RegisterKeyDownRepeating(CObserver*);static void UnregisterKeyDownRepeating(CObserver*);static void RegisterKeyUp(CObserver*);static void UnregisterKeyUp(CObserver*);static void RegisterChar(CObserver*);static void UnregisterChar(CObserver*);
    static void RegisterMouseDown(CObserver*);static void UnregisterMouseDown(CObserver*);static void RegisterMouseUp(CObserver*);static void UnregisterMouseUp(CObserver*);static void RegisterMouseMove(CObserver*);static void UnregisterMouseMove(CObserver*);static void RegisterMouseWheel(CObserver*);static void UnregisterMouseWheel(CObserver*);static void RegisterMouseMoveRelative(CObserver*);static void UnregisterMouseMoveRelative(CObserver*);static void RegisterMouseModeChanged(CObserver*);static void UnregisterMouseModeChanged(CObserver*);static void RegisterIme(CObserver*);static void UnregisterIme(CObserver*);static void RegisterWindowSize(CObserver*);static void UnregisterWindowSize(CObserver*);static void RegisterWindowFocus(CObserver*);static void UnregisterWindowFocus(CObserver*);
    static void SetMouseMode(MOUSEMODE,unsigned holdButton=0);
};
void InputObserverInitialize();void InputObserverDestroy();
