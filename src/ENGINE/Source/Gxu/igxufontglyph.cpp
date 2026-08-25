// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\IGxuFontGlyph.cpp
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x001CFB00  EA 0x5CFB00
// DIA: $E2
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E2_269()
{
  return _E1_269();
}


// -----------------------------------------------------------------------------
// RVA 0x001CFB10  EA 0x5CFB10
// DIA: $E1
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int _E1_269()
{
  INFINITY_269 = INFINITY;
  return 2139095040;
}


// -----------------------------------------------------------------------------
// RVA 0x001CFB20  EA 0x5CFB20
// DIA: IGxuFontGlyphRenderGlyph
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall IGxuFontGlyphRenderGlyph(
        unsigned int *pixelHeight,
        unsigned int a2,
        unsigned int code,
        unsigned int baseLine,
        GLYPHDATA *dataPtr,
        unsigned int noHinting,
        FT_FaceRec_ *monochrome)
{
  unsigned int *v8; // edi
  GLYPHDATA *v10; // esi
  unsigned int result; // eax
  unsigned int v12; // eax
  unsigned int v13; // edx
  unsigned int v14; // eax
  _DWORD *v15; // eax
  unsigned int v16; // ecx
  void *v17; // edx
  unsigned int v18; // ebx
  unsigned int v19; // eax
  unsigned int v20; // ebx
  void *v21; // eax
  void *v22; // edx
  unsigned int v23; // ebx
  double v24; // st7
  unsigned int v25; // edx
  void *srcData; // [esp+Ch] [ebp-1Ch]
  unsigned int YStart; // [esp+10h] [ebp-18h] BYREF
  unsigned int yOffset; // [esp+14h] [ebp-14h]
  unsigned int v29; // [esp+18h] [ebp-10h]
  int dummyGlyph; // [esp+1Ch] [ebp-Ch]
  unsigned int v31; // [esp+20h] [ebp-8h]
  unsigned int pitch; // [esp+24h] [ebp-4h]
  unsigned int width; // [esp+30h] [ebp+8h]
  unsigned int height; // [esp+3Ch] [ebp+14h]

  v8 = pixelHeight;
  v29 = a2;
  v31 = (unsigned int)pixelHeight;
  if ( pixelHeight == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_237, 99);
    SErrDisplayAppFatal(aFace);
  }
  if ( a2 == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_237, 100);
    SErrDisplayAppFatal(aPixelheight);
  }
  v10 = dataPtr;
  if ( dataPtr == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_237, 101);
    SErrDisplayAppFatal(aDataptr_0);
  }
  if ( dataPtr->data != nullptr )
    SMemFree(dataPtr->data, aDBuildBuildwow_237, 104, 0);
  dataPtr->data = nullptr;
  result = FT_Get_Char_Index((unsigned int)v8);
  if ( result != 0 )
  {
    result = FREETYPE_RenderGlyph(monochrome, (unsigned int)v8, noHinting, (int)monochrome);
    if ( result != 0 )
    {
      v12 = v8[21];
      v13 = *(_DWORD *)(v12 + 80);
      v14 = *(_DWORD *)(v12 + 76);
      width = v13;
      if ( a2 >= v14 )
        height = v14;
      else
        height = a2;
      v15 = (_DWORD *)v8[21];
      v16 = v15[21];
      v17 = (void *)v15[22];
      v18 = v16 * v15[19];
      srcData = v17;
      pitch = v16;
      dummyGlyph = 0;
      if ( width == 0 || height == 0 || v17 == nullptr || v16 == 0 || v18 == 0 )
      {
        v19 = (v29 + 3) >> 2;
        height = v29;
        width = v19;
        if ( v19 == 0 )
        {
          width = v29;
          v19 = v29;
        }
        if ( monochrome != nullptr )
        {
          v20 = (v19 + 7) & 0xFFFFFFF8;
          pitch = v20;
        }
        else
        {
          pitch = v19;
          v20 = v19;
        }
        v18 = v29 * v20;
        dummyGlyph = 1;
      }
      v21 = SMemAlloc(v18, aDBuildBuildwow_237, 143, 0);
      v22 = v21;
      if ( v21 != nullptr )
      {
        memset(v21, 0, v18);
        v22 = v21;
        v8 = (unsigned int *)v31;
      }
      if ( srcData != nullptr )
      {
        qmemcpy(v22, srcData, v18);
        v10 = dataPtr;
        v8 = (unsigned int *)v31;
      }
      v10->dataSize = v18;
      v23 = pitch;
      v10->data = v22;
      v10->freeTypeGlyphWidth = width;
      v10->freeTypeGlyphHeight = height;
      v10->freeTypeGlyphPitch = v23;
      v10->freeTypeGlyphAdvance = *(_DWORD *)(v8[21] + 56);
      v24 = (double)*(int *)(v8[21] + 32);
      yOffset = 0;
      YStart = 0;
      v10->freeTypeGlyphBearing = v24 * 0.015625;
      if ( height != 0 && width != 0 && v22 != nullptr && dummyGlyph == 0 )
        CalculateYOffset((FT_FaceRec_ *)&YStart, v8, &YStart, v29, baseLine, height);
      v25 = YStart;
      v10->yOffset = yOffset;
      v10->yStart = v25;
      return 1;
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001CFD60  EA 0x5CFD60
// DIA: FREETYPE_RenderGlyph
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall FREETYPE_RenderGlyph(unsigned int charCode, int noHinting, int monochrome)
{
  int result; // eax
  unsigned int v6; // ecx
  FT_GlyphSlotRec_ *v7; // ecx

  if ( charCode == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_237, 24, aFace, 0, 1u);
  result = FT_Get_Char_Index(charCode);
  if ( result != 0 )
  {
    v6 = 8328;
    if ( noHinting != 0 )
      v6 = 8330;
    if ( FT_Load_Glyph(charCode, result, v6) != 0 )
    {
      return 0;
    }
    else
    {
      v7 = *(FT_GlyphSlotRec_ **)(charCode + 84);
      if ( monochrome != 0 )
        return FT_Render_Glyph(v7, 1u) == 0;
      else
        return FT_Render_Glyph(v7, 0) == 0;
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001CFE00  EA 0x5CFE00
// DIA: CalculateYOffset
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall CalculateYOffset(
        unsigned int *yOffsetPtr,
        int *a2,
        unsigned int *glyphYStart,
        unsigned int pixelHeight,
        signed int baseLineRow,
        unsigned int glyphHeight)
{
  int v8; // ebx
  int v9; // ecx
  signed int v10; // eax
  unsigned int v11; // eax

  if ( yOffsetPtr == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_237, 56, aFace, 0, 1u);
  if ( a2 == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_237, 57, aYoffsetptr, 0, 1u);
  if ( glyphYStart == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_237, 58, aGlyphystart, 0, 1u);
  if ( pixelHeight == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_237, 59, aPixelheight, 0, 1u);
  if ( glyphHeight == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_237, 60, aGlyphheight, 0, 1u);
  v8 = 0;
  if ( glyphHeight <= pixelHeight )
  {
    v10 = *(_DWORD *)(yOffsetPtr[21] + 104);
    if ( v10 <= baseLineRow )
    {
      v9 = baseLineRow - v10;
    }
    else
    {
      v9 = 0;
      v8 = v10 - baseLineRow;
    }
  }
  else
  {
    v9 = 0;
  }
  v11 = v9 != 0 ? v9 : 0;
  if ( pixelHeight - glyphHeight >= v11 )
  {
    *a2 = v8;
    *glyphYStart = v11;
  }
  else
  {
    *a2 = pixelHeight - v11 - glyphHeight;
    *glyphYStart = pixelHeight - glyphHeight;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CFF10  EA 0x5CFF10
// DIA: $E5
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E5_266()
{
  return _E4_266();
}


// -----------------------------------------------------------------------------
// RVA 0x001CFF20  EA 0x5CFF20
// DIA: $E4
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E4_266()
{
  TWO_PI_266 = 3.1415927 + 3.1415927;
}


// -----------------------------------------------------------------------------
// RVA 0x001CFF30  EA 0x5CFF30
// DIA: $E8
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E8_266()
{
  return _E7_266();
}


// -----------------------------------------------------------------------------
// RVA 0x001CFF40  EA 0x5CFF40
// DIA: $E7
// Compiland: .\Release_Assertions_Enabled\IGxuFontGlyph.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E7_266()
{
  OO_TWO_PI_266 = 1.0 / TWO_PI_266;
}

