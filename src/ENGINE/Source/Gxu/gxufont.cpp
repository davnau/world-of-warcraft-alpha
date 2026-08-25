// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\GxuFont.cpp
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x001BD4D0  EA 0x5BD4D0
// DIA: $E2
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E2_266()
{
  return _E1_266();
}


// -----------------------------------------------------------------------------
// RVA 0x001BD4E0  EA 0x5BD4E0
// DIA: $E1
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int _E1_266()
{
  INFINITY_266 = INFINITY;
  return 2139095040;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD4F0  EA 0x5BD4F0
// DIA: FreeTypeAllocFunction
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void *__fastcall FreeTypeAllocFunction(int size, signed int a2)
{
  if ( a2 <= 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 29, aSize0, 0, 1u);
  return SMemAlloc(a2, aDBuildBuildwow_241, 30, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD530  EA 0x5BD530
// DIA: FreeTypeFreeFunction
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall FreeTypeFreeFunction(void *block, void *a2)
{
  if ( a2 != nullptr )
    SMemFree(a2, aDBuildBuildwow_241, 39, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD550  EA 0x5BD550
// DIA: FreeTypeReallocFunction
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void *__stdcall FreeTypeReallocFunction(int newSize, void *block)
{
  if ( newSize <= 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 51, aNewsize0, 0, 1u);
  return SMemReAlloc(block, newSize, aDBuildBuildwow_241, 56, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD590  EA 0x5BD590
// DIA: $E26
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E26_40()
{
  _E23_45();
  return _E25_31();
}


// -----------------------------------------------------------------------------
// RVA 0x001BD5A0  EA 0x5BD5A0
// DIA: $E23
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E23_45()
{
  s_fonts.m_linkoffset = 0;
  s_fonts.m_terminator.m_prevlink = &s_fonts.m_terminator;
  s_fonts.m_terminator.m_next = (CGxFont *)~(unsigned int)&s_fonts.m_terminator;
  return ~(unsigned int)&s_fonts.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD5D0  EA 0x5BD5D0
// DIA: $E25
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E25_31()
{
  return atexit(_E24_25);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD5E0  EA 0x5BD5E0
// DIA: $E24
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E24_25()
{
  TSLink<CGxFont> *m_prevlink; // esi
  CGxFont *m_next; // edi
  int v2; // ecx
  CGxFont **p_m_next; // edx
  int v4; // ecx
  int v5; // eax

  while ( (int)s_fonts.m_terminator.m_next > 0 )
  {
    m_prevlink = s_fonts.m_terminator.m_next->m_link.m_prevlink;
    m_next = s_fonts.m_terminator.m_next;
    if ( s_fonts.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v2 = (int)s_fonts.m_terminator.m_next->m_link.m_next;
      p_m_next = &s_fonts.m_terminator.m_next->m_link.m_next;
      if ( v2 > 0 )
        v4 = (char *)s_fonts.m_terminator.m_next - (char *)m_prevlink->m_next + v2;
      else
        v4 = ~v2;
      *(_DWORD *)v4 = m_prevlink;
      m_next->m_link.m_prevlink->m_next = *p_m_next;
      m_next->m_link.m_prevlink = nullptr;
      *p_m_next = nullptr;
    }
  }
  if ( s_fonts.m_terminator.m_prevlink != nullptr )
  {
    if ( (int)s_fonts.m_terminator.m_next > 0 )
      v5 = (int)s_fonts.m_terminator.m_next
         + (char *)&s_fonts.m_terminator
         - (char *)s_fonts.m_terminator.m_prevlink->m_next;
    else
      v5 = ~(int)s_fonts.m_terminator.m_next;
    *(_DWORD *)v5 = s_fonts.m_terminator.m_prevlink;
    s_fonts.m_terminator.m_prevlink->m_next = s_fonts.m_terminator.m_next;
    s_fonts.m_terminator.m_prevlink = nullptr;
    s_fonts.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BD670  EA 0x5BD670
// DIA: $E31
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E31_68()
{
  _E28_71();
  return _E30_65();
}


// -----------------------------------------------------------------------------
// RVA 0x001BD680  EA 0x5BD680
// DIA: $E28
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E28_71()
{
  s_unusedBatches.m_linkoffset = 0;
  s_unusedBatches.m_terminator.m_prevlink = &s_unusedBatches.m_terminator;
  s_unusedBatches.m_terminator.m_next = (CGxStringBatch *)~(unsigned int)&s_unusedBatches.m_terminator;
  return ~(unsigned int)&s_unusedBatches.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD6B0  EA 0x5BD6B0
// DIA: $E30
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E30_65()
{
  return atexit(_E29_42);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD6C0  EA 0x5BD6C0
// DIA: $E29
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E29_42()
{
  TSLink<CGxStringBatch> *m_prevlink; // esi
  CGxStringBatch *m_next; // edi
  int v2; // ecx
  CGxStringBatch **p_m_next; // edx
  int v4; // ecx
  int v5; // eax

  while ( (int)s_unusedBatches.m_terminator.m_next > 0 )
  {
    m_prevlink = s_unusedBatches.m_terminator.m_next->m_link.m_prevlink;
    m_next = s_unusedBatches.m_terminator.m_next;
    if ( s_unusedBatches.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v2 = (int)s_unusedBatches.m_terminator.m_next->m_link.m_next;
      p_m_next = &s_unusedBatches.m_terminator.m_next->m_link.m_next;
      if ( v2 > 0 )
        v4 = (char *)s_unusedBatches.m_terminator.m_next - (char *)m_prevlink->m_next + v2;
      else
        v4 = ~v2;
      *(_DWORD *)v4 = m_prevlink;
      m_next->m_link.m_prevlink->m_next = *p_m_next;
      m_next->m_link.m_prevlink = nullptr;
      *p_m_next = nullptr;
    }
  }
  if ( s_unusedBatches.m_terminator.m_prevlink != nullptr )
  {
    if ( (int)s_unusedBatches.m_terminator.m_next > 0 )
      v5 = (int)s_unusedBatches.m_terminator.m_next
         + (char *)&s_unusedBatches.m_terminator
         - (char *)s_unusedBatches.m_terminator.m_prevlink->m_next;
    else
      v5 = ~(int)s_unusedBatches.m_terminator.m_next;
    *(_DWORD *)v5 = s_unusedBatches.m_terminator.m_prevlink;
    s_unusedBatches.m_terminator.m_prevlink->m_next = s_unusedBatches.m_terminator.m_next;
    s_unusedBatches.m_terminator.m_prevlink = nullptr;
    s_unusedBatches.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BD750  EA 0x5BD750
// DIA: $E36
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E36_61()
{
  _E33_64();
  return _E35_38();
}


// -----------------------------------------------------------------------------
// RVA 0x001BD760  EA 0x5BD760
// DIA: $E33
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int _E33_64()
{
  int result; // eax
  int v1; // esi

  s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_prevlink = &s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator;
  result = ~(unsigned int)&s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator;
  s_stringBatch_0.m_link.m_prevlink = nullptr;
  s_stringBatch_0.m_link.m_next = nullptr;
  s_stringBatch_0.m_fontBatch.m_fulllist.m_linkoffset = -572662307;
  s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next = (BATCHEDRENDERFONTDESC *)~(unsigned int)&s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator;
  s_stringBatch_0.m_fontBatch.m_slotlistarray.m_alloc = 0;
  s_stringBatch_0.m_fontBatch.m_slotlistarray.m_count = 0;
  s_stringBatch_0.m_fontBatch.m_slotlistarray.m_data = nullptr;
  s_stringBatch_0.m_fontBatch.m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >_vtbl *)&TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::`vftable';
  s_stringBatch_0.m_fontBatch.m_slotlistarray.m_chunk = 0;
  s_stringBatch_0.m_fontBatch.__vftable = (TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>_vtbl *)&TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::`vftable';
  s_stringBatch_0.m_fontBatch.m_fullnessIndicator = 0;
  while ( result > 0 )
  {
    v1 = s_stringBatch_0.m_fontBatch.m_fulllist.m_linkoffset + result;
    if ( *(_DWORD *)(s_stringBatch_0.m_fontBatch.m_fulllist.m_linkoffset + result) != 0 )
    {
      TSLink<BATCHEDRENDERFONTDESC>::NextLink(
        (TSLink<BATCHEDRENDERFONTDESC> *)(s_stringBatch_0.m_fontBatch.m_fulllist.m_linkoffset + result),
        -1)->m_prevlink = *(TSLink<BATCHEDRENDERFONTDESC> **)v1;
      *(_DWORD *)(*(_DWORD *)v1 + 4) = *(_DWORD *)(v1 + 4);
      *(_DWORD *)v1 = 0;
      *(_DWORD *)(v1 + 4) = 0;
      result = (int)s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next;
    }
  }
  s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next = (BATCHEDRENDERFONTDESC *)~(unsigned int)&s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator;
  s_stringBatch_0.m_fontBatch.m_fulllist.m_linkoffset = 12;
  s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_prevlink = &s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator;
  s_stringBatch_0.m_fontBatch.m_slotmask = -1;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD820  EA 0x5BD820
// DIA: $E35
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E35_38()
{
  return atexit(_E34_68);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD830  EA 0x5BD830
// DIA: $E34
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E34_68()
{
  int v0; // ebx
  BATCHEDRENDERFONTDESC *m_next; // eax
  TSLink<BATCHEDRENDERFONTDESC> *v2; // eax
  unsigned int v3; // esi
  int v4; // edi
  TSLink<BATCHEDRENDERFONTDESC> *v5; // eax
  unsigned int i; // edi
  BATCHEDRENDERFONTDESC *v7; // esi
  TSLink<BATCHEDRENDERFONTDESC> *v8; // eax
  TSLink<BATCHEDRENDERFONTDESC> *v9; // eax

  v0 = 0;
  s_stringBatch_0.m_fontBatch.m_fullnessIndicator = 0;
  while ( 1 )
  {
    m_next = s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next;
    if ( (int)s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next <= 0 )
      break;
    v2 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
           &s_stringBatch_0.m_fontBatch.m_fulllist,
           s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next);
    TSLink<BATCHEDRENDERFONTDESC>::Unlink(v2);
  }
  v3 = 0;
  if ( s_stringBatch_0.m_fontBatch.m_slotlistarray.m_count != 0 )
  {
    v4 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
          &s_stringBatch_0.m_fontBatch.m_slotlistarray,
          v3);
        if ( (int)s_stringBatch_0.m_fontBatch.m_slotlistarray.m_data[v4].m_terminator.m_next <= 0 )
          break;
        s_stringBatch_0.m_fontBatch.TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::InternalDelete(
          &s_stringBatch_0.m_fontBatch,
          s_stringBatch_0.m_fontBatch.m_slotlistarray.m_data[v4].m_terminator.m_next);
      }
      ++v3;
      ++v4;
    }
    while ( v3 < s_stringBatch_0.m_fontBatch.m_slotlistarray.m_count );
    m_next = s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next;
  }
  s_stringBatch_0.m_fontBatch.__vftable = (TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>_vtbl *)&TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::`vftable';
  s_stringBatch_0.m_fontBatch.m_fullnessIndicator = 0;
  while ( (int)m_next > 0 )
  {
    v5 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
           &s_stringBatch_0.m_fontBatch.m_fulllist,
           m_next);
    TSLink<BATCHEDRENDERFONTDESC>::Unlink(v5);
    m_next = s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next;
  }
  for ( i = 0; i < s_stringBatch_0.m_fontBatch.m_slotlistarray.m_count; ++v0 )
  {
    while ( 1 )
    {
      TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
        &s_stringBatch_0.m_fontBatch.m_slotlistarray,
        i);
      v7 = s_stringBatch_0.m_fontBatch.m_slotlistarray.m_data[v0].m_terminator.m_next;
      if ( (int)v7 <= 0 )
        break;
      TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
        &s_stringBatch_0.m_fontBatch.m_slotlistarray,
        i);
      v8 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
             &s_stringBatch_0.m_fontBatch.m_slotlistarray.m_data[v0],
             v7);
      TSLink<BATCHEDRENDERFONTDESC>::Unlink(v8);
    }
    ++i;
  }
  TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::~TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>(&s_stringBatch_0.m_fontBatch.m_slotlistarray);
  while ( (int)s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next > 0 )
  {
    v9 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
           &s_stringBatch_0.m_fontBatch.m_fulllist,
           s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next);
    TSLink<BATCHEDRENDERFONTDESC>::Unlink(v9);
  }
  TSLink<BATCHEDRENDERFONTDESC>::Unlink(&s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator);
  if ( s_stringBatch_0.m_link.m_prevlink != nullptr )
  {
    TSLink<CGxStringBatch>::NextLink(&s_stringBatch_0.m_link, -1)->m_prevlink = s_stringBatch_0.m_link.m_prevlink;
    s_stringBatch_0.m_link.m_prevlink->m_next = s_stringBatch_0.m_link.m_next;
    s_stringBatch_0.m_link.m_prevlink = nullptr;
    s_stringBatch_0.m_link.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BD9A0  EA 0x5BD9A0
// DIA: SignOf
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __fastcall SignOf(float value)
{
  if ( value >= 0.0 )
    return 1.0;
  else
    return -1.0;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD9D0  EA 0x5BD9D0
// DIA: GetFreeTypeLibrary
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
FT_LibraryRec_ *__fastcall GetFreeTypeLibrary()
{
  return s_FTLibrary;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD9E0  EA 0x5BD9E0
// DIA: GetScreenPixelHeight
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall GetScreenPixelHeight()
{
  return g_heightPixels;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD9F0  EA 0x5BD9F0
// DIA: GetScreenPixelWidth
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall GetScreenPixelWidth()
{
  return g_widthPixels;
}


// -----------------------------------------------------------------------------
// RVA 0x001BDA00  EA 0x5BDA00
// DIA: ScreenToPixelHeight
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __thiscall ScreenToPixelHeight(int this, float height)
{
  float pixelCoords; // [esp+10h] [ebp+8h]

  if ( this != 0 )
    return height;
  pixelCoords = (double)g_heightPixels * height;
  return (double)(int)(__int64)(SignOf(pixelCoords) * 0.5 + pixelCoords);
}


// -----------------------------------------------------------------------------
// RVA 0x001BDA50  EA 0x5BDA50
// DIA: ScreenToPixelWidth
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __thiscall ScreenToPixelWidth(int this, float width)
{
  float pixelCoords; // [esp+10h] [ebp+8h]

  if ( this != 0 )
    return width;
  pixelCoords = (double)g_widthPixels * width;
  return (double)(int)(__int64)(SignOf(pixelCoords) * 0.5 + pixelCoords);
}


// -----------------------------------------------------------------------------
// RVA 0x001BDAA0  EA 0x5BDAA0
// DIA: GxuFontWindowSizeChanged
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __cdecl GxuFontWindowSizeChanged()
{
  double v0; // st7
  unsigned int v1; // esi
  __int64 v2; // rax
  CGxFont *i; // esi
  NTempest::CRect rect; // [esp+0h] [ebp-20h] BYREF
  int v5; // [esp+10h] [ebp-10h]
  int v6; // [esp+14h] [ebp-Ch]
  __int64 v7; // [esp+18h] [ebp-8h]

  if ( (__S38__1__GxuFontWindowSizeChanged__YIXXZ_4EA & 1) == 0 )
  {
    __S38__1__GxuFontWindowSizeChanged__YIXXZ_4EA |= 1u;
    s_currentRect.t = 0.0;
    s_currentRect.l = 0.0;
    s_currentRect.b = 0.0;
    s_currentRect.r = 0.0;
    atexit(_E39_63);
  }
  memset((void *)&rect, 0, sizeof(rect));
  GxCapsWindowSize(&rect);
  if ( rect.r - rect.l == 0.0 || rect.b - rect.t == 0.0 )
  {
    v5 = 0;
    v6 = 0;
    v7 = 0x4420000043F00000LL;
    rect.t = 0.0;
    rect.l = 0.0;
    rect.b = 480.0;
    rect.r = 640.0;
  }
  if ( rect.t != s_currentRect.t || rect.l != s_currentRect.l || rect.b != s_currentRect.b || rect.r != s_currentRect.r )
  {
    s_currentRect = rect;
    v0 = rect.b - rect.t;
    v1 = (__int64)(rect.r - rect.l);
    g_widthPixels = v1;
    v2 = (__int64)v0;
    g_heightPixels = (__int64)v0;
    if ( v1 != 0 )
    {
      v7 = v1;
      v0 = 1.0 / (double)v1;
      s_pixelWidth = v0;
    }
    else
    {
      s_pixelWidth = 0.0;
    }
    if ( (_DWORD)v2 != 0 )
    {
      v7 = (unsigned int)v2;
      v0 = 1.0 / (double)(unsigned int)v2;
      s_pixelHeight = v0;
    }
    else
    {
      s_pixelHeight = 0.0;
    }
    for ( i = (int)s_fonts.m_terminator.m_next <= 0 ? nullptr : s_fonts.m_terminator.m_next;
          (int)i > 0;
          i = i->m_link.m_next )
    {
      CGxFont::HandleScreenSizeChange(i, v0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BDC80  EA 0x5BDC80
// DIA: $E39
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E39_63()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001BDC90  EA 0x5BDC90
// DIA: GxuFontInitialize
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __cdecl GxuFontInitialize()
{
  int v0; // esi

  v0 = FT_New_Library((FT_LibraryRec_ **)&s_GxuMemoryRecord, &s_FTLibrary);
  FT_Add_Default_Modules((FT_ModuleRec_ *)s_FTLibrary);
  if ( v0 != 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 157, aError_2, 0, 1u);
  if ( s_FTLibrary == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 159, aSFtlibrary, 0, 1u);
  GxuFontWindowSizeChanged();
  IGxuStringInitialize();
}


// -----------------------------------------------------------------------------
// RVA 0x001BDD00  EA 0x5BDD00
// DIA: GxuFontShutdown
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __cdecl GxuFontShutdown()
{
  HFACE__ *v0; // eax
  CGxString *m_next; // esi
  CGxFont *v2; // esi
  CGxStringBatch *v3; // esi
  CGxString *v4; // esi
  TEXTLINETEXTURE *v5; // esi
  IGXUTEXTLINE *v6; // esi

  IGxuStringShutdown();
  while ( 1 )
  {
    m_next = g_strings.m_terminator.m_next;
    if ( (int)g_strings.m_terminator.m_next <= 0 )
      break;
    CGxString::~CGxString(g_strings.m_terminator.m_next);
    v0 = (HFACE__ *)SMemFree(m_next, aAucgxstring, -2, 0);
  }
  while ( (int)s_fonts.m_terminator.m_next > 0 )
  {
    v2 = s_fonts.m_terminator.m_next;
    CGxFont::~CGxFont(s_fonts.m_terminator.m_next, v0);
    v0 = (HFACE__ *)SMemFree(v2, aAucgxfont, -2, 0);
  }
  while ( (int)s_unusedBatches.m_terminator.m_next > 0 )
  {
    v3 = s_unusedBatches.m_terminator.m_next;
    CGxStringBatch::~CGxStringBatch(s_unusedBatches.m_terminator.m_next);
    SMemFree(v3, aAucgxstringbat, -2, 0);
  }
  while ( 1 )
  {
    v4 = g_freeStrings.m_terminator.m_next;
    if ( (int)g_freeStrings.m_terminator.m_next <= 0 )
      break;
    CGxString::~CGxString(g_freeStrings.m_terminator.m_next);
    SMemFree(v4, aAucgxstring, -2, 0);
  }
  while ( 1 )
  {
    v5 = g_freeTextLineTextures.m_terminator.m_next;
    if ( (int)g_freeTextLineTextures.m_terminator.m_next <= 0 )
      break;
    TEXTLINETEXTURE::~TEXTLINETEXTURE(g_freeTextLineTextures.m_terminator.m_next);
    SMemFree(v5, aAutextlinetext, -2, 0);
  }
  while ( 1 )
  {
    v6 = g_freeTextLines.m_terminator.m_next;
    if ( (int)g_freeTextLines.m_terminator.m_next <= 0 )
      break;
    IGXUTEXTLINE::Destroy(g_freeTextLines.m_terminator.m_next);
    TSFixedArray<TEXTLINETEXTURE *>::~TSFixedArray<TEXTLINETEXTURE *>(&v6->m_texturePages);
    TSLinkedNode<IGXUTEXTLINE>::~TSLinkedNode<IGXUTEXTLINE>(v6);
    SMemFree(v6, aAuigxutextline, -2, 0);
  }
  if ( s_FTLibrary != nullptr )
    FT_Done_Library((int)s_FTLibrary);
  s_FTLibrary = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001BDE10  EA 0x5BDE10
// DIA: CGxStringBatch::~CGxStringBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall CGxStringBatch::~CGxStringBatch(CGxStringBatch *this)
{
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *p_m_fulllist; // esi
  TSLink<BATCHEDRENDERFONTDESC> *v2; // eax
  TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *p_m_fontBatch; // esi
  unsigned int v4; // ebx
  int v5; // edi
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *m_data; // ecx
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v7; // edi
  TSLink<BATCHEDRENDERFONTDESC> *v8; // eax
  unsigned int v9; // ebx
  TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *p_m_slotlistarray; // esi
  BATCHEDRENDERFONTDESC *m_next; // edi
  TSLink<BATCHEDRENDERFONTDESC> *v12; // eax
  TSLink<BATCHEDRENDERFONTDESC> *v13; // eax
  int v14; // [esp+8h] [ebp-8h]

  this->m_fontBatch.m_fullnessIndicator = 0;
  p_m_fulllist = &this->m_fontBatch.m_fulllist;
  while ( (int)p_m_fulllist->m_terminator.m_next > 0 )
  {
    v2 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
           p_m_fulllist,
           p_m_fulllist->m_terminator.m_next);
    TSLink<BATCHEDRENDERFONTDESC>::Unlink(v2);
  }
  p_m_fontBatch = &this->m_fontBatch;
  v4 = 0;
  if ( this->m_fontBatch.m_slotlistarray.m_count != 0 )
  {
    v5 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
          &this->m_fontBatch.m_slotlistarray,
          v4);
        m_data = this->m_fontBatch.m_slotlistarray.m_data;
        if ( (int)m_data[v5].m_terminator.m_next <= 0 )
          break;
        p_m_fontBatch->TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::InternalDelete(
          p_m_fontBatch,
          m_data[v5].m_terminator.m_next);
      }
      ++v4;
      ++v5;
    }
    while ( v4 < this->m_fontBatch.m_slotlistarray.m_count );
  }
  v7 = &this->m_fontBatch.m_fulllist;
  p_m_fontBatch->__vftable = (TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>_vtbl *)&TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::`vftable';
  this->m_fontBatch.m_fullnessIndicator = 0;
  while ( (int)this->m_fontBatch.m_fulllist.m_terminator.m_next > 0 )
  {
    v8 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
           &this->m_fontBatch.m_fulllist,
           this->m_fontBatch.m_fulllist.m_terminator.m_next);
    TSLink<BATCHEDRENDERFONTDESC>::Unlink(v8);
  }
  v9 = 0;
  if ( this->m_fontBatch.m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &this->m_fontBatch.m_slotlistarray;
    v14 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v9);
        m_next = this->m_fontBatch.m_slotlistarray.m_data[v14].m_terminator.m_next;
        if ( (int)m_next <= 0 )
          break;
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v9);
        v12 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
                &this->m_fontBatch.m_slotlistarray.m_data[v14],
                m_next);
        TSLink<BATCHEDRENDERFONTDESC>::Unlink(v12);
      }
      ++v9;
      ++v14;
    }
    while ( v9 < this->m_fontBatch.m_slotlistarray.m_count );
    v7 = &this->m_fontBatch.m_fulllist;
  }
  TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::~TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>(&this->m_fontBatch.m_slotlistarray);
  while ( (int)v7->m_terminator.m_next > 0 )
  {
    v13 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(v7, v7->m_terminator.m_next);
    TSLink<BATCHEDRENDERFONTDESC>::Unlink(v13);
  }
  TSLink<BATCHEDRENDERFONTDESC>::Unlink(&v7->m_terminator);
  if ( this->m_link.m_prevlink != nullptr )
  {
    TSLink<CGxStringBatch>::NextLink(&this->m_link, -1)->m_prevlink = this->m_link.m_prevlink;
    this->m_link.m_prevlink->m_next = this->m_link.m_next;
    this->m_link.m_prevlink = nullptr;
    this->m_link.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BDF80  EA 0x5BDF80
// DIA: GxuFontCreateFont
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
HFACE__ *__fastcall GxuFontCreateFont(CGxFont **face, CGxFont **a2, float fontHeight, unsigned int flags)
{
  CGxFont *v6; // eax
  CGxFont *v7; // edi
  TSLink<CGxFont> *p_m_link; // esi
  TSLink<CGxFont> *m_prevlink; // eax
  unsigned int v10; // eax
  HFACE__ *v11; // esi

  if ( face == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 186);
    SErrDisplayAppFatal(aName);
  }
  if ( *(_BYTE *)face == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 187);
    SErrDisplayAppFatal(aName_0);
  }
  if ( fontHeight >= 1.0 || fontHeight <= 0.0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 188);
    SErrDisplayAppFatal(aFontheight10fF);
  }
  if ( s_FTLibrary == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 190, aSFtlibrary, 0, 1u);
  v6 = (CGxFont *)SMemAlloc(0x2F4u, aAucgxfont, -2, 8u);
  if ( v6 != nullptr )
  {
    v7 = CGxFont::CGxFont(v6);
    p_m_link = &v7->m_link;
    if ( v7 != nullptr )
      goto LABEL_13;
  }
  else
  {
    v7 = nullptr;
  }
  p_m_link = &s_fonts.m_terminator;
LABEL_13:
  if ( p_m_link->m_prevlink != nullptr )
  {
    TSLink<CGxFont>::NextLink(p_m_link, -1)->m_prevlink = p_m_link->m_prevlink;
    p_m_link->m_prevlink->m_next = p_m_link->m_next;
    p_m_link->m_prevlink = nullptr;
    p_m_link->m_next = nullptr;
  }
  m_prevlink = s_fonts.m_terminator.m_prevlink;
  p_m_link->m_prevlink = s_fonts.m_terminator.m_prevlink;
  p_m_link->m_next = m_prevlink->m_next;
  m_prevlink->m_next = v7;
  s_fonts.m_terminator.m_prevlink = p_m_link;
  if ( v7 == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 192, aNewface, 0, 1u);
  v10 = flags;
  if ( (flags & 8) != 0 )
    v10 = flags | 1;
  if ( (v10 & 0x10) != 0 )
    v10 |= 2u;
  v11 = CGxFont::Initialize(v7, (char *)face, v10, fontHeight);
  if ( v11 == nullptr )
  {
    CGxFont::~CGxFont(v7, nullptr);
    SMemFree(v7, aAucgxfont, -2, 0);
    v7 = nullptr;
  }
  *a2 = v7;
  return v11;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE120  EA 0x5BE120
// DIA: GxuFontGetFontName
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
const char *__thiscall GxuFontGetFontName(CGxFont *this)
{
  if ( this != nullptr )
    return CGxFont::GetName(this);
  else
    return nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE130  EA 0x5BE130
// DIA: GxuFontGetFontFlags
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __thiscall GxuFontGetFontFlags(int this)
{
  if ( this != 0 )
    return *(_DWORD *)(this + 452);
  else
    return 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE140  EA 0x5BE140
// DIA: GxuFontDestroyFont
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __usercall GxuFontDestroyFont(CGxFont **face@<eax>, CGxFont **a2@<ecx>)
{
  CGxFont *v3; // esi

  v3 = *a2;
  if ( *a2 != nullptr )
  {
    CGxFont::~CGxFont(*a2, (HFACE__ *)face);
    SMemFree(v3, aAucgxfont, -2, 0);
  }
  *a2 = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE170  EA 0x5BE170
// DIA: GxuFontCreateString
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuFontCreateString(
        CGxFont *text,
        const char *a2,
        float fontHeight,
        const NTempest::C3Vector *position,
        float blockWidth,
        float blockHeight,
        float spacing,
        CGxString **string,
        EGxFontVJusts vertJustification,
        EGxFontHJusts horzJustification,
        unsigned int flags,
        const NTempest::CImVector *color,
        float charSpacing)
{
  unsigned int v15; // ebx
  int v16; // esi
  CGxString *newString; // [esp+38h] [ebp+28h]

  if ( text == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 264);
    SErrDisplayAppFatal(aFace);
  }
  if ( a2 == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 265);
    SErrDisplayAppFatal(aText);
  }
  v15 = flags;
  if ( fontHeight == 0.0 && (flags & 4) == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 266);
    SErrDisplayAppFatal(aFontheightFlag);
  }
  if ( blockWidth == 0.0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 267);
    SErrDisplayAppFatal(aBlockwidth);
  }
  if ( blockHeight == 0.0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 268);
    SErrDisplayAppFatal(aBlockheight);
  }
  if ( vertJustification >= GxVJ_Last )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 269);
    SErrDisplayAppFatal(aVertjustificat);
  }
  if ( horzJustification >= GxHJ_Last )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 270);
    SErrDisplayAppFatal(aHorzjustificat);
  }
  if ( (flags & 0x80u) != 0 )
    v15 = flags & 0xFFFFFFFE;
  if ( (text->m_flags & 9) != 0 )
    v15 &= ~1u;
  newString = CGxString::GetNewString(1);
  v16 = CGxString::Initialize(
          newString,
          blockHeight,
          fontHeight,
          position,
          blockWidth,
          blockHeight,
          text,
          a2,
          vertJustification,
          horzJustification,
          spacing,
          v15,
          color);
  if ( v16 == 0 )
    GxuFontDestroyString(nullptr);
  *string = newString;
  return v16;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE2F0  EA 0x5BE2F0
// DIA: GxuFontDestroyString
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall GxuFontDestroyString(CGxString **this)
{
  CGxString *v3; // eax
  TSLink<CGxString> *m_prevlink; // esi
  int m_next; // edx
  int v6; // edx

  v3 = *this;
  if ( *this != nullptr )
  {
    m_prevlink = v3->m_link.m_prevlink;
    if ( v3->m_link.m_prevlink != nullptr )
    {
      m_next = (int)v3->m_link.m_next;
      if ( m_next > 0 )
        v6 = (char *)v3 - (char *)m_prevlink->m_next + m_next;
      else
        v6 = ~m_next;
      *(_DWORD *)v6 = m_prevlink;
      v3->m_link.m_prevlink->m_next = v3->m_link.m_next;
      v3->m_link.m_prevlink = nullptr;
      v3->m_link.m_next = nullptr;
    }
    CGxString::Recycle(*this);
    *this = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BE340  EA 0x5BE340
// DIA: GxuFontRenderString
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuFontRenderString(
        CGxFont *text,
        const char *a2,
        float textHeight,
        const NTempest::C3Vector *position,
        NTempest::CImVector color,
        float blockWidth,
        float blockHeight,
        EGxFontVJusts vertJustification,
        EGxFontHJusts horzJustification,
        unsigned int flags,
        float spacing,
        float charSpacing)
{
  CGxString *NewString; // esi
  int v16; // edi

  if ( textHeight == 0.0 && (flags & 4) == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 330);
    SErrDisplayAppFatal(aTextheightFlag);
  }
  if ( text == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 331);
    SErrDisplayAppFatal(aFont);
  }
  if ( a2 == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 332);
    SErrDisplayAppFatal(aText);
  }
  NewString = CGxString::GetNewString(0);
  v16 = CGxString::Initialize(
          NewString,
          textHeight,
          textHeight,
          position,
          blockWidth,
          blockHeight,
          text,
          a2,
          vertJustification,
          horzJustification,
          spacing,
          flags,
          &color);
  if ( v16 != 0 )
  {
    CGxString::Render(NewString);
    v16 = 1;
  }
  if ( NewString != nullptr )
    CGxString::Recycle(NewString);
  return v16;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE420  EA 0x5BE420
// DIA: GxuFontRender
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall GxuFontRender(CGxString *this)
{
  if ( this != nullptr )
    CGxString::Render(this);
}


// -----------------------------------------------------------------------------
// RVA 0x001BE430  EA 0x5BE430
// DIA: GxuFontRender
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuFontRender(CGxString *xform, const NTempest::C44Matrix *a2)
{
  if ( xform != nullptr )
    CGxString::Render(xform, a2);
}


// -----------------------------------------------------------------------------
// RVA 0x001BE440  EA 0x5BE440
// DIA: GxuFontGetStringHeight
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __thiscall GxuFontGetStringHeight(int this)
{
  if ( this != 0 )
    return *(float *)(this + 136);
  else
    return 0.0;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE460  EA 0x5BE460
// DIA: GxuFontCreateBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CGxStringBatch *__fastcall GxuFontCreateBatch()
{
  TSLink<CGxStringBatch> *m_prevlink; // esi
  CGxStringBatch *result; // eax
  CGxStringBatch *m_next; // ebx
  int v3; // edx
  CGxStringBatch **p_m_next; // edi
  int v5; // edx
  TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *v6; // eax
  TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *v7; // esi
  TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *v8; // edi

  if ( (int)s_unusedBatches.m_terminator.m_next <= 0 )
  {
    v6 = (TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *)SMemAlloc(
                                                                                      0x34u,
                                                                                      aAucgxstringbat,
                                                                                      -2,
                                                                                      8u);
    v7 = v6;
    if ( v6 != nullptr )
    {
      v8 = v6 + 1;
      v6->m_linkoffset = 0;
      v6->m_terminator.m_prevlink = nullptr;
      TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>(v6 + 1);
      v8->m_linkoffset = -572662307;
      TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::InitializeTerminator(v7 + 1);
      TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>((TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *)&v7[2].m_terminator);
      v7[2].m_terminator.m_prevlink = (TSLink<BATCHEDRENDERFONTDESC> *)&TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::`vftable';
      v7[3].m_terminator.m_next = nullptr;
      v7->m_terminator.m_next = (BATCHEDRENDERFONTDESC *)&TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::`vftable';
      v7[2].m_linkoffset = 0;
      TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::ChangeLinkOffset(v7 + 1, 12);
      v7[4].m_linkoffset = -1;
      return (CGxStringBatch *)v7;
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    m_prevlink = s_unusedBatches.m_terminator.m_next->m_link.m_prevlink;
    result = s_unusedBatches.m_terminator.m_next;
    m_next = s_unusedBatches.m_terminator.m_next;
    if ( s_unusedBatches.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v3 = (int)s_unusedBatches.m_terminator.m_next->m_link.m_next;
      p_m_next = &s_unusedBatches.m_terminator.m_next->m_link.m_next;
      if ( v3 > 0 )
        v5 = (char *)s_unusedBatches.m_terminator.m_next - (char *)m_prevlink->m_next + v3;
      else
        v5 = ~v3;
      *(_DWORD *)v5 = m_prevlink;
      m_next->m_link.m_prevlink->m_next = *p_m_next;
      *p_m_next = nullptr;
      m_next->m_link.m_prevlink = nullptr;
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE520  EA 0x5BE520
// DIA: GxuFontAddToBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuFontAddToBatch(CGxStringBatch *string, CGxString *a2)
{
  if ( string == nullptr || a2 == nullptr )
    return 0;
  CGxStringBatch::AddString(string, a2);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE540  EA 0x5BE540
// DIA: GxuFontRemoveFromBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall GxuFontRemoveFromBatch(int this)
{
  _DWORD *v3; // eax
  int v4; // esi
  int v5; // edx
  int v6; // edx

  if ( this == 0 )
    return 0;
  v3 = (_DWORD *)(this + 16);
  v4 = *(_DWORD *)(this + 16);
  if ( v4 != 0 )
  {
    v5 = *(_DWORD *)(this + 20);
    if ( v5 > 0 )
      v6 = (int)v3 + v5 - *(_DWORD *)(v4 + 4);
    else
      v6 = ~v5;
    *(_DWORD *)v6 = v4;
    *(_DWORD *)(*(_DWORD *)(this + 16) + 4) = *(_DWORD *)(this + 20);
    *v3 = 0;
    *(_DWORD *)(this + 20) = 0;
  }
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE590  EA 0x5BE590
// DIA: GxuFontRenderBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall GxuFontRenderBatch(CGxStringBatch *this)
{
  if ( this == nullptr )
    return 0;
  CGxStringBatch::RenderBatch(this);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE5B0  EA 0x5BE5B0
// DIA: GxuFontClearBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall GxuFontClearBatch(int this)
{
  int v3; // ebx
  int v4; // eax
  TSLink<BATCHEDRENDERFONTDESC> *v5; // esi
  unsigned int v6; // edi
  int v7; // esi
  int v8; // eax
  const char *v9; // eax
  int v10; // ecx
  int v11; // [esp-24h] [ebp-28h]
  int v12; // [esp-20h] [ebp-24h]
  unsigned int v13; // [esp-1Ch] [ebp-20h]
  const char *v14; // [esp-18h] [ebp-1Ch]
  int v15; // [esp+0h] [ebp-4h]

  if ( this == 0 )
    return 0;
  v3 = this + 8;
  *(_DWORD *)(this + 24) = 0;
  while ( 1 )
  {
    v4 = *(_DWORD *)(v3 + 12);
    if ( v4 <= 0 )
      break;
    v5 = (TSLink<BATCHEDRENDERFONTDESC> *)(v4 + *(_DWORD *)(v3 + 4));
    if ( v5->m_prevlink != nullptr )
    {
      TSLink<BATCHEDRENDERFONTDESC>::NextLink(v5, -1)->m_prevlink = v5->m_prevlink;
      v5->m_prevlink->m_next = v5->m_next;
      v5->m_prevlink = nullptr;
      v5->m_next = nullptr;
    }
  }
  v6 = 0;
  if ( *(_DWORD *)(v3 + 28) != 0 )
  {
    v7 = v3 + 20;
    v15 = 0;
    do
    {
      while ( 1 )
      {
        if ( v6 >= *(_DWORD *)(v3 + 28) )
        {
          v8 = (*(int (__thiscall **)(int, int, int, char *, unsigned int, _DWORD))(*(_DWORD *)v7 + 4))(
                 v3 + 20,
                 1,
                 1,
                 aIndex0x08xArra,
                 v6,
                 *(_DWORD *)(v3 + 28));
          v9 = (const char *)(**(int (__thiscall ***)(int, int))v7)(v3 + 20, v8);
          SErrDisplayErrorFmt(0x85100080, v9, v11, v12, v13, v14);
        }
        v10 = *(_DWORD *)(v3 + 32);
        if ( *(int *)(v10 + v15 + 8) <= 0 )
          break;
        (**(void (__thiscall ***)(int, _DWORD))v3)(v3, *(_DWORD *)(v10 + v15 + 8));
      }
      ++v6;
      v15 += 12;
    }
    while ( v6 < *(_DWORD *)(v3 + 28) );
  }
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE680  EA 0x5BE680
// DIA: GxuFontDestroyBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall GxuFontDestroyBatch(TSLink<CGxStringBatch> *this)
{
  TSLink<CGxStringBatch> *v3; // esi
  int m_next; // eax
  unsigned int v5; // edi
  int v6; // ebx
  TSLink<CGxStringBatch> *Link; // eax
  TSLink<CGxStringBatch> *m_prevlink; // eax
  TSLink<CGxStringBatch> *v9; // [esp+0h] [ebp-4h]

  v9 = this;
  if ( this == nullptr )
    return 0;
  v3 = this + 1;
  this[3].m_prevlink = nullptr;
  while ( 1 )
  {
    m_next = (int)v3[1].m_next;
    if ( m_next <= 0 )
      break;
    TSLink<BATCHEDRENDERFONTDESC>::Unlink((TSLink<BATCHEDRENDERFONTDESC> *)((char *)v3->m_next + m_next));
    this = v9;
  }
  v5 = 0;
  if ( v3[3].m_next != nullptr )
  {
    v6 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
          (TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *)&v3[2].m_next,
          v5);
        if ( *(int *)((char *)&v3[4].m_prevlink[1].m_prevlink + v6) <= 0 )
          break;
        ((void (__thiscall *)(TSLink<CGxStringBatch> *, _DWORD))v3->m_prevlink->m_prevlink)(
          v3,
          *(TSLink<CGxStringBatch> **)((char *)&v3[4].m_prevlink[1].m_prevlink + v6));
      }
      ++v5;
      v6 += 12;
    }
    while ( (CGxStringBatch *)v5 < v3[3].m_next );
    this = v9;
  }
  if ( this->m_prevlink != nullptr )
  {
    Link = TSLink<CGxStringBatch>::NextLink(this, -1);
    this = v9;
    Link->m_prevlink = v9->m_prevlink;
    v9->m_prevlink->m_next = v9->m_next;
    v9->m_prevlink = nullptr;
    v9->m_next = nullptr;
  }
  m_prevlink = s_unusedBatches.m_terminator.m_prevlink;
  this->m_prevlink = s_unusedBatches.m_terminator.m_prevlink;
  this->m_next = m_prevlink->m_next;
  m_prevlink->m_next = (CGxStringBatch *)this;
  s_unusedBatches.m_terminator.m_prevlink = this;
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE740  EA 0x5BE740
// DIA: GxuFontAddToInternalBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall GxuFontAddToInternalBatch(CGxString *this)
{
  if ( this == nullptr )
    return 0;
  CGxStringBatch::AddString(&s_stringBatch_0, this);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE760  EA 0x5BE760
// DIA: GxuFontRenderInternalBatch
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __cdecl GxuFontRenderInternalBatch()
{
  char *v0; // esi
  unsigned int m_count; // ecx
  unsigned int v2; // esi
  int v3; // edi
  int v4; // eax
  const char *v5; // eax
  int v6; // [esp-20h] [ebp-20h]
  int v7; // [esp-1Ch] [ebp-1Ch]
  unsigned int v8; // [esp-18h] [ebp-18h]
  const char *v9; // [esp-14h] [ebp-14h]

  CGxStringBatch::RenderBatch(&s_stringBatch_0);
  s_stringBatch_0.m_fontBatch.m_fullnessIndicator = 0;
  while ( (int)s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next > 0 )
  {
    v0 = (char *)s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next
       + s_stringBatch_0.m_fontBatch.m_fulllist.m_linkoffset;
    if ( *(unsigned int *)((char *)&s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next->m_hashval
                         + s_stringBatch_0.m_fontBatch.m_fulllist.m_linkoffset) != 0 )
    {
      TSLink<BATCHEDRENDERFONTDESC>::NextLink(
        (TSLink<BATCHEDRENDERFONTDESC> *)((char *)s_stringBatch_0.m_fontBatch.m_fulllist.m_terminator.m_next
                                        + s_stringBatch_0.m_fontBatch.m_fulllist.m_linkoffset),
        -1)->m_prevlink = *(TSLink<BATCHEDRENDERFONTDESC> **)v0;
      *(_DWORD *)(*(_DWORD *)v0 + 4) = *((_DWORD *)v0 + 1);
      *(_DWORD *)v0 = 0;
      *((_DWORD *)v0 + 1) = 0;
    }
  }
  m_count = s_stringBatch_0.m_fontBatch.m_slotlistarray.m_count;
  v2 = 0;
  if ( s_stringBatch_0.m_fontBatch.m_slotlistarray.m_count != 0 )
  {
    v3 = 0;
    while ( 1 )
    {
      if ( v2 >= m_count )
      {
        v4 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))s_stringBatch_0.m_fontBatch.m_slotlistarray.TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo)(
               &s_stringBatch_0.m_fontBatch.m_slotlistarray,
               1,
               1,
               aIndex0x08xArra,
               v2,
               m_count);
        v5 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int))s_stringBatch_0.m_fontBatch.m_slotlistarray.TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName)(
                             &s_stringBatch_0.m_fontBatch.m_slotlistarray,
                             v4);
        SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
        m_count = s_stringBatch_0.m_fontBatch.m_slotlistarray.m_count;
      }
      while ( (int)s_stringBatch_0.m_fontBatch.m_slotlistarray.m_data[v3].m_terminator.m_next <= 0 )
      {
        ++v2;
        ++v3;
        if ( v2 >= m_count )
          return;
      }
      s_stringBatch_0.m_fontBatch.TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::InternalDelete(
        &s_stringBatch_0.m_fontBatch,
        s_stringBatch_0.m_fontBatch.m_slotlistarray.m_data[v3].m_terminator.m_next);
      m_count = s_stringBatch_0.m_fontBatch.m_slotlistarray.m_count;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BE840  EA 0x5BE840
// DIA: GxuFontGetTextExtent
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuFontGetTextExtent(
        CGxFont *text,
        char *a2,
        unsigned int numBytes,
        int height,
        float *extent,
        float charSpacing,
        __int16 flags)
{
  InternalGetTextExtent(text, a2, numBytes, height, extent, flags);
}


// -----------------------------------------------------------------------------
// RVA 0x001BE860  EA 0x5BE860
// DIA: GxuFontGetWrapPoint
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuFontGetWrapPoint(
        char *text,
        const char *a2,
        CGxFont *fontHeight,
        float blockWidth,
        unsigned int *numBytes,
        float *pExtent,
        const char **pNextText,
        float spacing,
        unsigned int flags)
{
  if ( text == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 487);
    SErrDisplayAppFatal(aFace);
  }
  if ( a2 == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 488);
    SErrDisplayAppFatal(aText);
  }
  CalcWrapPoint(text, a2, fontHeight, blockWidth, numBytes, pExtent, pNextText, flags);
}


// -----------------------------------------------------------------------------
// RVA 0x001BE8E0  EA 0x5BE8E0
// DIA: GxuFontGetWrappedTextHeight
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __userpurge GxuFontGetWrappedTextHeight@<st0>(
        CGxFont *face@<eax>,
        char *text@<ecx>,
        char *a3@<edx>,
        double a4@<st0>,
        float fontHeight,
        float blockWidth,
        float lineSpacing,
        char *flags)
{
  char *v9; // esi
  unsigned int v10; // ebx
  unsigned int v11; // eax
  QUOTEDCODE v12; // eax
  double v13; // st7
  __int64 v15; // [esp+8h] [ebp-14h] BYREF
  float extent; // [esp+10h] [ebp-Ch] BYREF
  unsigned int wide; // [esp+14h] [ebp-8h] BYREF
  unsigned int lines; // [esp+18h] [ebp-4h]

  v9 = a3;
  if ( text == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 513);
    SErrDisplayAppFatal(aFace);
  }
  if ( a3 == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 514);
    SErrDisplayAppFatal(aText);
  }
  v10 = (unsigned int)flags;
  if ( ((unsigned __int8)flags & 4) != 0 )
  {
    GxuFontGetOneToOneHeight(face);
    fontHeight = a4;
  }
  flags = nullptr;
  extent = 0.0;
  lines = 0;
  do
  {
    if ( *v9 == 0 )
      break;
    v11 = SStrLen(v9);
    v12 = GxuDetermineQuotedCode(v9, (unsigned int *)&v15 + 1, nullptr, v10, &wide, v11);
    if ( wide == 10 || v12 == CODE_NEWLINE )
    {
      v9 += HIDWORD(v15);
      flags = v9;
    }
    else
    {
      CalcWrapPoint(text, v9, (CGxFont *)LODWORD(fontHeight), blockWidth, nullptr, &extent, (const char **)&flags, v10);
      v9 = flags;
    }
    ++lines;
    if ( (v10 & 2) != 0 )
      break;
  }
  while ( v9 != nullptr );
  v15 = lines - 1;
  v13 = (double)v15;
  LODWORD(v15) = lines;
  return v13 * lineSpacing + (double)v15 * fontHeight;
}


// -----------------------------------------------------------------------------
// RVA 0x001BE9F0  EA 0x5BE9F0
// DIA: GxuFontGetMaxCharsWithinWidth
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall GxuFontGetMaxCharsWithinWidth(
        CGxFont *text,
        char *a2,
        CGxFont *height,
        float maxWidth,
        unsigned int lineBytes,
        float *extent,
        float charSpacing,
        __int16 flags)
{
  return InternalGetMaxCharsWithinWidth(
           height,
           text,
           a2,
           *(float *)&height,
           maxWidth,
           lineBytes,
           extent,
           flags,
           nullptr,
           nullptr,
           nullptr);
}


// -----------------------------------------------------------------------------
// RVA 0x001BEA20  EA 0x5BEA20
// DIA: GxuFontGetMaxCharsWithinWidthFromEnd
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall GxuFontGetMaxCharsWithinWidthFromEnd(
        char *text,
        char *a2,
        CGxFont *fontHeight,
        float width,
        unsigned int lineBytes,
        float *extent,
        float charSpacing,
        __int16 flags)
{
  void *v10; // esp
  float *v11; // esi
  unsigned int result; // eax
  unsigned int v13; // ecx
  float v14[3]; // [esp+0h] [ebp-1Ch] BYREF
  unsigned int bytesInString; // [esp+Ch] [ebp-10h] BYREF
  char *v16; // [esp+10h] [ebp-Ch]
  char *v17; // [esp+14h] [ebp-8h]
  float textExtent; // [esp+18h] [ebp-4h] BYREF
  float remaining; // [esp+28h] [ebp+Ch]

  v17 = a2;
  v16 = text;
  if ( text == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 600, aFont, 0, 1u);
  if ( a2 == nullptr || *a2 == 0 || lineBytes == 0 || width == 0.0 )
    return 0;
  v10 = alloca(4 * lineBytes);
  v11 = v14;
  result = InternalGetMaxCharsWithinWidth(
             fontHeight,
             (CGxFont *)v16,
             v17,
             *(float *)&fontHeight,
             10000.0,
             lineBytes,
             &textExtent,
             flags,
             &bytesInString,
             v14,
             &v14[lineBytes]);
  if ( textExtent <= (double)width )
  {
    if ( extent != nullptr )
      *extent = textExtent;
    return result;
  }
  v13 = 0;
  remaining = textExtent - width;
  if ( result == 0 )
  {
LABEL_14:
    if ( extent != nullptr )
      *extent = 0.0;
    return 0;
  }
  while ( *v11 <= (double)remaining )
  {
    ++v13;
    ++v11;
    if ( v13 >= result )
      goto LABEL_14;
  }
  if ( extent != nullptr )
    *extent = textExtent - *v11;
  result -= v13;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001BEB50  EA 0x5BEB50
// DIA: GxuFontWrapText
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall GxuFontWrapText(
        char *text,
        char *a2,
        unsigned int lineBytes,
        float fontHeight,
        float blockWidth,
        unsigned int *outputList,
        unsigned int outputListElements,
        float charSpacing,
        unsigned int flags)
{
  CGxFont *v11; // eax
  __int16 v12; // fps
  bool v13; // c0
  char v14; // c2
  bool v15; // c3
  unsigned int v17; // edi
  unsigned int v18; // eax
  QUOTEDCODE v19; // eax
  unsigned int unusedNumBytes; // [esp+8h] [ebp-1Ch] BYREF
  float unusedExtents; // [esp+Ch] [ebp-18h] BYREF
  unsigned int advance; // [esp+10h] [ebp-14h] BYREF
  const char *nextText; // [esp+14h] [ebp-10h] BYREF
  char *currentText; // [esp+18h] [ebp-Ch]
  unsigned int wide; // [esp+1Ch] [ebp-8h] BYREF
  const char *originalText; // [esp+20h] [ebp-4h]
  unsigned int flagsa; // [esp+44h] [ebp+20h]

  currentText = text;
  if ( text == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 661, aFont, 0, 1u);
  if ( outputListElements == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 662, aOutputlistelem, 0, 1u);
  if ( a2 == nullptr )
    return 0;
  if ( *a2 == 0 )
    return 0;
  HIWORD(v11) = HIWORD(lineBytes);
  if ( lineBytes == 0 )
    return 0;
  if ( fontHeight == 0.0 )
    return 0;
  v13 = blockWidth < 0.0;
  v14 = 0;
  v15 = blockWidth == 0.0;
  LOWORD(v11) = v12;
  if ( blockWidth == 0.0 )
    return 0;
  if ( (flags & 4) != 0 )
  {
    GxuFontGetOneToOneHeight(v11);
    fontHeight = blockWidth;
  }
  originalText = a2;
  v17 = 0;
  flagsa = (unsigned int)&a2[lineBytes];
  do
  {
    if ( (unsigned int)a2 >= flagsa )
      break;
    if ( v17 < outputListElements )
      outputList[v17] = a2 - originalText;
    ++v17;
    v18 = SStrLen(a2);
    v19 = GxuDetermineQuotedCode(a2, &advance, nullptr, flags, &wide, v18);
    if ( wide == 10 || v19 == CODE_NEWLINE )
    {
      a2 += advance;
    }
    else
    {
      CalcWrapPoint(
        currentText,
        a2,
        (CGxFont *)LODWORD(fontHeight),
        blockWidth,
        &unusedNumBytes,
        &unusedExtents,
        &nextText,
        flags);
      a2 = (char *)nextText;
    }
  }
  while ( a2 != nullptr );
  if ( *(_BYTE *)(flagsa - 1) == 10 )
  {
    if ( v17 < outputListElements )
      outputList[v17] = lineBytes;
    ++v17;
  }
  return v17;
}


// -----------------------------------------------------------------------------
// RVA 0x001BECA0  EA 0x5BECA0
// DIA: GxuFontGetOneToOneHeight
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __thiscall GxuFontGetOneToOneHeight(int this)
{
  if ( this == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 723);
    SErrDisplayAppFatal(aFont);
  }
  if ( *(_DWORD *)(this + 444) == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 724, aFontMCellheigh, 0, 1u);
  if ( *(_DWORD *)(this + 444) > 0x20u )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 725, aFontMCellheigh_0, 0, 1u);
  return (double)*(unsigned int *)(this + 176) / (double)(int)g_heightPixels;
}


// -----------------------------------------------------------------------------
// RVA 0x001BED60  EA 0x5BED60
// DIA: GxuFontStripEscapeCodes
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
char *__fastcall GxuFontStripEscapeCodes(
        char *numBytes,
        unsigned int a2,
        unsigned int flags,
        char *buffer,
        unsigned int bufferSize)
{
  unsigned int v9; // eax
  unsigned int v10; // esi
  QUOTEDCODE v11; // eax
  unsigned int v12; // ecx
  unsigned int i; // eax
  QUOTEDCODE v14; // eax
  unsigned int v15; // eax
  unsigned int wide; // [esp+Ch] [ebp-Ch] BYREF
  const char *originalString; // [esp+10h] [ebp-8h]
  unsigned int advance; // [esp+14h] [ebp-4h] BYREF
  unsigned int remainingBytes; // [esp+24h] [ebp+Ch]

  if ( buffer == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 737, aBuffer, 0, 1u);
  if ( bufferSize == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 738, aBuffersize, 0, 1u);
  *buffer = 0;
  if ( numBytes != nullptr && *numBytes != 0 && a2 != 0 )
  {
    v9 = a2;
    v10 = 0;
    remainingBytes = v9;
    if ( *numBytes != 0 )
    {
      while ( v9 != 0 )
      {
        originalString = numBytes;
        v11 = GxuDetermineQuotedCode(numBytes, &advance, nullptr, 0, &wide, v9);
        v12 = advance;
        switch ( v11 )
        {
          case CODE_COLORON:
          case CODE_COLORRESTORE:
          case CODE_HYPERLINKSTART:
            if ( (flags & s_stripFlags[v11].stripFlags) != 0 )
            {
              numBytes += advance;
              remainingBytes -= advance;
              goto LABEL_30;
            }
            if ( v10 >= bufferSize - advance )
              goto LABEL_25;
            if ( advance != 0 )
            {
              for ( i = advance; i != 0; --i )
                buffer[v10++] = *numBytes++;
            }
            goto LABEL_29;
          case CODE_NEWLINE:
          case CODE_PIPE:
          case CODE_HYPERLINKSTOP:
            remainingBytes -= advance;
            v14 = v11;
            numBytes += advance;
            if ( (flags & s_stripFlags[v14].stripFlags) != 0 )
              goto LABEL_30;
            if ( v10 >= bufferSize - advance )
            {
LABEL_25:
              if ( v10 >= bufferSize - advance )
                goto LABEL_31;
              v15 = 0;
              if ( advance != 0 )
              {
                do
                  buffer[v10++] = originalString[v15++];
                while ( v15 < v12 );
              }
              numBytes += v12;
LABEL_29:
              remainingBytes -= v12;
            }
            else
            {
              if ( s_stripFlags[v14].addEscapeChar != 0 )
                buffer[v10++] = 124;
              buffer[v10++] = s_stripFlags[v14].charCode;
            }
LABEL_30:
            if ( *numBytes == 0 )
              goto LABEL_31;
            v9 = remainingBytes;
            break;
          default:
            goto LABEL_25;
        }
      }
    }
LABEL_31:
    buffer[v10] = 0;
    return buffer;
  }
  else
  {
    SStrPrintf(buffer, bufferSize, "%s", numBytes);
    return numBytes;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BEF10  EA 0x5BEF10
// DIA: GxuFontGetLastColorCode
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuFontGetLastColorCode(char *numBytes, unsigned int a2, NTempest::CImVector *color)
{
  char *v4; // esi
  int v5; // ebx
  bool v6; // zf
  QUOTEDCODE v8; // eax
  unsigned int wide; // [esp+Ch] [ebp-10h] BYREF
  unsigned int advance; // [esp+10h] [ebp-Ch] BYREF
  NTempest::CImVector colorCode; // [esp+14h] [ebp-8h]
  NTempest::CImVector foundColor; // [esp+18h] [ebp-4h] BYREF

  v4 = numBytes;
  v5 = 0;
  v6 = *numBytes == 0;
  colorCode = 0;
  if ( !v6 )
  {
    do
    {
      if ( a2 == 0 )
        break;
      foundColor = 0;
      v8 = GxuDetermineQuotedCode(v4, &advance, &foundColor, 0, &wide, a2);
      if ( v8 != CODE_COLORON )
      {
        if ( v8 == CODE_COLORRESTORE )
          v5 = 0;
      }
      else
      {
        v5 = 1;
        colorCode = foundColor;
      }
      v4 += advance;
      a2 -= advance;
    }
    while ( *v4 != 0 );
    if ( v5 != 0 )
      *color = colorCode;
  }
  return v5;
}


// -----------------------------------------------------------------------------
// RVA 0x001BEF90  EA 0x5BEF90
// DIA: GxuFontGenerateColorString
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuFontGenerateColorString(char *bufSize, unsigned int a2, const NTempest::CImVector *color)
{
  if ( bufSize == nullptr || a2 < 0xB )
    return 0;
  SStrPrintf(bufSize, a2, "|C%2.2x%2.2x%2.2x%2.2x", color->a, color->r, color->g, color->b);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BEFE0  EA 0x5BEFE0
// DIA: GxuFontSetStringColor
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall GxuFontSetStringColor(CGxString *this, NTempest::CImVector newColor)
{
  if ( this == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 889);
    SErrDisplayAppFatal(aString);
  }
  CGxString::SetColor(this, &newColor);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF030  EA 0x5BF030
// DIA: GxuFontSetStringPosition
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuFontSetStringPosition(CGxString *pos, const NTempest::C3Vector *a2)
{
  if ( pos == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_241, 899);
    SErrDisplayAppFatal(aString);
  }
  CGxString::SetStringPosition(pos, a2);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF060  EA 0x5BF060
// DIA: GxuFontSetCharSpacing
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __stdcall GxuFontSetCharSpacing(float spacing)
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF070  EA 0x5BF070
// DIA: GxuFontAddShadow
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuFontAddShadow(CGxString *color, const NTempest::CImVector *a2, const NTempest::C2Vector *offset)
{
  if ( color != nullptr && SLOBYTE(color->m_flags) >= 0 )
    CGxString::AddShadow(color, offset, a2);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF090  EA 0x5BF090
// DIA: GxuFontRemoveShadow
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall GxuFontRemoveShadow(CGxString *this)
{
  if ( this != nullptr )
    CGxString::RemoveShadow(this);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF0A0  EA 0x5BF0A0
// DIA: GxuFontDuplicateString
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CGxString *__thiscall GxuFontDuplicateString(CGxString *this)
{
  if ( this != nullptr )
    return CGxString::Duplicate(this);
  else
    return nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF0B0  EA 0x5BF0B0
// DIA: GxuFontGetStringWidth
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
BOOL __fastcall GxuFontGetStringWidth(float *width, float *a2)
{
  if ( a2 == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 935, aWidth, 0, 1u);
  if ( width != nullptr )
    *a2 = width[35];
  return width != nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF0F0  EA 0x5BF0F0
// DIA: GxuFontGetStringHeight
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
BOOL __fastcall GxuFontGetStringHeight(float *height, float *a2)
{
  if ( a2 == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_241, 943, aHeight, 0, 1u);
  if ( height != nullptr )
    *a2 = height[34];
  return height != nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF130  EA 0x5BF130
// DIA: GxuFontStringHyperLinkInfo
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall GxuFontStringHyperLinkInfo(const GXUFONTHYPERLINKINFO **list, GXUFONTHYPERLINKINFO **a2)
{
  if ( list != nullptr )
    return CGxString::GetHyperLinkInfo((CGxString *)list, a2);
  else
    return 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF140  EA 0x5BF140
// DIA: GxuFontStringSetGradient
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuFontStringSetGradient(CGxString *startCharacter, int a2, int length)
{
  if ( startCharacter != nullptr )
    return CGxString::SetGradient(startCharacter, a2, length);
  else
    return -1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF160  EA 0x5BF160
// DIA: TSLinkedNode<IGXUTEXTLINE>::~TSLinkedNode<IGXUTEXTLINE>
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLinkedNode<IGXUTEXTLINE>::~TSLinkedNode<IGXUTEXTLINE>(TSLinkedNode<IGXUTEXTLINE> *this)
{
  TSLink<IGXUTEXTLINE> *m_prevlink; // esi
  int m_next; // edx
  int v3; // edx

  m_prevlink = this->m_link.m_prevlink;
  if ( this->m_link.m_prevlink != nullptr )
  {
    m_next = (int)this->m_link.m_next;
    if ( m_next > 0 )
      v3 = (char *)this - (char *)m_prevlink->m_next + m_next;
    else
      v3 = ~m_next;
    *(_DWORD *)v3 = m_prevlink;
    this->m_link.m_prevlink->m_next = this->m_link.m_next;
    this->m_link.m_prevlink = nullptr;
    this->m_link.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BF1A0  EA 0x5BF1A0
// DIA: TSFixedArray<TEXTLINETEXTURE *>::~TSFixedArray<TEXTLINETEXTURE *>
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TEXTLINETEXTURE *>::~TSFixedArray<TEXTLINETEXTURE *>(
        TSFixedArray<TEXTLINETEXTURE *> *this)
{
  TEXTLINETEXTURE **m_data; // ecx

  this->__vftable = (TSFixedArray<TEXTLINETEXTURE *>_vtbl *)&TSFixedArray<TEXTLINETEXTURE *>::`vftable';
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aPautextlinetex, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF1C0  EA 0x5BF1C0
// DIA: TSBaseArray<TEXTLINETEXTURE *>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TEXTLINETEXTURE *>::MemFileName(TSBaseArray<TEXTLINETEXTURE *> *this)
{
  return aPautextlinetex;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF1D0  EA 0x5BF1D0
// DIA: TSBaseArray<TEXTLINETEXTURE *>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TEXTLINETEXTURE *>::MemLineNo(TSBaseArray<TEXTLINETEXTURE *> *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF1E0  EA 0x5BF1E0
// DIA: $E5
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E5_263()
{
  return _E4_263();
}


// -----------------------------------------------------------------------------
// RVA 0x001BF1F0  EA 0x5BF1F0
// DIA: $E4
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E4_263()
{
  TWO_PI_263 = 3.1415927 + 3.1415927;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF200  EA 0x5BF200
// DIA: $E8
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E8_263()
{
  return _E7_263();
}


// -----------------------------------------------------------------------------
// RVA 0x001BF210  EA 0x5BF210
// DIA: $E7
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E7_263()
{
  OO_TWO_PI_263 = 1.0 / TWO_PI_263;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF230  EA 0x5BF230
// DIA: TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::InternalDelete
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::InternalDelete(
        TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *this,
        BATCHEDRENDERFONTDESC *ptr)
{
  BATCHEDRENDERFONTDESC::~BATCHEDRENDERFONTDESC(ptr);
  SMemFree(ptr, aAubatchedrende, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF260  EA 0x5BF260
// DIA: TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::InternalNew
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
BATCHEDRENDERFONTDESC *__thiscall TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::InternalNew(
        TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *this,
        TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *listptr,
        unsigned int extrabytes,
        unsigned int flags)
{
  BATCHEDRENDERFONTDESC *v4; // eax
  TSLink<BATCHEDRENDERFONTDESC> *p_m_terminator; // esi
  int m_next; // ecx
  int m_linkoffset; // edx
  BATCHEDRENDERFONTDESC *flagsa; // [esp+1Ch] [ebp+10h]

  v4 = (BATCHEDRENDERFONTDESC *)SMemAlloc(extrabytes + 40, aAubatchedrende, -2, flags | 8);
  if ( v4 != nullptr )
  {
    v4->m_linktoslot.m_prevlink = nullptr;
    v4->m_linktoslot.m_next = nullptr;
    v4->m_linktofull.m_prevlink = nullptr;
    v4->m_linktofull.m_next = nullptr;
    v4->m_key.m_key = nullptr;
    v4->face = nullptr;
    v4->m_strings.m_terminator.m_prevlink = nullptr;
    v4->m_strings.m_terminator.m_next = nullptr;
    v4->m_strings.m_terminator.m_prevlink = &v4->m_strings.m_terminator;
    v4->m_strings.m_linkoffset = 16;
    v4->m_strings.m_terminator.m_next = (CGxString *)~(unsigned int)&v4->m_strings.m_terminator;
    flagsa = v4;
    p_m_terminator = (TSLink<BATCHEDRENDERFONTDESC> *)((char *)v4 + listptr->m_linkoffset);
  }
  else
  {
    flagsa = nullptr;
    p_m_terminator = &listptr->m_terminator;
  }
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    TSLink<BATCHEDRENDERFONTDESC>::NextLink(p_m_terminator, -1)->m_prevlink = p_m_terminator->m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  p_m_terminator->m_prevlink = &listptr->m_terminator;
  p_m_terminator->m_next = listptr->m_terminator.m_next;
  m_next = (int)listptr->m_terminator.m_next;
  m_linkoffset = listptr->m_linkoffset;
  if ( m_next > 0 )
  {
    if ( m_linkoffset < 0 )
      m_linkoffset = (char *)&listptr->m_terminator - (char *)listptr->m_terminator.m_prevlink->m_next;
    *(_DWORD *)(m_linkoffset + m_next) = p_m_terminator;
    listptr->m_terminator.m_next = flagsa;
    return flagsa;
  }
  else
  {
    *(_DWORD *)~m_next = p_m_terminator;
    listptr->m_terminator.m_next = flagsa;
    return flagsa;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BF330  EA 0x5BF330
// DIA: TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::Destroy(
        TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *this)
{
  TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *v1; // edi
  int m_next; // eax
  unsigned int v3; // ebx
  _DWORD *v4; // esi
  int v5; // eax
  const char *v6; // eax
  int v7; // eax
  const char *v8; // eax
  int v9; // edi
  int v10; // eax
  TSLink<BATCHEDRENDERFONTDESC> *v11; // edi
  unsigned int m_count; // eax
  unsigned int v13; // esi
  int v14; // ebx
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *m_data; // eax
  int v16; // [esp-20h] [ebp-30h]
  int v17; // [esp-20h] [ebp-30h]
  int v18; // [esp-1Ch] [ebp-2Ch]
  int v19; // [esp-1Ch] [ebp-2Ch]
  unsigned int v20; // [esp-18h] [ebp-28h]
  unsigned int v21; // [esp-18h] [ebp-28h]
  const char *v22; // [esp-14h] [ebp-24h]
  const char *v23; // [esp-14h] [ebp-24h]
  int v24; // [esp+4h] [ebp-Ch]
  int v26; // [esp+Ch] [ebp-4h]

  v1 = this;
  this->m_fullnessIndicator = 0;
  while ( 1 )
  {
    m_next = (int)v1->m_fulllist.m_terminator.m_next;
    if ( m_next <= 0 )
      break;
    TSLink<BATCHEDRENDERFONTDESC>::Unlink((TSLink<BATCHEDRENDERFONTDESC> *)(m_next + v1->m_fulllist.m_linkoffset));
  }
  v3 = 0;
  if ( v1->m_slotlistarray.m_count != 0 )
  {
    v4 = &v1->m_slotlistarray.__vftable;
    v26 = 0;
    do
    {
      while ( 1 )
      {
        if ( v3 >= v4[2] )
        {
          v5 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, _DWORD))(*v4 + 4))(
                 v4,
                 1,
                 1,
                 aIndex0x08xArra,
                 v3,
                 v4[2]);
          v6 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v4)(v4, v5);
          SErrDisplayErrorFmt(0x85100080, v6, v16, v18, v20, v22);
        }
        v24 = *(_DWORD *)(v4[3] + v26 + 8);
        if ( v24 <= 0 )
          break;
        if ( v3 >= v4[2] )
        {
          v7 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, _DWORD))(*v4 + 4))(
                 v4,
                 1,
                 1,
                 aIndex0x08xArra,
                 v3,
                 v4[2]);
          v8 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v4)(v4, v7);
          SErrDisplayErrorFmt(0x85100080, v8, v17, v19, v21, v23);
        }
        v9 = *(_DWORD *)(v4[3] + v26);
        v10 = *(_DWORD *)(v9 + v24);
        v11 = (TSLink<BATCHEDRENDERFONTDESC> *)(v24 + v9);
        if ( v10 != 0 )
        {
          TSLink<BATCHEDRENDERFONTDESC>::NextLink(v11, -1)->m_prevlink = v11->m_prevlink;
          v11->m_prevlink->m_next = v11->m_next;
          v11->m_prevlink = nullptr;
          v11->m_next = nullptr;
        }
        v1 = this;
      }
      ++v3;
      v26 += 12;
    }
    while ( v3 < v1->m_slotlistarray.m_count );
  }
  v1->m_fullnessIndicator = 0;
  v1->m_slotmask = -1;
  m_count = v1->m_slotlistarray.m_count;
  v13 = 0;
  v1->m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v14 = 0;
    do
    {
      TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::~TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>(&v1->m_slotlistarray.m_data[v14]);
      ++v13;
      ++v14;
    }
    while ( v13 < v1->m_slotlistarray.m_count );
  }
  m_data = v1->m_slotlistarray.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_51, -2, 0);
  v1->m_slotlistarray.m_alloc = 0;
  v1->m_slotlistarray.m_count = 0;
  v1->m_slotlistarray.m_data = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF4A0  EA 0x5BF4A0
// DIA: TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::MemFileName(
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this)
{
  return aAvTsexplicitli_51;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF4B0  EA 0x5BF4B0
// DIA: TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::MemLineNo(
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF4C0  EA 0x5BF4C0
// DIA: TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *__thiscall TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::`scalar deleting destructor'(
        TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *this,
        char __flags)
{
  TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *v2; // esi
  unsigned int v3; // ebx
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *p_m_fulllist; // edi
  TSLink<BATCHEDRENDERFONTDESC> *v5; // eax
  TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *p_m_slotlistarray; // esi
  int m_next; // edi
  TSLink<BATCHEDRENDERFONTDESC> **p_m_prevlink; // edi
  int v9; // eax
  int v12; // [esp+10h] [ebp-4h]

  v2 = this;
  v3 = 0;
  this->__vftable = (TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>_vtbl *)&TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::`vftable';
  this->m_fullnessIndicator = 0;
  p_m_fulllist = &this->m_fulllist;
  while ( (int)p_m_fulllist->m_terminator.m_next > 0 )
  {
    v5 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
           p_m_fulllist,
           p_m_fulllist->m_terminator.m_next);
    TSLink<BATCHEDRENDERFONTDESC>::Unlink(v5);
  }
  if ( v2->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &v2->m_slotlistarray;
    v12 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        m_next = (int)p_m_slotlistarray->m_data[v12].m_terminator.m_next;
        if ( m_next <= 0 )
          break;
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        TSLink<BATCHEDRENDERFONTDESC>::Unlink((TSLink<BATCHEDRENDERFONTDESC> *)(m_next
                                                                              + p_m_slotlistarray->m_data[v12].m_linkoffset));
      }
      ++v3;
      ++v12;
    }
    while ( v3 < this->m_slotlistarray.m_count );
    v2 = this;
  }
  TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::~TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>(&v2->m_slotlistarray);
  p_m_prevlink = &v2->m_fulllist.m_terminator.m_prevlink;
  while ( 1 )
  {
    v9 = (int)v2->m_fulllist.m_terminator.m_next;
    if ( v9 <= 0 )
      break;
    TSLink<BATCHEDRENDERFONTDESC>::Unlink((TSLink<BATCHEDRENDERFONTDESC> *)(v9 + v2->m_fulllist.m_linkoffset));
  }
  if ( *p_m_prevlink != nullptr )
  {
    TSLink<BATCHEDRENDERFONTDESC>::NextLink(&v2->m_fulllist.m_terminator, -1)->m_prevlink = *p_m_prevlink;
    (*p_m_prevlink)->m_next = v2->m_fulllist.m_terminator.m_next;
    *p_m_prevlink = nullptr;
    v2->m_fulllist.m_terminator.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF5B0  EA 0x5BF5B0
// DIA: TSLink<BATCHEDRENDERFONTDESC>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<BATCHEDRENDERFONTDESC>::Unlink(TSLink<BATCHEDRENDERFONTDESC> *this)
{
  TSLink<BATCHEDRENDERFONTDESC> *m_prevlink; // esi
  int m_next; // edx
  int v3; // edx

  m_prevlink = this->m_prevlink;
  if ( this->m_prevlink != nullptr )
  {
    m_next = (int)this->m_next;
    if ( m_next > 0 )
      v3 = (char *)this - (char *)m_prevlink->m_next + m_next;
    else
      v3 = ~m_next;
    *(_DWORD *)v3 = m_prevlink;
    this->m_prevlink->m_next = this->m_next;
    this->m_prevlink = nullptr;
    this->m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BF5F0  EA 0x5BF5F0
// DIA: TSLink<CGxStringBatch>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CGxStringBatch *__thiscall TSLink<CGxStringBatch>::NextLink(TSLink<CGxStringBatch> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (CGxStringBatch *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (CGxStringBatch *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF620  EA 0x5BF620
// DIA: TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >::~TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::~TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>(
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *this)
{
  TSLink<BATCHEDRENDERFONTDESC> *p_m_terminator; // ebx
  BATCHEDRENDERFONTDESC *m_next; // edx
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // esi
  int v6; // esi
  TSLink<BATCHEDRENDERFONTDESC> *m_prevlink; // edx
  int v8; // ecx
  int v9; // ecx

  p_m_terminator = &this->m_terminator;
  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v3 = *(unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    v4 = (unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    if ( v3 != 0 )
    {
      v5 = v4[1];
      if ( v5 > 0 )
        v6 = (int)v4 + v5 - *(_DWORD *)(v3 + 4);
      else
        v6 = ~v5;
      *(_DWORD *)v6 = v3;
      *(_DWORD *)(*v4 + 4) = v4[1];
      *v4 = 0;
      v4[1] = 0;
    }
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    v8 = (int)this->m_terminator.m_next;
    if ( v8 > 0 )
      v9 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v8;
    else
      v9 = ~v8;
    *(_DWORD *)v9 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BF6A0  EA 0x5BF6A0
// DIA: TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >::ChangeLinkOffset
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::ChangeLinkOffset(
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *this,
        int linkoffset)
{
  TSLink<BATCHEDRENDERFONTDESC> *p_m_terminator; // ebx
  BATCHEDRENDERFONTDESC *m_next; // edx
  int v4; // edi
  _DWORD *v5; // eax
  int v6; // esi
  int v7; // esi

  if ( linkoffset != this->m_linkoffset )
  {
    p_m_terminator = &this->m_terminator;
    while ( 1 )
    {
      m_next = this->m_terminator.m_next;
      if ( (int)m_next <= 0 )
        break;
      v4 = *(unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
      v5 = (unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
      if ( v4 != 0 )
      {
        v6 = v5[1];
        if ( v6 > 0 )
          v7 = (int)v5 + v6 - *(_DWORD *)(v4 + 4);
        else
          v7 = ~v6;
        *(_DWORD *)v7 = v4;
        *(_DWORD *)(*v5 + 4) = v5[1];
        *v5 = 0;
        v5[1] = 0;
      }
    }
    p_m_terminator->m_prevlink = p_m_terminator;
    this->m_terminator.m_next = (BATCHEDRENDERFONTDESC *)~(unsigned int)p_m_terminator;
    this->m_linkoffset = linkoffset;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BF700  EA 0x5BF700
// DIA: TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::~TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::~TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>(
        TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this)
{
  unsigned int m_count; // ecx
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v3; // esi
  TSLink<BATCHEDRENDERFONTDESC> *v4; // eax
  bool v5; // cf
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *m_data; // ebx
  unsigned int index; // [esp+4h] [ebp-8h]
  int v8; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v8 = 0;
    do
    {
      v3 = &this->m_data[v8];
      while ( (int)v3->m_terminator.m_next > 0 )
      {
        v4 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(v3, v3->m_terminator.m_next);
        TSLink<BATCHEDRENDERFONTDESC>::Unlink(v4);
      }
      TSLink<BATCHEDRENDERFONTDESC>::Unlink(&v3->m_terminator);
      v5 = ++index < this->m_count;
      ++v8;
    }
    while ( v5 );
  }
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_51, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF790  EA 0x5BF790
// DIA: TSLink<CGxFont>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CGxFont *__thiscall TSLink<CGxFont>::NextLink(TSLink<CGxFont> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (CGxFont *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (CGxFont *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF7C0  EA 0x5BF7C0
// DIA: TSLink<BATCHEDRENDERFONTDESC>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<BATCHEDRENDERFONTDESC> *__thiscall TSLink<BATCHEDRENDERFONTDESC>::NextLink(
        TSLink<BATCHEDRENDERFONTDESC> *this,
        int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<BATCHEDRENDERFONTDESC> *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<BATCHEDRENDERFONTDESC> *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF7F0  EA 0x5BF7F0
// DIA: TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >::TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *__thiscall TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>(
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 0;
  this->m_terminator.m_next = (BATCHEDRENDERFONTDESC *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF810  EA 0x5BF810
// DIA: TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *__thiscall TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>(
        TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this)
{
  this->__vftable = (TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::`vftable';
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF830  EA 0x5BF830
// DIA: TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::InitializeTerminator(
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (BATCHEDRENDERFONTDESC *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF840  EA 0x5BF840
// DIA: TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<BATCHEDRENDERFONTDESC> *__thiscall TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *this,
        const BATCHEDRENDERFONTDESC *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<BATCHEDRENDERFONTDESC> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF860  EA 0x5BF860
// DIA: TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::CheckArrayBounds
// Compiland: .\Release_Assertions_Enabled\GxuFont.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this,
        unsigned int index)
{
  unsigned int m_count; // eax
  int v4; // eax
  const char *v5; // eax
  int v6; // [esp-18h] [ebp-1Ch]
  int v7; // [esp-14h] [ebp-18h]
  unsigned int v8; // [esp-10h] [ebp-14h]
  const char *v9; // [esp-Ch] [ebp-10h]

  m_count = this->m_count;
  if ( index >= m_count )
  {
    v4 = ((int (__thiscall *)(TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))this->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo)(
           this,
           1,
           1,
           aIndex0x08xArra,
           index,
           m_count);
    v5 = (const char *)((int (__thiscall *)(TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int))this->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName)(
                         this,
                         v4);
    SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
  }
}

