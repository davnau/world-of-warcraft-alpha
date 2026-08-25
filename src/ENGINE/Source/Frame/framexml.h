#pragma once

class CStatus;
class CSimpleFrame;
class CSimpleTexture;
class CSimpleFontString;
class XMLNode;

using FrameXMLFactory = CSimpleFrame* (*)(CSimpleFrame* parent);
using FrameXMLProgressFunction = void (__fastcall*)(int loaded, int total);

void FrameXML_SetDebugLevel(int level);
int FrameXML_GetDebugLevel();
int FrameXML_CreateFrames(const char* tableOfContents, CStatus* status = nullptr);
int FrameXML_ProcessFile(const char* filename, CStatus* status = nullptr);
CSimpleFrame* FrameXML_CreateFrame(const XMLNode* node, CSimpleFrame* parent, CStatus* status);
void FrameXML_LoadTextureNode(CSimpleTexture* texture, const XMLNode* node, CStatus* status);
void FrameXML_LoadFontStringNode(CSimpleFontString* string, const XMLNode* node, CStatus* status);
void FrameXML_StoreHashNode(const XMLNode* node, const char* name, CStatus* status);
const XMLNode* FrameXML_FindHashNode(const char* name);
void FrameXML_ClearFactories();
int FrameXML_RegisterFactory(const char* frameType, FrameXMLFactory factory);
int FrameXML_RegisterDefault();
void FrameXML_RegisterLoadProgressCallback(FrameXMLProgressFunction callback);
void FrameXML_DestroyFrames();
