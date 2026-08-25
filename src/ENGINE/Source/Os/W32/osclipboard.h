#pragma once
int __fastcall OsClipboardGetString(char* buffer,unsigned int size);char* __fastcall OsClipboardGetString();void OsClipboardFreeString(void* text);int OsClipboardPutString(const char* text);
