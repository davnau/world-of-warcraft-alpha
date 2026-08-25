// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\GxuFontUtil.cpp
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x001BF8A0  EA 0x5BF8A0
// DIA: $E2
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E2_267()
{
  return _E1_267();
}


// -----------------------------------------------------------------------------
// RVA 0x001BF8B0  EA 0x5BF8B0
// DIA: $E1
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int _E1_267()
{
  INFINITY_267 = INFINITY;
  return 2139095040;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF8C0  EA 0x5BF8C0
// DIA: $E26
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E26_41()
{
  _E23_46();
  return _E25_32();
}


// -----------------------------------------------------------------------------
// RVA 0x001BF8D0  EA 0x5BF8D0
// DIA: $E23
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E23_46()
{
  g_freeTextLineTextures.m_linkoffset = 0;
  g_freeTextLineTextures.m_terminator.m_prevlink = &g_freeTextLineTextures.m_terminator;
  g_freeTextLineTextures.m_terminator.m_next = (TEXTLINETEXTURE *)~(unsigned int)&g_freeTextLineTextures.m_terminator;
  return ~(unsigned int)&g_freeTextLineTextures.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF900  EA 0x5BF900
// DIA: $E25
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E25_32()
{
  return atexit(_E24_26);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF910  EA 0x5BF910
// DIA: $E24
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E24_26()
{
  TEXTLINETEXTURE *m_next; // eax
  TSLink<TEXTLINETEXTURE> *m_prevlink; // esi
  int v2; // edx
  int v3; // eax

LABEL_1:
  m_next = g_freeTextLineTextures.m_terminator.m_next;
  while ( (int)g_freeTextLineTextures.m_terminator.m_next > 0 )
  {
    m_prevlink = g_freeTextLineTextures.m_terminator.m_next->m_link.m_prevlink;
    if ( g_freeTextLineTextures.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v2 = (int)g_freeTextLineTextures.m_terminator.m_next->m_link.m_next;
      if ( v2 > 0 )
        *(_DWORD *)((char *)g_freeTextLineTextures.m_terminator.m_next - (char *)m_prevlink->m_next + v2) = m_prevlink;
      else
        *(_DWORD *)~v2 = m_prevlink;
      m_next->m_link.m_prevlink->m_next = m_next->m_link.m_next;
      m_next->m_link.m_prevlink = nullptr;
      m_next->m_link.m_next = nullptr;
      goto LABEL_1;
    }
  }
  if ( g_freeTextLineTextures.m_terminator.m_prevlink != nullptr )
  {
    if ( (int)g_freeTextLineTextures.m_terminator.m_next > 0 )
      v3 = (int)g_freeTextLineTextures.m_terminator.m_next
         + (char *)&g_freeTextLineTextures.m_terminator
         - (char *)g_freeTextLineTextures.m_terminator.m_prevlink->m_next;
    else
      v3 = ~(int)g_freeTextLineTextures.m_terminator.m_next;
    *(_DWORD *)v3 = g_freeTextLineTextures.m_terminator.m_prevlink;
    g_freeTextLineTextures.m_terminator.m_prevlink->m_next = g_freeTextLineTextures.m_terminator.m_next;
    g_freeTextLineTextures.m_terminator.m_prevlink = nullptr;
    g_freeTextLineTextures.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BF9A0  EA 0x5BF9A0
// DIA: $E31
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E31_69()
{
  _E28_72();
  return _E30_66();
}


// -----------------------------------------------------------------------------
// RVA 0x001BF9B0  EA 0x5BF9B0
// DIA: $E28
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E28_72()
{
  g_freeTextLines.m_linkoffset = 0;
  g_freeTextLines.m_terminator.m_prevlink = &g_freeTextLines.m_terminator;
  g_freeTextLines.m_terminator.m_next = (IGXUTEXTLINE *)~(unsigned int)&g_freeTextLines.m_terminator;
  return ~(unsigned int)&g_freeTextLines.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BF9E0  EA 0x5BF9E0
// DIA: $E30
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E30_66()
{
  return atexit(_E29_43);
}


// -----------------------------------------------------------------------------
// RVA 0x001BF9F0  EA 0x5BF9F0
// DIA: $E29
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E29_43()
{
  IGXUTEXTLINE *m_next; // eax
  TSLink<IGXUTEXTLINE> *m_prevlink; // esi
  int v2; // edx
  int v3; // eax

LABEL_1:
  m_next = g_freeTextLines.m_terminator.m_next;
  while ( (int)g_freeTextLines.m_terminator.m_next > 0 )
  {
    m_prevlink = g_freeTextLines.m_terminator.m_next->m_link.m_prevlink;
    if ( g_freeTextLines.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v2 = (int)g_freeTextLines.m_terminator.m_next->m_link.m_next;
      if ( v2 > 0 )
        *(_DWORD *)((char *)g_freeTextLines.m_terminator.m_next - (char *)m_prevlink->m_next + v2) = m_prevlink;
      else
        *(_DWORD *)~v2 = m_prevlink;
      m_next->m_link.m_prevlink->m_next = m_next->m_link.m_next;
      m_next->m_link.m_prevlink = nullptr;
      m_next->m_link.m_next = nullptr;
      goto LABEL_1;
    }
  }
  if ( g_freeTextLines.m_terminator.m_prevlink != nullptr )
  {
    if ( (int)g_freeTextLines.m_terminator.m_next > 0 )
      v3 = (int)g_freeTextLines.m_terminator.m_next
         + (char *)&g_freeTextLines.m_terminator
         - (char *)g_freeTextLines.m_terminator.m_prevlink->m_next;
    else
      v3 = ~(int)g_freeTextLines.m_terminator.m_next;
    *(_DWORD *)v3 = g_freeTextLines.m_terminator.m_prevlink;
    g_freeTextLines.m_terminator.m_prevlink->m_next = g_freeTextLines.m_terminator.m_next;
    g_freeTextLines.m_terminator.m_prevlink = nullptr;
    g_freeTextLines.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BFA80  EA 0x5BFA80
// DIA: $E36
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E36_62()
{
  _E33_65();
  return _E35_39();
}


// -----------------------------------------------------------------------------
// RVA 0x001BFA90  EA 0x5BFA90
// DIA: $E33
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E33_65()
{
  g_freeStrings.m_linkoffset = 0;
  g_freeStrings.m_terminator.m_prevlink = &g_freeStrings.m_terminator;
  g_freeStrings.m_terminator.m_next = (CGxString *)~(unsigned int)&g_freeStrings.m_terminator;
  return ~(unsigned int)&g_freeStrings.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BFAC0  EA 0x5BFAC0
// DIA: $E35
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E35_39()
{
  return atexit(_E34_69);
}


// -----------------------------------------------------------------------------
// RVA 0x001BFAD0  EA 0x5BFAD0
// DIA: $E34
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E34_69()
{
  CGxString *m_next; // eax
  TSLink<CGxString> *m_prevlink; // esi
  int v2; // edx
  int v3; // eax

LABEL_1:
  m_next = g_freeStrings.m_terminator.m_next;
  while ( (int)g_freeStrings.m_terminator.m_next > 0 )
  {
    m_prevlink = g_freeStrings.m_terminator.m_next->m_link.m_prevlink;
    if ( g_freeStrings.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v2 = (int)g_freeStrings.m_terminator.m_next->m_link.m_next;
      if ( v2 > 0 )
        *(_DWORD *)((char *)g_freeStrings.m_terminator.m_next - (char *)m_prevlink->m_next + v2) = m_prevlink;
      else
        *(_DWORD *)~v2 = m_prevlink;
      m_next->m_link.m_prevlink->m_next = m_next->m_link.m_next;
      m_next->m_link.m_prevlink = nullptr;
      m_next->m_link.m_next = nullptr;
      goto LABEL_1;
    }
  }
  if ( g_freeStrings.m_terminator.m_prevlink != nullptr )
  {
    if ( (int)g_freeStrings.m_terminator.m_next > 0 )
      v3 = (int)g_freeStrings.m_terminator.m_next
         + (char *)&g_freeStrings.m_terminator
         - (char *)g_freeStrings.m_terminator.m_prevlink->m_next;
    else
      v3 = ~(int)g_freeStrings.m_terminator.m_next;
    *(_DWORD *)v3 = g_freeStrings.m_terminator.m_prevlink;
    g_freeStrings.m_terminator.m_prevlink->m_next = g_freeStrings.m_terminator.m_next;
    g_freeStrings.m_terminator.m_prevlink = nullptr;
    g_freeStrings.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BFB60  EA 0x5BFB60
// DIA: $E41
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E41_35()
{
  _E38_38();
  return _E40_66();
}


// -----------------------------------------------------------------------------
// RVA 0x001BFB70  EA 0x5BFB70
// DIA: $E38
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E38_38()
{
  g_strings.m_linkoffset = 0;
  g_strings.m_terminator.m_prevlink = &g_strings.m_terminator;
  g_strings.m_terminator.m_next = (CGxString *)~(unsigned int)&g_strings.m_terminator;
  return ~(unsigned int)&g_strings.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BFBA0  EA 0x5BFBA0
// DIA: $E40
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E40_66()
{
  return atexit(_E39_64);
}


// -----------------------------------------------------------------------------
// RVA 0x001BFBB0  EA 0x5BFBB0
// DIA: $E39
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E39_64()
{
  CGxString *m_next; // eax
  TSLink<CGxString> *m_prevlink; // esi
  int v2; // edx
  int v3; // eax

LABEL_1:
  m_next = g_strings.m_terminator.m_next;
  while ( (int)g_strings.m_terminator.m_next > 0 )
  {
    m_prevlink = g_strings.m_terminator.m_next->m_link.m_prevlink;
    if ( g_strings.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v2 = (int)g_strings.m_terminator.m_next->m_link.m_next;
      if ( v2 > 0 )
        *(_DWORD *)((char *)g_strings.m_terminator.m_next - (char *)m_prevlink->m_next + v2) = m_prevlink;
      else
        *(_DWORD *)~v2 = m_prevlink;
      m_next->m_link.m_prevlink->m_next = m_next->m_link.m_next;
      m_next->m_link.m_prevlink = nullptr;
      m_next->m_link.m_next = nullptr;
      goto LABEL_1;
    }
  }
  if ( g_strings.m_terminator.m_prevlink != nullptr )
  {
    if ( (int)g_strings.m_terminator.m_next > 0 )
      v3 = (int)g_strings.m_terminator.m_next
         + (char *)&g_strings.m_terminator
         - (char *)g_strings.m_terminator.m_prevlink->m_next;
    else
      v3 = ~(int)g_strings.m_terminator.m_next;
    *(_DWORD *)v3 = g_strings.m_terminator.m_prevlink;
    g_strings.m_terminator.m_prevlink->m_next = g_strings.m_terminator.m_next;
    g_strings.m_terminator.m_prevlink = nullptr;
    g_strings.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BFC40  EA 0x5BFC40
// DIA: $E44
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E44_37()
{
  return _E43_64();
}


// -----------------------------------------------------------------------------
// RVA 0x001BFC50  EA 0x5BFC50
// DIA: $E43
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void _E43_64()
{
  ONEHALFONEOVERTEXSIZE = 0.00390625 * 0.5;
}


// -----------------------------------------------------------------------------
// RVA 0x001BFC70  EA 0x5BFC70
// DIA: $E49
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E49_62()
{
  _E46_62();
  return _E48_64();
}


// -----------------------------------------------------------------------------
// RVA 0x001BFC80  EA 0x5BFC80
// DIA: $E46
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void _E46_62()
{
  int m_next; // ecx
  int v1; // edx
  _DWORD *v2; // eax
  int v3; // esi
  int v4; // esi

  s_stringViewMatrices.m_fulllist.m_terminator.m_prevlink = &s_stringViewMatrices.m_fulllist.m_terminator;
  m_next = ~(unsigned int)&s_stringViewMatrices.m_fulllist.m_terminator;
  s_stringViewMatrices.m_fulllist.m_linkoffset = -572662307;
  s_stringViewMatrices.m_fulllist.m_terminator.m_next = (STRINGVIEWMATRICES *)~(unsigned int)&s_stringViewMatrices.m_fulllist.m_terminator;
  s_stringViewMatrices.m_slotlistarray.m_alloc = 0;
  s_stringViewMatrices.m_slotlistarray.m_count = 0;
  s_stringViewMatrices.m_slotlistarray.m_data = nullptr;
  s_stringViewMatrices.m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >_vtbl *)&TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::`vftable';
  s_stringViewMatrices.m_slotlistarray.m_chunk = 0;
  s_stringViewMatrices.__vftable = (TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>_vtbl *)&TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::`vftable';
  s_stringViewMatrices.m_fullnessIndicator = 0;
  while ( m_next > 0 )
  {
    v1 = *(_DWORD *)(s_stringViewMatrices.m_fulllist.m_linkoffset + m_next);
    v2 = (_DWORD *)(m_next + s_stringViewMatrices.m_fulllist.m_linkoffset);
    if ( v1 != 0 )
    {
      v3 = v2[1];
      if ( v3 > 0 )
        v4 = (int)v2 + v3 - *(_DWORD *)(v1 + 4);
      else
        v4 = ~v3;
      *(_DWORD *)v4 = v1;
      *(_DWORD *)(*v2 + 4) = v2[1];
      *v2 = 0;
      v2[1] = 0;
      m_next = (int)s_stringViewMatrices.m_fulllist.m_terminator.m_next;
    }
  }
  s_stringViewMatrices.m_fulllist.m_terminator.m_next = (STRINGVIEWMATRICES *)~(unsigned int)&s_stringViewMatrices.m_fulllist.m_terminator;
  s_stringViewMatrices.m_fulllist.m_linkoffset = 12;
  s_stringViewMatrices.m_fulllist.m_terminator.m_prevlink = &s_stringViewMatrices.m_fulllist.m_terminator;
  s_stringViewMatrices.m_slotmask = -1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BFD40  EA 0x5BFD40
// DIA: $E48
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E48_64()
{
  return atexit(_E47_38);
}


// -----------------------------------------------------------------------------
// RVA 0x001BFD50  EA 0x5BFD50
// DIA: $E47
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E47_38()
{
  unsigned int v0; // edi
  int m_next; // eax
  TSLink<STRINGVIEWMATRICES> *v2; // eax
  unsigned int m_count; // ecx
  STRINGVIEWMATRICES *v4; // ebx
  int v5; // eax
  const char *v6; // eax
  int m_linkoffset; // esi
  int v8; // eax
  TSLink<STRINGVIEWMATRICES> *v9; // esi
  unsigned int v10; // esi
  int v11; // edi
  char *v12; // esi
  int v13; // [esp-1Ch] [ebp-28h]
  int v14; // [esp-18h] [ebp-24h]
  unsigned int v15; // [esp-14h] [ebp-20h]
  const char *v16; // [esp-10h] [ebp-1Ch]
  int v17; // [esp+8h] [ebp-4h]

  v0 = 0;
  s_stringViewMatrices.__vftable = (TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>_vtbl *)&TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::`vftable';
  s_stringViewMatrices.m_fullnessIndicator = 0;
  while ( 1 )
  {
    m_next = (int)s_stringViewMatrices.m_fulllist.m_terminator.m_next;
    if ( (int)s_stringViewMatrices.m_fulllist.m_terminator.m_next <= 0 )
      break;
    v2 = TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::Link(
           &s_stringViewMatrices.m_fulllist,
           s_stringViewMatrices.m_fulllist.m_terminator.m_next);
    TSLink<STRINGVIEWMATRICES>::Unlink(v2);
  }
  m_count = s_stringViewMatrices.m_slotlistarray.m_count;
  if ( s_stringViewMatrices.m_slotlistarray.m_count != 0 )
  {
    v17 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
          &s_stringViewMatrices.m_slotlistarray,
          v0);
        v4 = s_stringViewMatrices.m_slotlistarray.m_data[v17].m_terminator.m_next;
        if ( (int)v4 <= 0 )
          break;
        if ( v0 >= s_stringViewMatrices.m_slotlistarray.m_count )
        {
          v5 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int, int, char *, unsigned int, unsigned int))s_stringViewMatrices.m_slotlistarray.TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo)(
                 &s_stringViewMatrices.m_slotlistarray,
                 1,
                 1,
                 aIndex0x08xArra,
                 v0,
                 s_stringViewMatrices.m_slotlistarray.m_count);
          v6 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int))s_stringViewMatrices.m_slotlistarray.TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName)(
                               &s_stringViewMatrices.m_slotlistarray,
                               v5);
          SErrDisplayErrorFmt(0x85100080, v6, v13, v14, v15, v16);
        }
        m_linkoffset = s_stringViewMatrices.m_slotlistarray.m_data[v17].m_linkoffset;
        v8 = *(unsigned int *)((char *)&v4->m_hashval + m_linkoffset);
        v9 = (TSLink<STRINGVIEWMATRICES> *)((char *)v4 + m_linkoffset);
        if ( v8 != 0 )
        {
          TSLink<STRINGVIEWMATRICES>::NextLink(v9, -1)->m_prevlink = v9->m_prevlink;
          v9->m_prevlink->m_next = v9->m_next;
          v9->m_prevlink = nullptr;
          v9->m_next = nullptr;
        }
      }
      m_count = s_stringViewMatrices.m_slotlistarray.m_count;
      ++v0;
      ++v17;
    }
    while ( v0 < s_stringViewMatrices.m_slotlistarray.m_count );
    m_next = (int)s_stringViewMatrices.m_fulllist.m_terminator.m_next;
  }
  v10 = 0;
  s_stringViewMatrices.m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v11 = 0;
    do
    {
      TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::~TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>(&s_stringViewMatrices.m_slotlistarray.m_data[v11]);
      ++v10;
      ++v11;
    }
    while ( v10 < s_stringViewMatrices.m_slotlistarray.m_count );
    m_next = (int)s_stringViewMatrices.m_fulllist.m_terminator.m_next;
  }
  if ( s_stringViewMatrices.m_slotlistarray.m_data != nullptr )
  {
    SMemFree(s_stringViewMatrices.m_slotlistarray.m_data, aAvTsexplicitli_50, -2, 0);
LABEL_19:
    m_next = (int)s_stringViewMatrices.m_fulllist.m_terminator.m_next;
  }
  while ( m_next > 0 )
  {
    v12 = (char *)(s_stringViewMatrices.m_fulllist.m_linkoffset + m_next);
    if ( *(_DWORD *)(s_stringViewMatrices.m_fulllist.m_linkoffset + m_next) != 0 )
    {
      TSLink<STRINGVIEWMATRICES>::NextLink(
        (TSLink<STRINGVIEWMATRICES> *)(s_stringViewMatrices.m_fulllist.m_linkoffset + m_next),
        -1)->m_prevlink = *(TSLink<STRINGVIEWMATRICES> **)v12;
      *(_DWORD *)(*(_DWORD *)v12 + 4) = *((_DWORD *)v12 + 1);
      *(_DWORD *)v12 = 0;
      *((_DWORD *)v12 + 1) = 0;
      goto LABEL_19;
    }
  }
  if ( s_stringViewMatrices.m_fulllist.m_terminator.m_prevlink != nullptr )
  {
    *(_DWORD *)~m_next = s_stringViewMatrices.m_fulllist.m_terminator.m_prevlink;
    s_stringViewMatrices.m_fulllist.m_terminator.m_prevlink->m_next = s_stringViewMatrices.m_fulllist.m_terminator.m_next;
    s_stringViewMatrices.m_fulllist.m_terminator.m_prevlink = nullptr;
    s_stringViewMatrices.m_fulllist.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BFF20  EA 0x5BFF20
// DIA: $E54
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E54_65()
{
  _E51_60();
  return _E53_32();
}


// -----------------------------------------------------------------------------
// RVA 0x001BFF30  EA 0x5BFF30
// DIA: $E51
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E51_60()
{
  s_freeStringMatrices.m_linkoffset = 24;
  s_freeStringMatrices.m_terminator.m_prevlink = &s_freeStringMatrices.m_terminator;
  s_freeStringMatrices.m_terminator.m_next = (STRINGVIEWMATRICES *)~(unsigned int)&s_freeStringMatrices.m_terminator;
  return ~(unsigned int)&s_freeStringMatrices.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BFF60  EA 0x5BFF60
// DIA: $E53
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E53_32()
{
  return atexit(_E52_63);
}


// -----------------------------------------------------------------------------
// RVA 0x001BFF70  EA 0x5BFF70
// DIA: $E52
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E52_63()
{
  int v0; // edx
  _DWORD *v1; // eax
  int v2; // esi
  int v3; // ecx

  while ( (int)s_freeStringMatrices.m_terminator.m_next > 0 )
  {
    v0 = *(unsigned int *)((char *)&s_freeStringMatrices.m_terminator.m_next->m_hashval
                         + s_freeStringMatrices.m_linkoffset);
    v1 = (unsigned int *)((char *)&s_freeStringMatrices.m_terminator.m_next->m_hashval
                        + s_freeStringMatrices.m_linkoffset);
    if ( v0 != 0 )
    {
      v2 = v1[1];
      if ( v2 > 0 )
        *(_DWORD *)((char *)v1 + v2 - *(_DWORD *)(v0 + 4)) = v0;
      else
        *(_DWORD *)~v2 = v0;
      *(_DWORD *)(*v1 + 4) = v1[1];
      *v1 = 0;
      v1[1] = 0;
    }
  }
  if ( s_freeStringMatrices.m_terminator.m_prevlink != nullptr )
  {
    if ( (int)s_freeStringMatrices.m_terminator.m_next > 0 )
      v3 = (int)s_freeStringMatrices.m_terminator.m_next
         + (char *)&s_freeStringMatrices.m_terminator
         - (char *)s_freeStringMatrices.m_terminator.m_prevlink->m_next;
    else
      v3 = ~(int)s_freeStringMatrices.m_terminator.m_next;
    *(_DWORD *)v3 = s_freeStringMatrices.m_terminator.m_prevlink;
    s_freeStringMatrices.m_terminator.m_prevlink->m_next = s_freeStringMatrices.m_terminator.m_next;
    s_freeStringMatrices.m_terminator.m_prevlink = nullptr;
    s_freeStringMatrices.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C0010  EA 0x5C0010
// DIA: GxuDetermineQuotedCode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
QUOTEDCODE __fastcall GxuDetermineQuotedCode(
        char *advance,
        unsigned int *a2,
        NTempest::CImVector *color,
        __int16 flags,
        unsigned int *wide,
        unsigned int remainingBytes)
{
  int v8; // ebx
  unsigned int v10; // eax
  QUOTEDCODE result; // eax
  int *v12; // esi
  char v13; // al
  int *v14; // esi
  unsigned int v15; // eax
  unsigned int v16; // eax
  _BYTE *v17; // esi
  QUOTEDCODE v18; // ebx
  bool v19; // cf
  QUOTEDCODE v20; // ebx
  _BYTE *v21; // esi
  char v22; // al
  unsigned __int8 v23; // al
  int ignoreNewlines; // [esp+Ch] [ebp-10h]
  unsigned int i; // [esp+10h] [ebp-Ch]
  unsigned int ia; // [esp+10h] [ebp-Ch]
  char *error; // [esp+14h] [ebp-8h] BYREF
  unsigned __int8 comps[4]; // [esp+18h] [ebp-4h]
  unsigned int firstCharAdvance; // [esp+28h] [ebp+Ch]

  ignoreNewlines = flags & 0x200;
  v8 = flags & 0x100;
  *(_DWORD *)comps = flags & 0x400;
  i = flags & 0x800;
  if ( advance == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 100, aText, 0, 1u);
  if ( *advance == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 101, aText_0, 0, 1u);
  error = advance;
  v10 = sgetu8((int *)advance, a2);
  firstCharAdvance = *a2;
  *wide = v10;
  if ( v10 == -1 )
    return CODE_INVALIDCODE;
  if ( v10 == 10 )
  {
    *a2 = 1;
    return CODE_NEWLINE;
  }
  if ( v10 != 124 || i != 0 )
    return CODE_INVALIDCODE;
  v12 = (int *)&advance[*a2];
  *wide = sgetu8(v12, a2);
  switch ( *(_BYTE *)v12 )
  {
    case 'C':
    case 'c':
      if ( v8 != 0 )
      {
        *a2 = firstCharAdvance;
        *wide = 124;
        result = CODE_INVALIDCODE;
      }
      else
      {
        v18 = CODE_COLORON;
        v19 = remainingBytes < 0xA;
        *a2 = 10;
        if ( v19 )
          goto LABEL_40;
        v21 = (char *)v12 + 1;
        BYTE2(remainingBytes) = 0;
        ia = 0;
        while ( 1 )
        {
          v22 = v21[1];
          LOBYTE(remainingBytes) = *v21;
          error = nullptr;
          BYTE1(remainingBytes) = v22;
          v21 += 2;
          v23 = _strtol((const char *)&remainingBytes, &error, 16);
          if ( error != nullptr && *error != 0 )
            break;
          comps[ia++] = v23;
          if ( ia >= 4 )
            goto LABEL_47;
        }
        *a2 = firstCharAdvance;
        v18 = CODE_INVALIDCODE;
        *wide = 124;
LABEL_47:
        if ( color == nullptr )
          goto LABEL_50;
        *color = (NTempest::CImVector)(comps[3] | ((comps[2] | ((comps[1] | 0xFFFFFF00) << 8)) << 8));
        result = v18;
      }
      break;
    case 'H':
      if ( *(_DWORD *)comps != 0 )
      {
        *a2 = firstCharAdvance;
        *wide = 124;
        result = CODE_INVALIDCODE;
      }
      else
      {
        v13 = *((_BYTE *)v12 + *a2);
        v14 = (int *)((char *)v12 + *a2);
        if ( v13 == 0 )
          goto LABEL_40;
        while ( 1 )
        {
          v15 = sgetu8(v14, a2);
          v14 = (int *)((char *)v14 + *a2);
          if ( v15 == 124 )
            break;
          if ( *(_BYTE *)v14 == 0 )
          {
            *a2 = firstCharAdvance;
            *wide = 124;
            return CODE_INVALIDCODE;
          }
        }
        v16 = sgetu8(v14, a2);
        v17 = (char *)v14 + *a2;
        if ( v16 != 104 || v17 - error == 4 || *v17 == 124 && v17[1] == 104 )
        {
LABEL_40:
          *a2 = firstCharAdvance;
          *wide = 124;
          result = CODE_INVALIDCODE;
        }
        else
        {
          *a2 = v17 - error;
          result = CODE_HYPERLINKSTART;
        }
      }
      break;
    case 'N':
    case 'n':
      if ( ignoreNewlines != 0 )
      {
        *a2 = firstCharAdvance;
        *wide = 124;
        result = CODE_INVALIDCODE;
      }
      else
      {
        v18 = CODE_NEWLINE;
        v19 = remainingBytes < 2;
        *a2 = 2;
        if ( !v19 )
          goto LABEL_50;
        *a2 = firstCharAdvance;
        *wide = 124;
        result = CODE_NEWLINE;
      }
      break;
    case 'R':
    case 'r':
      if ( v8 != 0 )
      {
        *a2 = firstCharAdvance;
        *wide = 124;
        result = CODE_INVALIDCODE;
      }
      else
      {
        v19 = remainingBytes < 2;
        *a2 = 2;
        if ( v19 )
        {
          *a2 = firstCharAdvance;
          *wide = 124;
        }
        result = CODE_COLORRESTORE;
      }
      break;
    case 'h':
      if ( *(_DWORD *)comps != 0 )
      {
        *a2 = firstCharAdvance;
        *wide = 124;
        result = CODE_INVALIDCODE;
      }
      else
      {
        *a2 = 2;
        result = CODE_HYPERLINKSTOP;
      }
      break;
    case '|':
      v20 = CODE_PIPE;
      if ( remainingBytes < *a2 )
      {
        *a2 = firstCharAdvance;
        v20 = CODE_INVALIDCODE;
        *wide = 124;
      }
      *a2 = 2;
      result = v20;
      break;
    default:
      *wide = 124;
      *a2 = firstCharAdvance;
      v18 = CODE_INVALIDCODE;
LABEL_50:
      result = v18;
      break;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C0410  EA 0x5C0410
// DIA: CGxString::InitializeTextLine
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
// local variable allocation has failed, the output may be wrong!
void __userpurge CGxString::InitializeTextLine(
        CGxString *this@<ecx>,
        double a2@<st0>,
        char *currentText,
        unsigned int numBytes,
        NTempest::CImVector *workingColor,
        float position,
        unsigned int *texturePagesUsedFlag,
        HYPERLINKPARSEINFO *info)
{
  CGxString *v8; // ebx
  CGxFont *m_currentFace; // eax
  IGXUTEXTLINE *v10; // eax
  CGxFont *v11; // ecx
  IGXUTEXTLINE *v12; // esi
  unsigned int NumCurrentTextures; // eax
  TEXTLINETEXTURE **m_data; // edx
  unsigned int i; // ecx
  float v16; // edx
  float v17; // eax
  double X; // st7
  unsigned int m_flags; // eax
  unsigned int v20; // esi
  char *v21; // edi
  QUOTEDCODE v22; // eax
  const CHARCODEDESC *v23; // eax
  unsigned int m_count; // ecx
  TEXTLINETEXTURE **v25; // edi
  unsigned int v26; // esi
  unsigned int v27; // edx
  HYPERLINKPARSEINFO *v28; // esi
  float v29; // eax
  float y; // ecx
  bool v31; // zf
  double v32; // st7
  unsigned int v33; // ecx
  unsigned int v34; // eax
  unsigned int v35; // edi
  double v36; // st7
  const char *v37; // ecx
  unsigned int v38; // eax
  HYPERLINKPARSEINFO *v39; // eax
  GXUFONTHYPERLINKINFO *p_currentParseInfo; // esi
  const CHARCODEDESC *v41; // eax
  const CHARCODEDESC *v42; // edi
  unsigned int textureNumber; // edi
  TSGrowableArray<TEXTLINETEXTURE *> *p_m_texturePages; // esi
  int v45; // eax
  const char *v46; // eax
  TEXTLINETEXTURE *v47; // esi
  TSGrowableArray<VERT> *p_m_vert; // ebx
  unsigned int v49; // edi
  unsigned int m_alloc; // eax
  TSGrowableArray<NTempest::CImVector> *p_m_colors; // esi
  unsigned int v52; // edi
  unsigned int m_chunk; // eax
  unsigned int v54; // eax
  unsigned int j; // ecx
  NTempest::CImVector *v56; // eax
  int v57; // ecx
  unsigned int v58; // eax
  unsigned int v59; // edi
  int v60; // eax
  const char *v61; // eax
  unsigned int v62; // ecx
  int v63; // eax
  const char *v64; // eax
  unsigned int v65; // ecx
  int v66; // eax
  const char *v67; // eax
  NTempest::CImVector *v68; // edx
  unsigned int v69; // eax
  unsigned int v70; // edi
  int v71; // eax
  const char *v72; // eax
  NTempest::CImVector *v73; // ecx
  TSGrowableArray<NTempest::CImVector *> *p_m_colorGradients; // esi
  unsigned int v75; // eax
  unsigned int v76; // edi
  NTempest::CImVector **v77; // edx
  unsigned int v78; // eax
  unsigned int v79; // eax
  unsigned int k; // eax
  NTempest::CImVector **v81; // ecx
  CGxString *v82; // edx
  NTempest::CImVector m_shadowColor; // ebx
  TSGrowableArray_<NTempest::CImVector,1199076678,59> *p_m_shadowColors; // esi
  unsigned int v85; // edi
  unsigned int v86; // eax
  unsigned int v87; // eax
  unsigned int m; // ecx
  NTempest::CImVector *v89; // eax
  unsigned int v90; // edx
  unsigned int v91; // edi
  int v92; // eax
  const char *v93; // eax
  unsigned int v94; // eax
  int v95; // eax
  const char *v96; // eax
  unsigned int v97; // eax
  int v98; // eax
  const char *v99; // eax
  unsigned int v100; // eax
  int v101; // eax
  const char *v102; // eax
  TSFixedArray<NTempest::CImVector *> *p_m_colorGradientShadows; // edi
  unsigned int v104; // eax
  unsigned int v105; // esi
  NTempest::CImVector **v106; // edx
  unsigned int v107; // ecx
  unsigned int n; // eax
  NTempest::CImVector **v109; // ecx
  double v110; // st7
  char v111; // al
  double v112; // st7
  unsigned int v113; // edx
  char v114; // cl
  unsigned int v115; // eax
  unsigned int v116; // esi
  unsigned int v117; // ecx
  int ii; // eax
  unsigned int jj; // edx
  VERT *v120; // edi
  unsigned int v121; // eax
  unsigned int v122; // esi
  int v123; // eax
  const char *v124; // eax
  int v125; // edi
  float *p_x; // eax
  double v127; // st7
  int v128; // ecx
  double v129; // st7
  double v130; // st7
  double v131; // st7
  double v132; // st7
  double v133; // st7
  double v134; // st7
  int v135; // eax
  const char *v136; // eax
  VERT *v137; // eax
  float *v138; // ecx
  double v139; // st7
  TEXTLINETEXTURE *v140; // edx
  float *v141; // eax
  double v142; // st7
  double v143; // st7
  double v144; // st7
  int v145; // [esp-10h] [ebp-120h]
  int v146; // [esp-10h] [ebp-120h]
  int v147; // [esp-10h] [ebp-120h]
  int v148; // [esp-10h] [ebp-120h]
  int v149; // [esp-10h] [ebp-120h]
  int v150; // [esp-10h] [ebp-120h]
  int v151; // [esp-10h] [ebp-120h]
  int v152; // [esp-10h] [ebp-120h]
  int v153; // [esp-10h] [ebp-120h]
  int v154; // [esp-10h] [ebp-120h]
  int v155; // [esp-10h] [ebp-120h]
  int v156; // [esp-Ch] [ebp-11Ch]
  int v157; // [esp-Ch] [ebp-11Ch]
  int v158; // [esp-Ch] [ebp-11Ch]
  int v159; // [esp-Ch] [ebp-11Ch]
  int v160; // [esp-Ch] [ebp-11Ch]
  int v161; // [esp-Ch] [ebp-11Ch]
  int v162; // [esp-Ch] [ebp-11Ch]
  int v163; // [esp-Ch] [ebp-11Ch]
  int v164; // [esp-Ch] [ebp-11Ch]
  int v165; // [esp-Ch] [ebp-11Ch]
  int v166; // [esp-Ch] [ebp-11Ch]
  unsigned int v167; // [esp-8h] [ebp-118h]
  unsigned int v168; // [esp-8h] [ebp-118h]
  unsigned int v169; // [esp-8h] [ebp-118h]
  unsigned int v170; // [esp-8h] [ebp-118h]
  unsigned int v171; // [esp-8h] [ebp-118h]
  unsigned int v172; // [esp-8h] [ebp-118h]
  unsigned int v173; // [esp-8h] [ebp-118h]
  unsigned int v174; // [esp-8h] [ebp-118h]
  unsigned int v175; // [esp-8h] [ebp-118h]
  unsigned int v176; // [esp-8h] [ebp-118h]
  unsigned int v177; // [esp-8h] [ebp-118h]
  const char *v178; // [esp-4h] [ebp-114h]
  const char *v179; // [esp-4h] [ebp-114h]
  const char *v180; // [esp-4h] [ebp-114h]
  const char *v181; // [esp-4h] [ebp-114h]
  const char *v182; // [esp-4h] [ebp-114h]
  const char *v183; // [esp-4h] [ebp-114h]
  const char *v184; // [esp-4h] [ebp-114h]
  const char *v185; // [esp-4h] [ebp-114h]
  const char *v186; // [esp-4h] [ebp-114h]
  const char *v187; // [esp-4h] [ebp-114h]
  const char *v188; // [esp-4h] [ebp-114h]
  float X_4; // [esp+4h] [ebp-10Ch]
  float X_4a; // [esp+4h] [ebp-10Ch]
  int fixedCharWidths; // [esp+8Ch] [ebp-84h]
  _BYTE vert[32]; // [esp+90h] [ebp-80h] OVERLAPPED BYREF
  NTempest::C3Vector offset3; // [esp+B0h] [ebp-60h]
  NTempest::CImVector *ptrArray[4]; // [esp+BCh] [ebp-54h] BYREF
  float screenPixelHeight; // [esp+CCh] [ebp-44h]
  unsigned int m_pixelSize; // [esp+D0h] [ebp-40h]
  TSGrowableArray<VERT> *verts; // [esp+D4h] [ebp-3Ch]
  unsigned int v198; // [esp+D8h] [ebp-38h] BYREF
  IGXUTEXTLINE *newLine; // [esp+DCh] [ebp-34h]
  QUOTEDCODE quotedCode; // [esp+E0h] [ebp-30h]
  float step; // [esp+E4h] [ebp-2Ch]
  int oldIndex; // [esp+E8h] [ebp-28h]
  NTempest::C3Vector currPos; // [esp+ECh] [ebp-24h]
  float glyphToScreenPixels; // [esp+F8h] [ebp-18h]
  unsigned int wide; // [esp+FCh] [ebp-14h] BYREF
  float glyphPixelHeight; // [esp+100h] [ebp-10h]
  NTempest::CImVector color; // [esp+104h] [ebp-Ch] BYREF
  TEXTLINETEXTURE *textLinePage; // [esp+108h] [ebp-8h]
  CGxString *v209; // [esp+10Ch] [ebp-4h]
  GXUFONTHYPERLINKINFO v210; // 0:^8.24

  v8 = this;
  m_currentFace = this->m_currentFace;
  v209 = this;
  if ( m_currentFace == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 516, aMCurrentface, 0, 1u);
  if ( currentText == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 517, aCurrenttext, 0, 1u);
  if ( texturePagesUsedFlag == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 518, aTexturepagesus, 0, 1u);
  fixedCharWidths = v8->m_flags & 0x10;
  v10 = IGXUTEXTBLOCK::NewLine(&v8->m_textBlock);
  v11 = v8->m_currentFace;
  v12 = v10;
  newLine = v10;
  NumCurrentTextures = CGxFont::GetNumCurrentTextures(v11);
  IGXUTEXTLINE::Reserve(v12, NumCurrentTextures);
  if ( (v8->m_flags & 8) != 0 )
  {
    m_data = v12->m_texturePages.m_data;
    for ( i = 0; i < v12->m_texturePages.m_count; ++m_data )
    {
      (*m_data)->m_colors.m_count = 0;
      ++i;
    }
  }
  v16 = *(float *)(LODWORD(position) + 4);
  v17 = *(float *)(LODWORD(position) + 8);
  currPos.x = *(float *)LODWORD(position);
  X_4 = v8->m_currentFontHeight;
  currPos.y = v16;
  currPos.z = v17;
  *(float *)&textLinePage = 0.0;
  ScreenToPixelHeight(SLODWORD(v17), X_4);
  screenPixelHeight = a2;
  m_pixelSize = v8->m_currentFace->m_pixelSize;
  verts = nullptr;
  X_4a = v8->m_currentFontHeight;
  X = screenPixelHeight / (double)m_pixelSize;
  glyphToScreenPixels = X;
  ScreenToPixelHeight(m_pixelSize, X_4a);
  glyphPixelHeight = X;
  m_flags = v8->m_currentFace->m_flags;
  if ( (m_flags & 1) != 0 )
  {
    X = glyphPixelHeight + 2.0;
    glyphPixelHeight = X;
  }
  if ( (m_flags & 8) != 0 )
  {
    X = glyphPixelHeight + 2.0;
    glyphPixelHeight = X;
  }
  v20 = numBytes;
  memset(vert, 0, sizeof(vert));
  v21 = currentText;
  while ( *v21 != 0 )
  {
    if ( v20 == 0 )
      break;
    v22 = GxuDetermineQuotedCode(v21, (unsigned int *)&position, nullptr, v8->m_flags, &wide, v20);
    v21 += LODWORD(position);
    v20 -= LODWORD(position);
    switch ( v22 )
    {
      case CODE_COLORON:
      case CODE_COLORRESTORE:
      case CODE_NEWLINE:
      case CODE_HYPERLINKSTART:
      case CODE_HYPERLINKSTOP:
        break;
      default:
        v23 = CGxFont::NewCodeDesc(v8->m_currentFace, wide);
        if ( v23 != nullptr && v23->bitmapData != nullptr )
          ++*(_DWORD *)&vert[4 * v23->textureNumber];
        break;
    }
  }
  m_count = newLine->m_texturePages.m_count;
  v25 = newLine->m_texturePages.m_data;
  position = 0.0;
  if ( m_count != 0 )
  {
    do
    {
      v26 = 4 * *(_DWORD *)&vert[4 * LODWORD(position)];
      TSGrowableArray<VERT>::Reserve(&(*v25)->m_vert, v26, 0);
      if ( (v8->m_flags & 8) == 0 )
        TSGrowableArray<NTempest::CImVector>::Reserve(&(*v25)->m_colors, v26, 0);
      v27 = newLine->m_texturePages.m_count;
      ++v25;
      ++LODWORD(position);
    }
    while ( LODWORD(position) < v27 );
  }
  if ( adjustmentsInitialized == 0 )
  {
    adjustmentsInitialized = 1;
    if ( GxCaps()->m_pixelCenterOnEdge != 0 )
    {
      leftPixelAdjustment = 0.0;
      rightPixelAdjustment = 0.0;
      topPixelAdjustment = 0.0;
      bottomPixelAdjustment = 0.0;
    }
    else
    {
      leftPixelAdjustment = -0.5;
      rightPixelAdjustment = -0.5;
      topPixelAdjustment = -0.5;
      bottomPixelAdjustment = -0.5;
    }
  }
  v28 = info;
  if ( info->hyperlinkParseMode == HYPERLINKDISPLAY )
    info->currentParseInfo.extent.r = currPos.x;
  v29 = v8->m_shadowOffset.x;
  y = v8->m_shadowOffset.y;
  offset3.x = v29;
  v31 = (v8->m_flags & 1) == 0;
  offset3.y = y;
  if ( !v31 )
  {
    ScreenToPixelWidth(SLODWORD(v29), offset3.x);
    v32 = _floor(X);
    ScreenToPixelHeight(SLODWORD(offset3.y), offset3.y);
    _floor(v32);
  }
  v31 = *currentText == 0;
  step = 0.0;
  position = 0.0;
  if ( !v31 )
  {
    do
    {
      if ( numBytes == 0 )
        break;
      v33 = v8->m_flags;
      color = 0;
      quotedCode = GxuDetermineQuotedCode(currentText, &v198, &color, v33, &wide, numBytes);
      v34 = v198;
      currentText += v198;
      v35 = wide;
      numBytes -= v198;
      if ( *(float *)&textLinePage != 0.0 )
      {
        if ( fixedCharWidths != 0 )
          v36 = CGxFont::ComputeStepFixedWidth(v8->m_currentFace, (unsigned int)textLinePage, wide);
        else
          v36 = CGxFont::ComputeStep(v8->m_currentFace, (unsigned int)textLinePage, wide);
        v34 = v198;
        step = v36;
      }
      position = step * glyphToScreenPixels;
      switch ( quotedCode )
      {
        case CODE_COLORON:
          if ( (v8->m_flags & 8) == 0 )
          {
            color.a = v8->m_fontColor.a;
            *workingColor = color;
          }
          break;
        case CODE_COLORRESTORE:
          *workingColor = v8->m_fontColor;
          break;
        case CODE_NEWLINE:
          break;
        case CODE_HYPERLINKSTART:
          if ( v28->hyperlinkParseMode != HYPERLINKDISPLAY )
          {
            v37 = &currentText[-v34 + 2];
            v38 = v34 - 4;
            v28->currentParseInfo.extent.l = position + currPos.x;
            v28->hyperlinkParseMode = HYPERLINKDISPLAY;
            v28->lastLinkStartPtr = v37;
            v28->lastLinkLength = v38;
            v28->currentParseInfo.link = v37;
            v28->currentParseInfo.linkLength = v38;
          }
          break;
        case CODE_HYPERLINKSTOP:
          if ( v28->hyperlinkParseMode == HYPERLINKDISPLAY )
          {
            v39 = info;
            p_currentParseInfo = &info->currentParseInfo;
            info->currentParseInfo.extent.r = position + currPos.x;
            v210 = *p_currentParseInfo;
            v39->hyperlinkParseMode = HYPERLINKNONE;
            CGxString::AddHyperlinkParseInfo(v8, v210);
            goto LABEL_150;
          }
          break;
        default:
          v41 = CGxFont::NewCodeDesc(v8->m_currentFace, v35);
          v42 = v41;
          color = (NTempest::CImVector)v41;
          if ( v41 != nullptr && v41->bitmapData != nullptr )
          {
            if ( v41->dataValid == 0 )
              SErrDisplayError(0x85100000, aDBuildBuildwow_239, 721, aCodeDatavalid, 0, 1u);
            textureNumber = v42->textureNumber;
            if ( textureNumber >= 8 )
              SErrDisplayError(0x85100000, aDBuildBuildwow_239, 724, aTexturenumberS, 0, 1u);
            if ( v8->m_currentFace->m_textureCache[textureNumber].m_texture == nullptr )
              SErrDisplayError(0x85100000, aDBuildBuildwow_239, 725, aMCurrentfaceMT, 0, 1u);
            *texturePagesUsedFlag |= 1 << textureNumber;
            p_m_texturePages = &newLine->m_texturePages;
            if ( textureNumber >= newLine->m_texturePages.m_count )
            {
              v45 = ((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int, int, char *, unsigned int, unsigned int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo)(
                      &newLine->m_texturePages,
                      1,
                      1,
                      aIndex0x08xArra,
                      textureNumber,
                      newLine->m_texturePages.m_count);
              v46 = (const char *)((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemFileName)(
                                    p_m_texturePages,
                                    v45);
              SErrDisplayErrorFmt(0x85100080, v46, v145, v156, v167, v178);
            }
            v47 = p_m_texturePages->m_data[textureNumber];
            v31 = (v209->m_flags & 8) == 0;
            p_m_vert = &v47->m_vert;
            textLinePage = v47;
            verts = &v47->m_vert;
            if ( v31 )
            {
              v49 = v47->m_colors.m_count;
              m_alloc = v47->m_colors.m_alloc;
              p_m_colors = &v47->m_colors;
              v52 = v49 + 4;
              if ( v52 > m_alloc )
              {
                m_chunk = p_m_colors->m_chunk;
                if ( m_chunk == 0 )
                  m_chunk = TSGrowableArray<NTempest::CImVector>::CalcChunkSize(p_m_colors, v52);
                v54 = TSGrowableArray<NTempest::CImVector>::RoundToChunk(p_m_colors, v52, m_chunk);
                TSFixedArray<NTempest::CImVector>::ReallocData(p_m_colors, v54);
              }
              for ( j = 0; j < 4; ++j )
              {
                v56 = &p_m_colors->m_data[j + p_m_colors->m_count];
                if ( v56 != nullptr )
                  *v56 = *workingColor;
              }
              v57 = p_m_colors->m_count + 4;
              p_m_colors->m_count = v57;
              v58 = v209->m_flags;
              v59 = v57 - 4;
              oldIndex = v57 - 4;
              if ( (v58 & 0x1000) != 0 )
              {
                if ( v59 >= p_m_colors->m_count )
                {
                  v60 = ((int (__thiscall *)(TSGrowableArray<NTempest::CImVector> *, int, int, char *, unsigned int, unsigned int))p_m_colors->TSBaseArray<NTempest::CImVector>::MemLineNo)(
                          p_m_colors,
                          1,
                          1,
                          aIndex0x08xArra,
                          v59,
                          p_m_colors->m_count);
                  v61 = (const char *)((int (__thiscall *)(TSGrowableArray<NTempest::CImVector> *, int))p_m_colors->TSBaseArray<NTempest::CImVector>::MemFileName)(
                                        p_m_colors,
                                        v60);
                  SErrDisplayErrorFmt(0x85100080, v61, v146, v157, v168, v179);
                }
                v62 = p_m_colors->m_count;
                ptrArray[0] = &p_m_colors->m_data[v59];
                if ( v59 + 3 >= v62 )
                {
                  v63 = ((int (__thiscall *)(TSGrowableArray<NTempest::CImVector> *, int, int, char *, unsigned int, unsigned int))p_m_colors->TSBaseArray<NTempest::CImVector>::MemLineNo)(
                          p_m_colors,
                          1,
                          1,
                          aIndex0x08xArra,
                          v59 + 3,
                          v62);
                  v64 = (const char *)((int (__thiscall *)(TSGrowableArray<NTempest::CImVector> *, int))p_m_colors->TSBaseArray<NTempest::CImVector>::MemFileName)(
                                        p_m_colors,
                                        v63);
                  SErrDisplayErrorFmt(0x85100080, v64, v147, v158, v169, v180);
                }
                v65 = p_m_colors->m_count;
                ptrArray[1] = &p_m_colors->m_data[v59 + 3];
                if ( v59 + 1 >= v65 )
                {
                  v66 = ((int (__thiscall *)(TSGrowableArray<NTempest::CImVector> *, int, int, char *, unsigned int, unsigned int))p_m_colors->TSBaseArray<NTempest::CImVector>::MemLineNo)(
                          p_m_colors,
                          1,
                          1,
                          aIndex0x08xArra,
                          v59 + 1,
                          v65);
                  v67 = (const char *)((int (__thiscall *)(TSGrowableArray<NTempest::CImVector> *, int))p_m_colors->TSBaseArray<NTempest::CImVector>::MemFileName)(
                                        p_m_colors,
                                        v66);
                  SErrDisplayErrorFmt(0x85100080, v67, v148, v159, v170, v181);
                }
                v68 = &p_m_colors->m_data[v59 + 1];
                v69 = p_m_colors->m_count;
                v70 = v59 + 2;
                ptrArray[2] = v68;
                if ( v70 >= v69 )
                {
                  v71 = ((int (__thiscall *)(TSGrowableArray<NTempest::CImVector> *, int, int, char *, unsigned int, unsigned int))p_m_colors->TSBaseArray<NTempest::CImVector>::MemLineNo)(
                          p_m_colors,
                          1,
                          1,
                          aIndex0x08xArra,
                          v70,
                          v69);
                  v72 = (const char *)((int (__thiscall *)(TSGrowableArray<NTempest::CImVector> *, int))p_m_colors->TSBaseArray<NTempest::CImVector>::MemFileName)(
                                        p_m_colors,
                                        v71);
                  SErrDisplayErrorFmt(0x85100080, v72, v149, v160, v171, v182);
                }
                v73 = &p_m_colors->m_data[v70];
                p_m_colorGradients = &v209->m_colorGradients;
                v75 = v209->m_colorGradients.m_alloc;
                v76 = v209->m_colorGradients.m_count + 4;
                v77 = ptrArray;
                ptrArray[3] = v73;
                quotedCode = (QUOTEDCODE)ptrArray;
                if ( v76 > v75 )
                {
                  v78 = v209->m_colorGradients.m_chunk;
                  if ( v78 == 0 )
                    v78 = TSGrowableArray<NTempest::CImVector *>::CalcChunkSize(&v209->m_colorGradients, v76);
                  v79 = TSGrowableArray<NTempest::CImVector *>::RoundToChunk(p_m_colorGradients, v76, v78);
                  TSFixedArray<NTempest::CImVector *>::ReallocData(p_m_colorGradients, v79);
                  v77 = (NTempest::CImVector **)quotedCode;
                }
                for ( k = 0; k < 4; ++k )
                {
                  v81 = &(&p_m_colorGradients->m_data[k])[p_m_colorGradients->m_count];
                  if ( v81 != nullptr )
                    *v81 = *v77;
                  ++v77;
                }
                v82 = v209;
                p_m_colorGradients->m_count += 4;
                if ( (v82->m_flags & 1) != 0 )
                {
                  m_shadowColor = v82->m_shadowColor;
                  p_m_shadowColors = &textLinePage->m_shadowColors;
                  v85 = textLinePage->m_shadowColors.m_count + 4;
                  if ( v85 > textLinePage->m_shadowColors.m_alloc )
                  {
                    v86 = textLinePage->m_shadowColors.m_chunk;
                    if ( v86 == 0 )
                      v86 = TSGrowableArray<NTempest::CImVector>::CalcChunkSize(&textLinePage->m_shadowColors, v85);
                    v87 = TSGrowableArray<NTempest::CImVector>::RoundToChunk(p_m_shadowColors, v85, v86);
                    TSFixedArray<NTempest::CImVector>::ReallocData(p_m_shadowColors, v87);
                  }
                  for ( m = 0; m < 4; ++m )
                  {
                    v89 = &p_m_shadowColors->m_data[m + p_m_shadowColors->m_count];
                    if ( v89 != nullptr )
                      *v89 = m_shadowColor;
                  }
                  v90 = p_m_shadowColors->m_count + 4;
                  v91 = p_m_shadowColors->m_count;
                  p_m_shadowColors->m_count = v90;
                  if ( v91 >= v90 )
                  {
                    v92 = ((int (__thiscall *)(TSGrowableArray_<NTempest::CImVector,1199076678,59> *, int, int, char *, unsigned int, unsigned int))p_m_shadowColors->TSBaseArray<NTempest::CImVector>::MemLineNo)(
                            p_m_shadowColors,
                            1,
                            1,
                            aIndex0x08xArra,
                            v91,
                            v90);
                    v93 = (const char *)((int (__thiscall *)(TSGrowableArray_<NTempest::CImVector,1199076678,59> *, int))p_m_shadowColors->TSGrowableArray_<NTempest::CImVector,1199076678,59>::MemLineNo)(
                                          p_m_shadowColors,
                                          v92);
                    SErrDisplayErrorFmt(0x85100080, v93, v150, v161, v172, v183);
                  }
                  v94 = p_m_shadowColors->m_count;
                  ptrArray[0] = &p_m_shadowColors->m_data[v91];
                  if ( v91 + 3 >= v94 )
                  {
                    v95 = ((int (__thiscall *)(TSGrowableArray_<NTempest::CImVector,1199076678,59> *, int, int, char *, unsigned int, unsigned int))p_m_shadowColors->TSBaseArray<NTempest::CImVector>::MemLineNo)(
                            p_m_shadowColors,
                            1,
                            1,
                            aIndex0x08xArra,
                            v91 + 3,
                            v94);
                    v96 = (const char *)((int (__thiscall *)(TSGrowableArray_<NTempest::CImVector,1199076678,59> *, int))p_m_shadowColors->TSGrowableArray_<NTempest::CImVector,1199076678,59>::MemLineNo)(
                                          p_m_shadowColors,
                                          v95);
                    SErrDisplayErrorFmt(0x85100080, v96, v151, v162, v173, v184);
                  }
                  v97 = p_m_shadowColors->m_count;
                  ptrArray[1] = &p_m_shadowColors->m_data[v91 + 3];
                  if ( v91 + 1 >= v97 )
                  {
                    v98 = ((int (__thiscall *)(TSGrowableArray_<NTempest::CImVector,1199076678,59> *, int, int, char *, unsigned int, unsigned int))p_m_shadowColors->TSBaseArray<NTempest::CImVector>::MemLineNo)(
                            p_m_shadowColors,
                            1,
                            1,
                            aIndex0x08xArra,
                            v91 + 1,
                            v97);
                    v99 = (const char *)((int (__thiscall *)(TSGrowableArray_<NTempest::CImVector,1199076678,59> *, int))p_m_shadowColors->TSGrowableArray_<NTempest::CImVector,1199076678,59>::MemLineNo)(
                                          p_m_shadowColors,
                                          v98);
                    SErrDisplayErrorFmt(0x85100080, v99, v152, v163, v174, v185);
                  }
                  v100 = p_m_shadowColors->m_count;
                  ptrArray[2] = &p_m_shadowColors->m_data[v91 + 1];
                  if ( v91 + 2 >= v100 )
                  {
                    v101 = ((int (__thiscall *)(TSGrowableArray_<NTempest::CImVector,1199076678,59> *, int, int, char *, unsigned int, unsigned int))p_m_shadowColors->TSBaseArray<NTempest::CImVector>::MemLineNo)(
                             p_m_shadowColors,
                             1,
                             1,
                             aIndex0x08xArra,
                             v91 + 2,
                             v100);
                    v102 = (const char *)((int (__thiscall *)(TSGrowableArray_<NTempest::CImVector,1199076678,59> *, int))p_m_shadowColors->TSGrowableArray_<NTempest::CImVector,1199076678,59>::MemLineNo)(
                                           p_m_shadowColors,
                                           v101);
                    SErrDisplayErrorFmt(0x85100080, v102, v153, v164, v175, v186);
                  }
                  ptrArray[3] = &p_m_shadowColors->m_data[v91 + 2];
                  if ( v91 != oldIndex )
                    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 759, aOldshadowindex, 0, 1u);
                  p_m_colorGradientShadows = &v209->m_colorGradientShadows;
                  v104 = v209->m_colorGradientShadows.m_alloc;
                  v105 = v209->m_colorGradientShadows.m_count + 4;
                  v106 = ptrArray;
                  oldIndex = (int)ptrArray;
                  if ( v105 > v104 )
                  {
                    v107 = v209->m_colorGradientShadows.m_chunk;
                    if ( v107 == 0 )
                      v107 = TSGrowableArray<NTempest::CImVector *>::CalcChunkSize(&v209->m_colorGradientShadows, v105);
                    if ( v105 % v107 != 0 )
                      v105 += v107 - v105 % v107;
                    TSFixedArray<NTempest::CImVector *>::ReallocData(p_m_colorGradientShadows, v105);
                    v106 = (NTempest::CImVector **)oldIndex;
                  }
                  for ( n = 0; n < 4; ++n )
                  {
                    v109 = &(&p_m_colorGradientShadows->m_data[n])[p_m_colorGradientShadows->m_count];
                    if ( v109 != nullptr )
                      *v109 = *v106;
                    ++v106;
                  }
                  p_m_vert = verts;
                  p_m_colorGradientShadows->m_count += 4;
                }
              }
            }
            if ( SLOBYTE(v209->m_flags) >= 0 )
            {
              v110 = SignOf(position);
              position = _floor(v110 * 0.5 + position);
            }
            *(float *)&vert[20] = currPos.z;
            *(_DWORD *)&vert[24] = 0;
            currPos.x = position + currPos.x;
            *(float *)&vert[12] = currPos.x;
            v111 = v209->m_flags;
            *(_DWORD *)&vert[28] = 0;
            *(float *)&vert[16] = currPos.y;
            if ( v111 >= 0 )
              v112 = _ceil(*(float *)(*(_DWORD *)(*(_DWORD *)&color + 60) + 56));
            else
              v112 = glyphToScreenPixels * *(float *)(*(_DWORD *)(*(_DWORD *)&color + 60) + 56);
            v113 = *(_DWORD *)(*(_DWORD *)(*(_DWORD *)&color + 60) + 48);
            *(float *)&vert[12] = v112 + currPos.x;
            v114 = v209->m_flags;
            *(_QWORD *)&offset3.y = v113;
            *(float *)&textLinePage = (double)v113 * glyphToScreenPixels;
            if ( v114 >= 0 )
              *(float *)&textLinePage = _floor(*(float *)&textLinePage);
            v115 = v209->m_currentFace->m_flags;
            if ( (v115 & 1) != 0 )
              *(float *)&vert[16] = *(float *)&vert[16] - 1.0;
            if ( (v115 & 8) != 0 )
              *(float *)&vert[16] = *(float *)&vert[16] - 1.0;
            v116 = p_m_vert->m_count + 4;
            if ( v116 > p_m_vert->m_alloc )
            {
              v117 = p_m_vert->m_chunk;
              if ( v117 == 0 )
              {
                v117 = p_m_vert->m_count + 4;
                if ( v116 >= 0xC )
                {
                  p_m_vert->m_chunk = 12;
                  v117 = 12;
                }
                else
                {
                  for ( ii = (p_m_vert->m_count + 4) & (p_m_vert->m_count + 3); ii != 0; ii &= ii - 1 )
                    v117 = ii;
                  if ( v117 == 0 )
                    v117 = 1;
                }
              }
              if ( v116 % v117 != 0 )
                v116 += v117 - v116 % v117;
              TSFixedArray<VERT>::ReallocData(p_m_vert, v116);
            }
            for ( jj = 0; jj < 4; ++jj )
            {
              v120 = &p_m_vert->m_data[jj + p_m_vert->m_count];
              if ( v120 != nullptr )
                qmemcpy(v120, &vert[12], sizeof(VERT));
            }
            v121 = p_m_vert->m_count + 4;
            v122 = p_m_vert->m_count;
            p_m_vert->m_count = v121;
            if ( v122 >= v121 )
            {
              v123 = ((int (__thiscall *)(TSGrowableArray<VERT> *, int, int, char *, unsigned int, unsigned int))p_m_vert->TSBaseArray<VERT>::MemLineNo)(
                       p_m_vert,
                       1,
                       1,
                       aIndex0x08xArra,
                       v122,
                       v121);
              v124 = (const char *)((int (__thiscall *)(TSGrowableArray<VERT> *, int))p_m_vert->TSBaseArray<VERT>::MemFileName)(
                                     p_m_vert,
                                     v123);
              SErrDisplayErrorFmt(0x85100080, v124, v154, v165, v176, v187);
            }
            v125 = v122;
            p_x = &p_m_vert->m_data[v122].vc.x;
            if ( SLOBYTE(v209->m_flags) >= 0 )
            {
              v130 = leftPixelAdjustment + p_x[15];
              p_x[15] = v130;
              *p_x = v130;
              v131 = *(float *)&textLinePage + p_x[10] + rightPixelAdjustment;
              p_x[10] = v131;
              p_x[5] = v131;
              v132 = topPixelAdjustment + p_x[16] + glyphPixelHeight;
              p_x[16] = v132;
              p_x[11] = v132;
              v133 = bottomPixelAdjustment + p_x[6];
              p_x[6] = v133;
              p_x[1] = v133;
            }
            else
            {
              v127 = *(float *)&textLinePage + p_x[10];
              v128 = *((_DWORD *)p_x + 6);
              *p_x = p_x[15];
              *((_DWORD *)p_x + 1) = v128;
              p_x[10] = v127;
              p_x[5] = v127;
              v129 = screenPixelHeight + p_x[16];
              p_x[16] = v129;
              p_x[11] = v129;
            }
            verts = *(TSGrowableArray<VERT> **)(*(_DWORD *)(*(_DWORD *)&color + 60) + 64);
            if ( verts != nullptr )
            {
              v134 = (double)(int)verts * glyphToScreenPixels;
              p_x[1] = v134 + p_x[1];
              p_x[6] = v134 + p_x[6];
              p_x[11] = v134 + p_x[11];
              p_x[16] = v134 + p_x[16];
            }
            if ( v122 >= p_m_vert->m_count )
            {
              v135 = ((int (__thiscall *)(TSGrowableArray<VERT> *, int, int, char *, unsigned int, unsigned int))p_m_vert->TSBaseArray<VERT>::MemLineNo)(
                       p_m_vert,
                       1,
                       1,
                       aIndex0x08xArra,
                       v122,
                       p_m_vert->m_count);
              v136 = (const char *)((int (__thiscall *)(TSGrowableArray<VERT> *, int))p_m_vert->TSBaseArray<VERT>::MemFileName)(
                                     p_m_vert,
                                     v135);
              SErrDisplayErrorFmt(0x85100080, v136, v155, v166, v177, v188);
            }
            v137 = p_m_vert->m_data;
            v138 = *(float **)(*(_DWORD *)&color + 60);
            v139 = v138[20];
            v140 = (TEXTLINETEXTURE *)wide;
            v8 = v209;
            v137[v125 + 1].tc.y = v138[20];
            v141 = &v137[v125].vc.x;
            v138 += 18;
            v141[4] = v139;
            v142 = *v138;
            textLinePage = v140;
            v141[19] = v142;
            v141[14] = v142;
            v143 = v138[1];
            v141[18] = v138[1];
            v141[3] = v143;
            v144 = v138[3];
            v141[13] = v138[3];
            v141[8] = v144;
LABEL_150:
            v28 = info;
          }
          break;
      }
    }
    while ( *currentText != 0 );
  }
  v28->currentParseInfo.extent.r = position + currPos.x;
}


// -----------------------------------------------------------------------------
// RVA 0x001C1010  EA 0x5C1010
// DIA: CGxString::Duplicate
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CGxString *__thiscall CGxString::Duplicate(CGxString *this)
{
  CGxString *newString; // [esp+0h] [ebp-4h] BYREF

  newString = this;
  GxuFontCreateString(
    (CGxFont *)&this->m_position,
    (const char *)this->m_currentFace,
    this->m_requestedFontHeight,
    &this->m_position,
    this->m_blockWidth,
    this->m_blockHeight,
    this->m_spacing,
    &newString,
    this->m_vertJust,
    this->m_horzJust,
    this->m_flags,
    &this->m_fontColor,
    0.0);
  return newString;
}


// -----------------------------------------------------------------------------
// RVA 0x001C1050  EA 0x5C1050
// DIA: CalcWrapPoint
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall CalcWrapPoint(
        char *currentText,
        const char *a2,
        CGxFont *fontHeight,
        float blockWidth,
        unsigned int *numBytes,
        float *pExtent,
        const char **pNextText,
        unsigned int flags)
{
  unsigned int WrappingIndex; // edi
  unsigned int v11; // ebx
  const char *MaxCharsWithinWidth; // eax
  const char *v13; // ecx
  const char *nextText; // [esp+Ch] [ebp-Ch] BYREF
  char *text; // [esp+10h] [ebp-8h]
  float extent; // [esp+14h] [ebp-4h] BYREF

  text = currentText;
  if ( a2 != nullptr )
    WrappingIndex = SStrLen(a2);
  else
    WrappingIndex = 0;
  v11 = flags;
  extent = 0.0;
  nextText = nullptr;
  MaxCharsWithinWidth = (const char *)InternalGetMaxCharsWithinWidth(
                                        fontHeight,
                                        text,
                                        *(float *)&fontHeight,
                                        blockWidth,
                                        WrappingIndex,
                                        &extent,
                                        flags,
                                        &flags,
                                        nullptr,
                                        nullptr);
  if ( flags == WrappingIndex )
  {
    v13 = nullptr;
  }
  else
  {
    WrappingIndex = FindWrappingIndex(MaxCharsWithinWidth, (unsigned int)a2, v11, &nextText);
    InternalGetTextExtent(fontHeight, text, WrappingIndex, *(float *)&fontHeight, &extent, v11);
    v13 = nextText;
  }
  if ( numBytes != nullptr )
    *numBytes = WrappingIndex;
  *pExtent = extent;
  *pNextText = v13;
}


// -----------------------------------------------------------------------------
// RVA 0x001C10F0  EA 0x5C10F0
// DIA: FindWrappingIndex
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall FindWrappingIndex(char *lineBytes, unsigned int a2, unsigned int flags, const char **nextText)
{
  char v7; // cl
  int v8; // edi
  QUOTEDCODE v9; // eax
  unsigned int v10; // eax
  const char *v11; // eax
  const char *v12; // ecx
  int *v13; // esi
  wint_t v14; // ax
  int *v16; // esi
  wint_t v17; // ax
  char v18; // al
  unsigned int lastChar; // [esp+Ch] [ebp-10h]
  const char *startingText; // [esp+10h] [ebp-Ch]
  unsigned int wideChar; // [esp+14h] [ebp-8h] BYREF
  unsigned int advance; // [esp+18h] [ebp-4h] BYREF

  if ( nextText == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 411, aNexttext, 0, 1u);
  if ( lineBytes != nullptr && (v7 = *lineBytes, *lineBytes != 0) && a2 != 0 )
  {
    lastChar = 0;
    wideChar = 0;
    v8 = 0;
    startingText = lineBytes;
    if ( v7 != 0 )
    {
      while ( a2 != 0 )
      {
        v9 = GxuDetermineQuotedCode(lineBytes, &advance, nullptr, flags, &wideChar, a2);
        if ( v9 == CODE_NEWLINE )
        {
          v12 = &lineBytes[advance];
          v13 = (int *)&startingText[v8];
          *nextText = v12;
          if ( &startingText[v8] < v12 )
          {
            do
            {
              v14 = sgetu8(v13, &advance);
              v13 = (int *)((char *)v13 + advance);
              if ( _iswspace(v14) == 0 )
                v8 = (char *)v13 - startingText;
            }
            while ( v13 < (int *)*nextText );
          }
          return v8;
        }
        if ( v9 == CODE_INVALIDCODE )
        {
          if ( CanWrapBetween(6u, lastChar, flags) != 0 )
            v8 = lineBytes - startingText;
          lastChar = wideChar;
        }
        lineBytes += advance;
        a2 -= advance;
        if ( *lineBytes == 0 )
          break;
      }
    }
    v10 = SStrLen(lineBytes);
    if ( GxuDetermineQuotedCode(lineBytes, &advance, nullptr, flags, &wideChar, v10) == CODE_INVALIDCODE
      && CanWrapBetween(6u, lastChar, flags) != 0 )
    {
      v11 = startingText;
      v8 = lineBytes - startingText;
    }
    else
    {
      v11 = startingText;
    }
    if ( v8 == 0 )
      v8 = lineBytes - v11;
    v16 = (int *)&v11[v8];
    if ( v11[v8] != 0 )
    {
      do
      {
        v17 = sgetu8(v16, &advance);
        if ( _iswspace(v17) == 0 )
          break;
        v18 = *((_BYTE *)v16 + advance);
        v16 = (int *)((char *)v16 + advance);
      }
      while ( v18 != 0 );
    }
    *nextText = (const char *)v16;
    return v8;
  }
  else
  {
    *nextText = lineBytes;
    return 0;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C1280  EA 0x5C1280
// DIA: CanWrapBetween
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall CanWrapBetween(unsigned int wideChar, unsigned int a2, char flags)
{
  bool v7; // zf
  int v8; // esi
  int v9; // esi
  int v10; // esi
  bool v11; // zf

  if ( wideChar == 0 )
    return 0;
  if ( wideChar == 45 )
    return 1;
  if ( a2 == -1 )
    return 1;
  if ( _iswspace(wideChar) != 0 )
    return 0;
  if ( _iswspace(a2) != 0 )
    return 1;
  if ( wideChar > 0x3010 )
  {
    if ( wideChar > 0xFF04 )
    {
      switch ( wideChar )
      {
        case 0xFF08u:
        case 0xFF3Bu:
        case 0xFF5Bu:
        case 0xFFE1u:
        case 0xFFE5u:
        case 0xFFE6u:
          return 0;
        default:
          goto LABEL_20;
      }
    }
    if ( wideChar == 65284 )
      return 0;
    if ( wideChar > 0xFE59 )
    {
      v10 = wideChar - 65115;
      if ( v10 == 0 )
        return 0;
      v7 = v10 == 2;
    }
    else
    {
      if ( wideChar == 65113 )
        return 0;
      v9 = wideChar - 12308;
      if ( v9 == 0 )
        return 0;
      v7 = v9 == 9;
    }
  }
  else
  {
    if ( wideChar == 12304 )
      return 0;
    if ( wideChar > 0x201C )
    {
      if ( wideChar > 0x300A )
      {
        v8 = wideChar - 12300;
        if ( v8 == 0 )
          return 0;
        v7 = v8 == 2;
      }
      else
      {
        if ( wideChar == 12298 || wideChar == 8245 )
          return 0;
        v7 = wideChar == 12296;
      }
    }
    else
    {
      if ( wideChar == 8220 )
        return 0;
      if ( wideChar > 0x5C )
      {
        if ( wideChar == 123 )
          return 0;
        v7 = wideChar == 8216;
      }
      else
      {
        if ( wideChar >= 0x5B || wideChar == 36 )
          return 0;
        v7 = wideChar == 40;
      }
    }
  }
  if ( v7 )
    return 0;
LABEL_20:
  if ( a2 <= 0x300F )
  {
    if ( a2 != 12303 )
    {
      if ( a2 > 0x2014 )
      {
        if ( a2 > 0x2103 )
        {
          switch ( a2 )
          {
            case 0x3001u:
            case 0x3002u:
            case 0x3009u:
            case 0x300Bu:
            case 0x300Du:
              return 0;
            default:
              goto LABEL_55;
          }
        }
        if ( a2 != 8451 )
        {
          switch ( a2 )
          {
            case 0x2019u:
            case 0x201Du:
            case 0x2022u:
            case 0x2026u:
            case 0x2027u:
            case 0x2032u:
            case 0x2033u:
              return 0;
            default:
              goto LABEL_55;
          }
        }
      }
      else if ( a2 < 0x2013 )
      {
        switch ( a2 )
        {
          case 0x21u:
          case 0x25u:
          case 0x29u:
          case 0x2Cu:
          case 0x2Eu:
          case 0x3Au:
          case 0x3Bu:
          case 0x3Fu:
          case 0x5Du:
          case 0x7Du:
          case 0xB0u:
          case 0xB7u:
            return 0;
          default:
            goto LABEL_55;
        }
      }
    }
    return 0;
  }
  if ( a2 > 0xFF05 )
  {
    switch ( a2 )
    {
      case 0xFF09u:
      case 0xFF0Cu:
      case 0xFF0Eu:
      case 0xFF1Au:
      case 0xFF1Bu:
      case 0xFF1Fu:
      case 0xFF3Du:
      case 0xFF5Du:
      case 0xFF70u:
      case 0xFF9Eu:
      case 0xFF9Fu:
      case 0xFFE0u:
        return 0;
      default:
        goto LABEL_55;
    }
  }
  if ( a2 == 65285 )
    return 0;
  if ( a2 > 0xFE52 )
  {
    switch ( a2 )
    {
      case 0xFE54u:
      case 0xFE55u:
      case 0xFE56u:
      case 0xFE57u:
      case 0xFE5Au:
      case 0xFE5Cu:
      case 0xFE5Eu:
      case 0xFF01u:
        return 0;
      default:
        goto LABEL_55;
    }
  }
  if ( a2 >= 0xFE50 )
    return 0;
  if ( a2 > 0x301E )
  {
    if ( a2 == 12540 )
      return 0;
    v11 = a2 == 65072;
  }
  else
  {
    if ( a2 == 12318 || a2 == 12305 )
      return 0;
    v11 = a2 == 12309;
  }
  if ( v11 )
    return 0;
LABEL_55:
  if ( (a2 < 0x1100 || a2 > 0x11FF)
    && (a2 < 0x3000 || a2 > 0xD7AF)
    && (a2 < 0xF900 || a2 > 0xFAFF)
    && (a2 < 0xFF00 || a2 > 0xFF9F)
    && (a2 < 0xFFA0 || a2 > 0xFFDC) )
  {
    return flags & 0x40;
  }
  else
  {
    return 1;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C18C0  EA 0x5C18C0
// DIA: GLYPHBITMAPDATA::GLYPHBITMAPDATA
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
GLYPHBITMAPDATA *__thiscall GLYPHBITMAPDATA::GLYPHBITMAPDATA(GLYPHBITMAPDATA *this)
{
  this->m_linktoslot.m_prevlink = nullptr;
  this->m_linktoslot.m_next = nullptr;
  this->m_linktofull.m_prevlink = nullptr;
  this->m_linktofull.m_next = nullptr;
  this->m_code = 0;
  this->m_data = nullptr;
  this->m_dataSize = 0;
  this->m_dirty = 1;
  this->m_textureCoords.t = 0.0;
  this->m_textureCoords.l = 0.0;
  this->m_textureCoords.b = 0.0;
  this->m_textureCoords.r = 0.0;
  this->m_textureValid = 0;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001C18F0  EA 0x5C18F0
// DIA: GLYPHBITMAPDATA::~GLYPHBITMAPDATA
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall GLYPHBITMAPDATA::~GLYPHBITMAPDATA(GLYPHBITMAPDATA *this)
{
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // esi
  TSLink<GLYPHBITMAPDATA> *p_m_linktofull; // eax
  int m_next; // edx
  int v5; // edx
  TSLink<GLYPHBITMAPDATA> *v6; // esi
  TSLink<GLYPHBITMAPDATA> *p_m_linktoslot; // eax
  int v8; // edx
  int v9; // edx

  GLYPHBITMAPDATA::Clear(this);
  m_prevlink = this->m_linktofull.m_prevlink;
  p_m_linktofull = &this->m_linktofull;
  if ( m_prevlink != nullptr )
  {
    m_next = (int)this->m_linktofull.m_next;
    if ( m_next > 0 )
      v5 = (char *)p_m_linktofull - (char *)m_prevlink->m_next + m_next;
    else
      v5 = ~m_next;
    *(_DWORD *)v5 = m_prevlink;
    p_m_linktofull->m_prevlink->m_next = this->m_linktofull.m_next;
    p_m_linktofull->m_prevlink = nullptr;
    this->m_linktofull.m_next = nullptr;
  }
  v6 = this->m_linktoslot.m_prevlink;
  p_m_linktoslot = &this->m_linktoslot;
  if ( v6 != nullptr )
  {
    v8 = (int)this->m_linktoslot.m_next;
    if ( v8 > 0 )
      v9 = (char *)p_m_linktoslot - (char *)v6->m_next + v8;
    else
      v9 = ~v8;
    *(_DWORD *)v9 = v6;
    this->m_linktoslot.m_prevlink->m_next = this->m_linktoslot.m_next;
    p_m_linktoslot->m_prevlink = nullptr;
    this->m_linktoslot.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C1970  EA 0x5C1970
// DIA: CHARCODEDESC::GenerateTextureCoords
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CHARCODEDESC::GenerateTextureCoords(CHARCODEDESC *this, int rowNumber, unsigned int glyphSide)
{
  unsigned int v4; // edi
  unsigned int m_glyphCellWidth; // esi
  double glyphStartPixel; // st7
  double v7; // st7
  double v8; // st6
  NTempest::CRect *p_m_textureCoords; // eax
  float v10; // [esp+6Ch] [ebp-10h]
  float v11; // [esp+70h] [ebp-Ch]
  __int64 v12; // [esp+74h] [ebp-8h]
  __int64 v13; // [esp+74h] [ebp-8h]
  int top; // [esp+84h] [ebp+8h]
  float pr; // [esp+88h] [ebp+Ch]

  if ( this->bitmapData == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 948, aBitmapdata, 0, 1u);
  v4 = glyphSide;
  if ( glyphSide == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 949, aGlyphside, 0, 1u);
  if ( initialized != 0 )
  {
    initialized = 1;
    v4 = glyphSide;
    pixelCenterOnEdge = GxCaps()->m_pixelCenterOnEdge;
  }
  m_glyphCellWidth = this->bitmapData->m_glyphCellWidth;
  if ( m_glyphCellWidth == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 960, aWidth, 0, 1u);
  top = v4 * rowNumber;
  glyphStartPixel = (double)this->glyphStartPixel;
  v12 = m_glyphCellWidth;
  pr = (double)m_glyphCellWidth + glyphStartPixel;
  v7 = glyphStartPixel * 0.00390625;
  v8 = 0.00390625 * pr;
  if ( pixelCenterOnEdge == 0 )
  {
    v7 = v7 + ONEHALFONEOVERTEXSIZE;
    v8 = v8 - ONEHALFONEOVERTEXSIZE;
  }
  v10 = (double)top * 0.00390625;
  p_m_textureCoords = &this->bitmapData->m_textureCoords;
  v11 = v7;
  p_m_textureCoords->t = v10;
  p_m_textureCoords->l = v11;
  LODWORD(v12) = v4 + top;
  *(float *)&v13 = (double)v12 * 0.00390625;
  *((float *)&v13 + 1) = v8;
  *(_QWORD *)&p_m_textureCoords->b = v13;
  this->bitmapData->m_textureValid = 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001C1AD0  EA 0x5C1AD0
// DIA: CHARCODEDESC::GapToNextTexture
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __thiscall CHARCODEDESC::GapToNextTexture(CHARCODEDESC *this)
{
  CHARCODEDESC *m_next; // edi

  if ( this->glyphStartPixel > this->glyphEndPixel )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 983, aValidblockendp, 0, 1u);
  m_next = this->textureRowLink.m_next;
  if ( (int)m_next <= 0 )
    return 255 - this->glyphEndPixel;
  if ( m_next->glyphStartPixel > m_next->glyphEndPixel )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 986, aNextValidblock, 0, 1u);
  return m_next->glyphStartPixel - this->glyphEndPixel - 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001C1B40  EA 0x5C1B40
// DIA: CHARCODEDESC::GapToPreviousTexture
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __thiscall CHARCODEDESC::GapToPreviousTexture(CHARCODEDESC *this)
{
  CHARCODEDESC *m_next; // edi

  if ( this->glyphStartPixel > this->glyphEndPixel )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 995, aValidblockendp, 0, 1u);
  m_next = this->textureRowLink.m_prevlink->m_prevlink->m_next;
  if ( (int)m_next <= 0 )
    return this->glyphStartPixel;
  if ( m_next->glyphStartPixel > m_next->glyphEndPixel )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 998, aPrevValidblock, 0, 1u);
  return this->glyphStartPixel - m_next->glyphEndPixel - 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001C1BB0  EA 0x5C1BB0
// DIA: TEXTURECACHE::PasteGlyphOutlinedMonochrome
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::PasteGlyphOutlinedMonochrome(
        TEXTURECACHE *this,
        int glyphData,
        unsigned int *dst,
        int thick)
{
  char *v4; // edx
  _DWORD *v5; // esi
  unsigned int v6; // eax
  unsigned int v7; // edi
  unsigned int j; // eax
  unsigned int v9; // ebx
  int v10; // ecx
  unsigned int v11; // esi
  unsigned int v12; // eax
  unsigned int v13; // edx
  int v14; // ecx
  int v15; // eax
  int v16; // edi
  bool v17; // zf
  int v18; // esi
  int v19; // esi
  int v20; // esi
  int v21; // esi
  int v22; // esi
  char *v23; // edx
  unsigned int v24; // esi
  unsigned int *v25; // eax
  int v26; // ecx
  _DWORD v27[257]; // [esp+0h] [ebp-8418h]
  _DWORD v28[8192]; // [esp+404h] [ebp-8014h] BYREF
  int v29; // [esp+8404h] [ebp-14h]
  unsigned int v30; // [esp+8408h] [ebp-10h]
  TEXTURECACHE *v31; // [esp+840Ch] [ebp-Ch]
  unsigned int i; // [esp+8410h] [ebp-8h]
  unsigned int v33; // [esp+8414h] [ebp-4h]
  unsigned int v34; // [esp+8428h] [ebp+10h]
  unsigned int k; // [esp+8428h] [ebp+10h]

  v31 = this;
  if ( glyphData == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1031, aGlyphdata, 0, 1u);
  if ( dst == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1032, aDst, 0, 1u);
  v4 = *(char **)(glyphData + 28);
  memset(v28, 0, sizeof(v28));
  v5 = &v28[256 * *(_DWORD *)(glyphData + 68) + 257];
  if ( thick != 0 )
    ++v5;
  v6 = *(_DWORD *)(glyphData + 44);
  if ( v6 != 0 )
  {
    v7 = *(_DWORD *)(glyphData + 40);
    v33 = *(_DWORD *)(glyphData + 60);
    for ( i = v6; i != 0; --i )
    {
      for ( j = 0; j < v7; ++j )
        v5[j] = ((v4[j >> 3] >> (7 - (j & 7))) & 1) != 0;
      v5 += 256;
      v4 += v33;
    }
  }
  i = v31->m_theFace->m_cellHeight;
  if ( i == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1072, aCellheight, 0, 1u);
  v9 = *(_DWORD *)(glyphData + 48);
  v33 = 0;
  v30 = (thick != 0) + 1;
  if ( (thick != 0) != -1 )
  {
    v10 = *(_DWORD *)(glyphData + 68);
    v11 = i;
    v29 = v10;
    while ( 1 )
    {
      v12 = v10;
      if ( v33 == 1 && v10 != 0 )
        v12 = v10 - 1;
      v34 = v12;
      if ( v12 < v11 )
        break;
LABEL_84:
      if ( ++v33 >= v30 )
        goto LABEL_85;
    }
LABEL_23:
    v13 = v34 << 8;
    v14 = 0;
    if ( v9 == 0 )
      goto LABEL_82;
    while ( 1 )
    {
      v15 = 0;
      v16 = 0;
      if ( v33 == 1 )
      {
        if ( v28[v13 + v14] != 0 )
          goto LABEL_80;
        v15 = 4;
        v16 = 2;
      }
      else if ( v33 == 0 )
      {
        v15 = 1;
        v16 = 4;
      }
      if ( v34 != 0 )
      {
        if ( v34 == i - 1 )
        {
          if ( v14 != 0 )
          {
            v19 = v13 + v14;
            if ( v14 == v9 - 1 )
            {
              if ( (v15 & v27[v19 + 256]) != 0 || (v15 & v27[v13 + v14]) != 0 )
                goto LABEL_78;
              v17 = (v15 & v27[v13 + 1 + v14]) == 0;
            }
            else
            {
              if ( (v15 & v27[v19]) != 0
                || (v15 & v27[v13 + 1 + v14]) != 0
                || (v15 & v27[v13 + 2 + v14]) != 0
                || (v15 & v27[v13 + 256 + v14]) != 0 )
              {
                goto LABEL_78;
              }
              v17 = (v15 & v28[v13 + 1 + v14]) == 0;
            }
          }
          else
          {
            if ( (v15 & v27[256 * v34 + 1]) != 0 || (v15 & v27[256 * v34 + 2]) != 0 )
              goto LABEL_78;
            v17 = (v15 & v28[256 * v34 + 1]) == 0;
          }
        }
        else if ( v14 != 0 )
        {
          v20 = v13 + v14;
          if ( v14 == v9 - 1 )
          {
            if ( (v15 & v27[v20 + 1]) != 0 )
              goto LABEL_78;
            if ( (v15 & v27[v13 + v14]) != 0 )
              goto LABEL_78;
            v21 = v13 + v14;
            if ( (v15 & v27[v21 + 256]) != 0 || (v15 & v28[v21 + 255]) != 0 )
              goto LABEL_78;
            v17 = (v15 & v28[v21 + 256]) == 0;
          }
          else
          {
            if ( (v15 & v27[v20]) != 0
              || (v15 & v27[v13 + 1 + v14]) != 0
              || (v15 & v27[v13 + 2 + v14]) != 0
              || (v22 = v13 + v14, (v15 & v27[v22 + 256]) != 0)
              || (v15 & v28[v22 + 1]) != 0
              || (v15 & v28[v22 + 255]) != 0
              || (v15 & v28[v22 + 256]) != 0 )
            {
LABEL_78:
              if ( v28[v14 + v13] != 1 )
                v28[v14 + v13] = v16;
              goto LABEL_80;
            }
            v17 = (v15 & v28[v22 + 257]) == 0;
          }
        }
        else
        {
          if ( (v15 & v27[256 * v34 + 1]) != 0
            || (v15 & v27[256 * v34 + 2]) != 0
            || (v15 & v28[256 * v34 + 1]) != 0
            || (v15 & v28[256 * v34 + 257]) != 0 )
          {
            goto LABEL_78;
          }
          v17 = (v15 & v28[256 * v34 + 256]) == 0;
        }
      }
      else if ( v14 != 0 )
      {
        v18 = v27[v14 + 256];
        if ( v14 == v9 - 1 )
        {
          if ( (v18 & v15) != 0 || (v15 & v28[v14 + 255]) != 0 )
            goto LABEL_78;
          v17 = (v15 & v28[v14 + 256]) == 0;
        }
        else
        {
          if ( (v18 & v15) != 0
            || (v15 & v28[v14 + 1]) != 0
            || (v15 & v28[v14 + 255]) != 0
            || (v15 & v28[v14 + 256]) != 0 )
          {
            goto LABEL_78;
          }
          v17 = (v15 & v28[v14 + 257]) == 0;
        }
      }
      else
      {
        if ( (v15 & v28[1]) != 0 || (v15 & v28[257]) != 0 )
          goto LABEL_78;
        v17 = (v15 & v28[256]) == 0;
      }
      if ( !v17 )
        goto LABEL_78;
LABEL_80:
      if ( ++v14 >= v9 )
      {
        v11 = i;
LABEL_82:
        if ( ++v34 >= v11 )
        {
          v10 = v29;
          goto LABEL_84;
        }
        goto LABEL_23;
      }
    }
  }
LABEL_85:
  v23 = (char *)v28;
  for ( k = 0; k < v31->m_theFace->m_cellHeight; ++k )
  {
    v24 = 0;
    if ( v9 != 0 )
    {
      v25 = dst;
      do
      {
        v26 = *(unsigned int *)((char *)v25 + v23 - (char *)dst);
        if ( v26 == 1 )
        {
          *v25 = -1;
        }
        else if ( v26 == 2 )
        {
          *v25 = 2130706432;
        }
        else
        {
          *v25 = v26 != 4 ? 0 : 0xFF000000;
        }
        v9 = *(_DWORD *)(glyphData + 48);
        ++v24;
        ++v25;
      }
      while ( v24 < v9 );
    }
    dst += 256;
    v23 += 1024;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C2060  EA 0x5C2060
// DIA: $E5
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E5_264()
{
  return _E4_264();
}


// -----------------------------------------------------------------------------
// RVA 0x001C2070  EA 0x5C2070
// DIA: $E4
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E4_264()
{
  TWO_PI_264 = 3.1415927 + 3.1415927;
}


// -----------------------------------------------------------------------------
// RVA 0x001C2080  EA 0x5C2080
// DIA: $E8
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E8_264()
{
  return _E7_264();
}


// -----------------------------------------------------------------------------
// RVA 0x001C2090  EA 0x5C2090
// DIA: $E7
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E7_264()
{
  OO_TWO_PI_264 = 1.0 / TWO_PI_264;
}


// -----------------------------------------------------------------------------
// RVA 0x001C20B0  EA 0x5C20B0
// DIA: TEXTURECACHE::PasteGlyphNonOutlinedMonochrome
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::PasteGlyphNonOutlinedMonochrome(
        TEXTURECACHE *this,
        GLYPHBITMAPDATA *glyphData,
        unsigned int *dst)
{
  unsigned int *v4; // ebx
  unsigned int v5; // edx
  unsigned int *v6; // edi
  char v7; // cl
  unsigned int v8; // eax
  unsigned int m_glyphHeight; // ecx
  int m_yStart; // edi
  unsigned int pitch; // [esp+10h] [ebp-8h]
  char *src; // [esp+14h] [ebp-4h]
  unsigned int y; // [esp+20h] [ebp+8h]
  unsigned int ya; // [esp+20h] [ebp+8h]
  unsigned int yb; // [esp+20h] [ebp+8h]
  unsigned int dstCellStride; // [esp+24h] [ebp+Ch]

  if ( glyphData == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1204, aGlyphdata, 0, 1u);
  v4 = dst;
  if ( dst == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1205, aDst, 0, 1u);
  pitch = glyphData->m_glyphPitch;
  v5 = 4 * glyphData->m_glyphCellWidth;
  src = (char *)glyphData->m_data;
  dstCellStride = v5;
  for ( y = 0; y < glyphData->m_yStart; ++y )
  {
    memset(v4, 0, 4 * (v5 >> 2));
    v6 = &v4[v5 >> 2];
    v7 = v5;
    v5 = dstCellStride;
    memset(v6, 0, v7 & 3);
    v4 += 256;
  }
  for ( ya = 0; ya < glyphData->m_glyphHeight; ++ya )
  {
    v8 = 0;
    if ( glyphData->m_glyphWidth != 0 )
    {
      do
      {
        if ( ((src[v8 >> 3] >> (7 - (v8 & 7))) & 1) != 0 )
          v4[v8] = -1;
        else
          v4[v8] = 0;
        ++v8;
      }
      while ( v8 < glyphData->m_glyphWidth );
      v5 = dstCellStride;
    }
    v4 += 256;
    src += pitch;
  }
  m_glyphHeight = glyphData->m_glyphHeight;
  m_yStart = glyphData->m_yStart;
  if ( (int)(this->m_theFace->m_cellHeight - m_glyphHeight - m_yStart) > 0
    && this->m_theFace->m_cellHeight - m_glyphHeight != m_yStart )
  {
    yb = this->m_theFace->m_cellHeight - m_glyphHeight - m_yStart;
    do
    {
      memset(v4, 0, v5);
      v4 += 256;
      --yb;
    }
    while ( yb != 0 );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C2210  EA 0x5C2210
// DIA: TEXTURECACHE::PasteGlyphOutlinedAA
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::PasteGlyphOutlinedAA(TEXTURECACHE *this, int glyphData, unsigned int *dst, int thick)
{
  _BYTE *v4; // ebx
  char *v5; // edi
  unsigned int v6; // eax
  unsigned int v7; // ecx
  unsigned int v8; // edx
  _DWORD *v9; // ecx
  int v10; // esi
  unsigned __int8 v11; // al
  unsigned int m_cellHeight; // ebx
  _DWORD *v13; // edi
  int v14; // ecx
  int v15; // eax
  unsigned int v16; // esi
  unsigned int v17; // ebx
  unsigned int v18; // edx
  unsigned int v19; // ecx
  unsigned int v20; // esi
  int *v21; // edi
  bool v22; // zf
  int v23; // esi
  int v24; // ebx
  unsigned int v25; // ebx
  unsigned int v26; // edx
  unsigned int v27; // ebx
  unsigned int v28; // esi
  unsigned int v29; // eax
  unsigned int v30; // edi
  BOOL v31; // ecx
  int v32; // ecx
  bool v33; // zf
  int v34; // ebx
  int v35; // edx
  BOOL v36; // ebx
  int v37; // ecx
  int v38; // ecx
  char *v39; // edx
  _DWORD *v40; // ecx
  unsigned int i; // edx
  int v42; // eax
  bool v43; // cf
  _DWORD v44[8192]; // [esp+Ch] [ebp-1801Ch] BYREF
  _DWORD v45[8192]; // [esp+800Ch] [ebp-1001Ch] BYREF
  _DWORD v46[8192]; // [esp+1000Ch] [ebp-801Ch] BYREF
  int v47; // [esp+1800Ch] [ebp-1Ch]
  unsigned int v48; // [esp+18010h] [ebp-18h]
  TEXTURECACHE *v49; // [esp+18014h] [ebp-14h]
  int v50; // [esp+18018h] [ebp-10h]
  unsigned int v51; // [esp+1801Ch] [ebp-Ch]
  unsigned int v52; // [esp+18020h] [ebp-8h]
  _DWORD *v53; // [esp+18024h] [ebp-4h]
  unsigned int v54; // [esp+18038h] [ebp+10h]
  unsigned int v55; // [esp+18038h] [ebp+10h]
  int v56; // [esp+18038h] [ebp+10h]

  v49 = this;
  if ( glyphData == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1253, aGlyphdata, 0, 1u);
  if ( dst == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1254, aDst, 0, 1u);
  if ( *(_DWORD *)(glyphData + 28) == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1255, aGlyphdataMData, 0, 1u);
  v4 = *(_BYTE **)(glyphData + 28);
  memset(v46, 0, sizeof(v46));
  memset(v45, 0, sizeof(v45));
  v5 = (char *)&v46[256 * *(_DWORD *)(glyphData + 68) + 257];
  if ( thick != 0 )
    v5 += 4;
  v53 = &v45[(v5 - (char *)v46) >> 2];
  v6 = *(_DWORD *)(glyphData + 44);
  if ( v6 != 0 )
  {
    v7 = *(_DWORD *)(glyphData + 60);
    v51 = *(_DWORD *)(glyphData + 40);
    v50 = v7;
    v52 = v6;
    do
    {
      v8 = 0;
      if ( v51 != 0 )
      {
        v9 = v53;
        v10 = v5 - (char *)v53;
        do
        {
          v11 = v4[v8];
          if ( v11 != 0 )
          {
            *(_DWORD *)((char *)v9 + v10) = 1;
            *v9 = v11;
          }
          ++v8;
          ++v9;
        }
        while ( v8 < v51 );
      }
      v5 += 1024;
      v4 += v50;
      v22 = v52 == 1;
      v53 += 256;
      --v52;
    }
    while ( !v22 );
  }
  m_cellHeight = v49->m_theFace->m_cellHeight;
  v13 = nullptr;
  v51 = m_cellHeight;
  v53 = nullptr;
  v48 = (thick != 0) + 1;
  if ( (thick != 0) == -1 )
    goto LABEL_78;
  v14 = *(_DWORD *)(glyphData + 68) - 1;
  v47 = v14;
  do
  {
    v50 = 2 * (v13 != nullptr) + 2;
    v15 = 2 * (v13 != nullptr) + 1;
    v16 = v14 < 0 ? 0 : v14;
    v54 = v16;
    if ( v16 >= m_cellHeight )
      goto LABEL_77;
    do
    {
      v18 = v16 << 8;
      v19 = 0;
      v52 = *(_DWORD *)(glyphData + 48);
      v17 = v52;
      if ( v52 == 0 )
        goto LABEL_75;
      do
      {
        v20 = v18 + v19;
        v21 = &v46[v20];
        if ( (v15 & v46[v20]) != 0 )
          goto LABEL_73;
        if ( v54 != 0 )
        {
          if ( v54 == v51 - 1 )
          {
            if ( v19 == 0 )
            {
              if ( (v15 & v45[v18 + 7936]) != 0 || (v15 & v45[v18 + 7937]) != 0 )
                goto LABEL_72;
              v22 = (v15 & v46[v18 + 1]) == 0;
LABEL_71:
              if ( v22 )
                goto LABEL_73;
              goto LABEL_72;
            }
            if ( v19 == v52 - 1 )
            {
              if ( (v15 & *(v21 - 1)) != 0 || (v15 & v45[v18 + 7935 + v19]) != 0 )
                goto LABEL_72;
              v24 = v45[v18 + 7936 + v19];
            }
            else
            {
              if ( (v15 & v45[v18 + 7935 + v19]) != 0
                || (v15 & v45[v18 + 7936 + v19]) != 0
                || (v15 & v45[v18 + 7937 + v19]) != 0
                || (v15 & *(v21 - 1)) != 0 )
              {
                goto LABEL_72;
              }
              v24 = v46[v20 + 1];
            }
          }
          else
          {
            if ( v19 == 0 )
            {
              if ( (v15 & v45[v18 + 7936]) != 0
                || (v15 & v45[v18 + 7937]) != 0
                || (v15 & v46[v18 + 1]) != 0
                || (v15 & v46[v18 + 257]) != 0 )
              {
                goto LABEL_72;
              }
              v22 = (v15 & v46[v18 + 256]) == 0;
              goto LABEL_71;
            }
            v25 = v18 + v19;
            if ( v19 == v52 - 1 )
            {
              if ( (v15 & v45[v25 + 7936]) != 0
                || (v15 & v45[v18 + 7935 + v19]) != 0
                || (v15 & *(v21 - 1)) != 0
                || (v15 & v46[v20 + 255]) != 0 )
              {
                goto LABEL_72;
              }
              v24 = v46[v20 + 256];
            }
            else
            {
              if ( (v15 & v45[v25 + 7935]) != 0
                || (v15 & v45[v18 + 7936 + v19]) != 0
                || (v15 & v45[v18 + 7937 + v19]) != 0
                || (v15 & *(v21 - 1)) != 0
                || (v15 & v46[v20 + 1]) != 0
                || (v15 & v46[v20 + 255]) != 0
                || (v15 & v46[v20 + 256]) != 0 )
              {
                goto LABEL_72;
              }
              v24 = v46[v20 + 257];
            }
          }
          v22 = (v24 & v15) == 0;
          goto LABEL_71;
        }
        if ( v19 != 0 )
        {
          v23 = v45[v19 + 0x1FFF];
          if ( v19 == v17 - 1 )
          {
            if ( (v23 & v15) == 0 && (v15 & v46[v19 + 255]) == 0 )
            {
              v22 = (v15 & v46[v19 + 256]) == 0;
              goto LABEL_71;
            }
          }
          else if ( (v23 & v15) == 0
                 && (v15 & v46[v19 + 1]) == 0
                 && (v15 & v46[v19 + 255]) == 0
                 && (v15 & v46[v19 + 256]) == 0 )
          {
            v22 = (v15 & v46[v19 + 257]) == 0;
            goto LABEL_71;
          }
        }
        else if ( (v15 & v46[1]) == 0 && (v15 & v46[257]) == 0 )
        {
          v22 = (v15 & v46[256]) == 0;
          goto LABEL_71;
        }
LABEL_72:
        *v21 = v50;
LABEL_73:
        v17 = v52;
        ++v19;
      }
      while ( v19 < v52 );
      v16 = v54;
      v13 = v53;
LABEL_75:
      v54 = ++v16;
    }
    while ( v16 < v51 );
    m_cellHeight = v51;
    v14 = v47;
LABEL_77:
    v13 = (_DWORD *)((char *)v13 + 1);
    v53 = v13;
  }
  while ( (unsigned int)v13 < v48 );
LABEL_78:
  v26 = 0;
  memset(v44, 0, sizeof(v44));
  v55 = 0;
  if ( m_cellHeight != 0 )
  {
    while ( 1 )
    {
      v28 = v26 << 8;
      v29 = 0;
      v52 = *(_DWORD *)(glyphData + 48);
      v27 = v52;
      if ( v52 != 0 )
        break;
LABEL_103:
      v55 = ++v26;
      if ( v26 >= v51 )
        goto LABEL_104;
    }
    while ( 2 )
    {
      v30 = v28 + v29;
      v31 = v46[v30] != 0;
      v53 = (_DWORD *)v31;
      if ( v26 == 0 )
      {
        if ( v29 == 0 )
        {
          LOBYTE(v26) = v46[256] != 0;
          v32 = 0;
          v33 = v46[257] == 0;
          v34 = v46[1];
          goto LABEL_100;
        }
        LOBYTE(v26) = v45[v29 + 0x1FFF] != 0;
        if ( v29 == v27 - 1 )
        {
          v35 = (v46[v29 + 255] != 0) + v26;
          v36 = v46[v29 + 256] != 0;
        }
        else
        {
          v35 = (v46[v29 + 257] != 0) + (v46[v29 + 256] != 0) + (v46[v29 + 255] != 0) + v26;
          v36 = v46[v29 + 1] != 0;
        }
        v37 = v31 + v36 + v35;
LABEL_102:
        v27 = *(_DWORD *)(glyphData + 48);
        ++v29;
        v44[v30] = pixelsLitLevels[v37];
        v26 = v55;
        v52 = v27;
        if ( v29 >= v27 )
          goto LABEL_103;
        continue;
      }
      break;
    }
    if ( v26 == v51 - 1 )
    {
      if ( v29 == 0 )
      {
        v37 = v31 + (v46[v28 + 1] != 0) + (v45[v28 + 7937] != 0) + (v45[v28 + 7936] != 0);
        goto LABEL_102;
      }
      if ( v29 == v52 - 1 )
      {
        v38 = (v45[v28 + 7936 + v29] != 0) + (v45[v28 + 7935 + v29] != 0);
        goto LABEL_97;
      }
      v26 = (v45[v28 + 7937 + v29] != 0) + (v45[v28 + 7936 + v29] != 0) + (v45[v28 + 7935 + v29] != 0);
LABEL_99:
      v32 = 0;
      v33 = v46[v30 + 1] == 0;
      v34 = v45[v30 + 0x1FFF];
LABEL_100:
      LOBYTE(v32) = !v33;
      v39 = (char *)(v32 + v26);
      v38 = (int)v53 + (v34 != 0);
    }
    else
    {
      if ( v29 == 0 )
      {
        v37 = v31
            + (v46[v28 + 1] != 0)
            + (v46[v28 + 257] != 0)
            + (v46[v28 + 256] != 0)
            + (v45[v28 + 7937] != 0)
            + (v45[v28 + 7936] != 0);
        goto LABEL_102;
      }
      if ( v29 != v52 - 1 )
      {
        v26 = (v46[v30 + 257] != 0)
            + (v46[v30 + 256] != 0)
            + (v46[v30 + 255] != 0)
            + (v45[v28 + 7937 + v29] != 0)
            + (v45[v28 + 7936 + v29] != 0)
            + (v45[v28 + 7935 + v29] != 0);
        goto LABEL_99;
      }
      v38 = (v46[v30 + 256] != 0) + (v46[v30 + 255] != 0) + (v45[v28 + 7936 + v29] != 0) + (v45[v28 + 7935 + v29] != 0);
LABEL_97:
      v39 = (char *)v53 + (v45[v30 + 0x1FFF] != 0);
    }
    v37 = (int)&v39[v38];
    goto LABEL_102;
  }
LABEL_104:
  v40 = nullptr;
  v56 = 0;
  if ( v51 != 0 )
  {
    v53 = nullptr;
    do
    {
      for ( i = 0; i < *(_DWORD *)(glyphData + 48); ++v40 )
      {
        if ( *(_DWORD *)((char *)v46 + (_DWORD)v40) != 0 )
        {
          if ( *(_DWORD *)((char *)v45 + (_DWORD)v40) != 0 )
          {
            v42 = (unsigned __int8)((unsigned __int16)(255 * *((unsigned __int8 *)v45 + (_DWORD)v40)) >> 8);
            *(unsigned int *)((char *)dst + (_DWORD)v40) = v42
                                                         | ((v42
                                                           | ((v42 | (*(_DWORD *)((char *)v44 + (_DWORD)v40) << 8)) << 8)) << 8);
          }
          else
          {
            *(unsigned int *)((char *)dst + (_DWORD)v40) = *(_DWORD *)((char *)v44 + (_DWORD)v40) << 24;
          }
        }
        else
        {
          *(unsigned int *)((char *)dst + (_DWORD)v40) = 0;
        }
        ++i;
      }
      v40 = v53 + 256;
      v43 = v56 + 1 < v49->m_theFace->m_cellHeight;
      v53 += 256;
      ++v56;
    }
    while ( v43 );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C2A00  EA 0x5C2A00
// DIA: TEXTURECACHE::PasteGlyphNonOutlinedAA
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::PasteGlyphNonOutlinedAA(
        TEXTURECACHE *this,
        GLYPHBITMAPDATA *glyphData,
        unsigned int *dst)
{
  unsigned int *v4; // ebx
  char *m_data; // edi
  unsigned int v6; // edx
  unsigned int *v7; // edi
  char v8; // cl
  unsigned int m_glyphWidth; // ecx
  unsigned int i; // eax
  unsigned int m_glyphHeight; // ecx
  int m_yStart; // edi
  unsigned int pitch; // [esp+10h] [ebp-8h]
  char *src; // [esp+14h] [ebp-4h]
  unsigned int y; // [esp+20h] [ebp+8h]
  unsigned int ya; // [esp+20h] [ebp+8h]
  unsigned int yb; // [esp+20h] [ebp+8h]
  unsigned int dstCellStride; // [esp+24h] [ebp+Ch]

  if ( glyphData == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1517, aGlyphdata, 0, 1u);
  v4 = dst;
  if ( dst == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1518, aDst, 0, 1u);
  m_data = (char *)glyphData->m_data;
  pitch = glyphData->m_glyphPitch;
  v6 = 4 * glyphData->m_glyphCellWidth;
  src = m_data;
  dstCellStride = v6;
  y = 0;
  if ( glyphData->m_yStart != 0 )
  {
    do
    {
      memset(v4, 0, 4 * (v6 >> 2));
      v7 = &v4[v6 >> 2];
      v8 = v6;
      v6 = dstCellStride;
      memset(v7, 0, v8 & 3);
      v4 += 256;
      ++y;
    }
    while ( y < glyphData->m_yStart );
    m_data = src;
  }
  ya = 0;
  if ( glyphData->m_glyphHeight != 0 )
  {
    m_glyphWidth = glyphData->m_glyphWidth;
    do
    {
      for ( i = 0; i < m_glyphWidth; ++i )
      {
        v4[i] = (unsigned int)&s_textureData[81257].a | (m_data[i] << 24);
        m_glyphWidth = glyphData->m_glyphWidth;
      }
      m_data += pitch;
      v4 += 256;
      ++ya;
    }
    while ( ya < glyphData->m_glyphHeight );
  }
  m_glyphHeight = glyphData->m_glyphHeight;
  m_yStart = glyphData->m_yStart;
  if ( (int)(this->m_theFace->m_cellHeight - m_glyphHeight - m_yStart) > 0
    && this->m_theFace->m_cellHeight - m_glyphHeight != m_yStart )
  {
    yb = this->m_theFace->m_cellHeight - m_glyphHeight - m_yStart;
    do
    {
      memset(v4, 0, v6);
      v4 += 256;
      --yb;
    }
    while ( yb != 0 );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C2B30  EA 0x5C2B30
// DIA: TEXTURECACHE::PasteGlyph
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::PasteGlyph(TEXTURECACHE *this, GLYPHBITMAPDATA *data, unsigned int *dst, int thick)
{
  unsigned int m_flags; // eax

  if ( this->m_theFace == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1555, aMTheface, 0, 1u);
  m_flags = this->m_theFace->m_flags;
  if ( (m_flags & 1) != 0 )
  {
    if ( (m_flags & 0x10) != 0 )
      TEXTURECACHE::PasteGlyphOutlinedMonochrome(this, (int)data, dst, thick);
    else
      TEXTURECACHE::PasteGlyphOutlinedAA(this, (int)data, dst, thick);
  }
  else if ( (m_flags & 0x10) != 0 )
  {
    TEXTURECACHE::PasteGlyphNonOutlinedMonochrome(this, data, dst);
  }
  else
  {
    TEXTURECACHE::PasteGlyphNonOutlinedAA(this, data, dst);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C2BC0  EA 0x5C2BC0
// DIA: GLYPHBITMAPDATA::Clear
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall GLYPHBITMAPDATA::Clear(GLYPHBITMAPDATA *this)
{
  void *m_data; // eax

  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aDBuildBuildwow_239, 1571, 0);
  this->m_data = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001C2BF0  EA 0x5C2BF0
// DIA: CGxString::SetStringPosition
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::SetStringPosition(CGxString *this, const NTempest::C3Vector *position)
{
  unsigned int v3; // esi
  STRINGVIEWMATRICES *v4; // edi
  unsigned int v5; // esi
  TSLink<STRINGVIEWMATRICES> *m_prevlink; // esi
  TSLink<STRINGVIEWMATRICES> *p_m_linktoslot; // eax
  int m_next; // edx
  int v9; // ecx
  TSLink<STRINGVIEWMATRICES> *v10; // esi
  TSLink<STRINGVIEWMATRICES> *p_m_linktofull; // eax
  int v12; // edx
  int v13; // edx
  TSLink<STRINGVIEWMATRICES> *v14; // esi
  TSLink<STRINGVIEWMATRICES> *v15; // eax
  float minx; // [esp+Ch] [ebp-14h] BYREF
  float maxx; // [esp+10h] [ebp-10h] BYREF
  float miny; // [esp+14h] [ebp-Ch] BYREF
  float maxy; // [esp+18h] [ebp-8h] BYREF
  float minz; // [esp+1Ch] [ebp-4h] BYREF

  this->m_position = *position;
  CGxString::InitializeViewportOffsets(this);
  if ( this->m_batchedStringLink.m_next != nullptr )
  {
    GxXformViewport((CBoundingBox *)&minx, &maxx, &miny, &maxy, &minz, (float *)&position);
    if ( s_stringViewMatrices.m_slotmask != -1 )
    {
      v3 = (unsigned int)this & s_stringViewMatrices.m_slotmask;
      TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
        &s_stringViewMatrices.m_slotlistarray,
        (unsigned int)this & s_stringViewMatrices.m_slotmask);
      v4 = (int)s_stringViewMatrices.m_slotlistarray.m_data[v3].m_terminator.m_next <= 0
         ? nullptr
         : s_stringViewMatrices.m_slotlistarray.m_data[v3].m_terminator.m_next;
      if ( (int)v4 > 0 )
      {
        while ( (CGxString *)v4->m_hashval != this || v4->m_key.m_key != this )
        {
          v5 = (unsigned int)this & s_stringViewMatrices.m_slotmask;
          TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
            &s_stringViewMatrices.m_slotlistarray,
            (unsigned int)this & s_stringViewMatrices.m_slotmask);
          v4 = *(STRINGVIEWMATRICES **)((char *)&v4->m_linktoslot.m_prevlink
                                      + s_stringViewMatrices.m_slotlistarray.m_data[v5].m_linkoffset);
          if ( (int)v4 <= 0 )
            return;
        }
        if ( v4->m_linktoslot.m_next != nullptr )
        {
          m_prevlink = v4->m_linktoslot.m_prevlink;
          p_m_linktoslot = &v4->m_linktoslot;
          if ( m_prevlink != nullptr )
          {
            m_next = (int)v4->m_linktoslot.m_next;
            if ( m_next > 0 )
              v9 = m_next + (char *)p_m_linktoslot - (char *)m_prevlink->m_next;
            else
              v9 = ~m_next;
            *(_DWORD *)v9 = m_prevlink;
            p_m_linktoslot->m_prevlink->m_next = v4->m_linktoslot.m_next;
            p_m_linktoslot->m_prevlink = nullptr;
            v4->m_linktoslot.m_next = nullptr;
          }
          v10 = v4->m_linktofull.m_prevlink;
          p_m_linktofull = &v4->m_linktofull;
          if ( v10 != nullptr )
          {
            v12 = (int)v4->m_linktofull.m_next;
            if ( v12 > 0 )
              v13 = (char *)p_m_linktofull - (char *)v10->m_next + v12;
            else
              v13 = ~v12;
            *(_DWORD *)v13 = v10;
            p_m_linktofull->m_prevlink->m_next = v4->m_linktofull.m_next;
            p_m_linktofull->m_prevlink = nullptr;
            v4->m_linktofull.m_next = nullptr;
          }
        }
        v14 = (TSLink<STRINGVIEWMATRICES> *)((char *)v4 + s_freeStringMatrices.m_linkoffset);
        if ( *(unsigned int *)((char *)&v4->m_hashval + s_freeStringMatrices.m_linkoffset) != 0 )
          TSLink<STRINGVIEWMATRICES>::Unlink((TSLink<STRINGVIEWMATRICES> *)((char *)v4
                                                                          + s_freeStringMatrices.m_linkoffset));
        v15 = s_freeStringMatrices.m_terminator.m_prevlink;
        v14->m_prevlink = s_freeStringMatrices.m_terminator.m_prevlink;
        v14->m_next = v15->m_next;
        v15->m_next = v4;
        s_freeStringMatrices.m_terminator.m_prevlink = v14;
      }
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C2D70  EA 0x5C2D70
// DIA: CGxString::SetColor
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::SetColor(CGxString *this, const NTempest::CImVector *color)
{
  unsigned __int8 a; // al
  unsigned int m_flags; // eax

  if ( *color != *(_DWORD *)&this->m_fontColor )
  {
    this->m_fontColor = *color;
    a = color->a;
    if ( a >= this->m_shadowColor.a )
      a = this->m_shadowColor.a;
    this->m_shadowColor.a = a;
    m_flags = this->m_flags;
    if ( (m_flags & 8) == 0
      || (m_flags & 0x1000) != 0
      || this->m_colorGradients.m_count != 0
      || this->m_colorGradientShadows.m_count != 0 )
    {
      CGxString::CreateGeometry(this);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C2DC0  EA 0x5C2DC0
// DIA: CGxString::HandleScreenSizeChange
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __usercall CGxString::HandleScreenSizeChange(CGxString *this@<ecx>, double a2@<st0>)
{
  CGxFont *v3; // eax
  double m_requestedFontHeight; // st7

  if ( this->m_currentFace == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1620, aMCurrentface, 0, 1u);
  CGxString::ClearStringMatrixEntry(this);
  if ( (this->m_flags & 4) != 0 )
  {
    GxuFontGetOneToOneHeight(v3);
    this->m_requestedFontHeight = a2;
  }
  m_requestedFontHeight = 2.0 / (double)g_heightPixels;
  if ( this->m_requestedFontHeight > m_requestedFontHeight )
    m_requestedFontHeight = this->m_requestedFontHeight;
  this->m_currentFontHeight = m_requestedFontHeight;
  CGxString::CreateGeometry(this);
}


// -----------------------------------------------------------------------------
// RVA 0x001C2E40  EA 0x5C2E40
// DIA: CGxString::InternalRender
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::InternalRender(CGxString *this)
{
  CGxString *v1; // edi
  unsigned int v2; // ebx
  _DWORD *v3; // esi
  int v4; // eax
  const char *v5; // eax
  int v6; // eax
  const char *v7; // eax
  int v8; // edi
  unsigned int v9; // eax
  int v10; // ecx
  _DWORD *v11; // edi
  int v12; // eax
  const char *v13; // eax
  int v14; // [esp-1Ch] [ebp-2Ch]
  int v15; // [esp-1Ch] [ebp-2Ch]
  int v16; // [esp-1Ch] [ebp-2Ch]
  int v17; // [esp-18h] [ebp-28h]
  int v18; // [esp-18h] [ebp-28h]
  int v19; // [esp-18h] [ebp-28h]
  unsigned int v20; // [esp-14h] [ebp-24h]
  unsigned int v21; // [esp-14h] [ebp-24h]
  unsigned int v22; // [esp-14h] [ebp-24h]
  const char *v23; // [esp-10h] [ebp-20h]
  const char *v24; // [esp-10h] [ebp-20h]
  const char *v25; // [esp-10h] [ebp-20h]
  unsigned int i; // [esp+Ch] [ebp-4h]

  v1 = this;
  GxVertexShaderSelect(GxVS_PassThru);
  GxRsPush();
  GxRsSet(GxRs_Lighting, GxBlend_Opaque);
  GxRsSet(GxRs_Culling, GxBlend_Opaque);
  GxRsSet(GxRs_Blend, GxBlend_Alpha);
  if ( (__S56__1__InternalRender_CGxString__AAEXXZ_4EA & 1) == 0 )
  {
    __S56__1__InternalRender_CGxString__AAEXXZ_4EA |= 1u;
    normal_4.x = 0.0;
    normal_4.y = 0.0;
    normal_4.z = -1.0;
    atexit(_E57_52);
  }
  if ( SLOBYTE(v1->m_flags) >= 0 )
  {
    GxRsSet(GxRs_DepthWrite, GxBlend_Opaque);
    GxRsSet(GxRs_DepthTest, GxBlend_Opaque);
    GxRsSet(GxRs_Fog, GxBlend_Opaque);
  }
  if ( v1->m_currentFace == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1647, aMCurrentface, 0, 1u);
  v2 = 0;
  if ( v1->m_textBlock.m_lines.m_count != 0 )
  {
    v3 = &v1->m_textBlock.m_lines.__vftable;
    do
    {
      for ( i = 0; ; ++i )
      {
        if ( v2 >= v3[2] )
        {
          v4 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, _DWORD))(*v3 + 4))(
                 v3,
                 1,
                 1,
                 aIndex0x08xArra,
                 v2,
                 v3[2]);
          v5 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v3)(v3, v4);
          SErrDisplayErrorFmt(0x85100080, v5, v14, v17, v20, v23);
        }
        if ( i >= *(_DWORD *)(*(_DWORD *)(v3[3] + 4 * v2) + 16) )
          break;
        if ( v2 >= v3[2] )
        {
          v6 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, _DWORD))(*v3 + 4))(
                 v3,
                 1,
                 1,
                 aIndex0x08xArra,
                 v2,
                 v3[2]);
          v7 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v3)(v3, v6);
          SErrDisplayErrorFmt(0x85100080, v7, v15, v18, v21, v24);
        }
        v8 = *(_DWORD *)(v3[3] + 4 * v2);
        v9 = *(_DWORD *)(v8 + 16);
        v10 = i;
        v11 = (_DWORD *)(v8 + 8);
        if ( i >= v9 )
        {
          v12 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, unsigned int))(*v11 + 4))(
                  v11,
                  1,
                  1,
                  aIndex0x08xArra,
                  i,
                  v9);
          v13 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v11)(v11, v12);
          SErrDisplayErrorFmt(0x85100080, v13, v16, v19, v22, v25);
          v10 = i;
        }
        TEXTLINETEXTURE::InternalRenderTexture(
          *(TEXTLINETEXTURE **)(v11[3] + 4 * v10),
          v10,
          this->m_currentFace,
          this->m_flags & 1,
          &this->m_shadowColor,
          &this->m_shadowOffset,
          &this->m_fontColor);
        v1 = this;
      }
      ++v2;
    }
    while ( v2 < v1->m_textBlock.m_lines.m_count );
  }
  GxRsPop();
}


// -----------------------------------------------------------------------------
// RVA 0x001C3030  EA 0x5C3030
// DIA: $E57
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E57_52()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001C3040  EA 0x5C3040
// DIA: CGxString::Render
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::Render(CGxString *this, const NTempest::C44Matrix *xform)
{
  if ( this->m_textBlock.m_lines.m_count != 0 )
  {
    CGxString::CheckEvictedTextures(this);
    GxXformPush(GxXform_World, xform);
    CGxString::InternalRender(this);
    GxXformPop(GxXform_World);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C3080  EA 0x5C3080
// DIA: CGxString::Render
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::Render(CGxString *this)
{
  float width; // [esp+0h] [ebp-134h]
  float height; // [esp+4h] [ebp-130h]
  float maxz; // [esp+Ch] [ebp-128h] BYREF
  float minz; // [esp+10h] [ebp-124h] BYREF
  NTempest::C44Matrix oldProjection; // [esp+14h] [ebp-120h] BYREF
  NTempest::C44Matrix oldView; // [esp+54h] [ebp-E0h] BYREF
  NTempest::C44Matrix view; // [esp+94h] [ebp-A0h] BYREF
  NTempest::C44Matrix proj; // [esp+D4h] [ebp-60h] BYREF
  float pixHeight[2]; // [esp+114h] [ebp-20h]
  float pixWidth[2]; // [esp+11Ch] [ebp-18h]
  float minx; // [esp+124h] [ebp-10h] BYREF
  float maxy; // [esp+128h] [ebp-Ch] BYREF
  float maxx; // [esp+12Ch] [ebp-8h] BYREF
  float miny; // [esp+130h] [ebp-4h] BYREF

  if ( this->m_textBlock.m_lines.m_count != 0 )
  {
    CGxString::CheckEvictedTextures(this);
    GxXformViewport((CBoundingBox *)&minx, &maxx, &miny, &maxy, &minz, &maxz);
    oldProjection.a0 = 1.0;
    memset(&oldProjection.a1, 0, 16);
    oldProjection.b1 = 1.0;
    memset(&oldProjection.b2, 0, 16);
    oldProjection.c2 = 1.0;
    memset(&oldProjection.c3, 0, 16);
    oldProjection.d3 = 1.0;
    oldView.a0 = 1.0;
    memset(&oldView.a1, 0, 16);
    oldView.b1 = 1.0;
    memset(&oldView.b2, 0, 16);
    oldView.c2 = 1.0;
    memset(&oldView.c3, 0, 16);
    oldView.d3 = 1.0;
    GxXformProjection(&oldProjection);
    GxXformView(&oldView);
    proj.a0 = 1.0;
    memset(&proj.a1, 0, 16);
    proj.b1 = 1.0;
    memset(&proj.b2, 0, 16);
    proj.c2 = 1.0;
    memset(&proj.c3, 0, 16);
    proj.d3 = 1.0;
    LODWORD(pixWidth[0]) = GetScreenPixelWidth();
    pixWidth[1] = (float)LODWORD(pixWidth[0]);
    LODWORD(pixHeight[0]) = GetScreenPixelHeight();
    pixHeight[1] = (float)LODWORD(pixHeight[0]);
    CGxString::BuildProjection(this, &proj, minx, maxx, miny, maxy, pixWidth[1], pixHeight[1]);
    GxXformSetProjection(&proj);
    height = maxy - miny;
    view.a0 = 1.0;
    memset(&view.a1, 0, 16);
    width = maxx - minx;
    view.b1 = 1.0;
    memset(&view.b2, 0, 16);
    view.c2 = 1.0;
    memset(&view.c3, 0, 16);
    view.d3 = 1.0;
    CGxString::BuildView(this, &view, width, height);
    GxXformSetView(&view);
    CGxString::InternalRender(this);
    GxXformSetProjection(&oldProjection);
    GxXformSetView(&oldView);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C33C0  EA 0x5C33C0
// DIA: CGxString::Recycle
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::Recycle(CGxString *this)
{
  TSLink<CGxString> *m_prevlink; // esi
  TSLink<CGxString> *p_m_batchedStringLink; // eax
  int m_next; // edx
  int v5; // edx
  CGxString *p_m_terminator; // eax
  TSLink<CGxString> *v7; // esi
  int v8; // edx
  int v9; // edx
  TSLink<CGxString> *v10; // ecx

  m_prevlink = this->m_batchedStringLink.m_prevlink;
  p_m_batchedStringLink = &this->m_batchedStringLink;
  if ( m_prevlink != nullptr )
  {
    m_next = (int)this->m_batchedStringLink.m_next;
    if ( m_next > 0 )
      v5 = (char *)p_m_batchedStringLink - (char *)m_prevlink->m_next + m_next;
    else
      v5 = ~m_next;
    *(_DWORD *)v5 = m_prevlink;
    this->m_batchedStringLink.m_prevlink->m_next = this->m_batchedStringLink.m_next;
    p_m_batchedStringLink->m_prevlink = nullptr;
    this->m_batchedStringLink.m_next = nullptr;
  }
  p_m_terminator = this;
  if ( this == nullptr )
    p_m_terminator = (CGxString *)&g_freeStrings.m_terminator;
  v7 = p_m_terminator->m_link.m_prevlink;
  if ( p_m_terminator->m_link.m_prevlink != nullptr )
  {
    v8 = (int)p_m_terminator->m_link.m_next;
    if ( v8 > 0 )
      v9 = (char *)p_m_terminator - (char *)v7->m_next + v8;
    else
      v9 = ~v8;
    *(_DWORD *)v9 = v7;
    p_m_terminator->m_link.m_prevlink->m_next = p_m_terminator->m_link.m_next;
    p_m_terminator->m_link.m_prevlink = nullptr;
    p_m_terminator->m_link.m_next = nullptr;
  }
  v10 = g_freeStrings.m_terminator.m_prevlink;
  p_m_terminator->m_link.m_prevlink = g_freeStrings.m_terminator.m_prevlink;
  p_m_terminator->m_link.m_next = v10->m_next;
  v10->m_next = this;
  g_freeStrings.m_terminator.m_prevlink = &p_m_terminator->m_link;
  CGxString::ClearInstanceData(this);
  CGxString::ClearStringMatrixEntry(this);
}


// -----------------------------------------------------------------------------
// RVA 0x001C3460  EA 0x5C3460
// DIA: CGxString::ClearInstanceData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::ClearInstanceData(CGxString *this)
{
  this->m_hyperlinkInfo.m_count = 0;
  IGXUTEXTBLOCK::Recycle(&this->m_textBlock);
  this->m_colorGradients.m_count = 0;
  this->m_colorGradientShadows.m_count = 0;
  this->m_lastGradientStart = -1;
  this->m_lastGradientLength = -1;
}


// -----------------------------------------------------------------------------
// RVA 0x001C34A0  EA 0x5C34A0
// DIA: CGxString::CreateGeometry
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::CreateGeometry(CGxString *this)
{
  int m_lastGradientStart; // eax
  double v3; // st7
  int v4; // eax
  int v5; // eax
  NTempest::CImVector m_fontColor; // edx
  char *m_text; // esi
  double v8; // st7
  double v9; // st7
  __int64 v10; // rax
  double v11; // st7
  bool v12; // zf
  unsigned int v13; // eax
  QUOTEDCODE v14; // eax
  double v15; // st7
  unsigned int v16; // edi
  EGxFontHJusts m_horzJust; // eax
  double v18; // st7
  char m_flags; // al
  unsigned int v20; // ecx
  float height; // [esp+4h] [ebp-70h]
  float heighta; // [esp+4h] [ebp-70h]
  float heightb; // [esp+4h] [ebp-70h]
  HYPERLINKPARSEINFO info; // [esp+14h] [ebp-60h] BYREF
  NTempest::C3Vector linePos; // [esp+38h] [ebp-3Ch] BYREF
  int gStart; // [esp+44h] [ebp-30h]
  unsigned int advance; // [esp+48h] [ebp-2Ch] BYREF
  NTempest::CImVector workingColor; // [esp+4Ch] [ebp-28h] BYREF
  unsigned int wide; // [esp+50h] [ebp-24h] BYREF
  int gLength; // [esp+54h] [ebp-20h]
  unsigned int texturePagesUsedFlag; // [esp+58h] [ebp-1Ch] BYREF
  unsigned int numBytes; // [esp+5Ch] [ebp-18h] BYREF
  float widestLineExtent; // [esp+60h] [ebp-14h]
  const char *nextText; // [esp+64h] [ebp-10h] BYREF
  unsigned int loop; // [esp+68h] [ebp-Ch]
  float lineHeight; // [esp+6Ch] [ebp-8h]
  float extent; // [esp+70h] [ebp-4h] BYREF

  m_lastGradientStart = this->m_lastGradientStart;
  gLength = this->m_lastGradientLength;
  gStart = m_lastGradientStart;
  CGxString::ClearInstanceData(this);
  v3 = -this->m_currentFontHeight;
  height = v3;
  *(float *)&v5 = ScreenToPixelHeight(v4, height);
  *(float *)&loop = v3;
  m_fontColor = this->m_fontColor;
  linePos.y = v3;
  m_text = this->m_text;
  linePos.z = this->m_position.z;
  v8 = this->m_spacing + this->m_currentFontHeight;
  linePos.x = 0.0;
  heighta = v8;
  workingColor = m_fontColor;
  info.hyperlinkParseMode = HYPERLINKNONE;
  info.currentParseInfo.extent.l = 0.0;
  info.currentParseInfo.extent.r = 0.0;
  nextText = nullptr;
  numBytes = 0;
  ScreenToPixelHeight(v5, heighta);
  lineHeight = v8;
  LODWORD(info.currentParseInfo.extent.b) = loop;
  widestLineExtent = 0.0;
  info.currentParseInfo.extent.t = lineHeight + *(float *)&loop;
  v9 = this->m_blockHeight / (this->m_spacing + this->m_currentFontHeight);
  if ( v9 <= 0.0 )
  {
    v11 = -v9;
    extent = v11;
    if ( v11 < 0.0 )
      SErrDisplayErrorFmt(0x85100000, aTempestCmathH, 211, 0, 1u, "\"%s\", %s = %f", aR0f, aR, extent);
    LODWORD(v10) = -(int)(__int64)(extent + 0.5);
  }
  else
  {
    v10 = (__int64)(v9 + 0.5);
  }
  loop = v10;
  if ( m_text != nullptr )
  {
    while ( 1 )
    {
      v12 = loop-- == 0;
      if ( v12 || *m_text == 0 )
        goto LABEL_30;
      v13 = SStrLen(m_text);
      v14 = GxuDetermineQuotedCode(m_text, &advance, nullptr, this->m_flags, &wide, v13);
      extent = 0.0;
      if ( wide == 10 || v14 == CODE_NEWLINE )
      {
        m_text += advance;
        IGXUTEXTBLOCK::NewLine(&this->m_textBlock);
        linePos.y = linePos.y - lineHeight;
        goto LABEL_29;
      }
      CalcWrapPoint(
        (char *)this->m_currentFace,
        m_text,
        (CGxFont *)LODWORD(this->m_currentFontHeight),
        this->m_blockWidth,
        &numBytes,
        &extent,
        &nextText,
        this->m_flags);
      v15 = extent;
      if ( extent > (double)widestLineExtent )
        widestLineExtent = extent;
      v16 = numBytes;
      if ( numBytes == 0 && *nextText == 0 )
        goto LABEL_30;
      m_horzJust = this->m_horzJust;
      if ( m_horzJust == GxHJ_Right )
        break;
      if ( m_horzJust == GxHJ_Center )
      {
        v18 = extent * 0.5;
        goto LABEL_19;
      }
LABEL_20:
      texturePagesUsedFlag = 0;
      if ( info.hyperlinkParseMode != HYPERLINKNONE )
        info.currentParseInfo.extent.l = linePos.x;
      CGxString::InitializeTextLine(
        this,
        v15,
        m_text,
        v16,
        &workingColor,
        COERCE_FLOAT(&linePos),
        &texturePagesUsedFlag,
        &info);
      if ( info.hyperlinkParseMode == HYPERLINKHREF )
      {
        SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1825, aInfoHyperlinkp, 0, 1u);
      }
      else if ( info.hyperlinkParseMode == HYPERLINKDISPLAY )
      {
        CGxString::AddHyperlinkParseInfo(this, info.currentParseInfo);
      }
      m_flags = this->m_flags;
      m_text = (char *)nextText;
      v20 = texturePagesUsedFlag | this->m_texturePagesUsed;
      linePos.y = linePos.y - lineHeight;
      this->m_texturePagesUsed = v20;
      if ( (m_flags & 2) != 0 )
        goto LABEL_30;
      info.currentParseInfo.extent.b = info.currentParseInfo.extent.b - lineHeight;
      info.currentParseInfo.extent.t = info.currentParseInfo.extent.t - lineHeight;
LABEL_29:
      if ( m_text == nullptr )
        goto LABEL_30;
    }
    v18 = extent;
LABEL_19:
    v15 = -v18;
    heightb = v15;
    ScreenToPixelWidth(m_horzJust, heightb);
    linePos.x = v15;
    goto LABEL_20;
  }
LABEL_30:
  this->m_savedWidth = widestLineExtent;
  CGxString::InitializeViewportOffsets(this);
  CGxString::GenerateVertexIndices(this);
  if ( (this->m_flags & 0x1000) != 0 && (gStart != -1 || gLength != -1) )
    CGxString::SetGradient(this, gStart, gLength);
}


// -----------------------------------------------------------------------------
// RVA 0x001C37A0  EA 0x5C37A0
// DIA: CGxString::InitializeViewportOffsets
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::InitializeViewportOffsets(CGxString *this)
{
  unsigned int m_count; // eax
  double v3; // st7
  double v4; // st6
  double v5; // st5
  EGxFontHJusts m_horzJust; // eax
  double v7; // st7
  EGxFontVJusts m_vertJust; // eax
  double v9; // st6
  double v10; // st6
  double v11; // st5
  NTempest::C2Vector positiona; // [esp+8h] [ebp-2Ch]
  NTempest::C2Vector positionb; // [esp+8h] [ebp-2Ch]
  float position; // [esp+8h] [ebp-2Ch]
  float positionc; // [esp+8h] [ebp-2Ch]
  float position_4; // [esp+Ch] [ebp-28h]
  float position_4a; // [esp+Ch] [ebp-28h]
  float minz; // [esp+10h] [ebp-24h] BYREF
  float maxz; // [esp+14h] [ebp-20h] BYREF
  float miny; // [esp+18h] [ebp-1Ch] BYREF
  float maxy; // [esp+1Ch] [ebp-18h] BYREF
  float minx; // [esp+20h] [ebp-14h] BYREF
  float maxx; // [esp+24h] [ebp-10h] BYREF
  float blockHeight; // [esp+28h] [ebp-Ch]
  float blockWidth; // [esp+2Ch] [ebp-8h]
  float tHeight; // [esp+30h] [ebp-4h]

  m_count = this->m_textBlock.m_lines.m_count;
  positiona = (NTempest::C2Vector)g_heightPixels;
  v3 = (double)g_heightPixels;
  LODWORD(positiona.x) = m_count - 1;
  v4 = (double)(__int64)positiona;
  positionb = (NTempest::C2Vector)m_count;
  LODWORD(positionb.x) = g_widthPixels;
  tHeight = (v4 * this->m_spacing + (double)m_count * this->m_currentFontHeight) * v3;
  v5 = (double)(__int64)positionb;
  blockWidth = v5 * this->m_blockWidth;
  blockHeight = v3 * this->m_blockHeight;
  position = v5 * this->m_position.x;
  position_4 = v3 * this->m_position.y;
  this->m_stringHeight = tHeight;
  GxXformViewport((CBoundingBox *)&minx, &maxx, &miny, &maxy, &minz, &maxz);
  m_horzJust = this->m_horzJust;
  if ( m_horzJust == GxHJ_Right )
  {
    v7 = position + blockWidth;
  }
  else if ( m_horzJust == GxHJ_Center )
  {
    v7 = blockWidth * 0.5 + position;
  }
  else
  {
    v7 = position;
  }
  m_vertJust = this->m_vertJust;
  if ( m_vertJust != GxVJ_Top )
  {
    if ( m_vertJust == GxVJ_Middle )
      v10 = (blockHeight - tHeight) * 0.5 + position_4;
    else
      v10 = position_4;
    v9 = v10 + tHeight;
  }
  else
  {
    v9 = position_4 + blockHeight;
  }
  positionc = v7 / (maxx - minx);
  v11 = maxy - miny;
  this->m_viewportOffset.x = positionc;
  position_4a = v9 / v11;
  this->m_viewportOffset.y = position_4a;
}


// -----------------------------------------------------------------------------
// RVA 0x001C38B0  EA 0x5C38B0
// DIA: CGxString::TexturePageEvicted
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::TexturePageEvicted(CGxString *this, char pageNumber)
{
  if ( ((1 << pageNumber) & this->m_texturePagesUsed) != 0 )
    this->m_textureEvicted = 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001C38E0  EA 0x5C38E0
// DIA: CGxString::GenerateVertexIndices
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::GenerateVertexIndices(CGxString *this)
{
  IGXUTEXTLINE **m_data; // eax
  unsigned int m_count; // ecx
  TEXTLINETEXTURE **v3; // esi
  unsigned int v4; // ebx
  unsigned int v5; // eax
  unsigned int v6; // ebx
  TSFixedArray<unsigned short> *p_m_vertIndices; // edi
  unsigned int v8; // esi
  unsigned int v9; // ecx
  int i; // eax
  unsigned int v11; // ecx
  unsigned int v12; // edx
  unsigned __int16 *v13; // edi
  _WORD *v14; // eax
  bool v15; // zf
  unsigned int v16; // [esp+4h] [ebp-10h]
  unsigned int v17; // [esp+8h] [ebp-Ch]
  IGXUTEXTLINE **textLine; // [esp+Ch] [ebp-8h]
  TEXTLINETEXTURE **textureLine; // [esp+10h] [ebp-4h]

  m_data = this->m_textBlock.m_lines.m_data;
  m_count = this->m_textBlock.m_lines.m_count;
  textLine = m_data;
  if ( m_count != 0 )
  {
    v16 = m_count;
    do
    {
      v3 = (*textLine)->m_texturePages.m_data;
      textureLine = v3;
      if ( (*textLine)->m_texturePages.m_count != 0 )
      {
        v17 = (*textLine)->m_texturePages.m_count;
        while ( 1 )
        {
          v4 = (*v3)->m_vert.m_count;
          if ( (v4 & 3) != 0 )
            SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1917, aNumverts4, 0, 1u);
          v5 = (*v3)->m_vertIndices.m_count;
          v6 = v4 >> 2;
          p_m_vertIndices = &(*v3)->m_vertIndices;
          v8 = 6 * v6;
          if ( 6 * v6 > v5 && v8 > p_m_vertIndices->m_alloc )
          {
            v9 = (unsigned int)p_m_vertIndices[1].__vftable;
            if ( v9 == 0 )
            {
              v9 = 6 * v6;
              if ( v8 >= 0x80 )
              {
                p_m_vertIndices[1].__vftable = (TSFixedArray<unsigned short>_vtbl *)128;
                v9 = 128;
              }
              else
              {
                for ( i = v8 & (v8 - 1); i != 0; i &= i - 1 )
                  v9 = i;
                if ( v9 == 0 )
                  v9 = 1;
              }
            }
            if ( v8 % v9 != 0 )
              v11 = v8 + v9 - v8 % v9;
            else
              v11 = 6 * v6;
            TSFixedArray<unsigned short>::ReallocData(p_m_vertIndices, v11);
          }
          v12 = 0;
          p_m_vertIndices->m_count = v8;
          v13 = p_m_vertIndices->m_data;
          if ( v6 != 0 )
          {
            v14 = v13 + 2;
            do
            {
              *(v14 - 2) = 4 * v12 + baseIndices[0];
              *(v14 - 1) = 4 * v12 + baseIndices[1];
              *v14 = 4 * v12 + baseIndices[2];
              v14[1] = 4 * v12 + baseIndices[3];
              v14[2] = 4 * v12 + baseIndices[4];
              v14[3] = 4 * v12++ + baseIndices[5];
              v14 += 6;
            }
            while ( v12 < v6 );
          }
          v15 = v17 == 1;
          ++textureLine;
          --v17;
          if ( v15 )
            break;
          v3 = textureLine;
        }
      }
      v15 = v16 == 1;
      ++textLine;
      --v16;
    }
    while ( !v15 );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C3A60  EA 0x5C3A60
// DIA: CGxString::GetNewString
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CGxString *__thiscall CGxString::GetNewString(int this)
{
  CGxString *result; // eax
  TSLink<CGxString> *v3; // esi
  int v4; // edx
  int v5; // edx
  int v6; // ebx
  CGxString *v7; // eax
  CGxString *p_m_terminator; // ecx
  TSLink<CGxString> *m_prevlink; // edi
  int m_next; // esi
  int v11; // esi
  TSLink<CGxString> *v12; // edx

  result = g_freeStrings.m_terminator.m_next;
  if ( (int)g_freeStrings.m_terminator.m_next <= 0 )
  {
    v6 = this != 0 ? 2 : 0;
    v7 = (CGxString *)SMemAlloc(0xD4u, aAucgxstring, -2, 8u);
    if ( v7 != nullptr )
      result = CGxString::CGxString(v7);
    else
      result = nullptr;
    if ( v6 != 0 )
    {
      p_m_terminator = result;
      if ( result == nullptr )
        p_m_terminator = (CGxString *)&g_strings.m_terminator;
      m_prevlink = p_m_terminator->m_link.m_prevlink;
      if ( p_m_terminator->m_link.m_prevlink != nullptr )
      {
        m_next = (int)p_m_terminator->m_link.m_next;
        if ( m_next > 0 )
          v11 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
        else
          v11 = ~m_next;
        *(_DWORD *)v11 = m_prevlink;
        p_m_terminator->m_link.m_prevlink->m_next = p_m_terminator->m_link.m_next;
        p_m_terminator->m_link.m_prevlink = nullptr;
        p_m_terminator->m_link.m_next = nullptr;
      }
      v12 = g_strings.m_terminator.m_prevlink;
      p_m_terminator->m_link.m_prevlink = g_strings.m_terminator.m_prevlink;
      p_m_terminator->m_link.m_next = v12->m_next;
      v12->m_next = result;
      g_strings.m_terminator.m_prevlink = &p_m_terminator->m_link;
    }
  }
  else
  {
    v3 = g_freeStrings.m_terminator.m_next->m_link.m_prevlink;
    if ( g_freeStrings.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v4 = (int)g_freeStrings.m_terminator.m_next->m_link.m_next;
      if ( v4 > 0 )
        v5 = (char *)g_freeStrings.m_terminator.m_next - (char *)v3->m_next + v4;
      else
        v5 = ~v4;
      *(_DWORD *)v5 = v3;
      result->m_link.m_prevlink->m_next = result->m_link.m_next;
      result->m_link.m_prevlink = nullptr;
      result->m_link.m_next = nullptr;
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C3BB0  EA 0x5C3BB0
// DIA: CGxString::BuildView
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::BuildView(CGxString *this, NTempest::C44Matrix *viewPtr, float width, float height)
{
  bool v6; // zf
  double X; // st7
  NTempest::C44Matrix view; // [esp+10h] [ebp-4Ch] BYREF
  NTempest::C3Vector move; // [esp+50h] [ebp-Ch] BYREF
  float translateY; // [esp+64h] [ebp+8h]
  float widtha; // [esp+68h] [ebp+Ch]

  if ( viewPtr == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1950, aViewptr, 0, 1u);
  v6 = (this->m_flags & 0x20) == 0;
  view.a0 = 1.0;
  memset(&view.a1, 0, 16);
  view.b1 = 1.0;
  memset(&view.b2, 0, 16);
  view.c2 = 1.0;
  memset(&view.c3, 0, 16);
  view.d3 = 1.0;
  if ( v6 )
  {
    X = width * this->m_viewportOffset.x;
    translateY = height * this->m_viewportOffset.y;
  }
  else
  {
    X = this->m_viewportOffset.x;
    translateY = this->m_viewportOffset.y;
  }
  widtha = _floor(X);
  move.x = widtha;
  move.y = _floor(translateY);
  move.z = 0.0;
  NTempest::C44Matrix::Translate(&view, &move);
  *viewPtr = view;
}


// -----------------------------------------------------------------------------
// RVA 0x001C3CC0  EA 0x5C3CC0
// DIA: CGxString::BuildProjection
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::BuildProjection(
        CGxString *this,
        NTempest::C44Matrix *projPtr,
        float minx,
        float maxx,
        float miny,
        float maxy,
        float pixWidth,
        float pixHeight)
{
  float maxY_4; // [esp+4h] [ebp-54h]
  NTempest::C44Matrix proj; // [esp+18h] [ebp-40h] BYREF
  float projPtra; // [esp+60h] [ebp+8h]
  float minya; // [esp+6Ch] [ebp+14h]
  float pixWidtha; // [esp+74h] [ebp+1Ch]

  if ( projPtr == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 1982, aProjptr, 0, 1u);
  proj.a0 = 1.0;
  memset(&proj.a1, 0, 16);
  proj.b1 = 1.0;
  memset(&proj.b2, 0, 16);
  proj.c2 = 1.0;
  memset(&proj.c3, 0, 16);
  proj.d3 = 1.0;
  projPtra = _floor(miny * pixHeight);
  minya = _floor(maxx * pixWidth);
  pixWidtha = _floor(minx * pixWidth);
  maxY_4 = _floor(maxy * pixHeight);
  GxuXformCreateOrtho(pixWidtha, minya, projPtra, maxY_4, -1.0, 1.0, (NTempest::C44Matrix *)LODWORD(projPtra));
  *projPtr = proj;
}


// -----------------------------------------------------------------------------
// RVA 0x001C3DE0  EA 0x5C3DE0
// DIA: CGxString::SetCharSpacing
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::SetCharSpacing(CGxString *this, float spacing)
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001C3DF0  EA 0x5C3DF0
// DIA: CGxString::Initialize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __userpurge CGxString::Initialize@<eax>(
        CGxString *this@<ecx>,
        double a2@<st0>,
        float fontHeight,
        const NTempest::C3Vector *position,
        float blockWidth,
        float blockHeight,
        CGxFont *face,
        const char *text,
        EGxFontVJusts vertJust,
        EGxFontHJusts horzJust,
        float spacing,
        unsigned int flags,
        const NTempest::CImVector *color)
{
  unsigned int v14; // edi
  char *m_text; // eax
  int m_linkoffset; // eax
  int v17; // edi
  TSLink<CGxString> *v18; // eax
  int m_next; // edx
  int v20; // edx
  TSLink<CGxString> *m_prevlink; // ecx
  double m_requestedFontHeight; // st7

  if ( text == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2020, aText, 0, 1u);
  if ( face == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2021, aFace, 0, 1u);
  v14 = SStrLen(text) + 1;
  if ( v14 > this->m_textLen )
  {
    m_text = this->m_text;
    if ( m_text != nullptr )
      SMemFree(m_text, aDBuildBuildwow_239, 2027, 0);
    this->m_textLen = v14;
    this->m_text = (char *)SMemAlloc(v14, aDBuildBuildwow_239, 2029, 0);
  }
  SStrCopy(this->m_text, text, this->m_textLen);
  this->m_blockWidth = blockWidth;
  this->m_blockHeight = blockHeight;
  this->m_spacing = spacing;
  this->m_position = *position;
  this->m_vertJust = vertJust;
  this->m_horzJust = horzJust;
  this->m_flags = flags;
  this->m_fontColor = *color;
  if ( SLOBYTE(this->m_flags) >= 0 )
    this->m_position.z = 0.0;
  this->m_currentFace = face;
  m_linkoffset = face->m_strings.m_linkoffset;
  v17 = *(int *)((char *)&this->m_link.m_prevlink + m_linkoffset);
  v18 = (TSLink<CGxString> *)((char *)&this->m_link + m_linkoffset);
  if ( v17 != 0 )
  {
    m_next = (int)v18->m_next;
    if ( m_next > 0 )
      v20 = (int)v18 + m_next - *(_DWORD *)(v17 + 4);
    else
      v20 = ~m_next;
    *(_DWORD *)v20 = v17;
    v18->m_prevlink->m_next = v18->m_next;
    v18->m_prevlink = nullptr;
    v18->m_next = nullptr;
  }
  m_prevlink = face->m_strings.m_terminator.m_prevlink;
  v18->m_prevlink = m_prevlink;
  v18->m_next = m_prevlink->m_next;
  m_prevlink->m_next = this;
  face->m_strings.m_terminator.m_prevlink = v18;
  if ( (flags & 4) != 0 && (LOBYTE(v18) = this->m_flags, (char)v18 >= 0) )
  {
    GxuFontGetOneToOneHeight((CGxFont *)v18);
    this->m_requestedFontHeight = a2;
  }
  else
  {
    this->m_requestedFontHeight = fontHeight;
  }
  m_requestedFontHeight = 2.0 / (double)g_heightPixels;
  if ( this->m_requestedFontHeight > m_requestedFontHeight )
    m_requestedFontHeight = this->m_requestedFontHeight;
  this->m_currentFontHeight = m_requestedFontHeight;
  CGxString::CreateGeometry(this);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001C3F90  EA 0x5C3F90
// DIA: CGxString::CGxString
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CGxString *__thiscall CGxString::CGxString(CGxString *this)
{
  TSGrowableArray<IGXUTEXTLINE *> *p_m_lines; // ebx

  this->m_link.m_prevlink = nullptr;
  this->m_link.m_next = nullptr;
  this->m_fontStringLink.m_prevlink = nullptr;
  this->m_fontStringLink.m_next = nullptr;
  this->m_batchedStringLink.m_prevlink = nullptr;
  this->m_batchedStringLink.m_next = nullptr;
  this->m_requestedFontHeight = 0.02;
  this->m_currentFontHeight = 0.02;
  this->m_position.x = 0.0;
  this->m_position.y = 0.0;
  this->m_position.z = 0.0;
  this->m_fontColor = (NTempest::CImVector)-1;
  this->m_shadowColor = (NTempest::CImVector)-16777216;
  this->m_shadowOffset.x = 0.00125;
  this->m_shadowOffset.y = -0.00125;
  p_m_lines = &this->m_textBlock.m_lines;
  this->m_blockWidth = 1.0;
  this->m_blockHeight = 1.0;
  this->m_currentFace = nullptr;
  this->m_textBlock.m_offsetY = 0.0;
  TSFixedArray<IGXUTEXTLINE *>::TSFixedArray<IGXUTEXTLINE *>(&this->m_textBlock.m_lines);
  p_m_lines->m_chunk = 0;
  p_m_lines->__vftable = (TSGrowableArray<IGXUTEXTLINE *>_vtbl *)&TSGrowableArray<IGXUTEXTLINE *>::`vftable';
  this->m_text = nullptr;
  this->m_textLen = 0;
  this->m_vertJust = GxVJ_Top;
  this->m_horzJust = GxHJ_Left;
  this->m_spacing = 0.0;
  this->m_flags = 0;
  this->m_viewportOffset.x = 0.0;
  this->m_viewportOffset.y = 0.0;
  this->m_texturePagesUsed = 0;
  this->m_textureEvicted = 0;
  this->m_stringHeight = 0.0;
  this->m_savedWidth = 0.0;
  this->m_hyperlinkInfo.m_alloc = 0;
  this->m_hyperlinkInfo.m_count = 0;
  this->m_hyperlinkInfo.m_data = nullptr;
  this->m_hyperlinkInfo.m_chunk = 0;
  this->m_hyperlinkInfo.__vftable = (TSGrowableArray<GXUFONTHYPERLINKINFO>_vtbl *)&TSGrowableArray<GXUFONTHYPERLINKINFO>::`vftable';
  this->m_colorGradients.m_alloc = 0;
  this->m_colorGradients.m_count = 0;
  this->m_colorGradients.m_data = nullptr;
  this->m_colorGradients.m_chunk = 0;
  this->m_colorGradients.__vftable = (TSGrowableArray<NTempest::CImVector *>_vtbl *)&TSGrowableArray<NTempest::CImVector *>::`vftable';
  this->m_colorGradientShadows.__vftable = (TSGrowableArray<NTempest::CImVector *>_vtbl *)&TSGrowableArray<NTempest::CImVector *>::`vftable';
  this->m_colorGradientShadows.m_alloc = 0;
  this->m_colorGradientShadows.m_count = 0;
  this->m_colorGradientShadows.m_data = nullptr;
  this->m_colorGradientShadows.m_chunk = 0;
  this->m_lastGradientStart = -1;
  this->m_lastGradientLength = -1;
  this->m_colorGradients.m_chunk = 128;
  this->m_colorGradientShadows.m_chunk = 128;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001C40C0  EA 0x5C40C0
// DIA: CGxString::~CGxString
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::~CGxString(CGxString *this)
{
  char *m_text; // eax
  TSLink<CGxString> *m_prevlink; // ecx
  TSLink<CGxString> *p_m_batchedStringLink; // eax
  int m_next; // edi
  int v6; // edi
  TSLink<CGxString> *v7; // edi
  TSLink<CGxString> *p_m_fontStringLink; // eax
  int v9; // edx
  int v10; // edx
  TSLink<CGxString> *v11; // edx
  int v12; // ecx

  CGxString::ClearStringMatrixEntry(this);
  IGXUTEXTBLOCK::Destroy(&this->m_textBlock);
  m_text = this->m_text;
  if ( m_text != nullptr )
    SMemFree(m_text, aDBuildBuildwow_239, 2098, 0);
  TSFixedArray<GXUFONTHYPERLINKINFO>::~TSFixedArray<GXUFONTHYPERLINKINFO>(&this->m_hyperlinkInfo);
  this->m_hyperlinkInfo.m_alloc = 0;
  this->m_hyperlinkInfo.m_count = 0;
  this->m_hyperlinkInfo.m_data = nullptr;
  TSFixedArray<NTempest::CImVector *>::~TSFixedArray<NTempest::CImVector *>(&this->m_colorGradientShadows);
  TSFixedArray<NTempest::CImVector *>::~TSFixedArray<NTempest::CImVector *>(&this->m_colorGradients);
  TSFixedArray<GXUFONTHYPERLINKINFO>::~TSFixedArray<GXUFONTHYPERLINKINFO>(&this->m_hyperlinkInfo);
  TSFixedArray<IGXUTEXTLINE *>::~TSFixedArray<IGXUTEXTLINE *>(&this->m_textBlock.m_lines);
  this->m_textBlock.m_lines.m_alloc = 0;
  this->m_textBlock.m_lines.m_count = 0;
  this->m_textBlock.m_lines.m_data = nullptr;
  TSFixedArray<IGXUTEXTLINE *>::~TSFixedArray<IGXUTEXTLINE *>(&this->m_textBlock.m_lines);
  m_prevlink = this->m_batchedStringLink.m_prevlink;
  p_m_batchedStringLink = &this->m_batchedStringLink;
  if ( m_prevlink != nullptr )
  {
    m_next = (int)this->m_batchedStringLink.m_next;
    if ( m_next > 0 )
      v6 = (char *)p_m_batchedStringLink - (char *)m_prevlink->m_next + m_next;
    else
      v6 = ~m_next;
    *(_DWORD *)v6 = m_prevlink;
    p_m_batchedStringLink->m_prevlink->m_next = this->m_batchedStringLink.m_next;
    p_m_batchedStringLink->m_prevlink = nullptr;
    this->m_batchedStringLink.m_next = nullptr;
  }
  v7 = this->m_fontStringLink.m_prevlink;
  p_m_fontStringLink = &this->m_fontStringLink;
  if ( v7 != nullptr )
  {
    v9 = (int)this->m_fontStringLink.m_next;
    if ( v9 > 0 )
      v10 = (char *)p_m_fontStringLink - (char *)v7->m_next + v9;
    else
      v10 = ~v9;
    *(_DWORD *)v10 = v7;
    p_m_fontStringLink->m_prevlink->m_next = this->m_fontStringLink.m_next;
    p_m_fontStringLink->m_prevlink = nullptr;
    this->m_fontStringLink.m_next = nullptr;
  }
  v11 = this->m_link.m_prevlink;
  if ( this->m_link.m_prevlink != nullptr )
  {
    v12 = (int)this->m_link.m_next;
    if ( v12 > 0 )
      *(_DWORD *)((char *)this - (char *)v11->m_next + v12) = v11;
    else
      *(_DWORD *)~v12 = v11;
    this->m_link.m_prevlink->m_next = this->m_link.m_next;
    this->m_link.m_prevlink = nullptr;
    this->m_link.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C41D0  EA 0x5C41D0
// DIA: CGxString::RemoveShadow
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::RemoveShadow(CGxString *this)
{
  this->m_flags = (unsigned __int8)this->m_flags & (unsigned __int8)~(unsigned __int8)this->m_flags & 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001C41E0  EA 0x5C41E0
// DIA: CGxFont::RegisterEvictNotice
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxFont::RegisterEvictNotice(CGxFont *this, unsigned int pageNumber)
{
  CGxString *i; // esi

  if ( pageNumber >= 8 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2111, aPagenumber8, 0, 1u);
  for ( i = (int)this->m_strings.m_terminator.m_next <= 0 ? nullptr : this->m_strings.m_terminator.m_next;
        (int)i > 0;
        i = *(CGxString **)((char *)&i->m_link.m_next + this->m_strings.m_linkoffset) )
  {
    CGxString::TexturePageEvicted(i, pageNumber);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C4250  EA 0x5C4250
// DIA: CGxFont::CheckStringGlyphs
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall CGxFont::CheckStringGlyphs(CGxFont *this, const char *string)
{
  const char *v3; // ecx
  char v4; // al
  unsigned int m_slotmask; // ebx
  unsigned int v6; // ebx
  TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *p_m_slotlistarray; // esi
  int v8; // eax
  const char *v9; // eax
  CHARCODEDESC *m_next; // ebx
  unsigned int v11; // edi
  int v12; // eax
  const char *v13; // eax
  int v15; // [esp-18h] [ebp-2Ch]
  int v16; // [esp-18h] [ebp-2Ch]
  int v17; // [esp-14h] [ebp-28h]
  int v18; // [esp-14h] [ebp-28h]
  unsigned int v19; // [esp-10h] [ebp-24h]
  unsigned int v20; // [esp-10h] [ebp-24h]
  const char *v21; // [esp-Ch] [ebp-20h]
  const char *v22; // [esp-Ch] [ebp-20h]
  CGxFont *v23; // [esp+Ch] [ebp-8h]
  int v24; // [esp+10h] [ebp-4h]
  const char *stringa; // [esp+1Ch] [ebp+8h]

  v3 = string;
  v23 = this;
  if ( *string == 0 )
    return 1;
  while ( 1 )
  {
    v4 = *++v3;
    stringa = v3;
    if ( v4 != 10 )
      break;
LABEL_13:
    if ( *v3 == 0 )
      return 1;
  }
  m_slotmask = this->m_activeCharacters.m_slotmask;
  v24 = v4;
  if ( m_slotmask != -1 )
  {
    v6 = v4 & m_slotmask;
    p_m_slotlistarray = &this->m_activeCharacters.m_slotlistarray;
    if ( v6 >= this->m_activeCharacters.m_slotlistarray.m_count )
    {
      v8 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo)(
             &this->m_activeCharacters.m_slotlistarray,
             1,
             1,
             aIndex0x08xArra,
             v6,
             this->m_activeCharacters.m_slotlistarray.m_count);
      v9 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName)(
                           &this->m_activeCharacters.m_slotlistarray,
                           v8);
      SErrDisplayErrorFmt(0x85100080, v9, v15, v17, v19, v21);
    }
    m_next = this->m_activeCharacters.m_slotlistarray.m_data[v6].m_terminator.m_next;
    if ( (int)m_next > 0 )
    {
      while ( m_next->m_hashval != v24 )
      {
        v11 = v24 & this->m_activeCharacters.m_slotmask;
        if ( v11 >= p_m_slotlistarray->m_count )
        {
          v12 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo)(
                  p_m_slotlistarray,
                  1,
                  1,
                  aIndex0x08xArra,
                  v11,
                  p_m_slotlistarray->m_count);
          v13 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName)(
                                p_m_slotlistarray,
                                v12);
          SErrDisplayErrorFmt(0x85100080, v13, v16, v18, v20, v22);
        }
        m_next = *(CHARCODEDESC **)((char *)&m_next->m_linktoslot.m_prevlink
                                  + p_m_slotlistarray->m_data[v11].m_linkoffset);
        if ( (int)m_next <= 0 )
          return 0;
        this = v23;
      }
      v3 = stringa;
      goto LABEL_13;
    }
  }
  return 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001C4350  EA 0x5C4350
// DIA: CGxFont::UpdateDimensions
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
BOOL __thiscall CGxFont::UpdateDimensions(CGxFont *this)
{
  double v2; // st7
  __int16 v3; // fps
  double m_requestedFontHeight; // st6
  bool v5; // c0
  char v6; // c2
  bool v7; // c3
  int v8; // eax
  __int64 v9; // rax
  FT_FaceRec_ *Face; // edi
  __int64 v11; // rax
  unsigned int m_pixelSize; // edx
  unsigned int m_flags; // eax
  int v14; // ebx
  double v15; // st7
  float height_4; // [esp+20h] [ebp-4h]
  float height_4a; // [esp+20h] [ebp-4h]

  HIWORD(v8) = HIWORD(g_heightPixels);
  v2 = 2.0 / (double)g_heightPixels;
  m_requestedFontHeight = this->m_requestedFontHeight;
  v5 = m_requestedFontHeight < v2;
  v6 = 0;
  v7 = m_requestedFontHeight == v2;
  LOWORD(v8) = v3;
  if ( m_requestedFontHeight > v2 )
    v2 = this->m_requestedFontHeight;
  height_4 = v2;
  this->m_currentFontHeight = v2;
  ScreenToPixelHeight(v8, height_4);
  if ( (int)(__int64)v2 <= 28 )
  {
    ScreenToPixelHeight((__int64)v2, this->m_currentFontHeight);
    v9 = (__int64)v2;
  }
  else
  {
    LODWORD(v9) = 28;
  }
  this->m_pixelSize = v9;
  if ( (_DWORD)v9 == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_239, 2134);
    SErrDisplayAppFatal(
      "Error, font %s being created with height %g at screen res of %dx%d which is %d pixels!",
      this->m_fontName,
      this->m_currentFontHeight,
      g_widthPixels,
      g_heightPixels,
      this->m_pixelSize);
  }
  if ( this->m_faceHandle == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2143, aMFacehandle, 0, 1u);
  Face = FontFaceGetFace((int)this->m_faceHandle);
  if ( Face == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2145, aTheface, 0, 1u);
  height_4a = (double)Face->ascender
            / (fabs((double)Face->descender) + (double)Face->ascender)
            * (double)(int)this->m_pixelSize;
  v11 = (__int64)(SignOf(height_4a) * 0.5 + height_4a);
  m_pixelSize = this->m_pixelSize;
  this->m_baseline = v11;
  m_flags = this->m_flags;
  this->m_cellHeight = m_pixelSize;
  if ( (m_flags & 1) != 0 )
    this->m_cellHeight = m_pixelSize + 2;
  if ( (m_flags & 8) != 0 )
    this->m_cellHeight += 2;
  v14 = FT_Set_Pixel_Sizes((unsigned int)Face, m_pixelSize, 0);
  v15 = (double)Face->size->metrics.x_ppem / (double)Face->units_per_EM;
  this->m_pixelsPerUnit = v15;
  if ( v15 == 0.0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2164, aMPixelsperunit, 0, 1u);
  return v14 == 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001C4540  EA 0x5C4540
// DIA: CGxFont::NewCodeDesc
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
const CHARCODEDESC *__thiscall CGxFont::NewCodeDesc(CGxFont *this, unsigned int code)
{
  char *v2; // esi
  unsigned int m_slotmask; // eax
  unsigned int v4; // edi
  TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *p_m_slotlistarray; // ebx
  int v6; // eax
  CHARCODEDESC *v7; // edi
  unsigned int v8; // esi
  int v9; // edi
  unsigned int v10; // edi
  int v11; // eax
  GLYPHBITMAPDATA *m_next; // edi
  unsigned int v13; // esi
  int v14; // ebx
  int v15; // eax
  TSLink<CHARCODEDESC> *v16; // ebx
  TEXTURECACHE *v18; // ecx
  unsigned int v19; // edi
  GLYPHBITMAPDATA *v20; // eax
  GLYPHBITMAPDATA *v21; // edi
  CHARCODEDESC *NewGlyph; // ebx
  int v23; // ecx
  CHARCODEDESC *v24; // eax
  TEXTURECACHE *v25; // ebx
  unsigned int rowNumber; // edi
  TEXTURECACHE *v27; // ebx
  unsigned int m_count; // eax
  TSFixedArray<TEXTURECACHEROW> *p_m_textureRows; // ebx
  int v30; // eax
  const char *v31; // eax
  TEXTURECACHEROW *v32; // ebx
  unsigned int v33; // edx
  unsigned int v34; // edi
  int v35; // edi
  int v36; // eax
  TSLink<CHARCODEDESC> *v37; // edi
  int v38; // [esp-18h] [ebp-34h]
  int v39; // [esp-14h] [ebp-30h]
  unsigned int v40; // [esp-10h] [ebp-2Ch]
  const char *v41; // [esp-Ch] [ebp-28h]
  FT_FaceRec_ *texture; // [esp+Ch] [ebp-10h]
  TEXTURECACHE *texturea; // [esp+Ch] [ebp-10h]
  CHARCODEDESC *oldestDesc; // [esp+10h] [ebp-Ch] BYREF
  unsigned int textureNumber; // [esp+14h] [ebp-8h]
  GLYPHBITMAPDATA *data; // [esp+18h] [ebp-4h]

  v2 = (char *)this;
  m_slotmask = this->m_activeCharacters.m_slotmask;
  data = (GLYPHBITMAPDATA *)this;
  if ( m_slotmask == -1
    || (v4 = code & m_slotmask,
        p_m_slotlistarray = &this->m_activeCharacters.m_slotlistarray,
        TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(
          &this->m_activeCharacters.m_slotlistarray,
          code & m_slotmask),
        v6 = *(_DWORD *)(*((_DWORD *)v2 + 26) + 12 * v4 + 8),
        (int)(v7 = v6 <= 0 ? nullptr : (CHARCODEDESC *)v6) <= 0) )
  {
LABEL_5:
    if ( *((_DWORD *)v2 + 43) == 0 )
      SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2177, aMFacehandle, 0, 1u);
    texture = FontFaceGetFace(*((_DWORD *)v2 + 43));
    if ( texture == nullptr )
      SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2179, aTheface, 0, 1u);
    v9 = *((_DWORD *)v2 + 17);
    if ( v9 != -1 )
    {
      v10 = code & v9;
      textureNumber = (unsigned int)(v2 + 48);
      TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(
        (TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *)v2 + 3,
        v10);
      v11 = *(_DWORD *)(*((_DWORD *)v2 + 15) + 12 * v10 + 8);
      m_next = v11 <= 0 ? nullptr : (GLYPHBITMAPDATA *)v11;
      while ( (int)m_next > 0 )
      {
        if ( m_next->m_hashval == code )
        {
          v18 = (TEXTURECACHE *)(v2 + 468);
          data = m_next;
          v19 = 0;
          textureNumber = (unsigned int)(v2 + 468);
          goto LABEL_27;
        }
        v13 = code & *((_DWORD *)v2 + 17);
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(
          (TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *)textureNumber,
          v13);
        m_next = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(
                   (TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *)(*(_DWORD *)(textureNumber + 12)
                                                                                 + 12 * v13),
                   m_next)->m_next;
        v2 = (char *)data;
      }
    }
    v20 = (GLYPHBITMAPDATA *)SMemAlloc(0x5Cu, aDBuildBuildwow_239, 2185, 0);
    if ( v20 != nullptr )
    {
      v21 = GLYPHBITMAPDATA::GLYPHBITMAPDATA(v20);
      data = v21;
    }
    else
    {
      data = nullptr;
      v21 = nullptr;
    }
    if ( CGxFont::GetGlyphData((CGxFont *)v2, v21, texture, code) == 0 )
    {
      if ( v21 != nullptr )
      {
        GLYPHBITMAPDATA::~GLYPHBITMAPDATA(v21);
        SMemFree(v21, filename, -1, 0);
      }
      return nullptr;
    }
    TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalLinkNode(
      (TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *)(v2 + 28),
      v21,
      code);
    v21->m_hashval = code;
    v21->m_key = s_nullHashKey_1;
    v18 = (TEXTURECACHE *)(v2 + 468);
    v19 = 0;
    textureNumber = (unsigned int)(v2 + 468);
LABEL_27:
    while ( v18->m_texture != nullptr )
    {
      NewGlyph = TEXTURECACHE::AllocateNewGlyph(v18, data);
      if ( NewGlyph != nullptr )
        goto LABEL_33;
      ++v19;
      textureNumber += 36;
      if ( v19 >= 8 )
        goto LABEL_30;
      v18 = (TEXTURECACHE *)textureNumber;
    }
    v25 = (TEXTURECACHE *)&v2[36 * v19 + 468];
    TEXTURECACHE::CreateTexture(v25, *((_DWORD *)v2 + 113) & 4);
    TEXTURECACHE::Initialize(v25, (CGxFont *)v2, v19, *((_DWORD *)v2 + 111));
    NewGlyph = TEXTURECACHE::AllocateNewGlyph(v25, data);
    if ( NewGlyph != nullptr )
    {
LABEL_33:
      NewGlyph->textureNumber = v19;
    }
    else
    {
LABEL_30:
      v23 = *((_DWORD *)v2 + 41);
      v24 = *(CHARCODEDESC **)(*(_DWORD *)v23 + 4);
      if ( (int)v24 <= 0 )
      {
        oldestDesc = nullptr;
        goto finallylabel_15;
      }
      oldestDesc = *(CHARCODEDESC **)(*(_DWORD *)v23 + 4);
      textureNumber = v24->textureNumber;
      if ( textureNumber >= 8 )
      {
        SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2236, aTexturenumberS_0, 0, 1u);
        v24 = oldestDesc;
      }
      rowNumber = v24->rowNumber;
      v27 = (TEXTURECACHE *)&v2[36 * textureNumber + 468];
      texturea = v27;
      if ( rowNumber >= v27->m_textureRows.m_count )
        SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2240, aRownumberTextu, 0, 1u);
      m_count = v27->m_textureRows.m_count;
      p_m_textureRows = &v27->m_textureRows;
      if ( rowNumber >= m_count )
      {
        v30 = ((int (__thiscall *)(TSFixedArray<TEXTURECACHEROW> *, int, int, char *, unsigned int, unsigned int))p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemLineNo)(
                p_m_textureRows,
                1,
                1,
                aIndex0x08xArra,
                rowNumber,
                m_count);
        v31 = (const char *)((int (__thiscall *)(TSFixedArray<TEXTURECACHEROW> *, int))p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemFileName)(
                              p_m_textureRows,
                              v30);
        SErrDisplayErrorFmt(0x85100080, v31, v38, v39, v40, v41);
      }
      v32 = &texturea->m_textureRows.m_data[rowNumber];
      TEXTURECACHEROW::EvictGlyph(v32, &oldestDesc);
      CGxFont::RegisterEvictNotice((CGxFont *)v2, textureNumber);
      NewGlyph = TEXTURECACHEROW::CreateNewDesc(v32, data, rowNumber, *((_DWORD *)v2 + 111));
      if ( NewGlyph == nullptr )
      {
LABEL_53:
        if ( data != nullptr )
        {
          GLYPHBITMAPDATA::~GLYPHBITMAPDATA(data);
          SMemFree(data, filename, -1, 0);
        }
        return NewGlyph;
      }
      v33 = textureNumber;
      NewGlyph->rowNumber = rowNumber;
      NewGlyph->textureNumber = v33;
      texturea->m_anyDirtyGlyphs = 1;
    }
    data = nullptr;
finallylabel_15:
    if ( NewGlyph != nullptr )
    {
      if ( NewGlyph->bitmapData->m_textureValid == 0 )
        SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2269, aThedescValidte, 0, 1u);
      if ( *((_DWORD *)v2 + 28) == -1 )
        TSHashTable<CHARCODEDESC,HASHKEY_NONE>::Initialize((TSHashTable<CHARCODEDESC,HASHKEY_NONE> *)(v2 + 72));
      v34 = code & *((_DWORD *)v2 + 28);
      if ( TSHashTable<CHARCODEDESC,HASHKEY_NONE>::MonitorFullness(
             (TSHashTable<CHARCODEDESC,HASHKEY_NONE> *)(v2 + 72),
             v34) != 0 )
        v34 = code & *((_DWORD *)v2 + 28);
      TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(
        (TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> > *)(v2 + 92),
        v34);
      TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::LinkNode(
        (TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *)(*((_DWORD *)v2 + 26) + 12 * v34),
        NewGlyph,
        2u,
        nullptr);
      TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::LinkNode(
        (TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *)(v2 + 76),
        NewGlyph,
        2u,
        nullptr);
      NewGlyph->m_hashval = code;
      NewGlyph->m_key = s_nullHashKey_1;
      v35 = *((_DWORD *)v2 + 40);
      v36 = *(unsigned int *)((char *)&NewGlyph->m_hashval + v35);
      v37 = (TSLink<CHARCODEDESC> *)((char *)NewGlyph + v35);
      if ( v36 != 0 )
        TSLink<CHARCODEDESC>::Unlink(v37);
      v37->m_prevlink = (TSLink<CHARCODEDESC> *)(v2 + 164);
      v37->m_next = *((CHARCODEDESC **)v2 + 42);
      TSLink<CHARCODEDESC>::NextLink((TSLink<CHARCODEDESC> *)(v2 + 164), *((_DWORD *)v2 + 40))->m_prevlink = v37;
      *((_DWORD *)v2 + 42) = NewGlyph;
    }
    goto LABEL_53;
  }
  while ( v7->m_hashval != code )
  {
    v8 = code & *((_DWORD *)v2 + 28);
    TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v8);
    v7 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(&p_m_slotlistarray->m_data[v8], v7)->m_next;
    v2 = (char *)data;
    if ( (int)v7 <= 0 )
      goto LABEL_5;
  }
  v14 = *((_DWORD *)v2 + 40);
  v15 = *(unsigned int *)((char *)&v7->m_hashval + v14);
  v16 = (TSLink<CHARCODEDESC> *)((char *)v7 + v14);
  if ( v15 != 0 )
    TSLink<CHARCODEDESC>::Unlink(v16);
  v16->m_prevlink = (TSLink<CHARCODEDESC> *)(v2 + 164);
  v16->m_next = *((CHARCODEDESC **)v2 + 42);
  TSLink<CHARCODEDESC>::NextLink((TSLink<CHARCODEDESC> *)(v2 + 164), *((_DWORD *)v2 + 40))->m_prevlink = v16;
  *((_DWORD *)v2 + 42) = v7;
  return v7;
}


// -----------------------------------------------------------------------------
// RVA 0x001C49F0  EA 0x5C49F0
// DIA: CGxFont::GetGlyphData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall CGxFont::GetGlyphData(CGxFont *this, GLYPHBITMAPDATA *glyphData, FT_FaceRec_ *face, unsigned int code)
{
  unsigned int m_pixelSize; // edx
  void *v7; // ecx
  unsigned int freeTypeGlyphWidth; // eax
  unsigned int dataSize; // edx
  unsigned int freeTypeGlyphPitch; // ecx
  unsigned int yOffset; // edx
  unsigned int yStart; // eax
  int freeTypeGlyphAdvance; // ecx
  float freeTypeGlyphBearing; // edx
  unsigned int m_baseline; // [esp-10h] [ebp-40h]
  unsigned int v16; // [esp-8h] [ebp-38h]
  FT_FaceRec_ *v17; // [esp-4h] [ebp-34h]
  GLYPHDATA data; // [esp+Ch] [ebp-24h] BYREF

  if ( face == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2284, aFace, 0, 1u);
  if ( glyphData == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2285, aGlyphdata, 0, 1u);
  if ( FT_Set_Pixel_Sizes((unsigned int)face, this->m_pixelSize, 0) != 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2288, aError_2, 0, 1u);
  v17 = (FT_FaceRec_ *)(this->m_flags & 0x10);
  v16 = (~this->m_flags & 2) != 0;
  m_pixelSize = this->m_pixelSize;
  m_baseline = this->m_baseline;
  data.data = nullptr;
  if ( IGxuFontGlyphRenderGlyph((unsigned int *)face, m_pixelSize, code, m_baseline, &data, v16, v17) != 0 )
  {
    GLYPHBITMAPDATA::Clear(glyphData);
    v7 = data.data;
    freeTypeGlyphWidth = data.freeTypeGlyphWidth;
    dataSize = data.dataSize;
    glyphData->m_code = code;
    glyphData->m_data = v7;
    glyphData->m_dataSize = dataSize;
    glyphData->m_dirty = 1;
    glyphData->m_glyphWidth = freeTypeGlyphWidth;
    glyphData->m_glyphCellWidth = freeTypeGlyphWidth;
    if ( (this->m_flags & 1) != 0 )
      glyphData->m_glyphCellWidth = freeTypeGlyphWidth + 2;
    if ( (this->m_flags & 8) != 0 )
      glyphData->m_glyphCellWidth += 2;
    freeTypeGlyphPitch = data.freeTypeGlyphPitch;
    yOffset = data.yOffset;
    glyphData->m_glyphHeight = data.freeTypeGlyphHeight;
    yStart = data.yStart;
    glyphData->m_glyphPitch = freeTypeGlyphPitch;
    freeTypeGlyphAdvance = data.freeTypeGlyphAdvance;
    glyphData->m_yOffset = yOffset;
    freeTypeGlyphBearing = data.freeTypeGlyphBearing;
    glyphData->m_yStart = yStart;
    glyphData->m_glyphAdvance = freeTypeGlyphAdvance;
    glyphData->m_glyphBearing = freeTypeGlyphBearing;
    return 1;
  }
  else
  {
    if ( data.data != nullptr )
      SMemFree(data.data, aDBuildBuildwow_240, 123, 0);
    return 0;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C4B50  EA 0x5C4B50
// DIA: CGxFont::Initialize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
HFACE__ *__thiscall CGxFont::Initialize(CGxFont *this, char *name, unsigned int newFlags, float fontHeight)
{
  double v6; // st7
  __int64 v7; // rax
  HFACE__ *v8; // eax
  HFACE__ *m_faceHandle; // ecx
  FT_Open_Args_ *FreeTypeLibrary; // eax
  HFACE__ *result; // eax
  int namea; // [esp+2Ch] [ebp+8h]

  if ( name == nullptr || *name == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2338, aNameName, 0, 1u);
  SStrPrintf(this->m_fontName, 0x104u, "%s", name);
  this->m_requestedFontHeight = fontHeight;
  v6 = 2.0 / (double)g_heightPixels;
  if ( fontHeight > v6 )
    v6 = fontHeight;
  *(float *)&namea = v6;
  this->m_currentFontHeight = v6;
  ScreenToPixelHeight(namea, *(float *)&namea);
  if ( (int)(__int64)v6 <= 32 )
  {
    ScreenToPixelHeight((__int64)v6, this->m_currentFontHeight);
    v7 = (__int64)v6;
  }
  else
  {
    LODWORD(v7) = 32;
  }
  this->m_pixelSize = v7;
  if ( (_DWORD)v7 == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_239, 2349);
    SErrDisplayAppFatal(
      "Error, font %s being created with height %g at screen res of %dx%d which is %d pixels!",
      name,
      fontHeight,
      g_widthPixels,
      g_heightPixels,
      this->m_pixelSize);
  }
  CGxFont::Clear(this);
  m_faceHandle = this->m_faceHandle;
  this->m_flags = newFlags;
  if ( m_faceHandle != nullptr )
    FontFaceCloseHandle(v8, (int)m_faceHandle);
  FreeTypeLibrary = (FT_Open_Args_ *)GetFreeTypeLibrary();
  result = (HFACE__ *)FontFaceGetHandle((FT_LibraryRec_ *)name, FreeTypeLibrary);
  this->m_faceHandle = result;
  if ( result != nullptr )
    return (HFACE__ *)CGxFont::UpdateDimensions(this);
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C4CA0  EA 0x5C4CA0
// DIA: CGxFont::GetNumCurrentTextures
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __thiscall CGxFont::GetNumCurrentTextures(CGxFont *this)
{
  int v1; // edi
  CGxTex **p_m_texture; // esi
  int i; // ebx
  unsigned int count; // [esp+Ch] [ebp-4h]

  v1 = 0;
  count = 0;
  p_m_texture = &this->m_textureCache[0].m_texture;
  for ( i = 8; i != 0; --i )
  {
    if ( *p_m_texture != nullptr )
    {
      if ( v1 != 0 )
        SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2398, aEmptyfound, 0, 1u);
      ++count;
    }
    else
    {
      v1 = 1;
    }
    p_m_texture += 9;
  }
  return count;
}


// -----------------------------------------------------------------------------
// RVA 0x001C4D00  EA 0x5C4D00
// DIA: CGxFont::GetCharAdvance
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __thiscall CGxFont::GetCharAdvance(CGxFont *this, unsigned int code)
{
  CGxFont *v2; // edi
  unsigned int m_slotmask; // ebx
  TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *p_m_slotlistarray; // esi
  unsigned int v5; // ebx
  int v6; // eax
  const char *v7; // eax
  GLYPHBITMAPDATA *m_next; // eax
  _DWORD *i; // ebx
  unsigned int v10; // edi
  int v11; // eax
  const char *v12; // eax
  int v13; // eax
  int v15; // [esp-18h] [ebp-28h]
  int v16; // [esp-18h] [ebp-28h]
  int v17; // [esp-14h] [ebp-24h]
  int v18; // [esp-14h] [ebp-24h]
  unsigned int v19; // [esp-10h] [ebp-20h]
  unsigned int v20; // [esp-10h] [ebp-20h]
  const char *v21; // [esp-Ch] [ebp-1Ch]
  const char *v22; // [esp-Ch] [ebp-1Ch]

  v2 = this;
  m_slotmask = this->m_glyphBitmapData.m_slotmask;
  if ( m_slotmask != -1 )
  {
    p_m_slotlistarray = &this->m_glyphBitmapData.m_slotlistarray;
    v5 = code & m_slotmask;
    if ( v5 >= this->m_glyphBitmapData.m_slotlistarray.m_count )
    {
      v6 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo)(
             &this->m_glyphBitmapData.m_slotlistarray,
             1,
             1,
             aIndex0x08xArra,
             v5,
             this->m_glyphBitmapData.m_slotlistarray.m_count);
      v7 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName)(
                           &v2->m_glyphBitmapData.m_slotlistarray,
                           v6);
      SErrDisplayErrorFmt(0x85100080, v7, v15, v17, v19, v21);
    }
    m_next = v2->m_glyphBitmapData.m_slotlistarray.m_data[v5].m_terminator.m_next;
    for ( i = (int)m_next <= 0 ? nullptr : (_DWORD *)m_next;
          (int)i > 0;
          i = *(_DWORD **)((char *)i + p_m_slotlistarray->m_data[v13].m_linkoffset + 4) )
    {
      if ( *i == code )
        return (double)(int)i[13] * v2->m_pixelsPerUnit;
      v10 = code & v2->m_glyphBitmapData.m_slotmask;
      if ( v10 >= p_m_slotlistarray->m_count )
      {
        v11 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo)(
                p_m_slotlistarray,
                1,
                1,
                aIndex0x08xArra,
                v10,
                p_m_slotlistarray->m_count);
        v12 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName)(
                              p_m_slotlistarray,
                              v11);
        SErrDisplayErrorFmt(0x85100080, v12, v16, v18, v20, v22);
      }
      v13 = v10;
      v2 = this;
    }
  }
  SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2410, aGlyph, 0, 1u);
  return (double)MEMORY[0x34] * v2->m_pixelsPerUnit;
}


// -----------------------------------------------------------------------------
// RVA 0x001C4E00  EA 0x5C4E00
// DIA: CGxFont::ComputeStep
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __thiscall CGxFont::ComputeStep(
        CGxFont *this,
        unsigned int currentCode,
        TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *nextCode)
{
  CGxFont *v4; // edi
  unsigned int m_slotmask; // eax
  TSHashTable<KERNNODE,KERNINGHASHKEY> *p_m_kernInfo; // esi
  unsigned int v7; // edi
  KERNNODE *m_next; // eax
  int v9; // edi
  unsigned int v10; // esi
  bool v11; // zf
  double result; // st7
  FT_FaceRec_ *Face; // edi
  unsigned int Char_Index; // eax
  unsigned int v15; // eax
  unsigned int v16; // edi
  GLYPHBITMAPDATA *v17; // eax
  GLYPHBITMAPDATA *v18; // edi
  unsigned int v19; // esi
  int m_glyphAdvance; // ecx
  KERNNODE *v21; // edi
  unsigned int v22; // edi
  FT_Vector_ vector; // [esp+20h] [ebp-18h] BYREF
  unsigned int currentGlyph; // [esp+28h] [ebp-10h] BYREF
  unsigned int v25; // [esp+2Ch] [ebp-Ch]
  KERNNODE *node; // [esp+30h] [ebp-8h]
  CGxFont *v27; // [esp+34h] [ebp-4h]
  TSHashTable<KERNNODE,KERNINGHASHKEY> *spacing; // [esp+40h] [ebp+8h]
  float spacinga; // [esp+40h] [ebp+8h]

  v4 = this;
  m_slotmask = this->m_kernInfo.m_slotmask;
  p_m_kernInfo = &this->m_kernInfo;
  v27 = this;
  v25 = (currentCode << 16) ^ (unsigned __int16)nextCode;
  spacing = &this->m_kernInfo;
  if ( m_slotmask == -1 )
  {
    node = nullptr;
  }
  else
  {
    v7 = currentCode & m_slotmask;
    TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(
      &this->m_kernInfo.m_slotlistarray,
      currentCode & m_slotmask);
    m_next = p_m_kernInfo->m_slotlistarray.m_data[v7].m_terminator.m_next;
    v9 = (int)m_next <= 0 ? 0 : (unsigned int)m_next;
    while ( 1 )
    {
      if ( v9 <= 0 )
      {
        node = nullptr;
        goto LABEL_12;
      }
      if ( *(_DWORD *)v9 == currentCode && ((unsigned int *)(v9 + 20) == &currentGlyph || *(_DWORD *)(v9 + 20) == v25) )
        break;
      v10 = currentCode & p_m_kernInfo->m_slotmask;
      TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(&spacing->m_slotlistarray, v10);
      v9 = *(_DWORD *)(v9 + spacing->m_slotlistarray.m_data[v10].m_linkoffset + 4);
      p_m_kernInfo = spacing;
    }
    v11 = (*(_BYTE *)(v9 + 24) & 2) == 0;
    node = (KERNNODE *)v9;
    if ( !v11 )
      return *(float *)(v9 + 28);
LABEL_12:
    v4 = v27;
  }
  if ( v4->m_faceHandle == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2420, aMFacehandle, 0, 1u);
  Face = FontFaceGetFace((int)v4->m_faceHandle);
  if ( Face == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2422, aTheface, 0, 1u);
  currentGlyph = FT_Get_Char_Index((unsigned int)Face);
  Char_Index = FT_Get_Char_Index((unsigned int)Face);
  vector.x = 0;
  if ( (Face->face_flags & 0x40) != 0 )
  {
    FT_Get_Kerning((unsigned int)Face, Char_Index, 2u, &vector);
    vector.x &= (vector.x >= 0) - 1;
  }
  v15 = v27->m_glyphBitmapData.m_slotmask;
  if ( v15 != -1 )
  {
    v16 = currentCode & v15;
    nextCode = &v27->m_glyphBitmapData.m_slotlistarray;
    TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(
      &v27->m_glyphBitmapData.m_slotlistarray,
      currentCode & v15);
    v17 = nextCode->m_data[v16].m_terminator.m_next;
    v18 = (int)v17 <= 0 ? nullptr : v17;
    while ( (int)v18 > 0 )
    {
      if ( v18->m_hashval == currentCode )
        goto LABEL_25;
      v19 = currentCode & v27->m_glyphBitmapData.m_slotmask;
      TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(
        &v27->m_glyphBitmapData.m_slotlistarray,
        v19);
      v18 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(
              &v27->m_glyphBitmapData.m_slotlistarray.m_data[v19],
              v18)->m_next;
      p_m_kernInfo = spacing;
    }
  }
  v18 = nullptr;
  SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2436, aGlyph, 0, 1u);
LABEL_25:
  m_glyphAdvance = v18->m_glyphAdvance;
  v21 = node;
  nextCode = (TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *)(vector.x + m_glyphAdvance);
  spacinga = (double)(vector.x + m_glyphAdvance) * v27->m_pixelsPerUnit;
  if ( node == nullptr )
  {
    nextCode = (TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *)v25;
    if ( p_m_kernInfo->m_slotmask == -1 )
      TSHashTable<KERNNODE,KERNINGHASHKEY>::Initialize(p_m_kernInfo);
    v22 = currentCode & p_m_kernInfo->m_slotmask;
    if ( TSHashTable<KERNNODE,KERNINGHASHKEY>::MonitorFullness(p_m_kernInfo, v22) != 0 )
      v22 = currentCode & p_m_kernInfo->m_slotmask;
    TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(&p_m_kernInfo->m_slotlistarray, v22);
    v21 = p_m_kernInfo->TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalNew(
            p_m_kernInfo,
            &p_m_kernInfo->m_slotlistarray.m_data[v22],
            0,
            0);
    TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::LinkNode(&p_m_kernInfo->m_fulllist, v21, 2u, nullptr);
    v21->m_hashval = currentCode;
    KERNINGHASHKEY::operator=(&v21->m_key, (const KERNINGHASHKEY *)&nextCode);
    node = v21;
  }
  v21->flags |= 2u;
  result = _ceil(spacinga);
  v21->proporportionalSpacing = result;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C50B0  EA 0x5C50B0
// DIA: KERNINGHASHKEY::operator=
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
KERNINGHASHKEY *__thiscall KERNINGHASHKEY::operator=(KERNINGHASHKEY *this, const KERNINGHASHKEY *rhs)
{
  KERNINGHASHKEY *result; // eax

  result = this;
  if ( this != rhs )
    this->code = rhs->code;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C50D0  EA 0x5C50D0
// DIA: CGxFont::ComputeStepFixedWidth
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __thiscall CGxFont::ComputeStepFixedWidth(CGxFont *this, unsigned int currentCode, unsigned int nextCode)
{
  CGxFont *v3; // esi
  unsigned int m_slotmask; // eax
  unsigned int v5; // edi
  unsigned int v6; // edi
  TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *v7; // ebx
  GLYPHBITMAPDATA *v8; // eax
  GLYPHBITMAPDATA *v9; // edi
  unsigned int v10; // esi
  unsigned int v11; // edi
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *p_m_slotlistarray; // ebx
  KERNNODE *m_next; // eax
  int i; // edi
  unsigned int v15; // esi
  int v16; // ecx
  bool v17; // zf
  unsigned int v19; // edi
  unsigned int v20; // edi
  TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *v21; // ebx
  GLYPHBITMAPDATA *v22; // eax
  GLYPHBITMAPDATA *v23; // edi
  unsigned int v24; // esi
  __int64 v25; // rax
  unsigned int m_cellHeight; // ebx
  unsigned int v27; // ecx
  unsigned int v28; // edi
  __int64 v29; // rax
  unsigned int v30; // ebx
  unsigned int v31; // ebx
  unsigned int v32; // esi
  KERNNODE *v33; // eax
  KERNNODE *v34; // eax
  unsigned int v35; // [esp+10h] [ebp-18h]
  TSHashTable<KERNNODE,KERNINGHASHKEY> *p_m_kernInfo; // [esp+14h] [ebp-14h]
  GLYPHBITMAPDATA *currentGlyph; // [esp+18h] [ebp-10h] BYREF
  GLYPHBITMAPDATA *nextGlyph; // [esp+1Ch] [ebp-Ch]
  KERNNODE *node; // [esp+20h] [ebp-8h]
  CGxFont *v40; // [esp+24h] [ebp-4h]

  v3 = this;
  currentGlyph = (GLYPHBITMAPDATA *)((currentCode << 16) ^ (unsigned __int16)nextCode);
  m_slotmask = this->m_kernInfo.m_slotmask;
  v40 = this;
  v35 = currentCode << 16;
  p_m_kernInfo = &this->m_kernInfo;
  if ( m_slotmask != -1 )
  {
    v11 = currentCode & m_slotmask;
    p_m_slotlistarray = &this->m_kernInfo.m_slotlistarray;
    TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(
      &this->m_kernInfo.m_slotlistarray,
      currentCode & m_slotmask);
    m_next = v3->m_kernInfo.m_slotlistarray.m_data[v11].m_terminator.m_next;
    for ( i = (int)m_next <= 0 ? 0 : (unsigned int)m_next;
          i > 0;
          i = *(_DWORD *)(i + p_m_slotlistarray->m_data[v16].m_linkoffset + 4) )
    {
      if ( *(_DWORD *)i == currentCode
        && ((GLYPHBITMAPDATA **)(i + 20) == &currentGlyph || *(GLYPHBITMAPDATA **)(i + 20) == currentGlyph) )
      {
        v17 = (*(_BYTE *)(i + 24) & 1) == 0;
        node = (KERNNODE *)i;
        if ( v17 )
          goto LABEL_3;
        return *(float *)(i + 32);
      }
      v15 = currentCode & p_m_kernInfo->m_slotmask;
      TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v15);
      v16 = v15;
      v3 = v40;
    }
  }
  node = nullptr;
LABEL_3:
  v5 = v3->m_glyphBitmapData.m_slotmask;
  if ( v5 != -1 )
  {
    v6 = currentCode & v5;
    v7 = &v3->m_glyphBitmapData.m_slotlistarray;
    TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(
      &v3->m_glyphBitmapData.m_slotlistarray,
      v6);
    v8 = v3->m_glyphBitmapData.m_slotlistarray.m_data[v6].m_terminator.m_next;
    v9 = (int)v8 <= 0 ? nullptr : v8;
    while ( (int)v9 > 0 )
    {
      if ( v9->m_hashval == currentCode )
      {
        currentGlyph = v9;
        goto LABEL_18;
      }
      v10 = currentCode & v3->m_glyphBitmapData.m_slotmask;
      TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(v7, v10);
      v9 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(&v7->m_data[v10], v9)->m_next;
      v3 = v40;
    }
  }
  currentGlyph = nullptr;
LABEL_18:
  v19 = v3->m_glyphBitmapData.m_slotmask;
  if ( v19 != -1 )
  {
    v20 = nextCode & v19;
    v21 = &v3->m_glyphBitmapData.m_slotlistarray;
    TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(
      &v3->m_glyphBitmapData.m_slotlistarray,
      v20);
    v22 = v3->m_glyphBitmapData.m_slotlistarray.m_data[v20].m_terminator.m_next;
    v23 = (int)v22 <= 0 ? nullptr : v22;
    if ( (int)v23 > 0 )
    {
      while ( v23->m_hashval != nextCode )
      {
        v24 = nextCode & v40->m_glyphBitmapData.m_slotmask;
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(v21, v24);
        v23 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(&v21->m_data[v24], v23)->m_next;
        if ( (int)v23 <= 0 )
          goto LABEL_22;
      }
      v3 = v40;
      nextGlyph = v23;
      goto LABEL_24;
    }
LABEL_22:
    v3 = v40;
  }
  nextGlyph = nullptr;
LABEL_24:
  if ( currentGlyph != nullptr && nextGlyph != nullptr )
  {
    v25 = (__int64)((double)currentGlyph->m_glyphAdvance * v3->m_pixelsPerUnit);
    m_cellHeight = v3->m_cellHeight;
    if ( (unsigned int)v25 < m_cellHeight )
    {
      v27 = m_cellHeight - v25;
      if ( (((_BYTE)m_cellHeight - (_BYTE)v25) & 1) != 0 )
        --v27;
      v28 = m_cellHeight - (v27 >> 1);
      v29 = (__int64)((double)nextGlyph->m_glyphAdvance * v3->m_pixelsPerUnit);
      if ( (unsigned int)v29 < m_cellHeight )
      {
        v30 = m_cellHeight - v29;
        if ( (v30 & 1) != 0 )
          v31 = (v30 - 1) >> 1;
        else
          v31 = v30 >> 1;
        return (double)(v28 + v31);
      }
      else
      {
        return (double)v28;
      }
    }
    else
    {
      return (double)m_cellHeight;
    }
  }
  else
  {
    if ( node == nullptr )
    {
      v32 = currentCode;
      v33 = TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalNewNode(p_m_kernInfo, currentCode, 0, 0);
      v33->m_hashval = v32;
      if ( &v33->m_key != (KERNINGHASHKEY *)&currentCode )
        v33->m_key.code = v35 ^ (unsigned __int16)nextCode;
      node = v33;
    }
    v34 = node;
    node->flags |= 1u;
    v34->fixedWidthSpacing = 0.0;
    return 0.0;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C53A0  EA 0x5C53A0
// DIA: CGxFont::HandleScreenSizeChange
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __usercall CGxFont::HandleScreenSizeChange(CGxFont *this@<ecx>, double a2@<st0>)
{
  CGxString *i; // esi

  if ( this->m_faceHandle == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2504, aMFacehandle, 0, 1u);
  CGxFont::ClearGlyphs(this);
  if ( !CGxFont::UpdateDimensions(this) )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2511, aSuccess, 0, 1u);
  for ( i = (int)this->m_strings.m_terminator.m_next <= 0 ? nullptr : this->m_strings.m_terminator.m_next;
        (int)i > 0;
        i = *(CGxString **)((char *)&i->m_link.m_next + this->m_strings.m_linkoffset) )
  {
    CGxString::HandleScreenSizeChange(i, a2);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C5430  EA 0x5C5430
// DIA: CGxFont::GetName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
const char *__thiscall CGxFont::GetName(CGxFont *this)
{
  if ( this->m_faceHandle == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2520, aMFacehandle, 0, 1u);
  return FontFaceGetFontName((int)this->m_faceHandle);
}


// -----------------------------------------------------------------------------
// RVA 0x001C5470  EA 0x5C5470
// DIA: CGxFont::Clear
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __usercall CGxFont::Clear(CGxFont *this@<ecx>, HFACE__ *a2@<eax>)
{
  HFACE__ *m_faceHandle; // ecx

  m_faceHandle = this->m_faceHandle;
  if ( m_faceHandle != nullptr )
    FontFaceCloseHandle(a2, (int)m_faceHandle);
  this->m_faceHandle = nullptr;
  CGxFont::ClearGlyphs(this);
}


// -----------------------------------------------------------------------------
// RVA 0x001C54A0  EA 0x5C54A0
// DIA: CGxFont::ClearGlyphs
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxFont::ClearGlyphs(CGxFont *this)
{
  CGxTex **p_m_texture; // esi
  int i; // edi
  int v3; // ebx
  void *v4; // eax
  TSLink<CHARCODEDESC> *v5; // eax
  unsigned int j; // esi
  TSExplicitList<CHARCODEDESC,-572662307> *m_data; // ecx
  CGxFont *v8; // edi
  TSLink<GLYPHBITMAPDATA> *v9; // eax
  unsigned int v10; // esi
  int v11; // edi
  int m_next; // esi
  TSHashTable<KERNNODE,KERNINGHASHKEY> *p_m_kernInfo; // ecx
  TSLink<KERNNODE> *v14; // eax
  unsigned int v15; // esi
  TSBaseArray<TSExplicitList<KERNNODE,-572662307> > *p_m_slotlistarray; // ebx
  int v17; // edi
  int v18; // eax
  TSHashTable<KERNNODE,KERNINGHASHKEY> *v20; // [esp+Ch] [ebp-4h]

  p_m_texture = &this->m_textureCache[0].m_texture;
  for ( i = 8; i != 0; --i )
  {
    v3 = 0;
    if ( *p_m_texture != nullptr )
      GxTexDestroy(*p_m_texture);
    v4 = *(p_m_texture - 1);
    *p_m_texture = nullptr;
    if ( v4 != nullptr )
      SMemFree(v4, aDBuildBuildwow_240, 255, 0);
    *(p_m_texture - 1) = nullptr;
    TSFixedArray<TEXTURECACHEROW>::~TSFixedArray<TEXTURECACHEROW>((TSFixedArray<TEXTURECACHEROW> *)(p_m_texture + 3));
    p_m_texture[4] = nullptr;
    p_m_texture[5] = nullptr;
    p_m_texture[6] = nullptr;
    p_m_texture += 9;
  }
  this->m_activeCharacters.m_fullnessIndicator = 0;
  while ( (int)this->m_activeCharacters.m_fulllist.m_terminator.m_next > 0 )
  {
    v5 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(
           &this->m_activeCharacters.m_fulllist,
           this->m_activeCharacters.m_fulllist.m_terminator.m_next);
    TSLink<CHARCODEDESC>::Unlink(v5);
  }
  for ( j = 0; j < this->m_activeCharacters.m_slotlistarray.m_count; ++v3 )
  {
    while ( 1 )
    {
      TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(
        &this->m_activeCharacters.m_slotlistarray,
        j);
      m_data = this->m_activeCharacters.m_slotlistarray.m_data;
      if ( (int)m_data[v3].m_terminator.m_next <= 0 )
        break;
      this->m_activeCharacters.TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalDelete(
        &this->m_activeCharacters,
        m_data[v3].m_terminator.m_next);
    }
    ++j;
  }
  v8 = this;
  this->m_glyphBitmapData.m_fullnessIndicator = 0;
  while ( (int)this->m_glyphBitmapData.m_fulllist.m_terminator.m_next > 0 )
  {
    v9 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(
           &this->m_glyphBitmapData.m_fulllist,
           this->m_glyphBitmapData.m_fulllist.m_terminator.m_next);
    TSLink<GLYPHBITMAPDATA>::Unlink(v9);
  }
  v10 = 0;
  if ( this->m_glyphBitmapData.m_slotlistarray.m_count != 0 )
  {
    v11 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(
          &this->m_glyphBitmapData.m_slotlistarray,
          v10);
        if ( (int)this->m_glyphBitmapData.m_slotlistarray.m_data[v11].m_terminator.m_next <= 0 )
          break;
        this->m_glyphBitmapData.TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalDelete(
          &this->m_glyphBitmapData,
          this->m_glyphBitmapData.m_slotlistarray.m_data[v11].m_terminator.m_next);
      }
      ++v10;
      ++v11;
    }
    while ( v10 < this->m_glyphBitmapData.m_slotlistarray.m_count );
    v8 = this;
  }
  while ( 1 )
  {
    m_next = (int)v8->m_activeCharacterCache.m_terminator.m_next;
    if ( m_next <= 0 )
      break;
    TSLink<CHARCODEDESC>::Unlink((TSLink<CHARCODEDESC> *)(m_next + 32));
    TSLink<CHARCODEDESC>::Unlink((TSLink<CHARCODEDESC> *)(m_next + 24));
    TSHashObject<CHARCODEDESC,HASHKEY_NONE>::~TSHashObject<CHARCODEDESC,HASHKEY_NONE>((TSHashObject<CHARCODEDESC,HASHKEY_NONE> *)m_next);
    SMemFree((void *)m_next, aAucharcodedesc, -2, 0);
  }
  p_m_kernInfo = &v8->m_kernInfo;
  v20 = &v8->m_kernInfo;
  v8->m_kernInfo.m_fullnessIndicator = 0;
  while ( (int)v8->m_kernInfo.m_fulllist.m_terminator.m_next > 0 )
  {
    v14 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(
            &v8->m_kernInfo.m_fulllist,
            v8->m_kernInfo.m_fulllist.m_terminator.m_next);
    TSLink<KERNNODE>::Unlink(v14);
    p_m_kernInfo = &v8->m_kernInfo;
  }
  v15 = 0;
  if ( p_m_kernInfo->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &p_m_kernInfo->m_slotlistarray;
    v17 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v15);
        v18 = (int)p_m_slotlistarray->m_data[v17].m_terminator.m_next;
        if ( v18 <= 0 )
          break;
        v20->TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalDelete(v20, (KERNNODE *)v18);
      }
      ++v15;
      ++v17;
    }
    while ( v15 < v20->m_slotlistarray.m_count );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C5680  EA 0x5C5680
// DIA: TSHashObject<CHARCODEDESC,HASHKEY_NONE>::~TSHashObject<CHARCODEDESC,HASHKEY_NONE>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashObject<CHARCODEDESC,HASHKEY_NONE>::~TSHashObject<CHARCODEDESC,HASHKEY_NONE>(
        TSHashObject<CHARCODEDESC,HASHKEY_NONE> *this)
{
  TSLink<CHARCODEDESC> *p_m_linktofull; // eax
  TSLink<CHARCODEDESC> *m_prevlink; // edi
  int m_next; // esi
  int v4; // esi
  TSLink<CHARCODEDESC> *v5; // esi
  TSLink<CHARCODEDESC> *p_m_linktoslot; // eax
  int v7; // edx
  int v8; // edx

  p_m_linktofull = &this->m_linktofull;
  m_prevlink = this->m_linktofull.m_prevlink;
  if ( m_prevlink != nullptr )
  {
    m_next = (int)this->m_linktofull.m_next;
    if ( m_next > 0 )
      v4 = (char *)p_m_linktofull - (char *)m_prevlink->m_next + m_next;
    else
      v4 = ~m_next;
    *(_DWORD *)v4 = m_prevlink;
    p_m_linktofull->m_prevlink->m_next = this->m_linktofull.m_next;
    p_m_linktofull->m_prevlink = nullptr;
    this->m_linktofull.m_next = nullptr;
  }
  v5 = this->m_linktoslot.m_prevlink;
  p_m_linktoslot = &this->m_linktoslot;
  if ( v5 != nullptr )
  {
    v7 = (int)this->m_linktoslot.m_next;
    if ( v7 > 0 )
      v8 = (char *)p_m_linktoslot - (char *)v5->m_next + v7;
    else
      v8 = ~v7;
    *(_DWORD *)v8 = v5;
    this->m_linktoslot.m_prevlink->m_next = this->m_linktoslot.m_next;
    p_m_linktoslot->m_prevlink = nullptr;
    this->m_linktoslot.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C56F0  EA 0x5C56F0
// DIA: CGxFont::CGxFont
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CGxFont *__thiscall CGxFont::CGxFont(CGxFont *this)
{
  TSExplicitList<CGxString,8> *p_m_strings; // ebx
  TSLink<CGxString> *p_m_terminator; // edi
  TEXTURECACHE *m_textureCache; // edi
  int i; // ebx

  p_m_strings = &this->m_strings;
  this->m_link.m_prevlink = nullptr;
  this->m_link.m_next = nullptr;
  p_m_terminator = &this->m_strings.m_terminator;
  this->m_strings.m_terminator.m_prevlink = nullptr;
  this->m_strings.m_terminator.m_next = nullptr;
  this->m_strings.m_linkoffset = 0;
  TSList<CGxString,TSGetExplicitLink<CGxString>>::InitializeTerminator(&this->m_strings);
  p_m_terminator->m_prevlink = p_m_terminator;
  p_m_strings->m_terminator.m_next = (CGxString *)~(unsigned int)p_m_terminator;
  p_m_strings->m_linkoffset = 8;
  this->m_batchedRenderLink.m_prevlink = nullptr;
  this->m_batchedRenderLink.m_next = nullptr;
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>(&this->m_glyphBitmapData.m_fulllist);
  this->m_glyphBitmapData.m_fulllist.m_linkoffset = -572662307;
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::InitializeTerminator(&this->m_glyphBitmapData.m_fulllist);
  TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>(&this->m_glyphBitmapData.m_slotlistarray);
  this->m_glyphBitmapData.m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >_vtbl *)&TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::`vftable';
  this->m_glyphBitmapData.m_slotlistarray.m_chunk = 0;
  this->m_glyphBitmapData.__vftable = (TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>_vtbl *)&TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::`vftable';
  this->m_glyphBitmapData.m_fullnessIndicator = 0;
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::ChangeLinkOffset(&this->m_glyphBitmapData.m_fulllist, 12);
  this->m_glyphBitmapData.m_slotmask = -1;
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>(&this->m_activeCharacters.m_fulllist);
  this->m_activeCharacters.m_fulllist.m_linkoffset = -572662307;
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::InitializeTerminator(&this->m_activeCharacters.m_fulllist);
  TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>(&this->m_activeCharacters.m_slotlistarray);
  this->m_activeCharacters.__vftable = (TSHashTable<CHARCODEDESC,HASHKEY_NONE>_vtbl *)&TSHashTable<CHARCODEDESC,HASHKEY_NONE>::`vftable';
  this->m_activeCharacters.m_fullnessIndicator = 0;
  if ( this->m_activeCharacters.m_fulllist.m_linkoffset != 12 )
  {
    TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::UnlinkAll(&this->m_activeCharacters.m_fulllist);
    this->m_activeCharacters.m_fulllist.m_linkoffset = 12;
    TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::InitializeTerminator(&this->m_activeCharacters.m_fulllist);
  }
  this->m_activeCharacters.m_slotmask = -1;
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>(&this->m_kernInfo.m_fulllist);
  this->m_kernInfo.m_fulllist.m_linkoffset = -572662307;
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(&this->m_kernInfo.m_fulllist);
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>(&this->m_kernInfo.m_slotlistarray);
  this->m_kernInfo.__vftable = (TSHashTable<KERNNODE,KERNINGHASHKEY>_vtbl *)&TSHashTable<KERNNODE,KERNINGHASHKEY>::`vftable';
  this->m_kernInfo.m_fullnessIndicator = 0;
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::ChangeLinkOffset(&this->m_kernInfo.m_fulllist, 12);
  this->m_kernInfo.m_slotmask = -1;
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>(&this->m_activeCharacterCache);
  this->m_activeCharacterCache.m_terminator.m_prevlink = &this->m_activeCharacterCache.m_terminator;
  this->m_activeCharacterCache.m_linkoffset = 32;
  this->m_activeCharacterCache.m_terminator.m_next = (CHARCODEDESC *)~(unsigned int)&this->m_activeCharacterCache.m_terminator;
  this->m_cellHeight = 0;
  this->m_pixelsPerUnit = 0.0;
  m_textureCache = this->m_textureCache;
  for ( i = 8; i != 0; --i )
    TEXTURECACHE::TEXTURECACHE(m_textureCache++);
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001C5850  EA 0x5C5850
// DIA: CGxFont::~CGxFont
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __usercall CGxFont::~CGxFont(CGxFont *this@<ecx>, HFACE__ *a2@<eax>)
{
  TEXTURECACHE *v3; // esi
  int i; // ebx
  TSLink<CHARCODEDESC> *v5; // eax
  TSLink<KERNNODE> *v6; // eax
  TSLink<CHARCODEDESC> *v7; // eax
  TSLink<GLYPHBITMAPDATA> *v8; // eax
  TSLink<CGxFont> *p_m_batchedRenderLink; // esi
  TSLink<CGxString> *v10; // eax

  CGxFont::Clear(this, a2);
  v3 = (TEXTURECACHE *)(this + 1);
  for ( i = 8; i != 0; --i )
    TEXTURECACHE::~TEXTURECACHE(--v3);
  while ( (int)this->m_activeCharacterCache.m_terminator.m_next > 0 )
  {
    v5 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(
           &this->m_activeCharacterCache,
           this->m_activeCharacterCache.m_terminator.m_next);
    TSLink<CHARCODEDESC>::Unlink(v5);
  }
  TSLink<CHARCODEDESC>::Unlink(&this->m_activeCharacterCache.m_terminator);
  this->m_kernInfo.__vftable = (TSHashTable<KERNNODE,KERNINGHASHKEY>_vtbl *)&TSHashTable<KERNNODE,KERNINGHASHKEY>::`vftable';
  TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalClear(&this->m_kernInfo, 1);
  TSFixedArray<TSExplicitList<KERNNODE,-572662307>>::~TSFixedArray<TSExplicitList<KERNNODE,-572662307>>(&this->m_kernInfo.m_slotlistarray);
  while ( (int)this->m_kernInfo.m_fulllist.m_terminator.m_next > 0 )
  {
    v6 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(
           &this->m_kernInfo.m_fulllist,
           this->m_kernInfo.m_fulllist.m_terminator.m_next);
    TSLink<KERNNODE>::Unlink(v6);
  }
  TSLink<KERNNODE>::Unlink(&this->m_kernInfo.m_fulllist.m_terminator);
  this->m_activeCharacters.__vftable = (TSHashTable<CHARCODEDESC,HASHKEY_NONE>_vtbl *)&TSHashTable<CHARCODEDESC,HASHKEY_NONE>::`vftable';
  TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalClear(&this->m_activeCharacters, 1);
  TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>::~TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>(&this->m_activeCharacters.m_slotlistarray);
  while ( (int)this->m_activeCharacters.m_fulllist.m_terminator.m_next > 0 )
  {
    v7 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(
           &this->m_activeCharacters.m_fulllist,
           this->m_activeCharacters.m_fulllist.m_terminator.m_next);
    TSLink<CHARCODEDESC>::Unlink(v7);
  }
  TSLink<CHARCODEDESC>::Unlink(&this->m_activeCharacters.m_fulllist.m_terminator);
  this->m_glyphBitmapData.__vftable = (TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>_vtbl *)&TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::`vftable';
  TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalClear(&this->m_glyphBitmapData, 1);
  TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::~TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>(&this->m_glyphBitmapData.m_slotlistarray);
  while ( (int)this->m_glyphBitmapData.m_fulllist.m_terminator.m_next > 0 )
  {
    v8 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(
           &this->m_glyphBitmapData.m_fulllist,
           this->m_glyphBitmapData.m_fulllist.m_terminator.m_next);
    TSLink<GLYPHBITMAPDATA>::Unlink(v8);
  }
  TSLink<GLYPHBITMAPDATA>::Unlink(&this->m_glyphBitmapData.m_fulllist.m_terminator);
  p_m_batchedRenderLink = &this->m_batchedRenderLink;
  if ( this->m_batchedRenderLink.m_prevlink != nullptr )
  {
    TSLink<CGxFont>::NextLink(&this->m_batchedRenderLink, -1)->m_prevlink = p_m_batchedRenderLink->m_prevlink;
    p_m_batchedRenderLink->m_prevlink->m_next = this->m_batchedRenderLink.m_next;
    p_m_batchedRenderLink->m_prevlink = nullptr;
    this->m_batchedRenderLink.m_next = nullptr;
  }
  while ( (int)this->m_strings.m_terminator.m_next > 0 )
  {
    v10 = TSList<CGxString,TSGetExplicitLink<CGxString>>::Link(&this->m_strings, this->m_strings.m_terminator.m_next);
    TSLink<CGxString>::Unlink(v10);
  }
  TSLink<CGxString>::Unlink(&this->m_strings.m_terminator);
  TSLink<CGxFont>::Unlink(&this->m_link);
  if ( this->m_link.m_prevlink != nullptr )
  {
    TSLink<CGxFont>::NextLink(&this->m_link, -1)->m_prevlink = this->m_link.m_prevlink;
    this->m_link.m_prevlink->m_next = this->m_link.m_next;
    this->m_link.m_prevlink = nullptr;
    this->m_link.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C59F0  EA 0x5C59F0
// DIA: CGxFont::UpdateTextures
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxFont::UpdateTextures(CGxFont *this)
{
  TEXTURECACHE *m_textureCache; // esi
  int i; // edi

  m_textureCache = this->m_textureCache;
  for ( i = 8; i != 0; --i )
    TEXTURECACHE::Update(m_textureCache++);
}


// -----------------------------------------------------------------------------
// RVA 0x001C5A10  EA 0x5C5A10
// DIA: TEXTURECACHEROW::EvictGlyph
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHEROW::EvictGlyph(TEXTURECACHEROW *this, CHARCODEDESC **desc)
{
  TEXTURECACHEROW *v3; // edi
  CHARCODEDESC *v4; // eax
  unsigned int v5; // ebx
  unsigned int v6; // edi
  CHARCODEDESC *v7; // ebx
  CHARCODEDESC *v8; // edi
  bool v9; // zf
  CHARCODEDESC *m_next; // eax
  CHARCODEDESC *v11; // eax
  unsigned int v12; // edi
  CHARCODEDESC *v13; // ebx
  CHARCODEDESC *v14; // edi
  CHARCODEDESC *v15; // ecx
  unsigned int v16; // eax
  CHARCODEDESC *i; // esi
  unsigned int Texture; // eax
  unsigned int pixelsNeeded; // [esp+8h] [ebp-8h]
  unsigned int freedPixels; // [esp+18h] [ebp+8h]
  unsigned int freedPixelsa; // [esp+18h] [ebp+8h]

  v3 = this;
  if ( *desc == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2572, aDesc, 0, 1u);
  if ( (*desc)->bitmapData == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2573, aDescBitmapdata, 0, 1u);
  pixelsNeeded = (*desc)->bitmapData->m_glyphCellWidth;
  if ( pixelsNeeded == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2576, aPixelsneeded, 0, 1u);
  v4 = (int)v3->glyphList.m_terminator.m_next <= 0 ? nullptr : v3->glyphList.m_terminator.m_next;
  if ( (int)v4 <= 0 )
  {
LABEL_10:
    SErrPrepareAppFatal(aDBuildBuildwow_239, 2586);
    SErrDisplayAppFatal(nullptr, aErrorCanTLocat_1);
  }
  while ( v4 != *desc )
  {
    v4 = *(CHARCODEDESC **)((char *)&v4->m_linktoslot.m_prevlink + v3->glyphList.m_linkoffset);
    if ( (int)v4 <= 0 )
      goto LABEL_10;
  }
  v5 = 0;
  if ( *desc != nullptr )
  {
    while ( v5 < pixelsNeeded )
    {
      v6 = (*desc)->glyphEndPixel - (*desc)->glyphStartPixel + 1;
      if ( (*desc)->glyphEndPixel - (*desc)->glyphStartPixel == -1 )
        SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2595, aCurrentcellusa, 0, 1u);
      freedPixels = v6 + CHARCODEDESC::GapToNextTexture(*desc) + v5;
      v7 = *desc;
      v8 = (int)(*desc)->textureRowLink.m_next <= 0 ? nullptr : (*desc)->textureRowLink.m_next;
      CHARCODEDESC::~CHARCODEDESC(*desc);
      SMemFree(v7, aAucharcodedesc, -2, 0);
      v5 = freedPixels;
      *desc = v8;
      v9 = v8 == nullptr;
      v3 = this;
      if ( v9 )
        goto LABEL_16;
    }
  }
  else
  {
LABEL_16:
    if ( v5 < pixelsNeeded )
    {
      m_next = v3->glyphList.m_terminator.m_prevlink->m_prevlink->m_next;
      v11 = (int)m_next <= 0 ? nullptr : m_next;
      *desc = v11;
      if ( v11 != nullptr )
      {
        while ( v5 < pixelsNeeded )
        {
          v12 = (*desc)->glyphEndPixel - (*desc)->glyphStartPixel + 1;
          if ( (*desc)->glyphEndPixel - (*desc)->glyphStartPixel == -1 )
            SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2615, aCurrentcellusa, 0, 1u);
          freedPixelsa = v12 + CHARCODEDESC::GapToPreviousTexture(*desc) + v5;
          v13 = *desc;
          v14 = (int)(*desc)->textureRowLink.m_next <= 0 ? nullptr : (*desc)->textureRowLink.m_next;
          CHARCODEDESC::~CHARCODEDESC(*desc);
          SMemFree(v13, aAucharcodedesc, -2, 0);
          v5 = freedPixelsa;
          *desc = v14;
          v9 = v14 == nullptr;
          v3 = this;
          if ( v9 )
            goto LABEL_22;
        }
      }
      else
      {
LABEL_22:
        if ( v5 < pixelsNeeded )
          SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2626, aFreedpixelsPix, 0, 1u);
      }
    }
  }
  v15 = v3->glyphList.m_terminator.m_next;
  if ( (int)v15 <= 0 )
    v16 = 0;
  else
    v16 = CHARCODEDESC::GapToPreviousTexture(v15);
  v3->widestFreeSlot = v16;
  for ( i = (int)v3->glyphList.m_terminator.m_next <= 0 ? nullptr : v3->glyphList.m_terminator.m_next;
        (int)i > 0;
        i = *(CHARCODEDESC **)((char *)&i->m_linktoslot.m_prevlink + v3->glyphList.m_linkoffset) )
  {
    Texture = CHARCODEDESC::GapToNextTexture(i);
    if ( Texture > v3->widestFreeSlot )
      v3->widestFreeSlot = Texture;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C5C70  EA 0x5C5C70
// DIA: TEXTURECACHEROW::CreateNewDesc
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CHARCODEDESC *__thiscall TEXTURECACHEROW::CreateNewDesc(
        TEXTURECACHEROW *this,
        CHARCODEDESC *data,
        unsigned int rowNumber,
        unsigned int glyphCellHeight)
{
  unsigned int v4; // eax
  TEXTURECACHEROW *v5; // esi
  CHARCODEDESC *m_next; // ecx
  CHARCODEDESC *v7; // eax
  CHARCODEDESC *v8; // edi
  TSLink<CHARCODEDESC> *v10; // ecx
  TSLink<CHARCODEDESC> *v11; // eax
  CHARCODEDESC *v12; // edi
  unsigned int v13; // eax
  CHARCODEDESC *v14; // eax
  TSLink<CHARCODEDESC> *v15; // ecx
  TSLink<CHARCODEDESC> *v16; // eax
  TSLink<CHARCODEDESC> *m_prevlink; // ecx
  int v18; // eax
  CHARCODEDESC *v19; // ecx
  unsigned int v20; // eax
  CHARCODEDESC *v21; // ecx
  unsigned int v22; // eax
  int v23; // eax
  CHARCODEDESC *v24; // ecx
  TSLink<CHARCODEDESC> *p_m_terminator; // eax
  CHARCODEDESC *v26; // eax
  unsigned int Texture; // eax
  CHARCODEDESC *v28; // eax
  TSLink<CHARCODEDESC> *v29; // ebx
  TSLink<CHARCODEDESC> *v30; // esi
  int v31; // eax
  int inserted; // [esp+10h] [ebp-10h]
  unsigned int glyphWidth; // [esp+14h] [ebp-Ch]
  CHARCODEDESC *next; // [esp+18h] [ebp-8h]
  CHARCODEDESC *nexta; // [esp+18h] [ebp-8h]
  CHARCODEDESC *nextb; // [esp+18h] [ebp-8h]
  CHARCODEDESC *current; // [esp+1Ch] [ebp-4h]
  CHARCODEDESC *newNode; // [esp+28h] [ebp+8h]

  v4 = data->rowNumber;
  v5 = this;
  glyphWidth = v4;
  if ( v4 != 0 )
  {
    if ( this->widestFreeSlot < v4 )
      return nullptr;
  }
  else
  {
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2647, aGlyphwidth, 0, 1u);
  }
  m_next = v5->glyphList.m_terminator.m_next;
  if ( (int)m_next > 0 )
  {
    current = v5->glyphList.m_terminator.m_next;
    v12 = nullptr;
    inserted = 0;
    v13 = CHARCODEDESC::GapToPreviousTexture(m_next);
    v5->widestFreeSlot = v13;
    if ( v13 < glyphWidth )
    {
      do
      {
        v24 = current;
        if ( current != nullptr )
          p_m_terminator = (TSLink<CHARCODEDESC> *)((char *)current + v5->glyphList.m_linkoffset);
        else
          p_m_terminator = &v5->glyphList.m_terminator;
        v26 = (int)p_m_terminator->m_next <= 0 ? nullptr : p_m_terminator->m_next;
        nextb = v26;
        if ( current->bitmapData->m_textureValid == 0 )
        {
          SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2702, aCurrentValidte, 0, 1u);
          v26 = nextb;
          v24 = current;
        }
        if ( v26 != nullptr && v26->bitmapData->m_textureValid == 0 )
        {
          SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2703, aNextNextValidt, 0, 1u);
          v24 = current;
        }
        Texture = CHARCODEDESC::GapToNextTexture(v24);
        if ( inserted != 0 )
        {
          if ( Texture > v5->widestFreeSlot )
            v5->widestFreeSlot = Texture;
          current = nextb;
        }
        else
        {
          if ( Texture < glyphWidth )
          {
            if ( nextb == nullptr )
              return nullptr;
          }
          else
          {
            v28 = (CHARCODEDESC *)SMemAlloc(0x40u, aDBuildBuildwow_239, 2714, 0);
            if ( v28 != nullptr )
            {
              v28->m_linktoslot.m_prevlink = nullptr;
              v28->m_linktoslot.m_next = nullptr;
              v28->m_linktofull.m_prevlink = nullptr;
              v28->m_linktofull.m_next = nullptr;
              v28->textureRowLink.m_prevlink = nullptr;
              v28->textureRowLink.m_next = nullptr;
              v28->fontGlyphLink.m_prevlink = nullptr;
              v28->fontGlyphLink.m_next = nullptr;
              v28->dataValid = 0;
              v28->textureNumber = -1;
              v28->rowNumber = -1;
              v28->glyphStartPixel = -1;
              v28->glyphEndPixel = 0;
              v28->bitmapData = nullptr;
              v12 = v28;
            }
            else
            {
              v12 = nullptr;
            }
            if ( v12 != nullptr )
              v29 = (TSLink<CHARCODEDESC> *)((char *)v12 + v5->glyphList.m_linkoffset);
            else
              v29 = &v5->glyphList.m_terminator;
            if ( v29->m_prevlink != nullptr )
              TSLink<CHARCODEDESC>::Unlink(v29);
            v30 = (TSLink<CHARCODEDESC> *)((char *)current + v5->glyphList.m_linkoffset);
            v29->m_prevlink = v30;
            v29->m_next = v30->m_next;
            TSLink<CHARCODEDESC>::NextLink(v30, this->glyphList.m_linkoffset)->m_prevlink = v29;
            v30->m_next = v12;
            v31 = current->glyphEndPixel + 1;
            v12->glyphEndPixel = current->glyphEndPixel + glyphWidth;
            v12->glyphStartPixel = v31;
            v12->bitmapData = (GLYPHBITMAPDATA *)data;
            CHARCODEDESC::GenerateTextureCoords(v12, rowNumber, glyphCellHeight);
            v5 = this;
            v12->dataValid = 1;
            nextb = v12;
            inserted = 1;
          }
          current = nextb;
        }
      }
      while ( nextb != nullptr );
    }
    else
    {
      v14 = (CHARCODEDESC *)SMemAlloc(0x40u, aDBuildBuildwow_239, 2677, 0);
      if ( v14 != nullptr )
      {
        v14->m_linktoslot.m_prevlink = nullptr;
        v14->m_linktoslot.m_next = nullptr;
        v14->m_linktofull.m_prevlink = nullptr;
        v14->m_linktofull.m_next = nullptr;
        v14->textureRowLink.m_prevlink = nullptr;
        v14->textureRowLink.m_next = nullptr;
        v14->fontGlyphLink.m_prevlink = nullptr;
        v14->fontGlyphLink.m_next = nullptr;
        v14->dataValid = 0;
        v14->textureNumber = -1;
        v14->rowNumber = -1;
        v14->glyphStartPixel = -1;
        v14->glyphEndPixel = 0;
        v14->bitmapData = nullptr;
        v12 = v14;
      }
      else
      {
        v12 = nullptr;
      }
      if ( v12 != nullptr )
      {
        v15 = (TSLink<CHARCODEDESC> *)((char *)v12 + v5->glyphList.m_linkoffset);
        nexta = (CHARCODEDESC *)v15;
      }
      else
      {
        nexta = (CHARCODEDESC *)&v5->glyphList.m_terminator;
        v15 = &v5->glyphList.m_terminator;
      }
      if ( v15->m_prevlink != nullptr )
        TSLink<CHARCODEDESC>::Unlink(v15);
      v16 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(&v5->glyphList, current);
      m_prevlink = v16->m_prevlink;
      nexta->m_hashval = (unsigned int)v16->m_prevlink;
      nexta->m_linktoslot.m_prevlink = (TSLink<CHARCODEDESC> *)m_prevlink->m_next;
      m_prevlink->m_next = v12;
      v16->m_prevlink = (TSLink<CHARCODEDESC> *)nexta;
      if ( current->glyphStartPixel == 0 )
        SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2679, aCurrentGlyphst, 0, 1u);
      v18 = current->glyphStartPixel - 1;
      v12->glyphEndPixel = v18;
      v12->glyphStartPixel = v18 - glyphWidth + 1;
      v12->bitmapData = (GLYPHBITMAPDATA *)data;
      CHARCODEDESC::GenerateTextureCoords(v12, rowNumber, glyphCellHeight);
      v12->dataValid = 1;
      v19 = v5->glyphList.m_terminator.m_next;
      if ( (int)v19 <= 0 )
        v20 = 0;
      else
        v20 = CHARCODEDESC::GapToPreviousTexture(v19);
      v5->widestFreeSlot = v20;
      v21 = (int)v5->glyphList.m_terminator.m_next <= 0 ? nullptr : v5->glyphList.m_terminator.m_next;
      newNode = v21;
      if ( (int)v21 > 0 )
      {
        while ( 1 )
        {
          v22 = CHARCODEDESC::GapToNextTexture(v21);
          if ( v22 > v5->widestFreeSlot )
            v5->widestFreeSlot = v22;
          v23 = newNode != nullptr ? (int)newNode + v5->glyphList.m_linkoffset : (int)&v5->glyphList.m_terminator;
          newNode = *(CHARCODEDESC **)(v23 + 4);
          if ( (int)newNode <= 0 )
            break;
          v21 = *(CHARCODEDESC **)(v23 + 4);
        }
        return v12;
      }
    }
    return v12;
  }
  v7 = (CHARCODEDESC *)SMemAlloc(0x40u, aDBuildBuildwow_239, 2659, 0);
  if ( v7 != nullptr )
  {
    v7->m_linktoslot.m_prevlink = nullptr;
    v7->m_linktoslot.m_next = nullptr;
    v7->m_linktofull.m_prevlink = nullptr;
    v7->m_linktofull.m_next = nullptr;
    v7->textureRowLink.m_prevlink = nullptr;
    v7->textureRowLink.m_next = nullptr;
    v7->fontGlyphLink.m_prevlink = nullptr;
    v7->fontGlyphLink.m_next = nullptr;
    v7->dataValid = 0;
    v7->textureNumber = -1;
    v7->rowNumber = -1;
    v7->glyphStartPixel = -1;
    v7->glyphEndPixel = 0;
    v7->bitmapData = nullptr;
    v8 = v7;
  }
  else
  {
    v8 = nullptr;
  }
  if ( v8 != nullptr )
  {
    v10 = (TSLink<CHARCODEDESC> *)((char *)v8 + v5->glyphList.m_linkoffset);
    next = (CHARCODEDESC *)v10;
  }
  else
  {
    v10 = &v5->glyphList.m_terminator;
    next = (CHARCODEDESC *)&v5->glyphList.m_terminator;
  }
  if ( v10->m_prevlink != nullptr )
  {
    TSLink<CHARCODEDESC>::Unlink(v10);
    v10 = (TSLink<CHARCODEDESC> *)next;
  }
  v11 = v5->glyphList.m_terminator.m_prevlink;
  v10->m_prevlink = v11;
  v10->m_next = v11->m_next;
  v11->m_next = v8;
  v5->glyphList.m_terminator.m_prevlink = v10;
  v5->widestFreeSlot -= glyphWidth;
  v8->glyphEndPixel = glyphWidth - 1;
  v8->glyphStartPixel = 0;
  v8->bitmapData = (GLYPHBITMAPDATA *)data;
  CHARCODEDESC::GenerateTextureCoords(v8, rowNumber, glyphCellHeight);
  v8->dataValid = 1;
  return v8;
}


// -----------------------------------------------------------------------------
// RVA 0x001C6060  EA 0x5C6060
// DIA: TEXTURECACHE::TEXTURECACHE
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
TEXTURECACHE *__thiscall TEXTURECACHE::TEXTURECACHE(TEXTURECACHE *this)
{
  this->m_anyDirtyGlyphs = 1;
  this->m_data = nullptr;
  this->m_texture = nullptr;
  this->m_theFace = nullptr;
  this->m_page = 0;
  this->m_textureRows.__vftable = (TSFixedArray<TEXTURECACHEROW>_vtbl *)&TSFixedArray<TEXTURECACHEROW>::`vftable';
  this->m_textureRows.m_alloc = 0;
  this->m_textureRows.m_count = 0;
  this->m_textureRows.m_data = nullptr;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001C6090  EA 0x5C6090
// DIA: TEXTURECACHE::Initialize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::Initialize(
        TEXTURECACHE *this,
        CGxFont *face,
        unsigned int thePage,
        unsigned int pixelSize)
{
  unsigned int v5; // edi
  TSFixedArray<TEXTURECACHEROW> *p_m_textureRows; // esi
  unsigned int v7; // ebx
  unsigned int m_count; // eax
  TEXTURECACHEROW *v9; // edi
  TSFixedArray<TEXTURECACHEROW>_vtbl *v10; // edx
  const char *v11; // eax
  TEXTURECACHEROW *v12; // eax
  const char *v13; // eax
  TEXTURECACHEROW *v14; // edi
  unsigned int v15; // eax
  TEXTURECACHEROW *v16; // ecx
  bool v17; // zf
  const char *v18; // eax
  unsigned int v19; // eax
  int v20; // edi
  TEXTURECACHEROW *v21; // eax
  int v22; // eax
  const char *v23; // eax
  int v24; // [esp-18h] [ebp-24h]
  int v25; // [esp-14h] [ebp-20h]
  unsigned int v26; // [esp-10h] [ebp-1Ch]
  const char *v27; // [esp-Ch] [ebp-18h]
  int v28; // [esp-8h] [ebp-14h]
  int v29; // [esp-8h] [ebp-14h]
  int v30; // [esp-8h] [ebp-14h]
  CGxFont *facea; // [esp+14h] [ebp+8h]
  CGxFont *faceb; // [esp+14h] [ebp+8h]
  CGxFont *facec; // [esp+14h] [ebp+8h]
  CGxFont *faced; // [esp+14h] [ebp+8h]
  TEXTURECACHEROW *thePagea; // [esp+18h] [ebp+Ch]
  unsigned int pixelSizea; // [esp+1Ch] [ebp+10h]

  v5 = 0;
  this->m_theFace = face;
  this->m_page = thePage;
  if ( pixelSize == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2753, aPixelsize, 0, 1u);
  p_m_textureRows = &this->m_textureRows;
  v7 = 0x100 / pixelSize;
  m_count = p_m_textureRows->m_count;
  if ( 0x100 / pixelSize != m_count )
  {
    if ( v7 != 0 )
    {
      thePagea = p_m_textureRows->m_data;
      facea = (CGxFont *)(0x100 / pixelSize);
      if ( v7 < m_count )
      {
        v9 = &p_m_textureRows->m_data[v7];
        do
        {
          TEXTURECACHEROW::`scalar deleting destructor'(v9++, 0);
          facea = (CGxFont *)((char *)facea + 1);
        }
        while ( (unsigned int)facea < p_m_textureRows->m_count );
      }
      v10 = p_m_textureRows->__vftable;
      p_m_textureRows->m_alloc = v7;
      v28 = v10->TSBaseArray<TEXTURECACHEROW>::MemLineNo(p_m_textureRows);
      v11 = (const char *)p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemFileName(p_m_textureRows);
      v12 = (TEXTURECACHEROW *)SMemReAlloc(thePagea, 16 * v7, v11, v28, 0x10u);
      p_m_textureRows->m_data = v12;
      if ( v12 == nullptr )
      {
        v29 = p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemLineNo(p_m_textureRows);
        v13 = (const char *)p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemFileName(p_m_textureRows);
        v14 = thePagea;
        p_m_textureRows->m_data = (TEXTURECACHEROW *)SMemAlloc(16 * v7, v13, v29, 0);
        if ( thePagea != nullptr )
        {
          v15 = p_m_textureRows->m_count;
          if ( v7 < v15 )
            v15 = 0x100 / pixelSize;
          if ( v15 != 0 )
          {
            faceb = nullptr;
            pixelSizea = v15;
            do
            {
              v16 = (TEXTURECACHEROW *)((char *)faceb + (unsigned int)p_m_textureRows->m_data);
              if ( v16 != nullptr )
                TEXTURECACHEROW::TEXTURECACHEROW(v16, v14);
              TEXTURECACHEROW::`scalar deleting destructor'(v14++, 0);
              v17 = pixelSizea == 1;
              faceb = (CGxFont *)((char *)faceb + 16);
              --pixelSizea;
            }
            while ( !v17 );
          }
          v30 = p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemLineNo(p_m_textureRows);
          v18 = (const char *)p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemFileName(p_m_textureRows);
          SMemFree(thePagea, v18, v30, 0);
        }
      }
      v19 = p_m_textureRows->m_count;
      if ( v19 < v7 )
      {
        v20 = v19;
        facec = (CGxFont *)(v7 - v19);
        do
        {
          v21 = &p_m_textureRows->m_data[v20];
          if ( v21 != nullptr )
          {
            v21->widestFreeSlot = 0;
            TSExplicitList<CHARCODEDESC,24>::TSExplicitList<CHARCODEDESC,24>(&v21->glyphList);
          }
          ++v20;
          facec = (CGxFont *)((char *)facec - 1);
        }
        while ( facec != nullptr );
      }
      p_m_textureRows->m_count = v7;
      v5 = 0;
    }
    else
    {
      TSFixedArray<TEXTURECACHEROW>::~TSFixedArray<TEXTURECACHEROW>(p_m_textureRows);
      p_m_textureRows->m_alloc = 0;
      p_m_textureRows->m_count = 0;
      p_m_textureRows->m_data = nullptr;
    }
  }
  if ( v7 != 0 )
  {
    faced = nullptr;
    do
    {
      if ( v5 >= p_m_textureRows->m_count )
      {
        v22 = ((int (__thiscall *)(TSFixedArray<TEXTURECACHEROW> *, int, int, char *, unsigned int, unsigned int))p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemLineNo)(
                p_m_textureRows,
                1,
                1,
                aIndex0x08xArra,
                v5,
                p_m_textureRows->m_count);
        v23 = (const char *)((int (__thiscall *)(TSFixedArray<TEXTURECACHEROW> *, int))p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemFileName)(
                              p_m_textureRows,
                              v22);
        SErrDisplayErrorFmt(0x85100080, v23, v24, v25, v26, v27);
      }
      *(TSLink<CGxFont> **)((char *)&faced->m_link.m_prevlink + (unsigned int)p_m_textureRows->m_data) = (TSLink<CGxFont> *)256;
      ++v5;
      faced = (CGxFont *)((char *)faced + 16);
    }
    while ( v5 < v7 );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C6270  EA 0x5C6270
// DIA: TEXTURECACHE::CreateTexture
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::CreateTexture(TEXTURECACHE *this, int filter)
{
  CGxTex **p_m_texture; // edi
  CGxTexFlags *v4; // eax

  p_m_texture = &this->m_texture;
  if ( this->m_texture != nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2765, aMTexture_0, 0, 1u);
  v4 = CGxTexFlags::CGxTexFlags((CGxTexFlags *)&filter, (EGxTexFilter)(filter != 0), 0, 0, 0, 0, 0, 1u);
  if ( GxTexCreate(
         0,
         0x100u,
         0x100u,
         0,
         GxTex_Argb4444,
         GxTex_Argb8888,
         *v4,
         this,
         (void (__fastcall *)(EGxTexCommand, unsigned int, unsigned int, unsigned int, unsigned int, void *, unsigned int *, const void **))TEXTURECACHE::TextureCallback,
         p_m_texture) == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2778, aSuccess, 0, 1u);
}


// -----------------------------------------------------------------------------
// RVA 0x001C6310  EA 0x5C6310
// DIA: TEXTURECACHE::~TEXTURECACHE
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::~TEXTURECACHE(TEXTURECACHE *this)
{
  CGxTex *m_texture; // ecx
  unsigned int v3; // edi
  void *m_data; // eax
  bool v5; // zf
  int v6; // ebx
  TEXTURECACHEROW *v7; // eax
  TEXTURECACHEROW *v8; // eax
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *p_glyphList; // ebx
  TSLink<CHARCODEDESC> *p_m_terminator; // edi
  TSLink<CHARCODEDESC> *v11; // eax
  bool v12; // cf
  TEXTURECACHEROW *v13; // esi
  int v14; // [esp+8h] [ebp-8h]
  int v15; // [esp+Ch] [ebp-4h]

  m_texture = this->m_texture;
  v3 = 0;
  if ( m_texture != nullptr )
    GxTexDestroy(m_texture);
  m_data = this->m_data;
  this->m_texture = nullptr;
  if ( m_data != nullptr )
    SMemFree(m_data, aDBuildBuildwow_240, 255, 0);
  this->m_data = nullptr;
  v5 = this->m_textureRows.m_count == 0;
  this->m_textureRows.__vftable = (TSFixedArray<TEXTURECACHEROW>_vtbl *)&TSFixedArray<TEXTURECACHEROW>::`vftable';
  if ( !v5 )
  {
    v6 = 0;
    do
    {
      TEXTURECACHEROW::`scalar deleting destructor'(&this->m_textureRows.m_data[v6], 0);
      ++v3;
      ++v6;
    }
    while ( v3 < this->m_textureRows.m_count );
  }
  v7 = this->m_textureRows.m_data;
  if ( v7 != nullptr )
    SMemFree(v7, aAutexturecache, -2, 0);
  this->m_textureRows.m_alloc = 0;
  this->m_textureRows.m_count = 0;
  this->m_textureRows.m_data = nullptr;
  v5 = this->m_textureRows.m_count == 0;
  this->m_textureRows.__vftable = (TSFixedArray<TEXTURECACHEROW>_vtbl *)&TSFixedArray<TEXTURECACHEROW>::`vftable';
  v14 = 0;
  if ( !v5 )
  {
    v15 = 0;
    do
    {
      v8 = this->m_textureRows.m_data;
      p_glyphList = &v8[v15].glyphList;
      p_m_terminator = &v8[v15].glyphList.m_terminator;
      while ( (int)p_m_terminator->m_next > 0 )
      {
        v11 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(p_glyphList, p_m_terminator->m_next);
        TSLink<CHARCODEDESC>::Unlink(v11);
      }
      TSLink<CHARCODEDESC>::Unlink(p_m_terminator);
      v12 = ++v14 < this->m_textureRows.m_count;
      ++v15;
    }
    while ( v12 );
  }
  v13 = this->m_textureRows.m_data;
  if ( v13 != nullptr )
    SMemFree(v13, aAutexturecache, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C6410  EA 0x5C6410
// DIA: TEXTURECACHE::Update
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::Update(TEXTURECACHE *this)
{
  NTempest::CiRect *m_texture; // ecx

  m_texture = (NTempest::CiRect *)this->m_texture;
  if ( m_texture != nullptr && this->m_anyDirtyGlyphs != 0 )
    GxTexUpdate(m_texture, 0, (CGxTex *)0x100, 256, 1);
}


// -----------------------------------------------------------------------------
// RVA 0x001C6440  EA 0x5C6440
// DIA: TEXTURECACHE::TextureCallback
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall TEXTURECACHE::TextureCallback(
        EGxTexCommand w,
        unsigned int a2,
        unsigned int h,
        unsigned int d,
        unsigned int mipLevel,
        TEXTURECACHE *userArg,
        unsigned int *texelStrideInBytes,
        const void **texels)
{
  if ( a2 == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2804, aW, 0, 1u);
  if ( h == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2805, asc_85D08C, 0, 1u);
  if ( userArg == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2807, aUserarg, 0, 1u);
  TEXTURECACHE::TextureCallbackHandler(userArg, w, a2, h, mipLevel, texelStrideInBytes, texels);
}


// -----------------------------------------------------------------------------
// RVA 0x001C64E0  EA 0x5C64E0
// DIA: TEXTURECACHE::TextureCallbackHandler
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTURECACHE::TextureCallbackHandler(
        TEXTURECACHE *this,
        unsigned int cmd,
        unsigned int w,
        unsigned int h,
        unsigned int mipLevel,
        unsigned int *texelStrideInBytes,
        void **texels)
{
  TEXTURECACHE *v7; // esi
  unsigned int v8; // edi
  void *v9; // eax
  CGxFont *m_theFace; // eax
  signed int m_cellHeight; // edi
  unsigned int v12; // ebx
  _DWORD *v13; // esi
  int v14; // eax
  const char *v15; // eax
  _DWORD *i; // edi
  int v17; // eax
  GLYPHBITMAPDATA *v18; // ebx
  int v19; // eax
  const char *v20; // eax
  int v21; // [esp-18h] [ebp-2Ch]
  int v22; // [esp-18h] [ebp-2Ch]
  int v23; // [esp-14h] [ebp-28h]
  int v24; // [esp-14h] [ebp-28h]
  unsigned int v25; // [esp-10h] [ebp-24h]
  unsigned int v26; // [esp-10h] [ebp-24h]
  const char *v27; // [esp-Ch] [ebp-20h]
  const char *v28; // [esp-Ch] [ebp-20h]
  unsigned int count; // [esp+Ch] [ebp-8h]
  unsigned int row; // [esp+1Ch] [ebp+8h]
  unsigned int wa; // [esp+20h] [ebp+Ch]
  unsigned int ha; // [esp+24h] [ebp+10h]
  signed int glyphHeight; // [esp+2Ch] [ebp+18h]
  const void **texelsa; // [esp+30h] [ebp+1Ch]

  v7 = this;
  if ( w == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2829, aW, 0, 1u);
  if ( h == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2830, asc_85D08C, 0, 1u);
  if ( cmd != 2 )
  {
    if ( v7->m_data == nullptr )
    {
      v8 = 4 * h * w;
      v9 = SMemAlloc(v8, aDBuildBuildwow_239, 2836, 0);
      v7->m_data = v9;
      memset(v9, 0, v8);
    }
    *texelStrideInBytes = 1024;
    *texels = v7->m_data;
    m_theFace = v7->m_theFace;
    if ( m_theFace != nullptr && v7->m_anyDirtyGlyphs != 0 )
    {
      m_cellHeight = m_theFace->m_cellHeight;
      glyphHeight = m_cellHeight;
      if ( m_cellHeight == 0 )
        SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2850, aGlyphheight, 0, 1u);
      v12 = 0;
      count = v7->m_textureRows.m_count;
      row = 0;
      if ( count != 0 )
      {
        wa = 0;
        ha = 0;
        v13 = &v7->m_textureRows.__vftable;
        texelsa = (const void **)(m_cellHeight << 8);
        do
        {
          if ( v12 >= v13[2] )
          {
            v14 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, _DWORD))(*v13 + 4))(
                    v13,
                    1,
                    1,
                    aIndex0x08xArra,
                    v12,
                    v13[2]);
            v15 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v13)(v13, v14);
            SErrDisplayErrorFmt(0x85100080, v15, v21, v23, v25, v27);
          }
          for ( i = *(_DWORD **)(v13[3] + wa + 12);
                (int)i > 0;
                i = *(_DWORD **)((char *)i + *(_DWORD *)(v13[3] + wa + 4) + 4) )
          {
            v17 = i[15];
            if ( *(_DWORD *)(v17 + 36) != 0 )
            {
              *(_DWORD *)(v17 + 36) = 0;
              if ( i[15] == 0 )
                SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2861, aCurrBitmapdata, 0, 1u);
              if ( *(_DWORD *)(i[15] + 28) == 0 )
                SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2862, aCurrBitmapdata_0, 0, 1u);
              if ( *(_DWORD *)(i[15] + 88) == 0 )
                SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2863, aCurrValidtextu, 0, 1u);
              if ( i[13] > i[14] )
                SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2864, aCurrValidblock, 0, 1u);
              v18 = (GLYPHBITMAPDATA *)i[15];
              if ( v18->m_yStart > glyphHeight )
                SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2867, aGlyphdataMYsta, 0, 1u);
              TEXTURECACHE::PasteGlyph(
                this,
                v18,
                (unsigned int *)this->m_data + ha + i[13],
                this->m_theFace->m_flags & 8);
              v12 = row;
            }
            if ( v12 >= v13[2] )
            {
              v19 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, _DWORD))(*v13 + 4))(
                      v13,
                      1,
                      1,
                      aIndex0x08xArra,
                      v12,
                      v13[2]);
              v20 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v13)(v13, v19);
              SErrDisplayErrorFmt(0x85100080, v20, v22, v24, v26, v28);
            }
          }
          row = ++v12;
          wa += 16;
          ha += (unsigned int)texelsa;
        }
        while ( v12 < count );
        v7 = this;
      }
      v7->m_anyDirtyGlyphs = 0;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C67B0  EA 0x5C67B0
// DIA: TEXTURECACHE::AllocateNewGlyph
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CHARCODEDESC *__thiscall TEXTURECACHE::AllocateNewGlyph(TEXTURECACHE *this, GLYPHBITMAPDATA *data)
{
  unsigned int v3; // edi
  TSFixedArray<TEXTURECACHEROW> *p_m_textureRows; // esi
  int i; // ebx
  int v6; // eax
  const char *v7; // eax
  CHARCODEDESC *result; // eax
  int v9; // [esp-18h] [ebp-2Ch]
  int v10; // [esp-14h] [ebp-28h]
  unsigned int v11; // [esp-10h] [ebp-24h]
  const char *v12; // [esp-Ch] [ebp-20h]
  unsigned int count; // [esp+Ch] [ebp-8h]

  if ( data == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2923, aData_0, 0, 1u);
  if ( this->m_theFace == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 2924, aMTheface, 0, 1u);
  v3 = 0;
  count = this->m_textureRows.m_count;
  if ( count == 0 )
    return nullptr;
  p_m_textureRows = &this->m_textureRows;
  for ( i = 0; ; ++i )
  {
    if ( v3 >= p_m_textureRows->m_count )
    {
      v6 = ((int (__thiscall *)(TSFixedArray<TEXTURECACHEROW> *, int, int, char *, unsigned int, unsigned int))p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemLineNo)(
             p_m_textureRows,
             1,
             1,
             aIndex0x08xArra,
             v3,
             p_m_textureRows->m_count);
      v7 = (const char *)((int (__thiscall *)(TSFixedArray<TEXTURECACHEROW> *, int))p_m_textureRows->TSBaseArray<TEXTURECACHEROW>::MemFileName)(
                           p_m_textureRows,
                           v6);
      SErrDisplayErrorFmt(0x85100080, v7, v9, v10, v11, v12);
    }
    result = TEXTURECACHEROW::CreateNewDesc(
               &p_m_textureRows->m_data[i],
               (CHARCODEDESC *)data,
               v3,
               this->m_theFace->m_cellHeight);
    if ( result != nullptr )
      break;
    if ( ++v3 >= count )
      return nullptr;
  }
  this->m_anyDirtyGlyphs = 1;
  result->rowNumber = v3;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C6890  EA 0x5C6890
// DIA: IGXUTEXTBLOCK::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall IGXUTEXTBLOCK::Destroy(IGXUTEXTBLOCK *this)
{
  unsigned int v1; // ebx
  TSGrowableArray<IGXUTEXTLINE *> *p_m_lines; // esi
  int v3; // eax
  const char *v4; // eax
  int v5; // eax
  const char *v6; // eax
  IGXUTEXTLINE **m_data; // ecx
  TSLink<IGXUTEXTLINE> *p_m_link; // edi
  IGXUTEXTLINE *m_next; // eax
  int v10; // [esp-20h] [ebp-28h]
  int v11; // [esp-20h] [ebp-28h]
  int v12; // [esp-1Ch] [ebp-24h]
  int v13; // [esp-1Ch] [ebp-24h]
  unsigned int v14; // [esp-18h] [ebp-20h]
  unsigned int v15; // [esp-18h] [ebp-20h]
  const char *v16; // [esp-14h] [ebp-1Ch]
  const char *v17; // [esp-14h] [ebp-1Ch]

  v1 = 0;
  if ( this->m_lines.m_count != 0 )
  {
    p_m_lines = &this->m_lines;
    do
    {
      if ( v1 >= p_m_lines->m_count )
      {
        v3 = ((int (__thiscall *)(TSGrowableArray<IGXUTEXTLINE *> *, int, int, char *, unsigned int, unsigned int))p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemLineNo)(
               p_m_lines,
               1,
               1,
               aIndex0x08xArra,
               v1,
               p_m_lines->m_count);
        v4 = (const char *)((int (__thiscall *)(TSGrowableArray<IGXUTEXTLINE *> *, int))p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemFileName)(
                             p_m_lines,
                             v3);
        SErrDisplayErrorFmt(0x85100080, v4, v10, v12, v14, v16);
      }
      if ( p_m_lines->m_data[v1] != nullptr )
      {
        if ( v1 >= p_m_lines->m_count )
        {
          v5 = ((int (__thiscall *)(TSGrowableArray<IGXUTEXTLINE *> *, int, int, char *, unsigned int, unsigned int))p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemLineNo)(
                 p_m_lines,
                 1,
                 1,
                 aIndex0x08xArra,
                 v1,
                 p_m_lines->m_count);
          v6 = (const char *)((int (__thiscall *)(TSGrowableArray<IGXUTEXTLINE *> *, int))p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemFileName)(
                               p_m_lines,
                               v5);
          SErrDisplayErrorFmt(0x85100080, v6, v11, v13, v15, v17);
        }
        m_data = p_m_lines->m_data;
        p_m_link = &m_data[v1]->m_link;
        if ( p_m_link != nullptr )
        {
          IGXUTEXTLINE::Destroy(m_data[v1]);
          m_next = p_m_link[2].m_next;
          p_m_link[1].m_prevlink = (TSLink<IGXUTEXTLINE> *)&TSFixedArray<TEXTLINETEXTURE *>::`vftable';
          if ( m_next != nullptr )
            SMemFree(m_next, aPautextlinetex, -2, 0);
          if ( p_m_link->m_prevlink != nullptr )
          {
            TSLink<IGXUTEXTLINE>::NextLink(p_m_link, -1)->m_prevlink = p_m_link->m_prevlink;
            p_m_link->m_prevlink->m_next = p_m_link->m_next;
            p_m_link->m_prevlink = nullptr;
            p_m_link->m_next = nullptr;
          }
          SMemFree(p_m_link, filename, -1, 0);
        }
      }
      ++v1;
    }
    while ( v1 < this->m_lines.m_count );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C69A0  EA 0x5C69A0
// DIA: IGXUTEXTBLOCK::Recycle
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall IGXUTEXTBLOCK::Recycle(IGXUTEXTBLOCK *this)
{
  unsigned int v2; // edi
  TSGrowableArray<IGXUTEXTLINE *> *p_m_lines; // esi
  int v4; // eax
  const char *v5; // eax
  int v6; // [esp-1Ch] [ebp-24h]
  int v7; // [esp-18h] [ebp-20h]
  unsigned int v8; // [esp-14h] [ebp-1Ch]
  const char *v9; // [esp-10h] [ebp-18h]

  v2 = 0;
  if ( this->m_lines.m_count != 0 )
  {
    p_m_lines = &this->m_lines;
    do
    {
      if ( v2 >= this->m_lines.m_count )
      {
        v4 = ((int (__thiscall *)(TSGrowableArray<IGXUTEXTLINE *> *, int, int, char *, unsigned int, unsigned int))p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemLineNo)(
               &this->m_lines,
               1,
               1,
               aIndex0x08xArra,
               v2,
               this->m_lines.m_count);
        v5 = (const char *)((int (__thiscall *)(TSGrowableArray<IGXUTEXTLINE *> *, int))p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemFileName)(
                             &this->m_lines,
                             v4);
        SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
      }
      IGXUTEXTLINE::Recycle(this->m_lines.m_data[v2++]);
    }
    while ( v2 < this->m_lines.m_count );
  }
  this->m_lines.m_count = 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001C6A00  EA 0x5C6A00
// DIA: IGXUTEXTBLOCK::NewLine
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
IGXUTEXTLINE *__thiscall IGXUTEXTBLOCK::NewLine(IGXUTEXTBLOCK *this)
{
  TSGrowableArray<IGXUTEXTLINE *> *p_m_lines; // esi
  unsigned int m_count; // edi
  unsigned int m_chunk; // ecx
  int i; // eax
  TSGrowableArray<IGXUTEXTLINE *>_vtbl *v5; // edx
  const char *v6; // eax
  IGXUTEXTLINE **v7; // eax
  const char *v8; // eax
  unsigned int j; // edx
  IGXUTEXTLINE **v10; // eax
  const char *v11; // eax
  unsigned int v12; // eax
  IGXUTEXTLINE **v13; // edi
  IGXUTEXTLINE *result; // eax
  int v15; // [esp-8h] [ebp-18h]
  int v16; // [esp-8h] [ebp-18h]
  int v17; // [esp-8h] [ebp-18h]
  IGXUTEXTLINE **ptr; // [esp+Ch] [ebp-4h]

  p_m_lines = &this->m_lines;
  m_count = this->m_lines.m_count + 1;
  if ( m_count > this->m_lines.m_alloc )
  {
    m_chunk = this->m_lines.m_chunk;
    if ( m_chunk == 0 )
    {
      m_chunk = m_count;
      if ( m_count >= 0x40 )
      {
        p_m_lines->m_chunk = 64;
        m_chunk = 64;
      }
      else
      {
        for ( i = m_count & (m_count - 1); i != 0; i &= i - 1 )
          m_chunk = i;
        if ( m_chunk == 0 )
          m_chunk = 1;
      }
    }
    if ( m_count % m_chunk != 0 )
      m_count += m_chunk - m_count % m_chunk;
    v5 = p_m_lines->__vftable;
    ptr = p_m_lines->m_data;
    p_m_lines->m_alloc = m_count;
    v15 = v5->TSBaseArray<IGXUTEXTLINE *>::MemLineNo(p_m_lines);
    v6 = (const char *)p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemFileName(p_m_lines);
    v7 = (IGXUTEXTLINE **)SMemReAlloc(ptr, 4 * m_count, v6, v15, 0x10u);
    p_m_lines->m_data = v7;
    if ( v7 == nullptr )
    {
      v16 = p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemLineNo(p_m_lines);
      v8 = (const char *)p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemFileName(p_m_lines);
      p_m_lines->m_data = (IGXUTEXTLINE **)SMemAlloc(4 * m_count, v8, v16, 0);
      if ( ptr != nullptr )
      {
        if ( m_count >= p_m_lines->m_count )
          m_count = p_m_lines->m_count;
        for ( j = 0; j < m_count; ++j )
        {
          v10 = &p_m_lines->m_data[j];
          if ( v10 != nullptr )
            *v10 = ptr[j];
        }
        v17 = p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemLineNo(p_m_lines);
        v11 = (const char *)p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemFileName(p_m_lines);
        SMemFree(ptr, v11, v17, 0);
      }
    }
  }
  v12 = p_m_lines->m_count;
  v13 = &p_m_lines->m_data[v12];
  p_m_lines->m_count = v12 + 1;
  result = IGXUTEXTLINE::NewGxuTextLine();
  *v13 = result;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C6B10  EA 0x5C6B10
// DIA: IGXUTEXTLINE::NewGxuTextLine
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
IGXUTEXTLINE *__fastcall IGXUTEXTLINE::NewGxuTextLine()
{
  IGXUTEXTLINE *result; // eax
  TSLink<IGXUTEXTLINE> *m_prevlink; // esi
  int m_next; // edx
  int v3; // edx

  result = g_freeTextLines.m_terminator.m_next;
  if ( (int)g_freeTextLines.m_terminator.m_next <= 0 )
  {
    result = (IGXUTEXTLINE *)SMemAlloc(0x1Cu, aDBuildBuildwow_239, 2977, 0);
    if ( result != nullptr )
    {
      result->m_link.m_prevlink = nullptr;
      result->m_link.m_next = nullptr;
      result->m_texturePages.m_alloc = 0;
      result->m_texturePages.m_count = 0;
      result->m_texturePages.m_data = nullptr;
      result->m_texturePages.m_chunk = 0;
      result->m_texturePages.__vftable = (TSGrowableArray<TEXTLINETEXTURE *>_vtbl *)&TSGrowableArray<TEXTLINETEXTURE *>::`vftable';
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    m_prevlink = g_freeTextLines.m_terminator.m_next->m_link.m_prevlink;
    if ( g_freeTextLines.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      m_next = (int)g_freeTextLines.m_terminator.m_next->m_link.m_next;
      if ( m_next > 0 )
        v3 = (char *)g_freeTextLines.m_terminator.m_next - (char *)m_prevlink->m_next + m_next;
      else
        v3 = ~m_next;
      *(_DWORD *)v3 = m_prevlink;
      result->m_link.m_prevlink->m_next = result->m_link.m_next;
      result->m_link.m_prevlink = nullptr;
      result->m_link.m_next = nullptr;
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C6B80  EA 0x5C6B80
// DIA: IGXUTEXTLINE::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall IGXUTEXTLINE::Destroy(IGXUTEXTLINE *this)
{
  unsigned int v1; // edi
  TSGrowableArray<TEXTLINETEXTURE *> *p_m_texturePages; // esi
  int v3; // eax
  const char *v4; // eax
  int v5; // eax
  const char *v6; // eax
  TEXTLINETEXTURE **m_data; // ecx
  TEXTLINETEXTURE *v8; // ebx
  int v9; // [esp-20h] [ebp-28h]
  int v10; // [esp-20h] [ebp-28h]
  int v11; // [esp-1Ch] [ebp-24h]
  int v12; // [esp-1Ch] [ebp-24h]
  unsigned int v13; // [esp-18h] [ebp-20h]
  unsigned int v14; // [esp-18h] [ebp-20h]
  const char *v15; // [esp-14h] [ebp-1Ch]
  const char *v16; // [esp-14h] [ebp-1Ch]

  v1 = 0;
  if ( this->m_texturePages.m_count != 0 )
  {
    p_m_texturePages = &this->m_texturePages;
    do
    {
      if ( v1 >= p_m_texturePages->m_count )
      {
        v3 = ((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int, int, char *, unsigned int, unsigned int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo)(
               p_m_texturePages,
               1,
               1,
               aIndex0x08xArra,
               v1,
               p_m_texturePages->m_count);
        v4 = (const char *)((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemFileName)(
                             p_m_texturePages,
                             v3);
        SErrDisplayErrorFmt(0x85100080, v4, v9, v11, v13, v15);
      }
      if ( p_m_texturePages->m_data[v1] != nullptr )
      {
        if ( v1 >= p_m_texturePages->m_count )
        {
          v5 = ((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int, int, char *, unsigned int, unsigned int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo)(
                 p_m_texturePages,
                 1,
                 1,
                 aIndex0x08xArra,
                 v1,
                 p_m_texturePages->m_count);
          v6 = (const char *)((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemFileName)(
                               p_m_texturePages,
                               v5);
          SErrDisplayErrorFmt(0x85100080, v6, v10, v12, v14, v16);
        }
        m_data = p_m_texturePages->m_data;
        v8 = m_data[v1];
        if ( v8 != nullptr )
        {
          TEXTLINETEXTURE::~TEXTLINETEXTURE(m_data[v1]);
          SMemFree(v8, filename, -1, 0);
        }
      }
      ++v1;
    }
    while ( v1 < this->m_texturePages.m_count );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C6C40  EA 0x5C6C40
// DIA: IGXUTEXTLINE::Recycle
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall IGXUTEXTLINE::Recycle(IGXUTEXTLINE *this)
{
  unsigned int v2; // edi
  TSGrowableArray<TEXTLINETEXTURE *> *p_m_texturePages; // esi
  int v4; // eax
  const char *v5; // eax
  TSLink<IGXUTEXTLINE> *m_prevlink; // eax
  int v7; // [esp-1Ch] [ebp-24h]
  int v8; // [esp-18h] [ebp-20h]
  unsigned int v9; // [esp-14h] [ebp-1Ch]
  const char *v10; // [esp-10h] [ebp-18h]

  v2 = 0;
  if ( this->m_texturePages.m_count != 0 )
  {
    p_m_texturePages = &this->m_texturePages;
    do
    {
      if ( v2 >= this->m_texturePages.m_count )
      {
        v4 = ((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int, int, char *, unsigned int, unsigned int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo)(
               &this->m_texturePages,
               1,
               1,
               aIndex0x08xArra,
               v2,
               this->m_texturePages.m_count);
        v5 = (const char *)((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemFileName)(
                             &this->m_texturePages,
                             v4);
        SErrDisplayErrorFmt(0x85100080, v5, v7, v8, v9, v10);
      }
      TEXTLINETEXTURE::Recycle(this->m_texturePages.m_data[v2++]);
    }
    while ( v2 < this->m_texturePages.m_count );
  }
  this->m_texturePages.m_count = 0;
  if ( this->m_link.m_prevlink != nullptr )
  {
    TSLink<IGXUTEXTLINE>::NextLink(&this->m_link, -1)->m_prevlink = this->m_link.m_prevlink;
    this->m_link.m_prevlink->m_next = this->m_link.m_next;
    this->m_link.m_prevlink = nullptr;
    this->m_link.m_next = nullptr;
  }
  m_prevlink = g_freeTextLines.m_terminator.m_prevlink;
  this->m_link.m_prevlink = g_freeTextLines.m_terminator.m_prevlink;
  this->m_link.m_next = m_prevlink->m_next;
  m_prevlink->m_next = this;
  g_freeTextLines.m_terminator.m_prevlink = &this->m_link;
}


// -----------------------------------------------------------------------------
// RVA 0x001C6CE0  EA 0x5C6CE0
// DIA: IGXUTEXTLINE::Reserve
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall IGXUTEXTLINE::Reserve(IGXUTEXTLINE *this, unsigned int numTextLineTextures)
{
  unsigned int v3; // edi
  TSGrowableArray<TEXTLINETEXTURE *> *p_m_texturePages; // esi
  int v5; // eax
  const char *v6; // eax
  unsigned int m_count; // eax
  TSGrowableArray<TEXTLINETEXTURE *> *v8; // esi
  unsigned int m_chunk; // ecx
  unsigned int i; // eax
  unsigned int v12; // edi
  unsigned int v13; // edi
  int v14; // eax
  const char *v15; // eax
  int v16; // [esp-18h] [ebp-24h]
  int v17; // [esp-18h] [ebp-24h]
  int v18; // [esp-14h] [ebp-20h]
  int v19; // [esp-14h] [ebp-20h]
  unsigned int v20; // [esp-10h] [ebp-1Ch]
  unsigned int v21; // [esp-10h] [ebp-1Ch]
  const char *v22; // [esp-Ch] [ebp-18h]
  const char *v23; // [esp-Ch] [ebp-18h]
  TEXTLINETEXTURE **numTextLineTexturesa; // [esp+14h] [ebp+8h]

  v3 = 0;
  if ( this->m_texturePages.m_count != 0 )
  {
    p_m_texturePages = &this->m_texturePages;
    do
    {
      if ( v3 >= this->m_texturePages.m_count )
      {
        v5 = ((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int, int, char *, unsigned int, unsigned int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo)(
               &this->m_texturePages,
               1,
               1,
               aIndex0x08xArra,
               v3,
               this->m_texturePages.m_count);
        v6 = (const char *)((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int))p_m_texturePages->TSBaseArray<TEXTLINETEXTURE *>::MemFileName)(
                             &this->m_texturePages,
                             v5);
        SErrDisplayErrorFmt(0x85100080, v6, v16, v18, v20, v22);
      }
      TEXTLINETEXTURE::Recycle(this->m_texturePages.m_data[v3++]);
    }
    while ( v3 < this->m_texturePages.m_count );
  }
  m_count = this->m_texturePages.m_count;
  v8 = &this->m_texturePages;
  if ( numTextLineTextures > m_count && numTextLineTextures > v8->m_alloc )
  {
    m_chunk = v8->m_chunk;
    if ( m_chunk == 0 )
    {
      m_chunk = numTextLineTextures;
      if ( numTextLineTextures >= 0x40 )
      {
        v8->m_chunk = 64;
        m_chunk = 64;
      }
      else
      {
        for ( i = numTextLineTextures & (numTextLineTextures - 1); i != 0; i &= i - 1 )
          m_chunk = i;
        if ( m_chunk == 0 )
          m_chunk = 1;
      }
    }
    v12 = numTextLineTextures;
    if ( numTextLineTextures % m_chunk != 0 )
      v12 = numTextLineTextures + m_chunk - numTextLineTextures % m_chunk;
    TSFixedArray<TEXTLINETEXTURE *>::ReallocData(v8, v12);
  }
  v13 = 0;
  for ( v8->m_count = numTextLineTextures;
        v13 < numTextLineTextures;
        *numTextLineTexturesa = TEXTLINETEXTURE::NewTextLineTexture() )
  {
    if ( v13 >= v8->m_count )
    {
      v14 = ((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int, int, char *, unsigned int, unsigned int))v8->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo)(
              v8,
              1,
              1,
              aIndex0x08xArra,
              v13,
              v8->m_count);
      v15 = (const char *)((int (__thiscall *)(TSGrowableArray<TEXTLINETEXTURE *> *, int))v8->TSBaseArray<TEXTLINETEXTURE *>::MemFileName)(
                            v8,
                            v14);
      SErrDisplayErrorFmt(0x85100080, v15, v17, v19, v21, v23);
    }
    numTextLineTexturesa = &v8->m_data[v13++];
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C6E00  EA 0x5C6E00
// DIA: TEXTLINETEXTURE::Recycle
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTLINETEXTURE::Recycle(TEXTLINETEXTURE *this)
{
  unsigned int v2; // edi
  int v3; // ebx
  unsigned int v4; // edi

  if ( this->m_vert.m_count != 0 )
  {
    v2 = 0;
    v3 = 0;
    do
    {
      VERT::`scalar deleting destructor'(&this->m_vert.m_data[v3], 0);
      ++v2;
      ++v3;
    }
    while ( v2 < this->m_vert.m_count );
  }
  v4 = 0;
  for ( this->m_vert.m_count = 0; v4 < this->m_colors.m_count; ++v4 )
    NTempest::CImVector::`scalar deleting destructor'(&this->m_colors.m_data[v4], 0);
  this->m_colors.m_count = 0;
  this->m_vertIndices.m_count = 0;
  TSList<TEXTLINETEXTURE,TSGetLink<TEXTLINETEXTURE>>::LinkNode(&g_freeTextLineTextures, this, 2u, nullptr);
}


// -----------------------------------------------------------------------------
// RVA 0x001C6E60  EA 0x5C6E60
// DIA: TEXTLINETEXTURE::~TEXTLINETEXTURE
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall TEXTLINETEXTURE::~TEXTLINETEXTURE(TEXTLINETEXTURE *this)
{
  TEXTLINETEXTURE *v1; // esi
  TSGrowableArray_<VERT,1199076678,58> *p_m_vert; // edi
  _DWORD *v3; // ebx
  void *m_alloc; // esi
  void *v5; // ebx
  TEXTLINETEXTURE *v6; // ebx
  NTempest::CImVector *m_data; // eax
  unsigned int m_count; // eax
  unsigned int v9; // esi
  int v10; // ebx
  VERT *v11; // edi

  v1 = this;
  p_m_vert = &this->m_vert;
  TSFixedArray<VERT>::~TSFixedArray<VERT>(&this->m_vert);
  v3 = &v1->m_colors.__vftable;
  p_m_vert->m_alloc = 0;
  p_m_vert->m_count = 0;
  p_m_vert->m_data = nullptr;
  TSFixedArray<NTempest::CImVector>::~TSFixedArray<NTempest::CImVector>(&v1->m_colors);
  v1 = (TEXTLINETEXTURE *)((char *)v1 + 68);
  v3[1] = 0;
  v3[2] = 0;
  v3[3] = 0;
  TSFixedArray<unsigned short>::~TSFixedArray<unsigned short>((TSFixedArray<unsigned short> *)v1);
  v1->m_link.m_next = nullptr;
  v1->m_vert.__vftable = nullptr;
  v1->m_vert.m_alloc = 0;
  v1->m_link.m_prevlink = (TSLink<TEXTLINETEXTURE> *)&TSFixedArray<unsigned short>::`vftable';
  m_alloc = (void *)v1->m_vert.m_alloc;
  if ( m_alloc != nullptr )
    SMemFree(m_alloc, aG_5, -2, 0);
  *v3 = &TSFixedArray<NTempest::CImVector>::`vftable';
  v5 = (void *)v3[3];
  if ( v5 != nullptr )
    SMemFree(v5, aAvcimvectorNte, -2, 0);
  v6 = this;
  m_data = this->m_shadowColors.m_data;
  this->m_shadowColors.__vftable = (TSGrowableArray_<NTempest::CImVector,1199076678,59>_vtbl *)&TSFixedArray<NTempest::CImVector>::`vftable';
  if ( m_data != nullptr )
    SMemFree(m_data, aAvcimvectorNte, -2, 0);
  m_count = p_m_vert->m_count;
  v9 = 0;
  p_m_vert->__vftable = (TSGrowableArray_<VERT,1199076678,58>_vtbl *)&TSFixedArray<VERT>::`vftable';
  if ( m_count != 0 )
  {
    v10 = 0;
    do
    {
      VERT::~VERT(&p_m_vert->m_data[v10]);
      ++v9;
      ++v10;
    }
    while ( v9 < p_m_vert->m_count );
    v6 = this;
  }
  v11 = p_m_vert->m_data;
  if ( v11 != nullptr )
    SMemFree(v11, aAuvert, -2, 0);
  if ( v6->m_link.m_prevlink != nullptr )
  {
    TSLink<TEXTLINETEXTURE>::NextLink(&v6->m_link, -1)->m_prevlink = v6->m_link.m_prevlink;
    v6->m_link.m_prevlink->m_next = v6->m_link.m_next;
    v6->m_link.m_prevlink = nullptr;
    v6->m_link.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C6F70  EA 0x5C6F70
// DIA: VERT::~VERT
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __thiscall VERT::~VERT(VERT *this)
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001C6F80  EA 0x5C6F80
// DIA: TEXTLINETEXTURE::NewTextLineTexture
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
TEXTLINETEXTURE *__fastcall TEXTLINETEXTURE::NewTextLineTexture()
{
  TEXTLINETEXTURE *result; // eax
  TSLink<TEXTLINETEXTURE> *m_prevlink; // esi
  int m_next; // edx
  char *v3; // eax
  char *v4; // esi
  _DWORD *v5; // edi

  result = g_freeTextLineTextures.m_terminator.m_next;
  if ( (int)g_freeTextLineTextures.m_terminator.m_next <= 0 )
  {
    v3 = (char *)SMemAlloc(0x58u, aDBuildBuildwow_239, 3037, 0);
    v4 = v3;
    if ( v3 != nullptr )
    {
      v5 = v3 + 8;
      *(_DWORD *)v3 = 0;
      *((_DWORD *)v3 + 1) = 0;
      TSFixedArray<VERT>::TSFixedArray<VERT>((TSFixedArray<VERT> *)(v3 + 8));
      *((_DWORD *)v4 + 6) = 0;
      *v5 = &TSGrowableArray_<VERT,1199076678,58>::`vftable';
      TSFixedArray<NTempest::CImVector>::TSFixedArray<NTempest::CImVector>((TSFixedArray<NTempest::CImVector> *)(v4 + 28));
      *((_DWORD *)v4 + 11) = 0;
      *((_DWORD *)v4 + 7) = &TSGrowableArray_<NTempest::CImVector,1199076678,59>::`vftable';
      TSGrowableArray<NTempest::CImVector>::TSGrowableArray<NTempest::CImVector>((TSGrowableArray<NTempest::CImVector> *)(v4 + 48));
      *((_DWORD *)v4 + 12) = &TSGrowableArray_<NTempest::CImVector,1199076678,60>::`vftable';
      TSGrowableArray<unsigned short>::TSGrowableArray<unsigned short>((TSGrowableArray<unsigned short> *)(v4 + 68));
      *((_DWORD *)v4 + 17) = &TSGrowableArray_<unsigned short,1199076678,61>::`vftable';
      *((_DWORD *)v4 + 6) = 64;
      *((_DWORD *)v4 + 16) = 64;
      *((_DWORD *)v4 + 21) = 64;
      return (TEXTLINETEXTURE *)v4;
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    m_prevlink = g_freeTextLineTextures.m_terminator.m_next->m_link.m_prevlink;
    if ( g_freeTextLineTextures.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      m_next = (int)g_freeTextLineTextures.m_terminator.m_next->m_link.m_next;
      if ( m_next > 0 )
        *(_DWORD *)(m_next + (char *)g_freeTextLineTextures.m_terminator.m_next - (char *)m_prevlink->m_next) = m_prevlink;
      else
        *(_DWORD *)~m_next = m_prevlink;
      result->m_link.m_prevlink->m_next = result->m_link.m_next;
      result->m_link.m_prevlink = nullptr;
      result->m_link.m_next = nullptr;
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C7050  EA 0x5C7050
// DIA: BATCHEDRENDERFONTDESC::RenderBatch
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall BATCHEDRENDERFONTDESC::RenderBatch(BATCHEDRENDERFONTDESC *this)
{
  BATCHEDRENDERFONTDESC *v1; // esi
  CGxFont *face; // eax
  int m_next; // edi
  EGxBlend v4; // edx
  CGxString *p_m_terminator; // eax
  unsigned int v6; // esi
  STRINGVIEWMATRICES *NewStringMatrix; // ebx
  unsigned int m_count; // ecx
  unsigned int v9; // esi
  int v10; // eax
  const char *v11; // eax
  int v12; // [esp-10h] [ebp-54h]
  int v13; // [esp-Ch] [ebp-50h]
  unsigned int v14; // [esp-8h] [ebp-4Ch]
  const char *v15; // [esp-4h] [ebp-48h]
  float width; // [esp+0h] [ebp-44h]
  float height; // [esp+4h] [ebp-40h]
  float minz; // [esp+14h] [ebp-30h] BYREF
  float maxz; // [esp+18h] [ebp-2Ch] BYREF
  float pixWidth[2]; // [esp+1Ch] [ebp-28h]
  float pixHeight[2]; // [esp+24h] [ebp-20h]
  BATCHEDRENDERFONTDESC *v22; // [esp+2Ch] [ebp-18h]
  float minx; // [esp+30h] [ebp-14h] BYREF
  float maxx; // [esp+34h] [ebp-10h] BYREF
  float miny; // [esp+38h] [ebp-Ch] BYREF
  float maxy; // [esp+3Ch] [ebp-8h] BYREF
  unsigned int i; // [esp+40h] [ebp-4h]

  v1 = this;
  face = this->face;
  v22 = this;
  if ( face == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3042, aFace, 0, 1u);
  if ( (__S58__1__RenderBatch_BATCHEDRENDERFONTDESC__QAEXXZ_4EA & 1) == 0 )
  {
    __S58__1__RenderBatch_BATCHEDRENDERFONTDESC__QAEXXZ_4EA |= 1u;
    normal_3.x = 0.0;
    normal_3.y = 0.0;
    normal_3.z = -1.0;
    atexit(_E59_23);
  }
  GxRsPush();
  GxRsSet(GxRs_Blend, GxBlend_Alpha);
  GxRsSet(GxRs_Lighting, GxBlend_Opaque);
  GxXformViewport((CBoundingBox *)&minx, &maxx, &miny, &maxy, &minz, &maxz);
  LODWORD(pixWidth[0]) = GetScreenPixelWidth();
  pixWidth[1] = (float)LODWORD(pixWidth[0]);
  LODWORD(pixHeight[0]) = GetScreenPixelHeight();
  i = 0;
  pixHeight[1] = (float)LODWORD(pixHeight[0]);
  do
  {
    m_next = (int)v1->m_strings.m_terminator.m_next;
    if ( m_next > 0 )
    {
      do
      {
        if ( *(char *)(m_next + 116) < 0 )
        {
          GxRsSet(GxRs_DepthWrite, GxBlend_AlphaKey);
          GxRsSet(GxRs_DepthTest, GxBlend_AlphaKey);
          v4 = GxBlend_AlphaKey;
        }
        else
        {
          GxRsSet(GxRs_DepthWrite, GxBlend_Opaque);
          GxRsSet(GxRs_DepthTest, GxBlend_Opaque);
          v4 = GxBlend_Opaque;
        }
        GxRsSet(GxRs_Fog, v4);
        if ( s_stringViewMatrices.m_slotmask == -1
          || (v6 = m_next & s_stringViewMatrices.m_slotmask,
              TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
                &s_stringViewMatrices.m_slotlistarray,
                m_next & s_stringViewMatrices.m_slotmask),
              NewStringMatrix = s_stringViewMatrices.m_slotlistarray.m_data[v6].m_terminator.m_next,
              p_m_terminator = (CGxString *)&s_stringViewMatrices.m_slotlistarray.m_data[v6].m_terminator,
              (int)NewStringMatrix <= 0) )
        {
LABEL_18:
          NewStringMatrix = GetNewStringMatrix(p_m_terminator);
          CGxString::BuildProjection(
            (CGxString *)m_next,
            &NewStringMatrix->projection,
            minx,
            maxx,
            miny,
            maxy,
            pixWidth[1],
            pixHeight[1]);
          height = maxy - miny;
          width = maxx - minx;
          CGxString::BuildView((CGxString *)m_next, &NewStringMatrix->view, width, height);
        }
        else
        {
          m_count = s_stringViewMatrices.m_slotlistarray.m_count;
          while ( NewStringMatrix->m_hashval != m_next || NewStringMatrix->m_key.m_key != (void *)m_next )
          {
            v9 = m_next & s_stringViewMatrices.m_slotmask;
            if ( (m_next & s_stringViewMatrices.m_slotmask) >= m_count )
            {
              v10 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int, int, char *, unsigned int, unsigned int))s_stringViewMatrices.m_slotlistarray.TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo)(
                      &s_stringViewMatrices.m_slotlistarray,
                      1,
                      1,
                      aIndex0x08xArra,
                      m_next & s_stringViewMatrices.m_slotmask,
                      m_count);
              v11 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int))s_stringViewMatrices.m_slotlistarray.TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName)(
                                    &s_stringViewMatrices.m_slotlistarray,
                                    v10);
              SErrDisplayErrorFmt(0x85100080, v11, v12, v13, v14, v15);
              m_count = s_stringViewMatrices.m_slotlistarray.m_count;
            }
            p_m_terminator = (CGxString *)((char *)NewStringMatrix
                                         + s_stringViewMatrices.m_slotlistarray.m_data[v9].m_linkoffset);
            NewStringMatrix = (STRINGVIEWMATRICES *)p_m_terminator->m_link.m_next;
            if ( (int)NewStringMatrix <= 0 )
              goto LABEL_18;
          }
        }
        GxXformSetProjection(&NewStringMatrix->projection);
        GxXformSetView(&NewStringMatrix->view);
        CGxString::RenderTexture((CGxString *)m_next, 0, i);
        m_next = *(_DWORD *)(m_next + v22->m_strings.m_linkoffset + 4);
      }
      while ( m_next > 0 );
      v1 = v22;
    }
    ++i;
  }
  while ( i < 8 );
  GxRsPop();
}


// -----------------------------------------------------------------------------
// RVA 0x001C72C0  EA 0x5C72C0
// DIA: GetNewStringMatrix
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
STRINGVIEWMATRICES *__fastcall GetNewStringMatrix(unsigned int a1)
{
  unsigned int m_slotmask; // eax
  unsigned int v4; // esi
  STRINGVIEWMATRICES *m_next; // edi
  unsigned int v6; // esi
  _DWORD *v7; // ecx
  int v8; // edx
  int v9; // esi
  int v10; // esi
  TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR> *v12; // eax
  STRINGVIEWMATRICES *v13; // esi
  unsigned int v14; // esi

  m_slotmask = s_stringViewMatrices.m_slotmask;
  if ( s_stringViewMatrices.m_slotmask == -1 )
  {
LABEL_7:
    if ( (int)s_freeStringMatrices.m_terminator.m_next <= 0 )
    {
      v12 = (TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR> *)SMemAlloc(0xA0u, aAustringviewma, -2, 8u);
      v13 = (STRINGVIEWMATRICES *)v12;
      if ( v12 != nullptr )
      {
        TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR>::TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR>(v12);
        v13->m_freeLink.m_prevlink = nullptr;
        v13->m_freeLink.m_next = nullptr;
        NTempest::C44Matrix::C44Matrix(&v13->projection);
        NTempest::C44Matrix::C44Matrix(&v13->view);
      }
      else
      {
        v13 = nullptr;
      }
      TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::LinkNode(
        &s_freeStringMatrices.TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >,
        v13,
        2u,
        nullptr);
      m_next = v13;
    }
    else
    {
      m_next = s_freeStringMatrices.m_terminator.m_next;
      v7 = (unsigned int *)((char *)&s_freeStringMatrices.m_terminator.m_next->m_hashval
                          + s_freeStringMatrices.m_linkoffset);
      v8 = *(unsigned int *)((char *)&s_freeStringMatrices.m_terminator.m_next->m_hashval
                           + s_freeStringMatrices.m_linkoffset);
      if ( v8 == 0 )
      {
LABEL_20:
        if ( m_slotmask == -1 )
        {
          TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::Initialize(&s_stringViewMatrices);
          m_slotmask = s_stringViewMatrices.m_slotmask;
        }
        v14 = a1 & m_slotmask;
        if ( TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::MonitorFullness(&s_stringViewMatrices, a1 & m_slotmask) != 0 )
          v14 = a1 & s_stringViewMatrices.m_slotmask;
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
          &s_stringViewMatrices.m_slotlistarray,
          v14);
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::LinkNode(
          &s_stringViewMatrices.m_slotlistarray.m_data[v14],
          m_next,
          2u,
          nullptr);
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::LinkNode(
          &s_stringViewMatrices.m_fulllist,
          m_next,
          2u,
          nullptr);
        m_next->m_hashval = a1;
        m_next->m_key.m_key = (void *)a1;
        return m_next;
      }
      v9 = v7[1];
      if ( v9 > 0 )
        v10 = (int)v7 + v9 - *(_DWORD *)(v8 + 4);
      else
        v10 = ~v9;
      *(_DWORD *)v10 = v8;
      *(_DWORD *)(*v7 + 4) = v7[1];
      *v7 = 0;
      v7[1] = 0;
    }
    m_slotmask = s_stringViewMatrices.m_slotmask;
    goto LABEL_20;
  }
  v4 = a1 & s_stringViewMatrices.m_slotmask;
  TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
    &s_stringViewMatrices.m_slotlistarray,
    a1 & s_stringViewMatrices.m_slotmask);
  m_next = (int)s_stringViewMatrices.m_slotlistarray.m_data[v4].m_terminator.m_next <= 0
         ? nullptr
         : s_stringViewMatrices.m_slotlistarray.m_data[v4].m_terminator.m_next;
  if ( (int)m_next <= 0 )
  {
LABEL_6:
    m_slotmask = s_stringViewMatrices.m_slotmask;
    goto LABEL_7;
  }
  while ( m_next->m_hashval != a1 || m_next->m_key.m_key != (void *)a1 )
  {
    v6 = a1 & s_stringViewMatrices.m_slotmask;
    TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
      &s_stringViewMatrices.m_slotlistarray,
      a1 & s_stringViewMatrices.m_slotmask);
    m_next = TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::Link(
               &s_stringViewMatrices.m_slotlistarray.m_data[v6],
               m_next)->m_next;
    if ( (int)m_next <= 0 )
      goto LABEL_6;
  }
  if ( m_next->m_freeLink.m_next != nullptr )
  {
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 494, aHeadMFreelinkI, 0, 1u);
    return m_next;
  }
  return m_next;
}


// -----------------------------------------------------------------------------
// RVA 0x001C7470  EA 0x5C7470
// DIA: $E59
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E59_23()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001C7480  EA 0x5C7480
// DIA: BATCHEDRENDERFONTDESC::~BATCHEDRENDERFONTDESC
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall BATCHEDRENDERFONTDESC::~BATCHEDRENDERFONTDESC(BATCHEDRENDERFONTDESC *this)
{
  CGxString *i; // esi
  CGxString *m_next; // ecx
  int m_linkoffset; // eax
  int v5; // esi
  _DWORD *v6; // eax
  int v7; // edx
  TSLink<CGxString> *p_m_terminator; // esi
  CGxString *v9; // eax
  TSLink<BATCHEDRENDERFONTDESC> *p_m_linktofull; // esi
  TSLink<BATCHEDRENDERFONTDESC> *m_prevlink; // esi
  TSLink<BATCHEDRENDERFONTDESC> *p_m_linktoslot; // eax
  int v13; // edx

  for ( i = (int)this->m_strings.m_terminator.m_next <= 0 ? nullptr : this->m_strings.m_terminator.m_next;
        (int)i > 0;
        i = *(CGxString **)((char *)&i->m_link.m_next + this->m_strings.m_linkoffset) )
  {
    CGxString::ClearStringMatrixEntry(i);
  }
  while ( 1 )
  {
    m_next = this->m_strings.m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    m_linkoffset = this->m_strings.m_linkoffset;
    v5 = *(int *)((char *)&m_next->m_link.m_prevlink + m_linkoffset);
    v6 = (TSLink<CGxString> **)((char *)&m_next->m_link.m_prevlink + m_linkoffset);
    if ( v5 != 0 )
    {
      v7 = v6[1];
      if ( v7 > 0 )
        *(_DWORD *)((char *)v6 + v7 - *(_DWORD *)(v5 + 4)) = v5;
      else
        *(_DWORD *)~v7 = v5;
      *(_DWORD *)(*v6 + 4) = v6[1];
      *v6 = 0;
      v6[1] = 0;
    }
  }
  p_m_terminator = &this->m_strings.m_terminator;
  while ( 1 )
  {
    v9 = this->m_strings.m_terminator.m_next;
    if ( (int)v9 <= 0 )
      break;
    TSLink<CGxString>::Unlink((TSLink<CGxString> *)((char *)&v9->m_link + this->m_strings.m_linkoffset));
  }
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    TSLink<CGxString>::NextLink(&this->m_strings.m_terminator, -1)->m_prevlink = p_m_terminator->m_prevlink;
    p_m_terminator->m_prevlink->m_next = this->m_strings.m_terminator.m_next;
    p_m_terminator->m_prevlink = nullptr;
    this->m_strings.m_terminator.m_next = nullptr;
  }
  p_m_linktofull = &this->m_linktofull;
  if ( this->m_linktofull.m_prevlink != nullptr )
  {
    TSLink<BATCHEDRENDERFONTDESC>::NextLink(&this->m_linktofull, -1)->m_prevlink = p_m_linktofull->m_prevlink;
    p_m_linktofull->m_prevlink->m_next = this->m_linktofull.m_next;
    p_m_linktofull->m_prevlink = nullptr;
    this->m_linktofull.m_next = nullptr;
  }
  m_prevlink = this->m_linktoslot.m_prevlink;
  p_m_linktoslot = &this->m_linktoslot;
  if ( m_prevlink != nullptr )
  {
    v13 = (int)this->m_linktoslot.m_next;
    if ( v13 > 0 )
    {
      *(_DWORD *)((char *)p_m_linktoslot - (char *)m_prevlink->m_next + v13) = m_prevlink;
      this->m_linktoslot.m_prevlink->m_next = this->m_linktoslot.m_next;
    }
    else
    {
      *(_DWORD *)~v13 = m_prevlink;
      p_m_linktoslot->m_prevlink->m_next = this->m_linktoslot.m_next;
    }
    p_m_linktoslot->m_prevlink = nullptr;
    this->m_linktoslot.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C75B0  EA 0x5C75B0
// DIA: CGxStringBatch::RenderBatch
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxStringBatch::RenderBatch(CGxStringBatch *this)
{
  BATCHEDRENDERFONTDESC *v2; // esi
  NTempest::C44Matrix oldView; // [esp+8h] [ebp-80h] BYREF
  NTempest::C44Matrix oldProjection; // [esp+48h] [ebp-40h] BYREF

  GxVertexShaderSelect(GxVS_PassThru);
  oldProjection.a0 = 1.0;
  memset(&oldProjection.a1, 0, 16);
  oldProjection.b1 = 1.0;
  memset(&oldProjection.b2, 0, 16);
  oldProjection.c2 = 1.0;
  memset(&oldProjection.c3, 0, 16);
  oldProjection.d3 = 1.0;
  oldView.a0 = 1.0;
  memset(&oldView.a1, 0, 16);
  oldView.b1 = 1.0;
  memset(&oldView.b2, 0, 16);
  oldView.c2 = 1.0;
  memset(&oldView.c3, 0, 16);
  oldView.d3 = 1.0;
  GxXformProjection(&oldProjection);
  GxXformView(&oldView);
  v2 = (int)this->m_fontBatch.m_fulllist.m_terminator.m_next <= 0
     ? nullptr
     : this->m_fontBatch.m_fulllist.m_terminator.m_next;
  if ( (int)v2 > 0 )
  {
    while ( 1 )
    {
      BATCHEDRENDERFONTDESC::RenderBatch(v2);
      if ( *(int *)((char *)&v2->m_linktoslot.m_prevlink + this->m_fontBatch.m_fulllist.m_linkoffset) <= 0 )
        break;
      v2 = *(BATCHEDRENDERFONTDESC **)((char *)&v2->m_linktoslot.m_prevlink + this->m_fontBatch.m_fulllist.m_linkoffset);
    }
  }
  GxXformSetProjection(&oldProjection);
  GxXformSetView(&oldView);
}


// -----------------------------------------------------------------------------
// RVA 0x001C7700  EA 0x5C7700
// DIA: CGxStringBatch::AddString
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxStringBatch::AddString(CGxStringBatch *this, CGxString *string)
{
  CGxFont *m_currentFace; // esi
  unsigned int m_slotmask; // edi
  TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *p_m_fontBatch; // ebx
  unsigned int v6; // edi
  BATCHEDRENDERFONTDESC *m_next; // eax
  BATCHEDRENDERFONTDESC *v8; // edi
  unsigned int v9; // esi
  int v10; // eax
  unsigned int v11; // edi
  int m_linkoffset; // esi
  int v13; // eax
  TSLink<CGxString> *v14; // esi
  TSLink<CGxString> *m_prevlink; // eax
  CGxFont *currentFace; // [esp+Ch] [ebp-4h]

  if ( string == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3139, aString, 0, 1u);
  m_currentFace = string->m_currentFace;
  currentFace = m_currentFace;
  if ( m_currentFace == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3142, aCurrentface, 0, 1u);
  m_slotmask = this->m_fontBatch.m_slotmask;
  p_m_fontBatch = &this->m_fontBatch;
  if ( m_slotmask == -1
    || (v6 = (unsigned int)m_currentFace & m_slotmask,
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
          &p_m_fontBatch->m_slotlistarray,
          v6),
        m_next = p_m_fontBatch->m_slotlistarray.m_data[v6].m_terminator.m_next,
        (int)(v8 = (int)m_next <= 0 ? nullptr : m_next) <= 0) )
  {
LABEL_10:
    if ( p_m_fontBatch->m_slotmask == -1 )
      TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::Initialize(p_m_fontBatch);
    v11 = (unsigned int)m_currentFace & p_m_fontBatch->m_slotmask;
    if ( TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::MonitorFullness(p_m_fontBatch, v11) != 0 )
      v11 = (unsigned int)m_currentFace & p_m_fontBatch->m_slotmask;
    TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
      &p_m_fontBatch->m_slotlistarray,
      v11);
    v8 = p_m_fontBatch->TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::InternalNew(
           p_m_fontBatch,
           &p_m_fontBatch->m_slotlistarray.m_data[v11],
           0,
           0);
    TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::LinkNode(
      &p_m_fontBatch->m_fulllist,
      v8,
      2u,
      nullptr);
    v8->m_hashval = (unsigned int)m_currentFace;
    v8->m_key.m_key = m_currentFace;
    v8->face = m_currentFace;
  }
  else
  {
    while ( (CGxFont *)v8->m_hashval != m_currentFace || v8->m_key.m_key != m_currentFace )
    {
      v9 = (unsigned int)currentFace & p_m_fontBatch->m_slotmask;
      TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(
        &p_m_fontBatch->m_slotlistarray,
        v9);
      v10 = v9;
      m_currentFace = currentFace;
      v8 = *(BATCHEDRENDERFONTDESC **)((char *)&v8->m_linktoslot.m_prevlink
                                     + p_m_fontBatch->m_slotlistarray.m_data[v10].m_linkoffset);
      if ( (int)v8 <= 0 )
        goto LABEL_10;
    }
    if ( v8->face != m_currentFace )
      SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3154, aBatchdescFaceC, 0, 1u);
  }
  m_linkoffset = v8->m_strings.m_linkoffset;
  v13 = *(int *)((char *)&string->m_link.m_prevlink + m_linkoffset);
  v14 = (TSLink<CGxString> *)((char *)&string->m_link + m_linkoffset);
  if ( v13 != 0 )
  {
    TSLink<CGxString>::NextLink(v14, -1)->m_prevlink = v14->m_prevlink;
    v14->m_prevlink->m_next = v14->m_next;
    v14->m_prevlink = nullptr;
    v14->m_next = nullptr;
  }
  m_prevlink = v8->m_strings.m_terminator.m_prevlink;
  v14->m_prevlink = m_prevlink;
  v14->m_next = m_prevlink->m_next;
  m_prevlink->m_next = string;
  v8->m_strings.m_terminator.m_prevlink = v14;
}


// -----------------------------------------------------------------------------
// RVA 0x001C7880  EA 0x5C7880
// DIA: CGxString::ClearStringMatrixEntry
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::ClearStringMatrixEntry(CGxString *this)
{
  unsigned int v2; // esi
  STRINGVIEWMATRICES *v3; // edi
  unsigned int v4; // esi
  TSLink<STRINGVIEWMATRICES> *m_prevlink; // esi
  TSLink<STRINGVIEWMATRICES> *p_m_linktoslot; // eax
  int m_next; // edx
  int v8; // ecx
  TSLink<STRINGVIEWMATRICES> *v9; // esi
  TSLink<STRINGVIEWMATRICES> *p_m_linktofull; // eax
  int v11; // edx
  int v12; // edx
  TSLink<STRINGVIEWMATRICES> *v13; // esi
  TSLink<STRINGVIEWMATRICES> *v14; // eax

  if ( s_stringViewMatrices.m_slotmask != -1 )
  {
    v2 = (unsigned int)this & s_stringViewMatrices.m_slotmask;
    TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
      &s_stringViewMatrices.m_slotlistarray,
      (unsigned int)this & s_stringViewMatrices.m_slotmask);
    v3 = (int)s_stringViewMatrices.m_slotlistarray.m_data[v2].m_terminator.m_next <= 0
       ? nullptr
       : s_stringViewMatrices.m_slotlistarray.m_data[v2].m_terminator.m_next;
    if ( (int)v3 > 0 )
    {
      while ( (CGxString *)v3->m_hashval != this || v3->m_key.m_key != this )
      {
        v4 = (unsigned int)this & s_stringViewMatrices.m_slotmask;
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
          &s_stringViewMatrices.m_slotlistarray,
          (unsigned int)this & s_stringViewMatrices.m_slotmask);
        v3 = *(STRINGVIEWMATRICES **)((char *)&v3->m_linktoslot.m_prevlink
                                    + s_stringViewMatrices.m_slotlistarray.m_data[v4].m_linkoffset);
        if ( (int)v3 <= 0 )
          return;
      }
      if ( v3->m_linktoslot.m_next != nullptr )
      {
        m_prevlink = v3->m_linktoslot.m_prevlink;
        p_m_linktoslot = &v3->m_linktoslot;
        if ( m_prevlink != nullptr )
        {
          m_next = (int)v3->m_linktoslot.m_next;
          if ( m_next > 0 )
            v8 = m_next + (char *)p_m_linktoslot - (char *)m_prevlink->m_next;
          else
            v8 = ~m_next;
          *(_DWORD *)v8 = m_prevlink;
          p_m_linktoslot->m_prevlink->m_next = v3->m_linktoslot.m_next;
          p_m_linktoslot->m_prevlink = nullptr;
          v3->m_linktoslot.m_next = nullptr;
        }
        v9 = v3->m_linktofull.m_prevlink;
        p_m_linktofull = &v3->m_linktofull;
        if ( v9 != nullptr )
        {
          v11 = (int)v3->m_linktofull.m_next;
          if ( v11 > 0 )
            v12 = (char *)p_m_linktofull - (char *)v9->m_next + v11;
          else
            v12 = ~v11;
          *(_DWORD *)v12 = v9;
          p_m_linktofull->m_prevlink->m_next = v3->m_linktofull.m_next;
          p_m_linktofull->m_prevlink = nullptr;
          v3->m_linktofull.m_next = nullptr;
        }
      }
      v13 = (TSLink<STRINGVIEWMATRICES> *)((char *)v3 + s_freeStringMatrices.m_linkoffset);
      if ( *(unsigned int *)((char *)&v3->m_hashval + s_freeStringMatrices.m_linkoffset) != 0 )
        TSLink<STRINGVIEWMATRICES>::Unlink((TSLink<STRINGVIEWMATRICES> *)((char *)v3 + s_freeStringMatrices.m_linkoffset));
      v14 = s_freeStringMatrices.m_terminator.m_prevlink;
      v13->m_prevlink = s_freeStringMatrices.m_terminator.m_prevlink;
      v13->m_next = v14->m_next;
      v14->m_next = v3;
      s_freeStringMatrices.m_terminator.m_prevlink = v13;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C79A0  EA 0x5C79A0
// DIA: CGxString::AddHyperlinkParseInfo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::AddHyperlinkParseInfo(CGxString *this, GXUFONTHYPERLINKINFO currentParseInfo)
{
  double v2; // st7
  TSGrowableArray<GXUFONTHYPERLINKINFO> *p_m_hyperlinkInfo; // ebx
  unsigned int v4; // esi
  double v5; // st7
  unsigned int m_chunk; // ecx
  int i; // eax
  GXUFONTHYPERLINKINFO *v8; // edi
  __int64 v9; // [esp+0h] [ebp-8h]

  v9 = g_heightPixels;
  LODWORD(v9) = g_widthPixels;
  v2 = 1.0 / (double)g_heightPixels;
  p_m_hyperlinkInfo = &this->m_hyperlinkInfo;
  v4 = this->m_hyperlinkInfo.m_count + 1;
  currentParseInfo.extent.t = currentParseInfo.extent.t * v2;
  currentParseInfo.extent.b = v2 * currentParseInfo.extent.b;
  v5 = 1.0 / (double)v9;
  currentParseInfo.extent.l = currentParseInfo.extent.l * v5;
  currentParseInfo.extent.r = v5 * currentParseInfo.extent.r;
  if ( v4 > this->m_hyperlinkInfo.m_alloc )
  {
    m_chunk = this->m_hyperlinkInfo.m_chunk;
    if ( m_chunk == 0 )
    {
      m_chunk = v4;
      if ( v4 >= 0xA )
      {
        p_m_hyperlinkInfo->m_chunk = 10;
        m_chunk = 10;
      }
      else
      {
        for ( i = v4 & (v4 - 1); i != 0; i &= i - 1 )
          m_chunk = i;
        if ( m_chunk == 0 )
          m_chunk = 1;
      }
    }
    if ( v4 % m_chunk != 0 )
      v4 += m_chunk - v4 % m_chunk;
    TSFixedArray<GXUFONTHYPERLINKINFO>::ReallocData(p_m_hyperlinkInfo, v4);
  }
  v8 = &p_m_hyperlinkInfo->m_data[p_m_hyperlinkInfo->m_count];
  if ( v8 != nullptr )
    *v8 = currentParseInfo;
  ++p_m_hyperlinkInfo->m_count;
}


// -----------------------------------------------------------------------------
// RVA 0x001C7A80  EA 0x5C7A80
// DIA: CGxString::CheckEvictedTextures
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::CheckEvictedTextures(CGxString *this)
{
  if ( this->m_textureEvicted != 0 )
  {
    if ( this->m_currentFace == nullptr )
      SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3182, aMCurrentface, 0, 1u);
    if ( CGxFont::CheckStringGlyphs(this->m_currentFace, this->m_text) == 0 )
      CGxString::CreateGeometry(this);
    this->m_textureEvicted = 0;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C7AE0  EA 0x5C7AE0
// DIA: CGxString::GetHyperLinkInfo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __thiscall CGxString::GetHyperLinkInfo(CGxString *this, GXUFONTHYPERLINKINFO **list)
{
  unsigned int result; // eax

  result = this->m_hyperlinkInfo.m_count;
  if ( result == 0 || g_widthPixels == 0 || g_heightPixels == 0 )
    return 0;
  *list = this->m_hyperlinkInfo.m_data;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C7B20  EA 0x5C7B20
// DIA: CGxString::SetGradient
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall CGxString::SetGradient(CGxString *this, int startCharacter, int length)
{
  unsigned int m_flags; // eax

  if ( startCharacter < 0 || length < 0 )
    return 2;
  if ( startCharacter == this->m_lastGradientStart && length == this->m_lastGradientLength )
    return 0;
  m_flags = this->m_flags;
  this->m_lastGradientStart = startCharacter;
  this->m_lastGradientLength = length;
  if ( (m_flags & 0x1000) != 0 )
  {
    if ( this->m_colorGradients.m_count == 0 )
      SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3213, aMColorgradient, 0, 1u);
  }
  else
  {
    this->m_flags = m_flags & 0xFFFFEFF7 | 0x1000;
    CGxString::CreateGeometry(this);
  }
  if ( this->m_colorGradientShadows.m_count != 0 )
    CGxString::SetGradient(this, startCharacter, length, &this->m_colorGradientShadows, this->m_shadowColor.a);
  return CGxString::SetGradient(this, startCharacter, length, &this->m_colorGradients, this->m_fontColor.a);
}


// -----------------------------------------------------------------------------
// RVA 0x001C7BF0  EA 0x5C7BF0
// DIA: CGxString::SetGradient
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall CGxString::SetGradient(
        CGxString *this,
        int startCharacter,
        int length,
        const TSGrowableArray<NTempest::CImVector *> *array,
        int alpha)
{
  signed int v6; // ecx
  int v8; // edx
  bool v9; // cc
  signed int v10; // edi
  int v11; // eax
  const char *v12; // eax
  signed int v13; // edi
  int v14; // ebx
  unsigned int m_count; // ecx
  unsigned int v16; // eax
  int v17; // edi
  int v18; // ebx
  int v19; // eax
  const char *v20; // eax
  int v21; // eax
  unsigned int v22; // edx
  unsigned int v23; // ecx
  unsigned int v24; // edi
  int v25; // ebx
  int v26; // eax
  const char *v27; // eax
  unsigned int v28; // ecx
  int v29; // ebx
  int v30; // eax
  unsigned int v31; // edx
  unsigned int v32; // edi
  int v33; // eax
  const char *v34; // eax
  unsigned int v35; // edx
  unsigned int v36; // ecx
  int v37; // eax
  const char *v38; // eax
  int v39; // eax
  const char *v40; // eax
  NTempest::CImVector *v41; // eax
  int v42; // [esp-18h] [ebp-28h]
  int v43; // [esp-18h] [ebp-28h]
  int v44; // [esp-18h] [ebp-28h]
  int v45; // [esp-18h] [ebp-28h]
  int v46; // [esp-18h] [ebp-28h]
  int v47; // [esp-18h] [ebp-28h]
  int v48; // [esp-14h] [ebp-24h]
  int v49; // [esp-14h] [ebp-24h]
  int v50; // [esp-14h] [ebp-24h]
  int v51; // [esp-14h] [ebp-24h]
  int v52; // [esp-14h] [ebp-24h]
  int v53; // [esp-14h] [ebp-24h]
  unsigned int v54; // [esp-10h] [ebp-20h]
  unsigned int v55; // [esp-10h] [ebp-20h]
  unsigned int v56; // [esp-10h] [ebp-20h]
  unsigned int v57; // [esp-10h] [ebp-20h]
  unsigned int v58; // [esp-10h] [ebp-20h]
  unsigned int v59; // [esp-10h] [ebp-20h]
  const char *v60; // [esp-Ch] [ebp-1Ch]
  const char *v61; // [esp-Ch] [ebp-1Ch]
  const char *v62; // [esp-Ch] [ebp-1Ch]
  const char *v63; // [esp-Ch] [ebp-1Ch]
  const char *v64; // [esp-Ch] [ebp-1Ch]
  const char *v65; // [esp-Ch] [ebp-1Ch]
  int gradient; // [esp+Ch] [ebp-4h]
  int startIndex; // [esp+18h] [ebp+8h]
  int lengtha; // [esp+1Ch] [ebp+Ch]
  int lengthb; // [esp+1Ch] [ebp+Ch]
  int lengthc; // [esp+1Ch] [ebp+Ch]
  int lengthd; // [esp+1Ch] [ebp+Ch]
  signed int verts; // [esp+20h] [ebp+10h]

  verts = array->m_count;
  v6 = verts;
  if ( verts == 0 )
    return 2;
  v8 = 4 * startCharacter;
  v9 = verts < 4 * startCharacter;
  startIndex = 4 * startCharacter;
  v10 = verts;
  if ( !v9 )
    v10 = v8;
  for ( ; v10 != 0; array->m_data[v10]->a = alpha )
  {
    if ( --v10 >= array->m_count )
    {
      v11 = ((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int, int, char *, signed int, unsigned int))array->TSBaseArray<NTempest::CImVector *>::MemLineNo)(
              array,
              1,
              1,
              aIndex0x08xArra,
              v10,
              array->m_count);
      v12 = (const char *)((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int))array->TSBaseArray<NTempest::CImVector *>::MemFileName)(
                            array,
                            v11);
      SErrDisplayErrorFmt(0x85100080, v12, v42, v48, v54, v60);
      v6 = verts;
      v8 = startIndex;
    }
  }
  if ( v8 >= v6 )
    return 2;
  v13 = v8;
  if ( length > 0 )
  {
    alpha = (unsigned __int8)alpha;
    gradient = (__int64)((double)(unsigned __int8)alpha / (double)length);
    v14 = 4 * startIndex;
    while ( 1 )
    {
      m_count = array->m_count;
      v16 = v13;
      lengtha = v14;
      v17 = v13 + 1;
      v18 = v14 + 4;
      if ( v16 >= m_count )
      {
        v19 = ((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int, int, char *, unsigned int, unsigned int))array->TSBaseArray<NTempest::CImVector *>::MemLineNo)(
                array,
                1,
                1,
                aIndex0x08xArra,
                v16,
                m_count);
        v20 = (const char *)((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int))array->TSBaseArray<NTempest::CImVector *>::MemFileName)(
                              array,
                              v19);
        SErrDisplayErrorFmt(0x85100080, v20, v43, v49, v55, v61);
      }
      v21 = alpha;
      (*(NTempest::CImVector **)((char *)array->m_data + lengtha))->a = alpha;
      v22 = array->m_count;
      v23 = v17;
      lengthb = v18;
      v24 = v17 + 1;
      v25 = v18 + 4;
      if ( v23 >= v22 )
      {
        v26 = ((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int, int, char *, unsigned int, unsigned int))array->TSBaseArray<NTempest::CImVector *>::MemLineNo)(
                array,
                1,
                1,
                aIndex0x08xArra,
                v23,
                v22);
        v27 = (const char *)((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int))array->TSBaseArray<NTempest::CImVector *>::MemFileName)(
                              array,
                              v26);
        SErrDisplayErrorFmt(0x85100080, v27, v44, v50, v56, v62);
        v21 = alpha;
      }
      (*(NTempest::CImVector **)((char *)array->m_data + lengthb))->a = v21;
      v28 = v24;
      lengthc = v25;
      v29 = v25 + 4;
      v30 = v21 - gradient < 0 ? 0 : v21 - gradient;
      v31 = array->m_count;
      v32 = v24 + 1;
      alpha = v30;
      if ( v28 >= v31 )
      {
        v33 = ((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int, int, char *, unsigned int, unsigned int))array->TSBaseArray<NTempest::CImVector *>::MemLineNo)(
                array,
                1,
                1,
                aIndex0x08xArra,
                v28,
                v31);
        v34 = (const char *)((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int))array->TSBaseArray<NTempest::CImVector *>::MemFileName)(
                              array,
                              v33);
        SErrDisplayErrorFmt(0x85100080, v34, v45, v51, v57, v63);
        v30 = alpha;
      }
      (*(NTempest::CImVector **)((char *)array->m_data + lengthc))->a = v30;
      v35 = array->m_count;
      v36 = v32;
      lengthd = v29;
      v13 = v32 + 1;
      v14 = v29 + 4;
      if ( v36 >= v35 )
      {
        v37 = ((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int, int, char *, unsigned int, unsigned int))array->TSBaseArray<NTempest::CImVector *>::MemLineNo)(
                array,
                1,
                1,
                aIndex0x08xArra,
                v36,
                v35);
        v38 = (const char *)((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int))array->TSBaseArray<NTempest::CImVector *>::MemFileName)(
                              array,
                              v37);
        SErrDisplayErrorFmt(0x85100080, v38, v46, v52, v58, v64);
        v30 = alpha;
      }
      (*(NTempest::CImVector **)((char *)array->m_data + lengthd))->a = v30;
      if ( v30 == 0 )
        break;
      if ( v13 >= verts )
        return 1;
    }
    v6 = verts;
  }
  if ( v13 >= v6 )
    return 1;
  do
  {
    if ( v13 >= array->m_count )
    {
      v39 = ((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int, int, char *, signed int, unsigned int))array->TSBaseArray<NTempest::CImVector *>::MemLineNo)(
              array,
              1,
              1,
              aIndex0x08xArra,
              v13,
              array->m_count);
      v40 = (const char *)((int (__thiscall *)(const TSGrowableArray<NTempest::CImVector *> *, int))array->TSBaseArray<NTempest::CImVector *>::MemFileName)(
                            array,
                            v39);
      SErrDisplayErrorFmt(0x85100080, v40, v47, v53, v59, v65);
      v6 = verts;
    }
    v41 = array->m_data[v13++];
    v41->a = 0;
  }
  while ( v13 < v6 );
  return 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001C7E60  EA 0x5C7E60
// DIA: IGxuStringInitialize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __cdecl IGxuStringInitialize()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001C7E70  EA 0x5C7E70
// DIA: IGxuStringShutdown
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __cdecl IGxuStringShutdown()
{
  STRINGVIEWMATRICES *m_next; // edi
  TSLink<STRINGVIEWMATRICES> *p_m_freeLink; // esi

  while ( 1 )
  {
    m_next = s_freeStringMatrices.m_terminator.m_next;
    if ( (int)s_freeStringMatrices.m_terminator.m_next <= 0 )
      break;
    p_m_freeLink = &s_freeStringMatrices.m_terminator.m_next->m_freeLink;
    if ( s_freeStringMatrices.m_terminator.m_next->m_freeLink.m_prevlink != nullptr )
    {
      TSLink<STRINGVIEWMATRICES>::NextLink(&s_freeStringMatrices.m_terminator.m_next->m_freeLink, -1)->m_prevlink = p_m_freeLink->m_prevlink;
      p_m_freeLink->m_prevlink->m_next = m_next->m_freeLink.m_next;
      p_m_freeLink->m_prevlink = nullptr;
      m_next->m_freeLink.m_next = nullptr;
    }
    TSLink<STRINGVIEWMATRICES>::Unlink(&m_next->m_linktofull);
    TSLink<STRINGVIEWMATRICES>::Unlink(&m_next->m_linktoslot);
    SMemFree(m_next, aAustringviewma, -2, 0);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C7ED0  EA 0x5C7ED0
// DIA: InternalGetTextExtent
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall InternalGetTextExtent(
        CGxFont *text,
        char *a2,
        unsigned int numBytes,
        int height,
        float *extent,
        __int16 flags)
{
  char *v7; // edi
  CGxFont *v8; // eax
  __int16 v9; // fps
  bool v10; // c0
  char v11; // c2
  bool v12; // c3
  bool v13; // zf
  QUOTEDCODE v15; // eax
  TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *v16; // esi
  const CHARCODEDESC *v17; // eax
  double v18; // st7
  double X; // st7
  double v20; // st7
  double v21; // st7
  double v22; // st7
  unsigned int wide[2]; // [esp+14h] [ebp-20h] BYREF
  unsigned int advance; // [esp+1Ch] [ebp-18h] BYREF
  unsigned int prevCode; // [esp+20h] [ebp-14h]
  float maxWidth; // [esp+24h] [ebp-10h]
  float lastWidth; // [esp+28h] [ebp-Ch]
  CGxFont *v28; // [esp+2Ch] [ebp-8h]
  float width; // [esp+30h] [ebp-4h]
  float step; // [esp+3Ch] [ebp+8h]

  v7 = a2;
  v28 = text;
  if ( text == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_239, 3288);
    SErrDisplayAppFatal(aFace);
  }
  if ( a2 == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_239, 3289);
    SErrDisplayAppFatal(aText);
  }
  HIWORD(v8) = HIWORD(extent);
  if ( extent == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_239, 3290);
    SErrDisplayAppFatal(aExtent);
  }
  v10 = *(float *)&height < 0.0;
  v11 = 0;
  v12 = *(float *)&height == 0.0;
  LOWORD(v8) = v9;
  if ( *(float *)&height == 0.0 || (flags & 4) != 0 )
    GxuFontGetOneToOneHeight(v8);
  v13 = *v7 == 0;
  width = 0.0;
  prevCode = 0;
  lastWidth = 0.0;
  maxWidth = 0.0;
  if ( !v13 )
  {
    do
    {
      if ( numBytes == 0 )
        break;
      v15 = GxuDetermineQuotedCode(v7, &advance, nullptr, flags, &wide[1], numBytes);
      v7 += advance;
      numBytes -= advance;
      switch ( v15 )
      {
        case CODE_COLORON:
        case CODE_COLORRESTORE:
        case CODE_HYPERLINKSTART:
        case CODE_HYPERLINKSTOP:
          break;
        case CODE_NEWLINE:
          goto $L29510;
        default:
          v16 = (TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *)wide[1];
          if ( wide[1] == 10 )
          {
$L29510:
            v20 = lastWidth + width;
            if ( v20 > maxWidth )
              maxWidth = v20;
            width = 0.0;
            lastWidth = 0.0;
          }
          else
          {
            v17 = CGxFont::NewCodeDesc(v28, wide[1]);
            if ( v17 != nullptr && v17->bitmapData != nullptr )
            {
              if ( v17->dataValid == 0 )
                SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3343, aCodedescDatava, 0, 1u);
              step = 0.0;
              if ( prevCode != 0 )
              {
                if ( (flags & 0x10) != 0 )
                  v18 = CGxFont::ComputeStepFixedWidth(v28, prevCode, (unsigned int)v16);
                else
                  v18 = CGxFont::ComputeStep(v28, prevCode, v16);
                step = v18;
              }
              X = CGxFont::GetCharAdvance(v28, (unsigned int)v16);
              lastWidth = _ceil(X);
              prevCode = (unsigned int)v16;
              width = step + width;
            }
          }
          break;
      }
    }
    while ( *v7 != 0 );
  }
  v21 = lastWidth + width;
  if ( v21 <= maxWidth )
    width = maxWidth;
  else
    width = v21;
  ScreenToPixelHeight(height, *(float *)&height);
  wide[0] = v28->m_pixelSize;
  wide[1] = 0;
  v22 = v21 / (double)(int)wide[0] * width;
  if ( (flags & 0x80) != 0 )
    *extent = v22;
  else
    *extent = v22 / (double)g_widthPixels;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8140  EA 0x5C8140
// DIA: InternalGetMaxCharsWithinWidth
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __userpurge InternalGetMaxCharsWithinWidth@<eax>(
        CGxFont *face@<eax>,
        CGxFont *text@<ecx>,
        char *a3@<edx>,
        float height,
        float maxWidth,
        unsigned int lineBytes,
        float *extent,
        __int16 flags,
        unsigned int *bytesInString,
        float *widthArray,
        float *widthArrayGuard)
{
  char *v12; // edi
  __int16 v13; // fps
  bool v14; // c0
  char v15; // c2
  bool v16; // c3
  int m_pixelSize; // eax
  double v18; // st7
  bool v19; // zf
  QUOTEDCODE v20; // eax
  TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *v21; // esi
  const CHARCODEDESC *v22; // eax
  double v23; // st7
  double X; // st7
  double v25; // st7
  float *v26; // esi
  double v27; // st7
  double v28; // st7
  const char *originalText; // [esp+28h] [ebp-2Ch]
  unsigned int pixWidth; // [esp+2Ch] [ebp-28h]
  unsigned int wide; // [esp+30h] [ebp-24h] BYREF
  float pixelScale; // [esp+34h] [ebp-20h]
  float lastWidth; // [esp+38h] [ebp-1Ch]
  unsigned int advance; // [esp+3Ch] [ebp-18h] BYREF
  unsigned int prevCode; // [esp+40h] [ebp-14h]
  int v37; // [esp+44h] [ebp-10h]
  unsigned int numChars; // [esp+48h] [ebp-Ch]
  float width; // [esp+4Ch] [ebp-8h]
  CGxFont *v40; // [esp+50h] [ebp-4h]
  float step; // [esp+5Ch] [ebp+8h]
  float pixelWidth; // [esp+60h] [ebp+Ch]
  float *pixelWidtha; // [esp+60h] [ebp+Ch]

  v12 = a3;
  v40 = text;
  if ( text == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_239, 3385);
    SErrDisplayAppFatal(aFace);
  }
  if ( a3 == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_239, 3386);
    SErrDisplayAppFatal(aText);
  }
  if ( extent == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_239, 3387);
    SErrDisplayAppFatal(aExtent);
  }
  numChars = 0;
  v37 = flags & 0x80;
  if ( (flags & 0x80) == 0 )
  {
    v14 = height < 0.0;
    v15 = 0;
    v16 = height == 0.0;
    LOWORD(face) = v13;
    if ( height == 0.0 || (flags & 4) != 0 )
      GxuFontGetOneToOneHeight(face);
  }
  m_pixelSize = text->m_pixelSize;
  width = 0.0;
  prevCode = 0;
  lastWidth = 0.0;
  v18 = (double)(unsigned int)m_pixelSize * (double)(int)g_widthPixels * maxWidth;
  pixelWidth = v18;
  ScreenToPixelHeight(m_pixelSize, height);
  if ( v18 <= 1.0 )
    v18 = 1.0;
  pixWidth = (__int64)(pixelWidth / v18 + 1.0);
  ScreenToPixelHeight(pixWidth, height);
  v19 = *v12 == 0;
  pixelWidtha = widthArray;
  originalText = v12;
  pixelScale = v18 / (double)v40->m_pixelSize;
  if ( !v19 )
  {
    while ( lineBytes != 0 )
    {
      v20 = GxuDetermineQuotedCode(v12, &advance, nullptr, flags, &wide, lineBytes);
      v12 += advance;
      lineBytes -= advance;
      switch ( v20 )
      {
        case CODE_COLORON:
        case CODE_COLORRESTORE:
        case CODE_HYPERLINKSTART:
        case CODE_HYPERLINKSTOP:
          goto $L48915;
        case CODE_NEWLINE:
          goto $L29573;
        default:
          v21 = (TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *)wide;
          v22 = CGxFont::NewCodeDesc(v40, wide);
          if ( v22 != nullptr && v22->bitmapData != nullptr )
          {
            if ( v22->dataValid == 0 )
              SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3461, aCodedescDatava, 0, 1u);
            step = 0.0;
            if ( prevCode != 0 )
            {
              if ( (flags & 0x10) != 0 )
                v23 = CGxFont::ComputeStepFixedWidth(v40, prevCode, (unsigned int)v21);
              else
                v23 = CGxFont::ComputeStep(v40, prevCode, v21);
              step = v23;
            }
            X = CGxFont::GetCharAdvance(v40, (unsigned int)v21);
            v25 = _ceil(X);
            if ( (double)pixWidth < step + v25 + width )
            {
              v12 -= advance;
              goto $L29573;
            }
            width = step + width;
            prevCode = (unsigned int)v21;
            ++numChars;
            lastWidth = v25;
            if ( widthArray != nullptr )
            {
              v26 = pixelWidtha;
              if ( pixelWidtha >= widthArrayGuard )
                SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3484, aWidthindexWidt, 0, 1u);
              v27 = width * pixelScale;
              if ( v37 != 0 )
              {
                *pixelWidtha++ = v27;
              }
              else
              {
                ++pixelWidtha;
                *v26 = v27 / (double)g_widthPixels;
              }
            }
          }
          else
          {
            ++numChars;
          }
$L48915:
          if ( *v12 == 0 )
            goto $L29573;
          break;
      }
    }
  }
$L29573:
  v28 = (lastWidth + width) * pixelScale;
  if ( v37 != 0 )
    *extent = v28;
  else
    *extent = v28 / (double)g_widthPixels;
  if ( bytesInString != nullptr )
    *bytesInString = v12 - originalText;
  return numChars;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8450  EA 0x5C8450
// DIA: GxuFontSetUseAdvanceWidth
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void GxuFontSetUseAdvanceWidth()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8460  EA 0x5C8460
// DIA: CGxString::RenderTexture
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::RenderTexture(CGxString *this, unsigned __int8 initGxRenderStates, int texture)
{
  unsigned int i; // esi

  if ( initGxRenderStates != 0 )
  {
    GxRsPush();
    GxRsSet(GxRs_Lighting, GxBlend_Opaque);
    GxRsSet(GxRs_Culling, GxBlend_Opaque);
    GxRsSet(GxRs_Blend, GxBlend_Alpha);
  }
  for ( i = this->m_textBlock.m_lines.m_count; i != 0; CGxString::RenderTexture(this, i, texture) )
    --i;
  if ( initGxRenderStates != 0 )
    GxRsPop();
}


// -----------------------------------------------------------------------------
// RVA 0x001C84D0  EA 0x5C84D0
// DIA: CGxString::RenderTexture
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::RenderTexture(CGxString *this, unsigned int line, unsigned int texture)
{
  unsigned int m_count; // eax
  TSGrowableArray<IGXUTEXTLINE *> *p_m_lines; // esi
  int v6; // eax
  const char *v7; // eax
  IGXUTEXTLINE *v8; // eax
  _DWORD *v9; // esi
  unsigned int v10; // eax
  int v11; // eax
  const char *v12; // eax
  int v13; // [esp-18h] [ebp-24h]
  int v14; // [esp-18h] [ebp-24h]
  int v15; // [esp-14h] [ebp-20h]
  int v16; // [esp-14h] [ebp-20h]
  unsigned int v17; // [esp-10h] [ebp-1Ch]
  unsigned int v18; // [esp-10h] [ebp-1Ch]
  const char *v19; // [esp-Ch] [ebp-18h]
  const char *v20; // [esp-Ch] [ebp-18h]

  m_count = this->m_textBlock.m_lines.m_count;
  p_m_lines = &this->m_textBlock.m_lines;
  if ( (int)line < (int)m_count )
  {
    if ( line >= m_count )
    {
      v6 = ((int (__thiscall *)(TSGrowableArray<IGXUTEXTLINE *> *, int, int, char *, unsigned int, unsigned int))p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemLineNo)(
             &this->m_textBlock.m_lines,
             1,
             1,
             aIndex0x08xArra,
             line,
             this->m_textBlock.m_lines.m_count);
      v7 = (const char *)((int (__thiscall *)(TSGrowableArray<IGXUTEXTLINE *> *, int))p_m_lines->TSBaseArray<IGXUTEXTLINE *>::MemFileName)(
                           p_m_lines,
                           v6);
      SErrDisplayErrorFmt(0x85100080, v7, v13, v15, v17, v19);
    }
    v8 = p_m_lines->m_data[line];
    if ( v8 != nullptr && (signed int)texture < (signed int)v8->m_texturePages.m_count )
    {
      v9 = &v8->m_texturePages.__vftable;
      v10 = v8->m_texturePages.m_count;
      if ( texture >= v10 )
      {
        v11 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, unsigned int))(*v9 + 4))(
                v9,
                1,
                1,
                aIndex0x08xArra,
                texture,
                v10);
        v12 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v9)(v9, v11);
        SErrDisplayErrorFmt(0x85100080, v12, v14, v16, v18, v20);
      }
      TEXTLINETEXTURE::InternalRenderTexture(
        *(TEXTLINETEXTURE **)(v9[3] + 4 * texture),
        texture,
        this->m_currentFace,
        this->m_flags & 1,
        &this->m_shadowColor,
        &this->m_shadowOffset,
        &this->m_fontColor);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C8590  EA 0x5C8590
// DIA: TEXTLINETEXTURE::InternalRenderTexture
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __userpurge TEXTLINETEXTURE::InternalRenderTexture(
        TEXTLINETEXTURE *this@<ecx>,
        double X@<st0>,
        unsigned int textureNum,
        CGxFont *face,
        unsigned __int8 showShadow,
        NTempest::CImVector *shadowColor,
        const NTempest::C2Vector *shadowOffset,
        NTempest::CImVector *fontColor)
{
  CGxTex *m_texture; // esi
  NTempest::CImVector *m_data; // ebx
  float y; // edx
  double v12; // st7
  int v13; // eax
  TSGrowableArray_<VERT,1199076678,58> *p_m_vert; // esi
  int v15; // eax
  const char *v16; // eax
  int v17; // eax
  const char *v18; // eax
  TSGrowableArray_<VERT,1199076678,58> *v19; // esi
  int v20; // eax
  const char *v21; // eax
  int v22; // eax
  const char *v23; // eax
  int v24; // [esp-10h] [ebp-74h]
  int v25; // [esp-10h] [ebp-74h]
  int v26; // [esp-10h] [ebp-74h]
  int v27; // [esp-10h] [ebp-74h]
  int v28; // [esp-Ch] [ebp-70h]
  int v29; // [esp-Ch] [ebp-70h]
  int v30; // [esp-Ch] [ebp-70h]
  int v31; // [esp-Ch] [ebp-70h]
  unsigned int v32; // [esp-8h] [ebp-6Ch]
  unsigned int v33; // [esp-8h] [ebp-6Ch]
  unsigned int v34; // [esp-8h] [ebp-6Ch]
  unsigned int v35; // [esp-8h] [ebp-6Ch]
  const char *v36; // [esp-4h] [ebp-68h]
  const char *v37; // [esp-4h] [ebp-68h]
  const char *v38; // [esp-4h] [ebp-68h]
  const char *v39; // [esp-4h] [ebp-68h]
  NTempest::C44Matrix mat; // [esp+14h] [ebp-50h] BYREF
  NTempest::C3Vector translate; // [esp+54h] [ebp-10h] BYREF
  unsigned int numVerts; // [esp+60h] [ebp-4h]
  unsigned int colorStride; // [esp+6Ch] [ebp+8h]
  unsigned int shadowStride; // [esp+70h] [ebp+Ch]
  NTempest::CImVector *colorPointer; // [esp+78h] [ebp+14h]

  if ( face == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3564, aFace, 0, 1u);
  m_texture = face->m_textureCache[textureNum].m_texture;
  if ( m_texture == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_239, 3566, aTexture, 0, 1u);
  GxRsSet(GxRs_Texture0, m_texture);
  CGxFont::UpdateTextures(face);
  numVerts = this->m_vert.m_count;
  if ( numVerts != 0 )
  {
    if ( (__S60__3__InternalRenderTexture_TEXTLINETEXTURE__QAEXHPAUCGxFont___NABVCImVector_NTempest__ABVC2Vector_5_2_Z_4EA
        & 1) == 0 )
    {
      __S60__3__InternalRenderTexture_TEXTLINETEXTURE__QAEXHPAUCGxFont___NABVCImVector_NTempest__ABVC2Vector_5_2_Z_4EA |= 1u;
      normal_5.x = 0.0;
      normal_5.y = 0.0;
      normal_5.z = -1.0;
      atexit(_E61_56);
    }
    if ( this->m_shadowColors.m_count != 0 )
    {
      m_data = this->m_shadowColors.m_data;
      shadowStride = 4;
    }
    else
    {
      m_data = shadowColor;
      shadowStride = 0;
    }
    if ( this->m_colors.m_count != 0 )
    {
      colorStride = 4;
      colorPointer = this->m_colors.m_data;
    }
    else
    {
      colorStride = 0;
      colorPointer = fontColor;
    }
    if ( showShadow != 0 )
    {
      y = shadowOffset->y;
      translate.x = shadowOffset->x;
      translate.y = y;
      translate.z = 0.0;
      ScreenToPixelWidth(SLODWORD(translate.x), translate.x);
      v12 = _floor(X);
      translate.x = v12;
      ScreenToPixelHeight(v13, translate.y);
      translate.y = _floor(v12);
      mat.a0 = 1.0;
      memset(&mat.a1, 0, 16);
      mat.b1 = 1.0;
      memset(&mat.b2, 0, 16);
      mat.c2 = 1.0;
      memset(&mat.c3, 0, 16);
      mat.d3 = 1.0;
      NTempest::C44Matrix::Translate(&mat, &translate);
      GxXformPush(GxXform_World, &mat);
      p_m_vert = &this->m_vert;
      if ( this->m_vert.m_count == 0 )
      {
        v15 = ((int (__thiscall *)(TSGrowableArray_<VERT,1199076678,58> *, int, int, char *, _DWORD, _DWORD))p_m_vert->TSBaseArray<VERT>::MemLineNo)(
                &this->m_vert,
                1,
                1,
                aIndex0x08xArra,
                0,
                0);
        v16 = (const char *)((int (__thiscall *)(TSGrowableArray_<VERT,1199076678,58> *, int))p_m_vert->TSGrowableArray_<VERT,1199076678,58>::MemLineNo)(
                              &this->m_vert,
                              v15);
        SErrDisplayErrorFmt(0x85100080, v16, v24, v28, v32, v36);
      }
      if ( this->m_vert.m_count == 0 )
      {
        v17 = ((int (__thiscall *)(TSGrowableArray_<VERT,1199076678,58> *, int, int, char *, _DWORD, _DWORD))p_m_vert->TSBaseArray<VERT>::MemLineNo)(
                &this->m_vert,
                1,
                1,
                aIndex0x08xArra,
                0,
                0);
        v18 = (const char *)((int (__thiscall *)(TSGrowableArray_<VERT,1199076678,58> *, int))p_m_vert->TSGrowableArray_<VERT,1199076678,58>::MemLineNo)(
                              &this->m_vert,
                              v17);
        SErrDisplayErrorFmt(0x85100080, v18, v25, v29, v33, v37);
      }
      GxPrimLockVertexPtrs(
        (const NTempest::C3Vector *)this->m_vert.m_count,
        &this->m_vert.m_data->vc,
        0x14u,
        &normal_5,
        0,
        m_data,
        shadowStride,
        nullptr,
        0,
        &this->m_vert.m_data->tc,
        0x14u,
        nullptr,
        0);
      GxPrimDrawElements(GxPrim_Triangles, this->m_vertIndices.m_count, this->m_vertIndices.m_data);
      GxPrimUnlockVertexPtrs();
      GxXformPop(GxXform_World);
    }
    v19 = &this->m_vert;
    if ( this->m_vert.m_count == 0 )
    {
      v20 = ((int (__thiscall *)(TSGrowableArray_<VERT,1199076678,58> *, int, int, char *, _DWORD, _DWORD))v19->TSBaseArray<VERT>::MemLineNo)(
              &this->m_vert,
              1,
              1,
              aIndex0x08xArra,
              0,
              0);
      v21 = (const char *)((int (__thiscall *)(TSGrowableArray_<VERT,1199076678,58> *, int))v19->TSGrowableArray_<VERT,1199076678,58>::MemLineNo)(
                            &this->m_vert,
                            v20);
      SErrDisplayErrorFmt(0x85100080, v21, v26, v30, v34, v38);
    }
    if ( this->m_vert.m_count == 0 )
    {
      v22 = ((int (__thiscall *)(TSGrowableArray_<VERT,1199076678,58> *, int, int, char *, _DWORD, _DWORD))v19->TSBaseArray<VERT>::MemLineNo)(
              &this->m_vert,
              1,
              1,
              aIndex0x08xArra,
              0,
              0);
      v23 = (const char *)((int (__thiscall *)(TSGrowableArray_<VERT,1199076678,58> *, int))v19->TSGrowableArray_<VERT,1199076678,58>::MemLineNo)(
                            &this->m_vert,
                            v22);
      SErrDisplayErrorFmt(0x85100080, v23, v27, v31, v35, v39);
    }
    GxPrimLockVertexPtrs(
      (const NTempest::C3Vector *)numVerts,
      &this->m_vert.m_data->vc,
      0x14u,
      &normal_5,
      0,
      colorPointer,
      colorStride,
      nullptr,
      0,
      &this->m_vert.m_data->tc,
      0x14u,
      nullptr,
      0);
    GxPrimDrawElements(GxPrim_Triangles, this->m_vertIndices.m_count, this->m_vertIndices.m_data);
    GxPrimUnlockVertexPtrs();
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C88C0  EA 0x5C88C0
// DIA: $E61
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E61_56()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001C88D0  EA 0x5C88D0
// DIA: CGxString::AddShadow
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall CGxString::AddShadow(
        CGxString *this,
        const NTempest::C2Vector *offset,
        const NTempest::CImVector *color)
{
  unsigned __int8 a; // al
  unsigned int v4; // edx

  this->m_shadowColor = *color;
  a = color->a;
  if ( a >= this->m_fontColor.a )
    a = this->m_fontColor.a;
  this->m_shadowColor.a = a;
  this->m_shadowOffset.x = offset->x;
  v4 = this->m_flags | 1;
  this->m_shadowOffset.y = offset->y;
  this->m_flags = v4;
  if ( (v4 & 0x1000) != 0 )
    CGxString::CreateGeometry(this);
  else
    this->m_colorGradientShadows.m_count = 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8920  EA 0x5C8920
// DIA: CGxString::AddShadowFixedGeometry
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __usercall CGxString::AddShadowFixedGeometry(CGxString *this@<ecx>, double X@<st0>)
{
  CGxString *v2; // ebx
  double v3; // st7
  int v4; // eax
  IGXUTEXTLINE *v5; // esi
  unsigned int v6; // edi
  unsigned int m_count; // eax
  _DWORD *v8; // esi
  int v9; // eax
  const char *v10; // eax
  TEXTLINETEXTURE *v11; // edx
  unsigned int v12; // ebx
  TSGrowableArray<NTempest::CImVector> *p_m_shadowColors; // esi
  unsigned int m_chunk; // eax
  unsigned int v15; // eax
  unsigned int j; // eax
  NTempest::CImVector *v17; // ecx
  signed int v18; // edi
  _DWORD *v19; // esi
  int v20; // eax
  const char *v21; // eax
  bool v22; // cf
  int v23; // [esp-18h] [ebp-44h]
  int v24; // [esp-18h] [ebp-44h]
  int v25; // [esp-14h] [ebp-40h]
  int v26; // [esp-14h] [ebp-40h]
  unsigned int v27; // [esp-10h] [ebp-3Ch]
  unsigned int v28; // [esp-10h] [ebp-3Ch]
  const char *v29; // [esp-Ch] [ebp-38h]
  const char *v30; // [esp-Ch] [ebp-38h]
  float offset3_4; // [esp+10h] [ebp-1Ch]
  TEXTLINETEXTURE *p; // [esp+18h] [ebp-14h]
  unsigned int i; // [esp+1Ch] [ebp-10h]
  unsigned int ti; // [esp+24h] [ebp-8h]
  IGXUTEXTLINE **curr; // [esp+28h] [ebp-4h]

  v2 = this;
  curr = this->m_textBlock.m_lines.m_data;
  offset3_4 = this->m_shadowOffset.y;
  ScreenToPixelWidth(LODWORD(this->m_shadowOffset.x), this->m_shadowOffset.x);
  v3 = _floor(X);
  ScreenToPixelHeight(v4, offset3_4);
  _floor(v3);
  i = 0;
  if ( v2->m_textBlock.m_lines.m_count != 0 )
  {
    do
    {
      v5 = *curr;
      v6 = 0;
      ti = 0;
      if ( (*curr)->m_texturePages.m_count != 0 )
      {
        while ( 1 )
        {
          m_count = v5->m_texturePages.m_count;
          v8 = &v5->m_texturePages.__vftable;
          if ( v6 >= m_count )
          {
            v9 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, unsigned int))(*v8 + 4))(
                   v8,
                   1,
                   1,
                   aIndex0x08xArra,
                   v6,
                   m_count);
            v10 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v8)(v8, v9);
            SErrDisplayErrorFmt(0x85100080, v10, v23, v25, v27, v29);
          }
          v11 = *(TEXTLINETEXTURE **)(v8[3] + 4 * v6);
          v12 = v11->m_vert.m_count;
          p = v11;
          if ( v11->m_shadowColors.m_count != v12 )
          {
            p_m_shadowColors = &v11->m_shadowColors;
            if ( v12 > v11->m_shadowColors.m_count )
            {
              if ( v12 > v11->m_shadowColors.m_alloc )
              {
                m_chunk = v11->m_shadowColors.m_chunk;
                if ( m_chunk == 0 )
                  m_chunk = TSGrowableArray<NTempest::CImVector>::CalcChunkSize(&v11->m_shadowColors, v12);
                v15 = TSGrowableArray<NTempest::CImVector>::RoundToChunk(p_m_shadowColors, v12, m_chunk);
                TSFixedArray<NTempest::CImVector>::ReallocData(p_m_shadowColors, v15);
                v11 = p;
              }
              for ( j = p_m_shadowColors->m_count; j < v12; ++j )
              {
                v17 = &p_m_shadowColors->m_data[j];
                if ( v17 != nullptr )
                  *v17 = 0;
              }
            }
            p_m_shadowColors->m_count = v12;
          }
          v18 = 0;
          if ( (int)v12 > 0 )
          {
            v19 = &v11->m_shadowColors.__vftable;
            do
            {
              if ( (unsigned int)v18 >= v19[2] )
              {
                v20 = (*(int (__thiscall **)(_DWORD *, int, int, char *, signed int, _DWORD))(*v19 + 4))(
                        v19,
                        1,
                        1,
                        aIndex0x08xArra,
                        v18,
                        v19[2]);
                v21 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v19)(v19, v20);
                SErrDisplayErrorFmt(0x85100080, v21, v24, v26, v28, v30);
              }
              *(NTempest::CImVector *)(v19[3] + 4 * v18++) = this->m_shadowColor;
            }
            while ( v18 < (int)v12 );
          }
          v5 = *curr;
          if ( ++ti >= (*curr)->m_texturePages.m_count )
            break;
          v6 = ti;
        }
        v2 = this;
      }
      v22 = ++i < v2->m_textBlock.m_lines.m_count;
      ++curr;
    }
    while ( v22 );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C8AD0  EA 0x5C8AD0
// DIA: TSGrowableArray_<VERT,1199076678,58>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSGrowableArray_<VERT,1199076678,58>::MemFileName(TSGrowableArray_<VERT,1199076678,58> *this)
{
  return TSGrowableArray_<VERT,1199076678,58>::s_name;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8AE0  EA 0x5C8AE0
// DIA: TSGrowableArray_<VERT,1199076678,58>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSGrowableArray_<VERT,1199076678,58>::MemLineNo(TSGrowableArray_<VERT,1199076678,58> *this)
{
  return 58;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8AF0  EA 0x5C8AF0
// DIA: TSFixedArray<VERT>::~TSFixedArray<VERT>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<VERT>::~TSFixedArray<VERT>(TSFixedArray<VERT> *this)
{
  VERT *m_data; // ecx

  this->__vftable = (TSFixedArray<VERT>_vtbl *)&TSFixedArray<VERT>::`vftable';
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAuvert, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C8B10  EA 0x5C8B10
// DIA: TSBaseArray<VERT>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<VERT>::MemFileName(TSBaseArray<VERT> *this)
{
  return aAuvert;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8B20  EA 0x5C8B20
// DIA: TSBaseArray<VERT>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<VERT>::MemLineNo(TSBaseArray<VERT> *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8B30  EA 0x5C8B30
// DIA: TSGrowableArray_<NTempest::CImVector,1199076678,59>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSGrowableArray_<NTempest::CImVector,1199076678,59>::MemFileName(
        TSGrowableArray_<NTempest::CImVector,1199076678,59> *this)
{
  return TSGrowableArray_<NTempest::CImVector,1199076678,59>::s_name;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8B40  EA 0x5C8B40
// DIA: TSGrowableArray_<NTempest::CImVector,1199076678,59>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSGrowableArray_<NTempest::CImVector,1199076678,59>::MemLineNo(
        TSGrowableArray_<NTempest::CImVector,1199076678,59> *this)
{
  return 59;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8B50  EA 0x5C8B50
// DIA: TSGrowableArray<NTempest::CImVector>::TSGrowableArray<NTempest::CImVector>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSGrowableArray<NTempest::CImVector> *__thiscall TSGrowableArray<NTempest::CImVector>::TSGrowableArray<NTempest::CImVector>(
        TSGrowableArray<NTempest::CImVector> *this)
{
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  this->__vftable = (TSGrowableArray<NTempest::CImVector>_vtbl *)&TSGrowableArray<NTempest::CImVector>::`vftable';
  this->m_chunk = 0;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8B70  EA 0x5C8B70
// DIA: TSGrowableArray_<NTempest::CImVector,1199076678,60>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSGrowableArray_<NTempest::CImVector,1199076678,60>::MemFileName(
        TSGrowableArray_<NTempest::CImVector,1199076678,60> *this)
{
  return TSGrowableArray_<NTempest::CImVector,1199076678,60>::s_name;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8B80  EA 0x5C8B80
// DIA: TSGrowableArray_<NTempest::CImVector,1199076678,60>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSGrowableArray_<NTempest::CImVector,1199076678,60>::MemLineNo(
        TSGrowableArray_<NTempest::CImVector,1199076678,60> *this)
{
  return 60;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8B90  EA 0x5C8B90
// DIA: TSGrowableArray_<unsigned short,1199076678,61>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSGrowableArray_<unsigned short,1199076678,61>::MemFileName(
        TSGrowableArray_<unsigned short,1199076678,61> *this)
{
  return TSGrowableArray_<unsigned short,1199076678,61>::s_name;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8BA0  EA 0x5C8BA0
// DIA: TSGrowableArray_<unsigned short,1199076678,61>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSGrowableArray_<unsigned short,1199076678,61>::MemLineNo(
        TSGrowableArray_<unsigned short,1199076678,61> *this)
{
  return 61;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8BB0  EA 0x5C8BB0
// DIA: TSGrowableArray<unsigned short>::TSGrowableArray<unsigned short>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSGrowableArray<unsigned short> *__thiscall TSGrowableArray<unsigned short>::TSGrowableArray<unsigned short>(
        TSGrowableArray<unsigned short> *this)
{
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  this->__vftable = (TSGrowableArray<unsigned short>_vtbl *)&TSGrowableArray<unsigned short>::`vftable';
  this->m_chunk = 0;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8BD0  EA 0x5C8BD0
// DIA: TSFixedArray<IGXUTEXTLINE *>::~TSFixedArray<IGXUTEXTLINE *>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<IGXUTEXTLINE *>::~TSFixedArray<IGXUTEXTLINE *>(TSFixedArray<IGXUTEXTLINE *> *this)
{
  IGXUTEXTLINE **m_data; // ecx

  this->__vftable = (TSFixedArray<IGXUTEXTLINE *>_vtbl *)&TSFixedArray<IGXUTEXTLINE *>::`vftable';
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aPauigxutextlin, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C8BF0  EA 0x5C8BF0
// DIA: TSBaseArray<IGXUTEXTLINE *>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<IGXUTEXTLINE *>::MemFileName(TSBaseArray<IGXUTEXTLINE *> *this)
{
  return aPauigxutextlin;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8C00  EA 0x5C8C00
// DIA: TSBaseArray<IGXUTEXTLINE *>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<IGXUTEXTLINE *>::MemLineNo(TSBaseArray<IGXUTEXTLINE *> *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8C10  EA 0x5C8C10
// DIA: TSExplicitList<CHARCODEDESC,24>::TSExplicitList<CHARCODEDESC,24>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSExplicitList<CHARCODEDESC,24> *__thiscall TSExplicitList<CHARCODEDESC,24>::TSExplicitList<CHARCODEDESC,24>(
        TSExplicitList<CHARCODEDESC,24> *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 24;
  this->m_terminator.m_next = (CHARCODEDESC *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8C30  EA 0x5C8C30
// DIA: TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::LinkNode(
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *this,
        CHARCODEDESC *ptr,
        unsigned int linktype,
        CHARCODEDESC *existingptr)
{
  TSLink<CHARCODEDESC> *p_m_terminator; // eax
  TSLink<CHARCODEDESC> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<CHARCODEDESC> *v8; // edx
  TSLink<CHARCODEDESC> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<CHARCODEDESC> *)((char *)ptr + this->m_linkoffset);
  else
    p_m_terminator = &this->m_terminator;
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v7 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v7 = ~m_next;
    *(_DWORD *)v7 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( existingptr != nullptr )
    v8 = (TSLink<CHARCODEDESC> *)((char *)existingptr + this->m_linkoffset);
  else
    v8 = &this->m_terminator;
  if ( linktype == 1 )
  {
    p_m_terminator->m_prevlink = v8;
    p_m_terminator->m_next = v8->m_next;
    m_linkoffset = this->m_linkoffset;
    v11 = (int)v8->m_next;
    if ( v11 > 0 )
    {
      if ( m_linkoffset < 0 )
        m_linkoffset = (char *)v8 - (char *)v8->m_prevlink->m_next;
      *(_DWORD *)(m_linkoffset + v11) = p_m_terminator;
      v8->m_next = ptr;
    }
    else
    {
      *(_DWORD *)~v11 = p_m_terminator;
      v8->m_next = ptr;
    }
  }
  else
  {
    if ( linktype != 2 )
    {
      SErrPrepareAppFatal(aStormHStplH, 2061);
      SErrDisplayAppFatal("Invalid case: %s=%u", aLinktype, linktype);
    }
    v9 = v8->m_prevlink;
    p_m_terminator->m_prevlink = v8->m_prevlink;
    p_m_terminator->m_next = v9->m_next;
    v9->m_next = ptr;
    v8->m_prevlink = p_m_terminator;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C8D00  EA 0x5C8D00
// DIA: TSFixedArray<TEXTURECACHEROW>::~TSFixedArray<TEXTURECACHEROW>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TEXTURECACHEROW>::~TSFixedArray<TEXTURECACHEROW>(TSFixedArray<TEXTURECACHEROW> *this)
{
  unsigned int m_count; // ecx
  TEXTURECACHEROW *m_data; // eax
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *p_glyphList; // ebx
  TSLink<CHARCODEDESC> *p_m_terminator; // esi
  TSLink<CHARCODEDESC> *v6; // eax
  bool v7; // cf
  TEXTURECACHEROW *v8; // edi
  unsigned int index; // [esp+4h] [ebp-8h]
  int v10; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<TEXTURECACHEROW>_vtbl *)&TSFixedArray<TEXTURECACHEROW>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v10 = 0;
    do
    {
      m_data = this->m_data;
      p_glyphList = &m_data[v10].glyphList;
      p_m_terminator = &m_data[v10].glyphList.m_terminator;
      while ( (int)p_m_terminator->m_next > 0 )
      {
        v6 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(p_glyphList, p_m_terminator->m_next);
        TSLink<CHARCODEDESC>::Unlink(v6);
      }
      TSLink<CHARCODEDESC>::Unlink(p_m_terminator);
      v7 = ++index < this->m_count;
      ++v10;
    }
    while ( v7 );
  }
  v8 = this->m_data;
  if ( v8 != nullptr )
    SMemFree(v8, aAutexturecache, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C8D90  EA 0x5C8D90
// DIA: TSBaseArray<TEXTURECACHEROW>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TEXTURECACHEROW>::MemFileName(TSBaseArray<TEXTURECACHEROW> *this)
{
  return aAutexturecache;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8DA0  EA 0x5C8DA0
// DIA: TSBaseArray<TEXTURECACHEROW>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TEXTURECACHEROW>::MemLineNo(TSBaseArray<TEXTURECACHEROW> *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8DB0  EA 0x5C8DB0
// DIA: TSLink<CGxString>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<CGxString>::Unlink(TSLink<CGxString> *this)
{
  TSLink<CGxString> *m_prevlink; // esi
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
// RVA 0x001C8DF0  EA 0x5C8DF0
// DIA: TSFixedArray<GXUFONTHYPERLINKINFO>::~TSFixedArray<GXUFONTHYPERLINKINFO>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<GXUFONTHYPERLINKINFO>::~TSFixedArray<GXUFONTHYPERLINKINFO>(
        TSFixedArray<GXUFONTHYPERLINKINFO> *this)
{
  GXUFONTHYPERLINKINFO *m_data; // ecx

  this->__vftable = (TSFixedArray<GXUFONTHYPERLINKINFO>_vtbl *)&TSFixedArray<GXUFONTHYPERLINKINFO>::`vftable';
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAugxufonthyper, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C8E10  EA 0x5C8E10
// DIA: TSBaseArray<GXUFONTHYPERLINKINFO>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<GXUFONTHYPERLINKINFO>::MemFileName(TSBaseArray<GXUFONTHYPERLINKINFO> *this)
{
  return aAugxufonthyper;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8E20  EA 0x5C8E20
// DIA: TSBaseArray<GXUFONTHYPERLINKINFO>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<GXUFONTHYPERLINKINFO>::MemLineNo(TSBaseArray<GXUFONTHYPERLINKINFO> *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8E30  EA 0x5C8E30
// DIA: TSFixedArray<NTempest::CImVector *>::~TSFixedArray<NTempest::CImVector *>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<NTempest::CImVector *>::~TSFixedArray<NTempest::CImVector *>(
        TSFixedArray<NTempest::CImVector *> *this)
{
  NTempest::CImVector **m_data; // ecx

  this->__vftable = (TSFixedArray<NTempest::CImVector *>_vtbl *)&TSFixedArray<NTempest::CImVector *>::`vftable';
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aPavcimvectorNt, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C8E50  EA 0x5C8E50
// DIA: TSBaseArray<NTempest::CImVector *>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<NTempest::CImVector *>::MemFileName(TSBaseArray<NTempest::CImVector *> *this)
{
  return aPavcimvectorNt;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8E60  EA 0x5C8E60
// DIA: TSBaseArray<NTempest::CImVector *>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<NTempest::CImVector *>::MemLineNo(TSBaseArray<NTempest::CImVector *> *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8E70  EA 0x5C8E70
// DIA: TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalDelete
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalDelete(
        TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *this,
        GLYPHBITMAPDATA *ptr)
{
  GLYPHBITMAPDATA::~GLYPHBITMAPDATA(ptr);
  SMemFree(ptr, aAuglyphbitmapd, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C8EA0  EA 0x5C8EA0
// DIA: TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalNew
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
GLYPHBITMAPDATA *__thiscall TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalNew(
        TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *this,
        TSExplicitList<GLYPHBITMAPDATA,-572662307> *listptr,
        unsigned int extrabytes,
        unsigned int flags)
{
  GLYPHBITMAPDATA *v4; // eax
  GLYPHBITMAPDATA *result; // eax
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v6; // ebx
  TSLink<GLYPHBITMAPDATA> *p_m_terminator; // ecx
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // edi
  int m_next; // esi
  int v10; // esi
  int v11; // esi
  int m_linkoffset; // edi

  v4 = (GLYPHBITMAPDATA *)SMemAlloc(extrabytes + 92, aAuglyphbitmapd, -2, flags | 8);
  if ( v4 != nullptr )
  {
    result = GLYPHBITMAPDATA::GLYPHBITMAPDATA(v4);
    if ( result != nullptr )
    {
      v6 = listptr;
      p_m_terminator = (TSLink<GLYPHBITMAPDATA> *)((char *)result + listptr->m_linkoffset);
      goto LABEL_6;
    }
  }
  else
  {
    result = nullptr;
  }
  v6 = listptr;
  p_m_terminator = &listptr->m_terminator;
LABEL_6:
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v10 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v10 = ~m_next;
    *(_DWORD *)v10 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  p_m_terminator->m_prevlink = &v6->m_terminator;
  p_m_terminator->m_next = v6->m_terminator.m_next;
  v11 = (int)v6->m_terminator.m_next;
  m_linkoffset = v6->m_linkoffset;
  if ( v11 > 0 )
  {
    if ( m_linkoffset < 0 )
      m_linkoffset = (char *)&v6->m_terminator - (char *)v6->m_terminator.m_prevlink->m_next;
    *(_DWORD *)(m_linkoffset + v11) = p_m_terminator;
    v6->m_terminator.m_next = result;
  }
  else
  {
    *(_DWORD *)~v11 = p_m_terminator;
    v6->m_terminator.m_next = result;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C8F50  EA 0x5C8F50
// DIA: TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::Destroy(TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *this)
{
  TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *v1; // edi
  int m_next; // eax
  unsigned int v3; // ebx
  _DWORD *v4; // esi
  int v5; // eax
  const char *v6; // eax
  int v7; // eax
  const char *v8; // eax
  int v9; // edi
  int v10; // eax
  TSLink<GLYPHBITMAPDATA> *v11; // edi
  unsigned int m_count; // eax
  unsigned int v13; // esi
  int v14; // ebx
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *m_data; // eax
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
    TSLink<GLYPHBITMAPDATA>::Unlink((TSLink<GLYPHBITMAPDATA> *)(m_next + v1->m_fulllist.m_linkoffset));
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
        v11 = (TSLink<GLYPHBITMAPDATA> *)(v24 + v9);
        if ( v10 != 0 )
        {
          TSLink<GLYPHBITMAPDATA>::NextLink(v11, -1)->m_prevlink = v11->m_prevlink;
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
  v1->m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v14 = 0;
    do
    {
      TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::~TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>(&v1->m_slotlistarray.m_data[v14]);
      ++v13;
      ++v14;
    }
    while ( v13 < v1->m_slotlistarray.m_count );
  }
  m_data = v1->m_slotlistarray.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_47, -2, 0);
  v1->m_slotlistarray.m_alloc = 0;
  v1->m_slotlistarray.m_count = 0;
  v1->m_slotlistarray.m_data = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001C90C0  EA 0x5C90C0
// DIA: TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::MemFileName(
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *this)
{
  return aAvTsexplicitli_47;
}


// -----------------------------------------------------------------------------
// RVA 0x001C90D0  EA 0x5C90D0
// DIA: TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::MemLineNo(
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C90E0  EA 0x5C90E0
// DIA: TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalDelete
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalDelete(
        TSHashTable<CHARCODEDESC,HASHKEY_NONE> *this,
        CHARCODEDESC *ptr)
{
  TSLink<CHARCODEDESC> *p_textureRowLink; // esi
  TSLink<CHARCODEDESC> *p_m_linktoslot; // esi

  TSLink<CHARCODEDESC>::Unlink(&ptr->fontGlyphLink);
  p_textureRowLink = &ptr->textureRowLink;
  if ( ptr->textureRowLink.m_prevlink != nullptr )
  {
    TSLink<CHARCODEDESC>::NextLink(&ptr->textureRowLink, -1)->m_prevlink = p_textureRowLink->m_prevlink;
    p_textureRowLink->m_prevlink->m_next = ptr->textureRowLink.m_next;
    p_textureRowLink->m_prevlink = nullptr;
    ptr->textureRowLink.m_next = nullptr;
  }
  TSLink<CHARCODEDESC>::Unlink(&ptr->m_linktofull);
  p_m_linktoslot = &ptr->m_linktoslot;
  if ( ptr->m_linktoslot.m_prevlink != nullptr )
  {
    TSLink<CHARCODEDESC>::NextLink(&ptr->m_linktoslot, -1)->m_prevlink = p_m_linktoslot->m_prevlink;
    p_m_linktoslot->m_prevlink->m_next = ptr->m_linktoslot.m_next;
    p_m_linktoslot->m_prevlink = nullptr;
    ptr->m_linktoslot.m_next = nullptr;
  }
  SMemFree(ptr, aAucharcodedesc, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C9170  EA 0x5C9170
// DIA: TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalNew
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CHARCODEDESC *__thiscall TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalNew(
        TSHashTable<CHARCODEDESC,HASHKEY_NONE> *this,
        TSExplicitList<CHARCODEDESC,-572662307> *listptr,
        unsigned int extrabytes,
        unsigned int flags)
{
  CHARCODEDESC *result; // eax
  TSLink<CHARCODEDESC> *p_m_terminator; // ecx
  TSLink<CHARCODEDESC> *m_prevlink; // ebx
  int m_next; // esi
  int v8; // esi
  int m_linkoffset; // ebx
  int v10; // esi

  result = (CHARCODEDESC *)SMemAlloc(extrabytes + 64, aAucharcodedesc, -2, flags | 8);
  if ( result != nullptr )
  {
    result->m_linktoslot.m_prevlink = nullptr;
    result->m_linktoslot.m_next = nullptr;
    result->m_linktofull.m_prevlink = nullptr;
    result->m_linktofull.m_next = nullptr;
    result->textureRowLink.m_prevlink = nullptr;
    result->textureRowLink.m_next = nullptr;
    result->fontGlyphLink.m_prevlink = nullptr;
    result->fontGlyphLink.m_next = nullptr;
    result->textureNumber = -1;
    result->rowNumber = -1;
    result->glyphStartPixel = -1;
    result->dataValid = 0;
    result->glyphEndPixel = 0;
    result->bitmapData = nullptr;
    p_m_terminator = (TSLink<CHARCODEDESC> *)((char *)result + listptr->m_linkoffset);
  }
  else
  {
    result = nullptr;
    p_m_terminator = &listptr->m_terminator;
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v8 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v8 = ~m_next;
    *(_DWORD *)v8 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  p_m_terminator->m_prevlink = &listptr->m_terminator;
  p_m_terminator->m_next = listptr->m_terminator.m_next;
  m_linkoffset = listptr->m_linkoffset;
  v10 = (int)listptr->m_terminator.m_next;
  if ( v10 > 0 )
  {
    if ( m_linkoffset < 0 )
      m_linkoffset = (char *)&listptr->m_terminator - (char *)listptr->m_terminator.m_prevlink->m_next;
    *(_DWORD *)(m_linkoffset + v10) = p_m_terminator;
    listptr->m_terminator.m_next = result;
  }
  else
  {
    *(_DWORD *)~v10 = p_m_terminator;
    listptr->m_terminator.m_next = result;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9240  EA 0x5C9240
// DIA: TSHashTable<CHARCODEDESC,HASHKEY_NONE>::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CHARCODEDESC,HASHKEY_NONE>::Destroy(TSHashTable<CHARCODEDESC,HASHKEY_NONE> *this)
{
  TSHashTable<CHARCODEDESC,HASHKEY_NONE> *v1; // edi
  int m_next; // eax
  unsigned int v3; // ebx
  _DWORD *v4; // esi
  int v5; // eax
  const char *v6; // eax
  int v7; // eax
  const char *v8; // eax
  int v9; // edi
  int v10; // eax
  TSLink<CHARCODEDESC> *v11; // edi
  unsigned int m_count; // eax
  unsigned int v13; // esi
  int v14; // ebx
  TSExplicitList<CHARCODEDESC,-572662307> *m_data; // eax
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
    TSLink<CHARCODEDESC>::Unlink((TSLink<CHARCODEDESC> *)(m_next + v1->m_fulllist.m_linkoffset));
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
        v11 = (TSLink<CHARCODEDESC> *)(v24 + v9);
        if ( v10 != 0 )
        {
          TSLink<CHARCODEDESC>::NextLink(v11, -1)->m_prevlink = v11->m_prevlink;
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
  v1->m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v14 = 0;
    do
    {
      TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::~TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>(&v1->m_slotlistarray.m_data[v14]);
      ++v13;
      ++v14;
    }
    while ( v13 < v1->m_slotlistarray.m_count );
  }
  m_data = v1->m_slotlistarray.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_48, -2, 0);
  v1->m_slotlistarray.m_alloc = 0;
  v1->m_slotlistarray.m_count = 0;
  v1->m_slotlistarray.m_data = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001C93B0  EA 0x5C93B0
// DIA: TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> >::~TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::~TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>(
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *this)
{
  TSLink<CHARCODEDESC> *p_m_terminator; // ebx
  CHARCODEDESC *m_next; // edx
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // esi
  int v6; // esi
  TSLink<CHARCODEDESC> *m_prevlink; // edx
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
// RVA 0x001C9430  EA 0x5C9430
// DIA: TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::MemFileName(
        TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> > *this)
{
  return aAvTsexplicitli_48;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9440  EA 0x5C9440
// DIA: TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::MemLineNo(
        TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> > *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9450  EA 0x5C9450
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalDelete
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalDelete(
        TSHashTable<KERNNODE,KERNINGHASHKEY> *this,
        KERNNODE *ptr)
{
  TSLink<KERNNODE> *p_m_linktofull; // esi
  TSLink<KERNNODE> *p_m_linktoslot; // esi

  p_m_linktofull = &ptr->m_linktofull;
  if ( ptr->m_linktofull.m_prevlink != nullptr )
  {
    TSLink<KERNNODE>::NextLink(&ptr->m_linktofull, -1)->m_prevlink = p_m_linktofull->m_prevlink;
    ptr->m_linktofull.m_prevlink->m_next = ptr->m_linktofull.m_next;
    p_m_linktofull->m_prevlink = nullptr;
    ptr->m_linktofull.m_next = nullptr;
  }
  p_m_linktoslot = &ptr->m_linktoslot;
  if ( ptr->m_linktoslot.m_prevlink != nullptr )
  {
    TSLink<KERNNODE>::NextLink(&ptr->m_linktoslot, -1)->m_prevlink = p_m_linktoslot->m_prevlink;
    p_m_linktoslot->m_prevlink->m_next = ptr->m_linktoslot.m_next;
    p_m_linktoslot->m_prevlink = nullptr;
    ptr->m_linktoslot.m_next = nullptr;
  }
  SMemFree(ptr, aAukernnode, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C94D0  EA 0x5C94D0
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalNew
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
KERNNODE *__thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalNew(
        TSHashTable<KERNNODE,KERNINGHASHKEY> *this,
        TSExplicitList<KERNNODE,-572662307> *listptr,
        unsigned int extrabytes,
        unsigned int flags)
{
  KERNNODE *result; // eax
  TSLink<KERNNODE> *p_m_terminator; // ecx
  TSLink<KERNNODE> *m_prevlink; // edi
  int m_next; // esi
  int v8; // esi
  int v9; // esi
  int m_linkoffset; // edi

  result = (KERNNODE *)SMemAlloc(extrabytes + 36, aAukernnode, -2, flags | 8);
  if ( result != nullptr )
  {
    result->m_linktoslot.m_prevlink = nullptr;
    result->m_linktoslot.m_next = nullptr;
    result->m_linktofull.m_prevlink = nullptr;
    result->m_linktofull.m_next = nullptr;
    result->m_key.code = 0;
    result->flags = 0;
    result->proporportionalSpacing = 0.0;
    result->fixedWidthSpacing = 0.0;
    p_m_terminator = (TSLink<KERNNODE> *)((char *)result + listptr->m_linkoffset);
  }
  else
  {
    result = nullptr;
    p_m_terminator = &listptr->m_terminator;
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v8 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v8 = ~m_next;
    *(_DWORD *)v8 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  p_m_terminator->m_prevlink = &listptr->m_terminator;
  p_m_terminator->m_next = listptr->m_terminator.m_next;
  v9 = (int)listptr->m_terminator.m_next;
  m_linkoffset = listptr->m_linkoffset;
  if ( v9 > 0 )
  {
    if ( m_linkoffset < 0 )
      m_linkoffset = (char *)&listptr->m_terminator - (char *)listptr->m_terminator.m_prevlink->m_next;
    *(_DWORD *)(m_linkoffset + v9) = p_m_terminator;
    listptr->m_terminator.m_next = result;
  }
  else
  {
    *(_DWORD *)~v9 = p_m_terminator;
    listptr->m_terminator.m_next = result;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9590  EA 0x5C9590
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::Destroy(TSHashTable<KERNNODE,KERNINGHASHKEY> *this)
{
  TSHashTable<KERNNODE,KERNINGHASHKEY> *v1; // edi
  int m_next; // eax
  unsigned int v3; // ebx
  _DWORD *v4; // esi
  int v5; // eax
  const char *v6; // eax
  int v7; // eax
  const char *v8; // eax
  int v9; // edi
  int v10; // eax
  TSLink<KERNNODE> *v11; // edi
  unsigned int m_count; // eax
  unsigned int v13; // esi
  int v14; // ebx
  TSExplicitList<KERNNODE,-572662307> *m_data; // eax
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
    TSLink<KERNNODE>::Unlink((TSLink<KERNNODE> *)(m_next + v1->m_fulllist.m_linkoffset));
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
        v11 = (TSLink<KERNNODE> *)(v24 + v9);
        if ( v10 != 0 )
        {
          TSLink<KERNNODE>::NextLink(v11, -1)->m_prevlink = v11->m_prevlink;
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
  v1->m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<KERNNODE,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v14 = 0;
    do
    {
      TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::~TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>(&v1->m_slotlistarray.m_data[v14]);
      ++v13;
      ++v14;
    }
    while ( v13 < v1->m_slotlistarray.m_count );
  }
  m_data = v1->m_slotlistarray.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_49, -2, 0);
  v1->m_slotlistarray.m_alloc = 0;
  v1->m_slotlistarray.m_count = 0;
  v1->m_slotlistarray.m_data = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9700  EA 0x5C9700
// DIA: TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::MemFileName(
        TSBaseArray<TSExplicitList<KERNNODE,-572662307> > *this)
{
  return aAvTsexplicitli_49;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9710  EA 0x5C9710
// DIA: TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::MemLineNo(
        TSBaseArray<TSExplicitList<KERNNODE,-572662307> > *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9720  EA 0x5C9720
// DIA: TSList<TEXTLINETEXTURE,TSGetLink<TEXTLINETEXTURE> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<TEXTLINETEXTURE,TSGetLink<TEXTLINETEXTURE>>::LinkNode(
        TSList<TEXTLINETEXTURE,TSGetLink<TEXTLINETEXTURE> > *this,
        TEXTLINETEXTURE *ptr,
        unsigned int linktype,
        TEXTLINETEXTURE *existingptr)
{
  TEXTLINETEXTURE *p_m_terminator; // eax
  TSLink<TEXTLINETEXTURE> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TEXTLINETEXTURE *v8; // edx
  TSLink<TEXTLINETEXTURE> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  p_m_terminator = ptr;
  if ( ptr == nullptr )
    p_m_terminator = (TEXTLINETEXTURE *)&this->m_terminator;
  m_prevlink = p_m_terminator->m_link.m_prevlink;
  if ( p_m_terminator->m_link.m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_link.m_next;
    if ( m_next > 0 )
      v7 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v7 = ~m_next;
    *(_DWORD *)v7 = m_prevlink;
    p_m_terminator->m_link.m_prevlink->m_next = p_m_terminator->m_link.m_next;
    p_m_terminator->m_link.m_prevlink = nullptr;
    p_m_terminator->m_link.m_next = nullptr;
  }
  v8 = existingptr;
  if ( existingptr == nullptr )
    v8 = (TEXTLINETEXTURE *)&this->m_terminator;
  if ( linktype == 1 )
  {
    p_m_terminator->m_link.m_prevlink = &v8->m_link;
    p_m_terminator->m_link.m_next = v8->m_link.m_next;
    m_linkoffset = this->m_linkoffset;
    v11 = (int)v8->m_link.m_next;
    if ( v11 > 0 )
    {
      if ( m_linkoffset < 0 )
        m_linkoffset = (char *)v8 - (char *)v8->m_link.m_prevlink->m_next;
      *(_DWORD *)(m_linkoffset + v11) = p_m_terminator;
      v8->m_link.m_next = ptr;
    }
    else
    {
      *(_DWORD *)~v11 = p_m_terminator;
      v8->m_link.m_next = ptr;
    }
  }
  else
  {
    if ( linktype != 2 )
    {
      SErrPrepareAppFatal(aStormHStplH, 2061);
      SErrDisplayAppFatal("Invalid case: %s=%u", aLinktype, linktype);
    }
    v9 = v8->m_link.m_prevlink;
    p_m_terminator->m_link.m_prevlink = v8->m_link.m_prevlink;
    p_m_terminator->m_link.m_next = v9->m_next;
    v9->m_next = ptr;
    v8->m_link.m_prevlink = &p_m_terminator->m_link;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C97F0  EA 0x5C97F0
// DIA: TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::InternalDelete
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::InternalDelete(
        TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *this,
        STRINGVIEWMATRICES *ptr)
{
  TSLink<STRINGVIEWMATRICES> *p_m_freeLink; // esi

  p_m_freeLink = &ptr->m_freeLink;
  if ( ptr->m_freeLink.m_prevlink != nullptr )
  {
    TSLink<STRINGVIEWMATRICES>::NextLink(&ptr->m_freeLink, -1)->m_prevlink = p_m_freeLink->m_prevlink;
    ptr->m_freeLink.m_prevlink->m_next = ptr->m_freeLink.m_next;
    p_m_freeLink->m_prevlink = nullptr;
    ptr->m_freeLink.m_next = nullptr;
  }
  TSLink<STRINGVIEWMATRICES>::Unlink(&ptr->m_linktofull);
  TSLink<STRINGVIEWMATRICES>::Unlink(&ptr->m_linktoslot);
  SMemFree(ptr, aAustringviewma, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001C9850  EA 0x5C9850
// DIA: TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::InternalNew
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
STRINGVIEWMATRICES *__thiscall TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::InternalNew(
        TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *this,
        TSExplicitList<STRINGVIEWMATRICES,-572662307> *listptr,
        unsigned int extrabytes,
        unsigned int flags)
{
  _DWORD *v4; // eax
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v5; // ebx
  TSLink<STRINGVIEWMATRICES> *p_m_terminator; // edi
  unsigned int flagsa; // [esp+1Ch] [ebp+10h]

  v4 = SMemAlloc(extrabytes + 160, aAustringviewma, -2, flags | 8);
  if ( v4 != nullptr )
  {
    v4[1] = 0;
    v4[2] = 0;
    v4[3] = 0;
    v4[4] = 0;
    v4[5] = 0;
    v4[6] = 0;
    v4[7] = 0;
    v5 = listptr;
    v4[9] = 0;
    v4[10] = 0;
    v4[11] = 0;
    v4[12] = 0;
    v4[14] = 0;
    v4[15] = 0;
    v4[16] = 0;
    v4[17] = 0;
    v4[19] = 0;
    v4[20] = 0;
    v4[21] = 0;
    v4[22] = 0;
    v4[8] = 1065353216;
    v4[13] = 1065353216;
    v4[18] = 1065353216;
    v4[23] = 1065353216;
    v4[24] = 1065353216;
    v4[25] = 0;
    v4[26] = 0;
    v4[27] = 0;
    v4[28] = 0;
    v4[29] = 1065353216;
    v4[30] = 0;
    v4[31] = 0;
    v4[32] = 0;
    v4[33] = 0;
    v4[34] = 1065353216;
    v4[35] = 0;
    v4[36] = 0;
    v4[37] = 0;
    v4[38] = 0;
    v4[39] = 1065353216;
    flagsa = (unsigned int)v4;
    p_m_terminator = (TSLink<STRINGVIEWMATRICES> *)((char *)v4 + listptr->m_linkoffset);
  }
  else
  {
    v5 = listptr;
    flagsa = 0;
    p_m_terminator = &listptr->m_terminator;
  }
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    TSLink<STRINGVIEWMATRICES>::NextLink(p_m_terminator, -1)->m_prevlink = p_m_terminator->m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  p_m_terminator->m_prevlink = &v5->m_terminator;
  p_m_terminator->m_next = v5->m_terminator.m_next;
  TSLink<STRINGVIEWMATRICES>::NextLink(&v5->m_terminator, v5->m_linkoffset)->m_prevlink = p_m_terminator;
  v5->m_terminator.m_next = (STRINGVIEWMATRICES *)flagsa;
  return (STRINGVIEWMATRICES *)flagsa;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9970  EA 0x5C9970
// DIA: TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::Destroy(TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *this)
{
  TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *v1; // edi
  int m_next; // eax
  unsigned int v3; // ebx
  _DWORD *v4; // esi
  int v5; // eax
  const char *v6; // eax
  int v7; // eax
  const char *v8; // eax
  int v9; // edi
  int v10; // eax
  TSLink<STRINGVIEWMATRICES> *v11; // edi
  unsigned int m_count; // eax
  unsigned int v13; // esi
  int v14; // ebx
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *m_data; // eax
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
    TSLink<STRINGVIEWMATRICES>::Unlink((TSLink<STRINGVIEWMATRICES> *)(m_next + v1->m_fulllist.m_linkoffset));
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
        v11 = (TSLink<STRINGVIEWMATRICES> *)(v24 + v9);
        if ( v10 != 0 )
        {
          TSLink<STRINGVIEWMATRICES>::NextLink(v11, -1)->m_prevlink = v11->m_prevlink;
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
  v1->m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v14 = 0;
    do
    {
      TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::~TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>(&v1->m_slotlistarray.m_data[v14]);
      ++v13;
      ++v14;
    }
    while ( v13 < v1->m_slotlistarray.m_count );
  }
  m_data = v1->m_slotlistarray.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_50, -2, 0);
  v1->m_slotlistarray.m_alloc = 0;
  v1->m_slotlistarray.m_count = 0;
  v1->m_slotlistarray.m_data = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9AE0  EA 0x5C9AE0
// DIA: TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >::~TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::~TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>(
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *this)
{
  TSLink<STRINGVIEWMATRICES> *p_m_terminator; // ebx
  STRINGVIEWMATRICES *m_next; // edx
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // esi
  int v6; // esi
  TSLink<STRINGVIEWMATRICES> *m_prevlink; // edx
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
// RVA 0x001C9B60  EA 0x5C9B60
// DIA: TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::LinkNode(
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *this,
        STRINGVIEWMATRICES *ptr,
        unsigned int linktype,
        STRINGVIEWMATRICES *existingptr)
{
  TSLink<STRINGVIEWMATRICES> *p_m_terminator; // eax
  TSLink<STRINGVIEWMATRICES> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<STRINGVIEWMATRICES> *v8; // edx
  TSLink<STRINGVIEWMATRICES> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<STRINGVIEWMATRICES> *)((char *)ptr + this->m_linkoffset);
  else
    p_m_terminator = &this->m_terminator;
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v7 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v7 = ~m_next;
    *(_DWORD *)v7 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( existingptr != nullptr )
    v8 = (TSLink<STRINGVIEWMATRICES> *)((char *)existingptr + this->m_linkoffset);
  else
    v8 = &this->m_terminator;
  if ( linktype == 1 )
  {
    p_m_terminator->m_prevlink = v8;
    p_m_terminator->m_next = v8->m_next;
    m_linkoffset = this->m_linkoffset;
    v11 = (int)v8->m_next;
    if ( v11 > 0 )
    {
      if ( m_linkoffset < 0 )
        m_linkoffset = (char *)v8 - (char *)v8->m_prevlink->m_next;
      *(_DWORD *)(m_linkoffset + v11) = p_m_terminator;
      v8->m_next = ptr;
    }
    else
    {
      *(_DWORD *)~v11 = p_m_terminator;
      v8->m_next = ptr;
    }
  }
  else
  {
    if ( linktype != 2 )
    {
      SErrPrepareAppFatal(aStormHStplH, 2061);
      SErrDisplayAppFatal("Invalid case: %s=%u", aLinktype, linktype);
    }
    v9 = v8->m_prevlink;
    p_m_terminator->m_prevlink = v8->m_prevlink;
    p_m_terminator->m_next = v9->m_next;
    v9->m_next = ptr;
    v8->m_prevlink = p_m_terminator;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001C9C30  EA 0x5C9C30
// DIA: TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::MemFileName(
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *this)
{
  return aAvTsexplicitli_50;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9C40  EA 0x5C9C40
// DIA: TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::MemLineNo(
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9C50  EA 0x5C9C50
// DIA: VERT::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
VERT *__thiscall VERT::`scalar deleting destructor'(VERT *this, char __flags)
{
  if ( (__flags & 1) != 0 && this != nullptr )
    SMemFree(this, filename, -1, 0);
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9C80  EA 0x5C9C80
// DIA: TEXTURECACHEROW::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TEXTURECACHEROW *__thiscall TEXTURECACHEROW::`scalar deleting destructor'(TEXTURECACHEROW *this, char __flags)
{
  TEXTURECACHEROW *v2; // esi
  TSLink<CHARCODEDESC> *p_m_terminator; // edi
  int m_next; // ecx
  int m_linkoffset; // eax
  int v6; // esi
  _DWORD *v7; // eax
  int v8; // edx
  int v9; // edx
  TSLink<CHARCODEDESC> *m_prevlink; // edx
  int v11; // ecx
  int v12; // ecx

  v2 = this;
  p_m_terminator = &this->glyphList.m_terminator;
  while ( 1 )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next <= 0 )
      break;
    m_linkoffset = v2->glyphList.m_linkoffset;
    v6 = *(_DWORD *)(m_linkoffset + m_next);
    v7 = (_DWORD *)(m_next + m_linkoffset);
    if ( v6 != 0 )
    {
      v8 = v7[1];
      if ( v8 > 0 )
        v9 = (int)v7 + v8 - *(_DWORD *)(v6 + 4);
      else
        v9 = ~v8;
      *(_DWORD *)v9 = v6;
      *(_DWORD *)(*v7 + 4) = v7[1];
      *v7 = 0;
      v7[1] = 0;
    }
    v2 = this;
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    v11 = (int)p_m_terminator->m_next;
    if ( v11 > 0 )
      v12 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v11;
    else
      v12 = ~v11;
    *(_DWORD *)v12 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( (__flags & 1) != 0 && v2 != nullptr )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9D30  EA 0x5C9D30
// DIA: TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *__thiscall TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::`vector deleting destructor'(
        TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *this,
        char __flags)
{
  TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *v2; // esi
  unsigned int v3; // ebx
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *p_m_fulllist; // edi
  TSLink<GLYPHBITMAPDATA> *v5; // eax
  TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *p_m_slotlistarray; // esi
  int m_next; // edi
  TSLink<GLYPHBITMAPDATA> **p_m_prevlink; // edi
  int v9; // eax
  int v12; // [esp+10h] [ebp-4h]

  v2 = this;
  v3 = 0;
  this->__vftable = (TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>_vtbl *)&TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::`vftable';
  this->m_fullnessIndicator = 0;
  p_m_fulllist = &this->m_fulllist;
  while ( (int)p_m_fulllist->m_terminator.m_next > 0 )
  {
    v5 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(
           p_m_fulllist,
           p_m_fulllist->m_terminator.m_next);
    TSLink<GLYPHBITMAPDATA>::Unlink(v5);
  }
  if ( v2->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &v2->m_slotlistarray;
    v12 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        m_next = (int)p_m_slotlistarray->m_data[v12].m_terminator.m_next;
        if ( m_next <= 0 )
          break;
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        TSLink<GLYPHBITMAPDATA>::Unlink((TSLink<GLYPHBITMAPDATA> *)(m_next + p_m_slotlistarray->m_data[v12].m_linkoffset));
      }
      ++v3;
      ++v12;
    }
    while ( v3 < this->m_slotlistarray.m_count );
    v2 = this;
  }
  TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::~TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>(&v2->m_slotlistarray);
  p_m_prevlink = &v2->m_fulllist.m_terminator.m_prevlink;
  while ( 1 )
  {
    v9 = (int)v2->m_fulllist.m_terminator.m_next;
    if ( v9 <= 0 )
      break;
    TSLink<GLYPHBITMAPDATA>::Unlink((TSLink<GLYPHBITMAPDATA> *)(v9 + v2->m_fulllist.m_linkoffset));
  }
  if ( *p_m_prevlink != nullptr )
  {
    TSLink<GLYPHBITMAPDATA>::NextLink(&v2->m_fulllist.m_terminator, -1)->m_prevlink = *p_m_prevlink;
    (*p_m_prevlink)->m_next = v2->m_fulllist.m_terminator.m_next;
    *p_m_prevlink = nullptr;
    v2->m_fulllist.m_terminator.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9E20  EA 0x5C9E20
// DIA: TSHashTable<CHARCODEDESC,HASHKEY_NONE>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashTable<CHARCODEDESC,HASHKEY_NONE> *__thiscall TSHashTable<CHARCODEDESC,HASHKEY_NONE>::`scalar deleting destructor'(
        TSHashTable<CHARCODEDESC,HASHKEY_NONE> *this,
        char __flags)
{
  TSHashTable<CHARCODEDESC,HASHKEY_NONE> *v2; // esi
  unsigned int v3; // ebx
  TSExplicitList<CHARCODEDESC,-572662307> *p_m_fulllist; // edi
  TSLink<CHARCODEDESC> *v5; // eax
  TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> > *p_m_slotlistarray; // esi
  int m_next; // edi
  TSLink<CHARCODEDESC> **p_m_prevlink; // edi
  int v9; // eax
  int v12; // [esp+10h] [ebp-4h]

  v2 = this;
  v3 = 0;
  this->__vftable = (TSHashTable<CHARCODEDESC,HASHKEY_NONE>_vtbl *)&TSHashTable<CHARCODEDESC,HASHKEY_NONE>::`vftable';
  this->m_fullnessIndicator = 0;
  p_m_fulllist = &this->m_fulllist;
  while ( (int)p_m_fulllist->m_terminator.m_next > 0 )
  {
    v5 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(p_m_fulllist, p_m_fulllist->m_terminator.m_next);
    TSLink<CHARCODEDESC>::Unlink(v5);
  }
  if ( v2->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &v2->m_slotlistarray;
    v12 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        m_next = (int)p_m_slotlistarray->m_data[v12].m_terminator.m_next;
        if ( m_next <= 0 )
          break;
        TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        TSLink<CHARCODEDESC>::Unlink((TSLink<CHARCODEDESC> *)(m_next + p_m_slotlistarray->m_data[v12].m_linkoffset));
      }
      ++v3;
      ++v12;
    }
    while ( v3 < this->m_slotlistarray.m_count );
    v2 = this;
  }
  TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>::~TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>(&v2->m_slotlistarray);
  p_m_prevlink = &v2->m_fulllist.m_terminator.m_prevlink;
  while ( 1 )
  {
    v9 = (int)v2->m_fulllist.m_terminator.m_next;
    if ( v9 <= 0 )
      break;
    TSLink<CHARCODEDESC>::Unlink((TSLink<CHARCODEDESC> *)(v9 + v2->m_fulllist.m_linkoffset));
  }
  if ( *p_m_prevlink != nullptr )
  {
    TSLink<CHARCODEDESC>::NextLink(&v2->m_fulllist.m_terminator, -1)->m_prevlink = *p_m_prevlink;
    (*p_m_prevlink)->m_next = v2->m_fulllist.m_terminator.m_next;
    *p_m_prevlink = nullptr;
    v2->m_fulllist.m_terminator.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001C9F10  EA 0x5C9F10
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashTable<KERNNODE,KERNINGHASHKEY> *__thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::`scalar deleting destructor'(
        TSHashTable<KERNNODE,KERNINGHASHKEY> *this,
        char __flags)
{
  TSHashTable<KERNNODE,KERNINGHASHKEY> *v2; // esi
  unsigned int v3; // ebx
  TSExplicitList<KERNNODE,-572662307> *p_m_fulllist; // edi
  TSLink<KERNNODE> *v5; // eax
  TSBaseArray<TSExplicitList<KERNNODE,-572662307> > *p_m_slotlistarray; // esi
  int m_next; // edi
  TSLink<KERNNODE> **p_m_prevlink; // edi
  int v9; // eax
  int v12; // [esp+10h] [ebp-4h]

  v2 = this;
  v3 = 0;
  this->__vftable = (TSHashTable<KERNNODE,KERNINGHASHKEY>_vtbl *)&TSHashTable<KERNNODE,KERNINGHASHKEY>::`vftable';
  this->m_fullnessIndicator = 0;
  p_m_fulllist = &this->m_fulllist;
  while ( (int)p_m_fulllist->m_terminator.m_next > 0 )
  {
    v5 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(p_m_fulllist, p_m_fulllist->m_terminator.m_next);
    TSLink<KERNNODE>::Unlink(v5);
  }
  if ( v2->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &v2->m_slotlistarray;
    v12 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        m_next = (int)p_m_slotlistarray->m_data[v12].m_terminator.m_next;
        if ( m_next <= 0 )
          break;
        TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        TSLink<KERNNODE>::Unlink((TSLink<KERNNODE> *)(m_next + p_m_slotlistarray->m_data[v12].m_linkoffset));
      }
      ++v3;
      ++v12;
    }
    while ( v3 < this->m_slotlistarray.m_count );
    v2 = this;
  }
  TSFixedArray<TSExplicitList<KERNNODE,-572662307>>::~TSFixedArray<TSExplicitList<KERNNODE,-572662307>>(&v2->m_slotlistarray);
  p_m_prevlink = &v2->m_fulllist.m_terminator.m_prevlink;
  while ( 1 )
  {
    v9 = (int)v2->m_fulllist.m_terminator.m_next;
    if ( v9 <= 0 )
      break;
    TSLink<KERNNODE>::Unlink((TSLink<KERNNODE> *)(v9 + v2->m_fulllist.m_linkoffset));
  }
  if ( *p_m_prevlink != nullptr )
  {
    TSLink<KERNNODE>::NextLink(&v2->m_fulllist.m_terminator, -1)->m_prevlink = *p_m_prevlink;
    (*p_m_prevlink)->m_next = v2->m_fulllist.m_terminator.m_next;
    *p_m_prevlink = nullptr;
    v2->m_fulllist.m_terminator.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CA000  EA 0x5CA000
// DIA: TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *__thiscall TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::`vector deleting destructor'(
        TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *this,
        char __flags)
{
  TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *v2; // esi
  unsigned int v3; // ebx
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *p_m_fulllist; // edi
  TSLink<STRINGVIEWMATRICES> *v5; // eax
  TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *p_m_slotlistarray; // esi
  int m_next; // edi
  TSLink<STRINGVIEWMATRICES> **p_m_prevlink; // edi
  int v9; // eax
  int v12; // [esp+10h] [ebp-4h]

  v2 = this;
  v3 = 0;
  this->__vftable = (TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>_vtbl *)&TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::`vftable';
  this->m_fullnessIndicator = 0;
  p_m_fulllist = &this->m_fulllist;
  while ( (int)p_m_fulllist->m_terminator.m_next > 0 )
  {
    v5 = TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::Link(
           p_m_fulllist,
           p_m_fulllist->m_terminator.m_next);
    TSLink<STRINGVIEWMATRICES>::Unlink(v5);
  }
  if ( v2->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &v2->m_slotlistarray;
    v12 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        m_next = (int)p_m_slotlistarray->m_data[v12].m_terminator.m_next;
        if ( m_next <= 0 )
          break;
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        TSLink<STRINGVIEWMATRICES>::Unlink((TSLink<STRINGVIEWMATRICES> *)(m_next
                                                                        + p_m_slotlistarray->m_data[v12].m_linkoffset));
      }
      ++v3;
      ++v12;
    }
    while ( v3 < this->m_slotlistarray.m_count );
    v2 = this;
  }
  TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::~TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>(&v2->m_slotlistarray);
  p_m_prevlink = &v2->m_fulllist.m_terminator.m_prevlink;
  while ( 1 )
  {
    v9 = (int)v2->m_fulllist.m_terminator.m_next;
    if ( v9 <= 0 )
      break;
    TSLink<STRINGVIEWMATRICES>::Unlink((TSLink<STRINGVIEWMATRICES> *)(v9 + v2->m_fulllist.m_linkoffset));
  }
  if ( *p_m_prevlink != nullptr )
  {
    TSLink<STRINGVIEWMATRICES>::NextLink(&v2->m_fulllist.m_terminator, -1)->m_prevlink = *p_m_prevlink;
    (*p_m_prevlink)->m_next = v2->m_fulllist.m_terminator.m_next;
    *p_m_prevlink = nullptr;
    v2->m_fulllist.m_terminator.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 )
    SMemFree((void *)v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CA0F0  EA 0x5CA0F0
// DIA: CHARCODEDESC::~CHARCODEDESC
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __thiscall CHARCODEDESC::~CHARCODEDESC(CHARCODEDESC *this)
{
  TSLink<CHARCODEDESC> *p_fontGlyphLink; // eax
  TSLink<CHARCODEDESC> *m_prevlink; // edi
  int m_next; // esi
  int v4; // esi
  TSLink<CHARCODEDESC> *v5; // edi
  TSLink<CHARCODEDESC> *p_textureRowLink; // eax
  int v7; // esi
  int v8; // esi
  TSLink<CHARCODEDESC> *v9; // edi
  TSLink<CHARCODEDESC> *p_m_linktofull; // eax
  int v11; // esi
  int v12; // esi
  TSLink<CHARCODEDESC> *v13; // esi
  TSLink<CHARCODEDESC> *p_m_linktoslot; // eax
  int v15; // edx

  p_fontGlyphLink = &this->fontGlyphLink;
  m_prevlink = this->fontGlyphLink.m_prevlink;
  if ( m_prevlink != nullptr )
  {
    m_next = (int)this->fontGlyphLink.m_next;
    if ( m_next > 0 )
      v4 = (char *)p_fontGlyphLink - (char *)m_prevlink->m_next + m_next;
    else
      v4 = ~m_next;
    *(_DWORD *)v4 = m_prevlink;
    p_fontGlyphLink->m_prevlink->m_next = this->fontGlyphLink.m_next;
    p_fontGlyphLink->m_prevlink = nullptr;
    this->fontGlyphLink.m_next = nullptr;
  }
  v5 = this->textureRowLink.m_prevlink;
  p_textureRowLink = &this->textureRowLink;
  if ( v5 != nullptr )
  {
    v7 = (int)this->textureRowLink.m_next;
    if ( v7 > 0 )
      v8 = (char *)p_textureRowLink - (char *)v5->m_next + v7;
    else
      v8 = ~v7;
    *(_DWORD *)v8 = v5;
    p_textureRowLink->m_prevlink->m_next = this->textureRowLink.m_next;
    p_textureRowLink->m_prevlink = nullptr;
    this->textureRowLink.m_next = nullptr;
  }
  v9 = this->m_linktofull.m_prevlink;
  p_m_linktofull = &this->m_linktofull;
  if ( v9 != nullptr )
  {
    v11 = (int)this->m_linktofull.m_next;
    if ( v11 > 0 )
      v12 = (char *)p_m_linktofull - (char *)v9->m_next + v11;
    else
      v12 = ~v11;
    *(_DWORD *)v12 = v9;
    p_m_linktofull->m_prevlink->m_next = this->m_linktofull.m_next;
    p_m_linktofull->m_prevlink = nullptr;
    this->m_linktofull.m_next = nullptr;
  }
  v13 = this->m_linktoslot.m_prevlink;
  p_m_linktoslot = &this->m_linktoslot;
  if ( v13 != nullptr )
  {
    v15 = (int)this->m_linktoslot.m_next;
    if ( v15 > 0 )
      *(_DWORD *)((char *)p_m_linktoslot - (char *)v13->m_next + v15) = v13;
    else
      *(_DWORD *)~v15 = v13;
    this->m_linktoslot.m_prevlink->m_next = this->m_linktoslot.m_next;
    p_m_linktoslot->m_prevlink = nullptr;
    this->m_linktoslot.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CA1C0  EA 0x5CA1C0
// DIA: TSLink<TEXTLINETEXTURE>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TEXTLINETEXTURE *__thiscall TSLink<TEXTLINETEXTURE>::NextLink(TSLink<TEXTLINETEXTURE> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TEXTLINETEXTURE *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TEXTLINETEXTURE *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001CA1F0  EA 0x5CA1F0
// DIA: TSGrowableArray<VERT>::Reserve
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<VERT>::Reserve(TSGrowableArray<VERT> *this, unsigned int count, int round)
{
  unsigned int m_count; // esi
  unsigned int m_chunk; // ecx
  int i; // eax
  TSGrowableArray<VERT>_vtbl *v7; // edx
  const char *v8; // eax
  VERT *v9; // eax
  const char *v10; // eax
  VERT *v11; // edi
  int v12; // edx
  VERT *v13; // eax
  const char *v14; // eax
  int v15; // [esp-8h] [ebp-14h]
  int v16; // [esp-8h] [ebp-14h]
  int v17; // [esp-8h] [ebp-14h]
  VERT *counta; // [esp+14h] [ebp+8h]
  int rounda; // [esp+18h] [ebp+Ch]

  m_count = this->m_count + count;
  if ( m_count > this->m_alloc )
  {
    if ( round != 0 )
    {
      m_chunk = this->m_chunk;
      if ( m_chunk == 0 )
      {
        m_chunk = m_count;
        if ( m_count >= 0xC )
        {
          this->m_chunk = 12;
          m_chunk = 12;
        }
        else
        {
          for ( i = m_count & (m_count - 1); i != 0; i &= i - 1 )
            m_chunk = i;
          if ( m_chunk == 0 )
            m_chunk = 1;
        }
      }
      if ( m_count % m_chunk != 0 )
        m_count += m_chunk - m_count % m_chunk;
    }
    v7 = this->__vftable;
    counta = this->m_data;
    this->m_alloc = m_count;
    v15 = v7->TSBaseArray<VERT>::MemLineNo(this);
    v8 = (const char *)this->TSBaseArray<VERT>::MemFileName(this);
    v9 = (VERT *)SMemReAlloc(counta, 20 * m_count, v8, v15, 0x10u);
    this->m_data = v9;
    if ( v9 == nullptr )
    {
      v16 = this->TSBaseArray<VERT>::MemLineNo(this);
      v10 = (const char *)this->TSBaseArray<VERT>::MemFileName(this);
      v11 = counta;
      this->m_data = (VERT *)SMemAlloc(20 * m_count, v10, v16, 0);
      if ( counta != nullptr )
      {
        if ( m_count >= this->m_count )
          m_count = this->m_count;
        if ( m_count != 0 )
        {
          v12 = 0;
          for ( rounda = m_count; rounda != 0; --rounda )
          {
            v13 = &this->m_data[v12];
            if ( v13 != nullptr )
            {
              qmemcpy(v13, &v11[v12], sizeof(VERT));
              v11 = counta;
            }
            ++v12;
          }
        }
        v17 = this->TSBaseArray<VERT>::MemLineNo(this);
        v14 = (const char *)this->TSBaseArray<VERT>::MemFileName(this);
        SMemFree(v11, v14, v17, 0);
      }
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CA300  EA 0x5CA300
// DIA: TSFixedArray<VERT>::TSFixedArray<VERT>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSFixedArray<VERT> *__thiscall TSFixedArray<VERT>::TSFixedArray<VERT>(TSFixedArray<VERT> *this)
{
  this->__vftable = (TSFixedArray<VERT>_vtbl *)&TSFixedArray<VERT>::`vftable';
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CA320  EA 0x5CA320
// DIA: TSLink<IGXUTEXTLINE>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
IGXUTEXTLINE *__thiscall TSLink<IGXUTEXTLINE>::NextLink(TSLink<IGXUTEXTLINE> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (IGXUTEXTLINE *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (IGXUTEXTLINE *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001CA350  EA 0x5CA350
// DIA: TSFixedArray<IGXUTEXTLINE *>::TSFixedArray<IGXUTEXTLINE *>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSFixedArray<IGXUTEXTLINE *> *__thiscall TSFixedArray<IGXUTEXTLINE *>::TSFixedArray<IGXUTEXTLINE *>(
        TSFixedArray<IGXUTEXTLINE *> *this)
{
  this->__vftable = (TSFixedArray<IGXUTEXTLINE *>_vtbl *)&TSFixedArray<IGXUTEXTLINE *>::`vftable';
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CA370  EA 0x5CA370
// DIA: TSLink<GLYPHBITMAPDATA>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<GLYPHBITMAPDATA>::Unlink(TSLink<GLYPHBITMAPDATA> *this)
{
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // esi
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
// RVA 0x001CA3B0  EA 0x5CA3B0
// DIA: TSLink<CHARCODEDESC>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<CHARCODEDESC> *__thiscall TSLink<CHARCODEDESC>::NextLink(TSLink<CHARCODEDESC> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<CHARCODEDESC> *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<CHARCODEDESC> *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001CA3E0  EA 0x5CA3E0
// DIA: TSLink<CHARCODEDESC>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<CHARCODEDESC>::Unlink(TSLink<CHARCODEDESC> *this)
{
  TSLink<CHARCODEDESC> *m_prevlink; // esi
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
// RVA 0x001CA420  EA 0x5CA420
// DIA: TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<CHARCODEDESC> *__thiscall TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *this,
        const CHARCODEDESC *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<CHARCODEDESC> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CA440  EA 0x5CA440
// DIA: TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> >::TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *__thiscall TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>(
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 0;
  this->m_terminator.m_next = (CHARCODEDESC *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CA460  EA 0x5CA460
// DIA: TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> >::UnlinkAll
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::UnlinkAll(
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *this)
{
  CHARCODEDESC *m_next; // edx
  int v2; // edi
  _DWORD *v3; // eax
  int v4; // esi
  int v5; // esi

  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v2 = *(unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    v3 = (unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    if ( v2 != 0 )
    {
      v4 = v3[1];
      if ( v4 > 0 )
        v5 = (int)v3 + v4 - *(_DWORD *)(v2 + 4);
      else
        v5 = ~v4;
      *(_DWORD *)v5 = v2;
      *(_DWORD *)(*v3 + 4) = v3[1];
      *v3 = 0;
      v3[1] = 0;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CA4B0  EA 0x5CA4B0
// DIA: TSLink<CGxString>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CGxString *__thiscall TSLink<CGxString>::NextLink(TSLink<CGxString> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (CGxString *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (CGxString *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001CA4E0  EA 0x5CA4E0
// DIA: TSLink<KERNNODE>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<KERNNODE>::Unlink(TSLink<KERNNODE> *this)
{
  TSLink<KERNNODE> *m_prevlink; // esi
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
// RVA 0x001CA520  EA 0x5CA520
// DIA: TSLink<CGxFont>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<CGxFont>::Unlink(TSLink<CGxFont> *this)
{
  TSLink<CGxFont> *m_prevlink; // esi
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
// RVA 0x001CA560  EA 0x5CA560
// DIA: TSList<CGxString,TSGetExplicitLink<CGxString> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<CGxString> *__thiscall TSList<CGxString,TSGetExplicitLink<CGxString>>::Link(
        TSList<CGxString,TSGetExplicitLink<CGxString> > *this,
        const CGxString *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<CGxString> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CA580  EA 0x5CA580
// DIA: TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalClear
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalClear(
        TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *this,
        int warn)
{
  unsigned int v3; // ebx
  GLYPHBITMAPDATA *m_next; // eax
  TSLink<GLYPHBITMAPDATA> *v5; // esi
  TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *p_m_slotlistarray; // esi
  int v7; // eax
  const char *v8; // eax
  GLYPHBITMAPDATA *v9; // edi
  int v10; // eax
  const char *v11; // eax
  int m_linkoffset; // eax
  int v13; // ecx
  _DWORD *v14; // eax
  int v15; // edi
  int v16; // edi
  int v17; // [esp-18h] [ebp-2Ch]
  int v18; // [esp-18h] [ebp-2Ch]
  int v19; // [esp-14h] [ebp-28h]
  int v20; // [esp-14h] [ebp-28h]
  unsigned int v21; // [esp-10h] [ebp-24h]
  unsigned int v22; // [esp-10h] [ebp-24h]
  const char *v23; // [esp-Ch] [ebp-20h]
  const char *v24; // [esp-Ch] [ebp-20h]
  int v26; // [esp+10h] [ebp-4h]

  v3 = 0;
  this->m_fullnessIndicator = 0;
  while ( 1 )
  {
    m_next = this->m_fulllist.m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v5 = (TSLink<GLYPHBITMAPDATA> *)((char *)m_next + this->m_fulllist.m_linkoffset);
    if ( v5->m_prevlink != nullptr )
    {
      TSLink<GLYPHBITMAPDATA>::NextLink(v5, -1)->m_prevlink = v5->m_prevlink;
      v5->m_prevlink->m_next = v5->m_next;
      v5->m_prevlink = nullptr;
      v5->m_next = nullptr;
    }
  }
  if ( this->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &this->m_slotlistarray;
    v26 = 0;
    do
    {
      while ( 1 )
      {
        if ( v3 >= p_m_slotlistarray->m_count )
        {
          v7 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo)(
                 p_m_slotlistarray,
                 1,
                 1,
                 aIndex0x08xArra,
                 v3,
                 p_m_slotlistarray->m_count);
          v8 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName)(
                               p_m_slotlistarray,
                               v7);
          SErrDisplayErrorFmt(0x85100080, v8, v17, v19, v21, v23);
        }
        v9 = p_m_slotlistarray->m_data[v26].m_terminator.m_next;
        if ( (int)v9 <= 0 )
          break;
        if ( warn != 0 )
        {
          if ( v3 >= p_m_slotlistarray->m_count )
          {
            v10 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo)(
                    p_m_slotlistarray,
                    1,
                    1,
                    aIndex0x08xArra,
                    v3,
                    p_m_slotlistarray->m_count);
            v11 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName)(
                                  p_m_slotlistarray,
                                  v10);
            SErrDisplayErrorFmt(0x85100080, v11, v18, v20, v22, v24);
          }
          m_linkoffset = p_m_slotlistarray->m_data[v26].m_linkoffset;
          v13 = *(unsigned int *)((char *)&v9->m_hashval + m_linkoffset);
          v14 = (unsigned int *)((char *)&v9->m_hashval + m_linkoffset);
          if ( v13 != 0 )
          {
            v15 = v14[1];
            if ( v15 > 0 )
              v16 = (int)v14 + v15 - *(_DWORD *)(v13 + 4);
            else
              v16 = ~v15;
            *(_DWORD *)v16 = v13;
            *(_DWORD *)(*v14 + 4) = v14[1];
            *v14 = 0;
            v14[1] = 0;
          }
        }
        else
        {
          this->TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalDelete(this, v9);
        }
      }
      ++v26;
      ++v3;
    }
    while ( v3 < this->m_slotlistarray.m_count );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CA6C0  EA 0x5CA6C0
// DIA: TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalLinkNode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::InternalLinkNode(
        TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *this,
        GLYPHBITMAPDATA *ptr,
        unsigned int hashval)
{
  unsigned int v4; // edi
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *m_data; // esi
  int m_linkoffset; // eax
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *v7; // esi
  TSLink<GLYPHBITMAPDATA> *v8; // eax
  unsigned int v9; // esi
  unsigned int m_fullnessIndicator; // eax
  GLYPHBITMAPDATA *m_next; // edi
  unsigned int v12; // eax
  TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *p_m_slotlistarray; // edi
  int v14; // eax
  const char *v15; // eax
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v16; // edi
  GLYPHBITMAPDATA *v17; // ecx
  TSLink<GLYPHBITMAPDATA> *p_m_terminator; // esi
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // eax
  int v20; // [esp-18h] [ebp-28h]
  int v21; // [esp-14h] [ebp-24h]
  unsigned int v22; // [esp-10h] [ebp-20h]
  const char *v23; // [esp-Ch] [ebp-1Ch]
  unsigned int v24; // [esp-4h] [ebp-14h]
  int v25; // [esp+Ch] [ebp-4h]

  if ( this->m_slotmask == -1 )
  {
    this->m_slotmask = 3;
    TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::SetCount(&this->m_slotlistarray, 4u);
    v4 = 0;
    v25 = 0;
    do
    {
      TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v4);
      m_data = this->m_slotlistarray.m_data;
      m_linkoffset = m_data[v25].m_linkoffset;
      v7 = &m_data[v25];
      if ( m_linkoffset != 4 )
      {
        while ( (int)v7->m_terminator.m_next > 0 )
        {
          v8 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(v7, v7->m_terminator.m_next);
          TSLink<GLYPHBITMAPDATA>::Unlink(v8);
        }
        v7->m_linkoffset = 4;
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::InitializeTerminator(v7);
      }
      ++v4;
      ++v25;
    }
    while ( v4 <= this->m_slotmask );
  }
  v9 = hashval & this->m_slotmask;
  if ( this->m_slotmask < 0x1FFF )
  {
    m_fullnessIndicator = this->m_fullnessIndicator;
    this->m_fullnessIndicator = m_fullnessIndicator <= 3 ? 0 : m_fullnessIndicator - 3;
    TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v9);
    m_next = (int)this->m_slotlistarray.m_data[v9].m_terminator.m_next <= 0
           ? nullptr
           : this->m_slotlistarray.m_data[v9].m_terminator.m_next;
    if ( (int)m_next > 0 )
    {
      while ( 1 )
      {
        v12 = this->m_fullnessIndicator + 1;
        this->m_fullnessIndicator = v12;
        if ( v12 > 0xD )
          break;
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v9);
        m_next = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(
                   &this->m_slotlistarray.m_data[v9],
                   m_next)->m_next;
        if ( (int)m_next <= 0 )
          goto LABEL_17;
      }
      v24 = 2 * this->m_slotmask + 2;
      this->m_fullnessIndicator = 0;
      TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::GrowListArray(this, v24);
      v9 = this->m_slotmask & hashval;
    }
  }
LABEL_17:
  p_m_slotlistarray = &this->m_slotlistarray;
  if ( v9 >= this->m_slotlistarray.m_count )
  {
    v14 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo)(
            &this->m_slotlistarray,
            1,
            1,
            aIndex0x08xArra,
            v9,
            this->m_slotlistarray.m_count);
    v15 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName)(
                          &this->m_slotlistarray,
                          v14);
    SErrDisplayErrorFmt(0x85100080, v15, v20, v21, v22, v23);
  }
  v16 = &this->m_slotlistarray.m_data[v9];
  v17 = ptr;
  if ( ptr != nullptr )
    p_m_terminator = (TSLink<GLYPHBITMAPDATA> *)((char *)ptr + v16->m_linkoffset);
  else
    p_m_terminator = &v16->m_terminator;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    TSLink<GLYPHBITMAPDATA>::Unlink(p_m_terminator);
    v17 = ptr;
  }
  m_prevlink = v16->m_terminator.m_prevlink;
  p_m_terminator->m_prevlink = m_prevlink;
  p_m_terminator->m_next = m_prevlink->m_next;
  m_prevlink->m_next = v17;
  v16->m_terminator.m_prevlink = p_m_terminator;
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::LinkNode(&this->m_fulllist, v17, 2u, nullptr);
}


// -----------------------------------------------------------------------------
// RVA 0x001CA860  EA 0x5CA860
// DIA: TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >::~TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::~TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>(
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *this)
{
  TSLink<GLYPHBITMAPDATA> *p_m_terminator; // ebx
  GLYPHBITMAPDATA *m_next; // edx
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // esi
  int v6; // esi
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // edx
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
// RVA 0x001CA8E0  EA 0x5CA8E0
// DIA: TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >::ChangeLinkOffset
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::ChangeLinkOffset(
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *this,
        int linkoffset)
{
  TSLink<GLYPHBITMAPDATA> *p_m_terminator; // ebx
  GLYPHBITMAPDATA *m_next; // edx
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
    this->m_terminator.m_next = (GLYPHBITMAPDATA *)~(unsigned int)p_m_terminator;
    this->m_linkoffset = linkoffset;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CA940  EA 0x5CA940
// DIA: TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::~TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::~TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>(
        TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *this)
{
  unsigned int m_count; // ecx
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v3; // esi
  TSLink<GLYPHBITMAPDATA> *v4; // eax
  bool v5; // cf
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *m_data; // ebx
  unsigned int index; // [esp+4h] [ebp-8h]
  int v8; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v8 = 0;
    do
    {
      v3 = &this->m_data[v8];
      while ( (int)v3->m_terminator.m_next > 0 )
      {
        v4 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(v3, v3->m_terminator.m_next);
        TSLink<GLYPHBITMAPDATA>::Unlink(v4);
      }
      TSLink<GLYPHBITMAPDATA>::Unlink(&v3->m_terminator);
      v5 = ++index < this->m_count;
      ++v8;
    }
    while ( v5 );
  }
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_47, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001CA9D0  EA 0x5CA9D0
// DIA: TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalClear
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalClear(
        TSHashTable<CHARCODEDESC,HASHKEY_NONE> *this,
        int warn)
{
  TSHashTable<CHARCODEDESC,HASHKEY_NONE> *v2; // edx
  int m_next; // eax
  int m_linkoffset; // esi
  TSLink<CHARCODEDESC> *v5; // edi
  TSLink<CHARCODEDESC> *v6; // esi
  TSLink<CHARCODEDESC> *Link; // eax
  unsigned int v8; // ebx
  _DWORD *v9; // esi
  int v10; // eax
  const char *v11; // eax
  int v12; // edi
  int v13; // eax
  const char *v14; // eax
  int v15; // eax
  int v16; // ecx
  _DWORD *v17; // eax
  int v18; // edi
  int v19; // edi
  int v20; // [esp-18h] [ebp-2Ch]
  int v21; // [esp-18h] [ebp-2Ch]
  int v22; // [esp-14h] [ebp-28h]
  int v23; // [esp-14h] [ebp-28h]
  unsigned int v24; // [esp-10h] [ebp-24h]
  unsigned int v25; // [esp-10h] [ebp-24h]
  const char *v26; // [esp-Ch] [ebp-20h]
  const char *v27; // [esp-Ch] [ebp-20h]
  int v28; // [esp+Ch] [ebp-8h]

  v2 = this;
  this->m_fullnessIndicator = 0;
  while ( 1 )
  {
    m_next = (int)v2->m_fulllist.m_terminator.m_next;
    if ( m_next <= 0 )
      break;
    m_linkoffset = v2->m_fulllist.m_linkoffset;
    v5 = *(TSLink<CHARCODEDESC> **)(m_linkoffset + m_next);
    v6 = (TSLink<CHARCODEDESC> *)(m_next + m_linkoffset);
    if ( v5 != nullptr )
    {
      Link = TSLink<CHARCODEDESC>::NextLink(v6, -1);
      v2 = this;
      Link->m_prevlink = v5;
      v6->m_prevlink->m_next = v6->m_next;
      v6->m_prevlink = nullptr;
      v6->m_next = nullptr;
    }
  }
  v8 = 0;
  if ( v2->m_slotlistarray.m_count != 0 )
  {
    v9 = &v2->m_slotlistarray.__vftable;
    v28 = 0;
    do
    {
      while ( 1 )
      {
        if ( v8 >= v9[2] )
        {
          v10 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, _DWORD))(*v9 + 4))(
                  v9,
                  1,
                  1,
                  aIndex0x08xArra,
                  v8,
                  v9[2]);
          v11 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v9)(v9, v10);
          SErrDisplayErrorFmt(0x85100080, v11, v20, v22, v24, v26);
          v2 = this;
        }
        v12 = *(_DWORD *)(v9[3] + v28 + 8);
        if ( v12 <= 0 )
          break;
        if ( warn != 0 )
        {
          if ( v8 >= v9[2] )
          {
            v13 = (*(int (__thiscall **)(_DWORD *, int, int, char *, unsigned int, _DWORD))(*v9 + 4))(
                    v9,
                    1,
                    1,
                    aIndex0x08xArra,
                    v8,
                    v9[2]);
            v14 = (const char *)(*(int (__thiscall **)(_DWORD *, int))*v9)(v9, v13);
            SErrDisplayErrorFmt(0x85100080, v14, v21, v23, v25, v27);
            v2 = this;
          }
          v15 = *(_DWORD *)(v28 + v9[3]);
          v16 = *(_DWORD *)(v15 + v12);
          v17 = (_DWORD *)(v12 + v15);
          if ( v16 != 0 )
          {
            v18 = v17[1];
            if ( v18 > 0 )
            {
              v19 = (int)v17 + v18 - *(_DWORD *)(v16 + 4);
              v2 = this;
            }
            else
            {
              v19 = ~v18;
            }
            *(_DWORD *)v19 = v16;
            *(_DWORD *)(*v17 + 4) = v17[1];
            *v17 = 0;
            v17[1] = 0;
          }
        }
        else
        {
          v2->TSHashTable<CHARCODEDESC,HASHKEY_NONE>::InternalDelete(v2, (CHARCODEDESC *)v12);
          v2 = this;
        }
      }
      ++v8;
      v28 += 12;
    }
    while ( v8 < v2->m_slotlistarray.m_count );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CAB30  EA 0x5CAB30
// DIA: TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> >::TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *__thiscall TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>(
        TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *this)
{
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  this->__vftable = (TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> >_vtbl *)&TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::`vftable';
  this->m_chunk = 0;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CAB50  EA 0x5CAB50
// DIA: TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307> >::~TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>::~TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>(
        TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307> > *this)
{
  unsigned int m_count; // ecx
  TSExplicitList<CHARCODEDESC,-572662307> *v3; // esi
  TSLink<CHARCODEDESC> *v4; // eax
  bool v5; // cf
  TSExplicitList<CHARCODEDESC,-572662307> *m_data; // ebx
  unsigned int index; // [esp+4h] [ebp-8h]
  int v8; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v8 = 0;
    do
    {
      v3 = &this->m_data[v8];
      while ( (int)v3->m_terminator.m_next > 0 )
      {
        v4 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(v3, v3->m_terminator.m_next);
        TSLink<CHARCODEDESC>::Unlink(v4);
      }
      TSLink<CHARCODEDESC>::Unlink(&v3->m_terminator);
      v5 = ++index < this->m_count;
      ++v8;
    }
    while ( v5 );
  }
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_48, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001CABE0  EA 0x5CABE0
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalClear
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalClear(
        TSHashTable<KERNNODE,KERNINGHASHKEY> *this,
        int warn)
{
  unsigned int v3; // ebx
  KERNNODE *m_next; // eax
  TSLink<KERNNODE> *v5; // esi
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *p_m_slotlistarray; // esi
  int v7; // eax
  const char *v8; // eax
  KERNNODE *v9; // edi
  int v10; // eax
  const char *v11; // eax
  int m_linkoffset; // eax
  int v13; // ecx
  _DWORD *v14; // eax
  int v15; // edi
  int v16; // edi
  int v17; // [esp-18h] [ebp-2Ch]
  int v18; // [esp-18h] [ebp-2Ch]
  int v19; // [esp-14h] [ebp-28h]
  int v20; // [esp-14h] [ebp-28h]
  unsigned int v21; // [esp-10h] [ebp-24h]
  unsigned int v22; // [esp-10h] [ebp-24h]
  const char *v23; // [esp-Ch] [ebp-20h]
  const char *v24; // [esp-Ch] [ebp-20h]
  int v26; // [esp+10h] [ebp-4h]

  v3 = 0;
  this->m_fullnessIndicator = 0;
  while ( 1 )
  {
    m_next = this->m_fulllist.m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v5 = (TSLink<KERNNODE> *)((char *)m_next + this->m_fulllist.m_linkoffset);
    if ( v5->m_prevlink != nullptr )
    {
      TSLink<KERNNODE>::NextLink(v5, -1)->m_prevlink = v5->m_prevlink;
      v5->m_prevlink->m_next = v5->m_next;
      v5->m_prevlink = nullptr;
      v5->m_next = nullptr;
    }
  }
  if ( this->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &this->m_slotlistarray;
    v26 = 0;
    do
    {
      while ( 1 )
      {
        if ( v3 >= p_m_slotlistarray->m_count )
        {
          v7 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo)(
                 p_m_slotlistarray,
                 1,
                 1,
                 aIndex0x08xArra,
                 v3,
                 p_m_slotlistarray->m_count);
          v8 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName)(
                               p_m_slotlistarray,
                               v7);
          SErrDisplayErrorFmt(0x85100080, v8, v17, v19, v21, v23);
        }
        v9 = p_m_slotlistarray->m_data[v26].m_terminator.m_next;
        if ( (int)v9 <= 0 )
          break;
        if ( warn != 0 )
        {
          if ( v3 >= p_m_slotlistarray->m_count )
          {
            v10 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo)(
                    p_m_slotlistarray,
                    1,
                    1,
                    aIndex0x08xArra,
                    v3,
                    p_m_slotlistarray->m_count);
            v11 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName)(
                                  p_m_slotlistarray,
                                  v10);
            SErrDisplayErrorFmt(0x85100080, v11, v18, v20, v22, v24);
          }
          m_linkoffset = p_m_slotlistarray->m_data[v26].m_linkoffset;
          v13 = *(unsigned int *)((char *)&v9->m_hashval + m_linkoffset);
          v14 = (unsigned int *)((char *)&v9->m_hashval + m_linkoffset);
          if ( v13 != 0 )
          {
            v15 = v14[1];
            if ( v15 > 0 )
              v16 = (int)v14 + v15 - *(_DWORD *)(v13 + 4);
            else
              v16 = ~v15;
            *(_DWORD *)v16 = v13;
            *(_DWORD *)(*v14 + 4) = v14[1];
            *v14 = 0;
            v14[1] = 0;
          }
        }
        else
        {
          this->TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalDelete(this, v9);
        }
      }
      ++v26;
      ++v3;
    }
    while ( v3 < this->m_slotlistarray.m_count );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CAD20  EA 0x5CAD20
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalNewNode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
KERNNODE *__thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalNewNode(
        TSHashTable<KERNNODE,KERNINGHASHKEY> *this,
        unsigned int hashval,
        unsigned int extrabytes,
        unsigned int flags)
{
  unsigned int v5; // ebx
  TSExplicitList<KERNNODE,-572662307> *m_data; // edi
  int m_linkoffset; // eax
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *v8; // edi
  TSLink<KERNNODE> *p_m_terminator; // ebx
  TSLink<KERNNODE> *v10; // eax
  unsigned int v11; // edi
  unsigned int m_fullnessIndicator; // eax
  KERNNODE *m_next; // ebx
  unsigned int v14; // eax
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *p_m_slotlistarray; // ebx
  int v16; // eax
  const char *v17; // eax
  KERNNODE *v18; // eax
  KERNNODE *v19; // ebx
  TSLink<KERNNODE> *v20; // edi
  TSLink<KERNNODE> *m_prevlink; // eax
  int v23; // [esp-18h] [ebp-2Ch]
  int v24; // [esp-14h] [ebp-28h]
  unsigned int v25; // [esp-10h] [ebp-24h]
  const char *v26; // [esp-Ch] [ebp-20h]
  unsigned int v27; // [esp-4h] [ebp-18h]
  unsigned int v28; // [esp+Ch] [ebp-8h]
  int v29; // [esp+10h] [ebp-4h]

  if ( this->m_slotmask == -1 )
  {
    this->m_slotmask = 3;
    TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::SetCount(&this->m_slotlistarray, 4u);
    v5 = 0;
    v28 = 0;
    v29 = 0;
    do
    {
      TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v5);
      m_data = this->m_slotlistarray.m_data;
      m_linkoffset = m_data[v29].m_linkoffset;
      v8 = &m_data[v29];
      if ( m_linkoffset != 4 )
      {
        p_m_terminator = &v8->m_terminator;
        while ( (int)v8->m_terminator.m_next > 0 )
        {
          v10 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(v8, v8->m_terminator.m_next);
          TSLink<KERNNODE>::Unlink(v10);
        }
        p_m_terminator->m_prevlink = p_m_terminator;
        v8->m_terminator.m_next = (KERNNODE *)~(unsigned int)p_m_terminator;
        v5 = v28;
        v8->m_linkoffset = 4;
      }
      v28 = ++v5;
      ++v29;
    }
    while ( v5 <= this->m_slotmask );
  }
  v11 = hashval & this->m_slotmask;
  if ( this->m_slotmask < 0x1FFF )
  {
    m_fullnessIndicator = this->m_fullnessIndicator;
    this->m_fullnessIndicator = m_fullnessIndicator <= 3 ? 0 : m_fullnessIndicator - 3;
    TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v11);
    m_next = (int)this->m_slotlistarray.m_data[v11].m_terminator.m_next <= 0
           ? nullptr
           : this->m_slotlistarray.m_data[v11].m_terminator.m_next;
    if ( (int)m_next > 0 )
    {
      while ( 1 )
      {
        v14 = this->m_fullnessIndicator + 1;
        this->m_fullnessIndicator = v14;
        if ( v14 > 0xD )
          break;
        TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v11);
        m_next = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(&this->m_slotlistarray.m_data[v11], m_next)->m_next;
        if ( (int)m_next <= 0 )
          goto LABEL_18;
      }
      v27 = 2 * this->m_slotmask + 2;
      this->m_fullnessIndicator = 0;
      TSHashTable<KERNNODE,KERNINGHASHKEY>::GrowListArray(this, v27);
      v11 = this->m_slotmask & hashval;
    }
  }
LABEL_18:
  p_m_slotlistarray = &this->m_slotlistarray;
  if ( v11 >= this->m_slotlistarray.m_count )
  {
    v16 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo)(
            &this->m_slotlistarray,
            1,
            1,
            aIndex0x08xArra,
            v11,
            this->m_slotlistarray.m_count);
    v17 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName)(
                          &this->m_slotlistarray,
                          v16);
    SErrDisplayErrorFmt(0x85100080, v17, v23, v24, v25, v26);
  }
  v18 = this->TSHashTable<KERNNODE,KERNINGHASHKEY>::InternalNew(
          this,
          &this->m_slotlistarray.m_data[v11],
          extrabytes,
          flags);
  v19 = v18;
  if ( v18 != nullptr )
    v20 = (TSLink<KERNNODE> *)((char *)v18 + this->m_fulllist.m_linkoffset);
  else
    v20 = &this->m_fulllist.m_terminator;
  if ( v20->m_prevlink != nullptr )
  {
    TSLink<KERNNODE>::NextLink(v20, -1)->m_prevlink = v20->m_prevlink;
    v20->m_prevlink->m_next = v20->m_next;
    v20->m_prevlink = nullptr;
    v20->m_next = nullptr;
  }
  m_prevlink = this->m_fulllist.m_terminator.m_prevlink;
  v20->m_prevlink = m_prevlink;
  v20->m_next = m_prevlink->m_next;
  m_prevlink->m_next = v19;
  this->m_fulllist.m_terminator.m_prevlink = v20;
  return v19;
}


// -----------------------------------------------------------------------------
// RVA 0x001CAEF0  EA 0x5CAEF0
// DIA: TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >::~TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::~TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>(
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *this)
{
  TSLink<KERNNODE> *p_m_terminator; // ebx
  KERNNODE *m_next; // edx
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // esi
  int v6; // esi
  TSLink<KERNNODE> *m_prevlink; // edx
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
// RVA 0x001CAF70  EA 0x5CAF70
// DIA: TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >::ChangeLinkOffset
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::ChangeLinkOffset(
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *this,
        int linkoffset)
{
  TSLink<KERNNODE> *p_m_terminator; // ebx
  KERNNODE *m_next; // edx
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
    this->m_terminator.m_next = (KERNNODE *)~(unsigned int)p_m_terminator;
    this->m_linkoffset = linkoffset;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CAFD0  EA 0x5CAFD0
// DIA: TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >::TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *__thiscall TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>(
        TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *this)
{
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  this->__vftable = (TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >_vtbl *)&TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::`vftable';
  this->m_chunk = 0;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CAFF0  EA 0x5CAFF0
// DIA: TSFixedArray<TSExplicitList<KERNNODE,-572662307> >::~TSFixedArray<TSExplicitList<KERNNODE,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<KERNNODE,-572662307>>::~TSFixedArray<TSExplicitList<KERNNODE,-572662307>>(
        TSFixedArray<TSExplicitList<KERNNODE,-572662307> > *this)
{
  unsigned int m_count; // ecx
  TSExplicitList<KERNNODE,-572662307> *v3; // esi
  TSLink<KERNNODE> *v4; // eax
  bool v5; // cf
  TSExplicitList<KERNNODE,-572662307> *m_data; // ebx
  unsigned int index; // [esp+4h] [ebp-8h]
  int v8; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<TSExplicitList<KERNNODE,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<KERNNODE,-572662307>>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v8 = 0;
    do
    {
      v3 = &this->m_data[v8];
      while ( (int)v3->m_terminator.m_next > 0 )
      {
        v4 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(v3, v3->m_terminator.m_next);
        TSLink<KERNNODE>::Unlink(v4);
      }
      TSLink<KERNNODE>::Unlink(&v3->m_terminator);
      v5 = ++index < this->m_count;
      ++v8;
    }
    while ( v5 );
  }
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_49, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001CB080  EA 0x5CB080
// DIA: TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR>::TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR> *__thiscall TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR>::TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR>(
        TSHashObject<STRINGVIEWMATRICES,HASHKEY_PTR> *this)
{
  this->m_linktoslot.m_prevlink = nullptr;
  this->m_linktoslot.m_next = nullptr;
  this->m_linktofull.m_prevlink = nullptr;
  this->m_linktofull.m_next = nullptr;
  this->m_key.m_key = nullptr;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB0A0  EA 0x5CB0A0
// DIA: TSLink<STRINGVIEWMATRICES>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<STRINGVIEWMATRICES> *__thiscall TSLink<STRINGVIEWMATRICES>::NextLink(
        TSLink<STRINGVIEWMATRICES> *this,
        int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<STRINGVIEWMATRICES> *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<STRINGVIEWMATRICES> *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001CB0D0  EA 0x5CB0D0
// DIA: TSLink<STRINGVIEWMATRICES>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<STRINGVIEWMATRICES>::Unlink(TSLink<STRINGVIEWMATRICES> *this)
{
  TSLink<STRINGVIEWMATRICES> *m_prevlink; // esi
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
// RVA 0x001CB110  EA 0x5CB110
// DIA: TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<STRINGVIEWMATRICES> *__thiscall TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::Link(
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *this,
        const STRINGVIEWMATRICES *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<STRINGVIEWMATRICES> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB130  EA 0x5CB130
// DIA: TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::~TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::~TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>(
        TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *this)
{
  unsigned int m_count; // ecx
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v3; // esi
  TSLink<STRINGVIEWMATRICES> *v4; // eax
  bool v5; // cf
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *m_data; // ebx
  unsigned int index; // [esp+4h] [ebp-8h]
  int v8; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v8 = 0;
    do
    {
      v3 = &this->m_data[v8];
      while ( (int)v3->m_terminator.m_next > 0 )
      {
        v4 = TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::Link(v3, v3->m_terminator.m_next);
        TSLink<STRINGVIEWMATRICES>::Unlink(v4);
      }
      TSLink<STRINGVIEWMATRICES>::Unlink(&v3->m_terminator);
      v5 = ++index < this->m_count;
      ++v8;
    }
    while ( v5 );
  }
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_50, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001CB1C0  EA 0x5CB1C0
// DIA: TEXTURECACHEROW::TEXTURECACHEROW
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TEXTURECACHEROW *__thiscall TEXTURECACHEROW::TEXTURECACHEROW(TEXTURECACHEROW *this, const TEXTURECACHEROW *__that)
{
  TEXTURECACHEROW *result; // eax
  int m_linkoffset; // ecx

  result = this;
  this->widestFreeSlot = __that->widestFreeSlot;
  this->glyphList.m_terminator.m_prevlink = nullptr;
  this->glyphList.m_terminator.m_next = nullptr;
  m_linkoffset = __that->glyphList.m_linkoffset;
  result->glyphList.m_terminator.m_prevlink = &result->glyphList.m_terminator;
  result->glyphList.m_linkoffset = m_linkoffset;
  result->glyphList.m_terminator.m_next = (CHARCODEDESC *)~(unsigned int)&result->glyphList.m_terminator;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB1F0  EA 0x5CB1F0
// DIA: TSFixedArray<VERT>::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<VERT>::ReallocData(TSFixedArray<VERT> *this, unsigned int count)
{
  unsigned int m_count; // edi
  TSFixedArray<VERT>_vtbl *v4; // edx
  const char *v5; // eax
  VERT *v6; // eax
  const char *v7; // eax
  VERT *v8; // esi
  int v9; // edx
  VERT *v10; // eax
  const char *v11; // eax
  int v12; // [esp-8h] [ebp-18h]
  int v13; // [esp-8h] [ebp-18h]
  int v14; // [esp-8h] [ebp-18h]
  VERT *oldData; // [esp+Ch] [ebp-4h]
  unsigned int counta; // [esp+18h] [ebp+8h]

  m_count = count;
  v4 = this->__vftable;
  oldData = this->m_data;
  this->m_alloc = count;
  v12 = ((int (*)(void))v4->TSBaseArray<VERT>::MemLineNo)();
  v5 = (const char *)this->TSBaseArray<VERT>::MemFileName(this);
  v6 = (VERT *)SMemReAlloc(oldData, 20 * count, v5, v12, 0x10u);
  this->m_data = v6;
  if ( v6 == nullptr )
  {
    v13 = this->TSBaseArray<VERT>::MemLineNo(this);
    v7 = (const char *)this->TSBaseArray<VERT>::MemFileName(this);
    v8 = oldData;
    this->m_data = (VERT *)SMemAlloc(20 * count, v7, v13, 0);
    if ( oldData != nullptr )
    {
      if ( count >= this->m_count )
        m_count = this->m_count;
      if ( m_count != 0 )
      {
        v9 = 0;
        for ( counta = m_count; counta != 0; --counta )
        {
          v10 = &this->m_data[v9];
          if ( v10 != nullptr )
          {
            qmemcpy(v10, &v8[v9], sizeof(VERT));
            v8 = oldData;
          }
          ++v9;
        }
      }
      v14 = this->TSBaseArray<VERT>::MemLineNo(this);
      v11 = (const char *)this->TSBaseArray<VERT>::MemFileName(this);
      SMemFree(v8, v11, v14, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CB2B0  EA 0x5CB2B0
// DIA: TSFixedArray<TEXTLINETEXTURE *>::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TEXTLINETEXTURE *>::ReallocData(TSFixedArray<TEXTLINETEXTURE *> *this, unsigned int count)
{
  unsigned int m_count; // ebx
  TSFixedArray<TEXTLINETEXTURE *>_vtbl *v4; // edx
  const char *v5; // eax
  TEXTLINETEXTURE **v6; // eax
  const char *v7; // eax
  unsigned int i; // edx
  TEXTLINETEXTURE **v9; // eax
  const char *v10; // eax
  int v11; // [esp-8h] [ebp-18h]
  int v12; // [esp-8h] [ebp-18h]
  int v13; // [esp-8h] [ebp-18h]
  TEXTLINETEXTURE **oldData; // [esp+Ch] [ebp-4h]

  m_count = count;
  v4 = this->__vftable;
  oldData = this->m_data;
  this->m_alloc = count;
  v11 = ((int (*)(void))v4->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo)();
  v5 = (const char *)this->TSBaseArray<TEXTLINETEXTURE *>::MemFileName(this);
  v6 = (TEXTLINETEXTURE **)SMemReAlloc(oldData, 4 * count, v5, v11, 0x10u);
  this->m_data = v6;
  if ( v6 == nullptr )
  {
    v12 = this->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo(this);
    v7 = (const char *)this->TSBaseArray<TEXTLINETEXTURE *>::MemFileName(this);
    this->m_data = (TEXTLINETEXTURE **)SMemAlloc(4 * count, v7, v12, 0);
    if ( oldData != nullptr )
    {
      if ( count >= this->m_count )
        m_count = this->m_count;
      for ( i = 0; i < m_count; ++i )
      {
        v9 = &this->m_data[i];
        if ( v9 != nullptr )
          *v9 = oldData[i];
      }
      v13 = this->TSBaseArray<TEXTLINETEXTURE *>::MemLineNo(this);
      v10 = (const char *)this->TSBaseArray<TEXTLINETEXTURE *>::MemFileName(this);
      SMemFree(oldData, v10, v13, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CB360  EA 0x5CB360
// DIA: TSLink<GLYPHBITMAPDATA>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<GLYPHBITMAPDATA> *__thiscall TSLink<GLYPHBITMAPDATA>::NextLink(TSLink<GLYPHBITMAPDATA> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<GLYPHBITMAPDATA> *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<GLYPHBITMAPDATA> *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001CB390  EA 0x5CB390
// DIA: TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::InitializeTerminator(
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (CHARCODEDESC *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB3A0  EA 0x5CB3A0
// DIA: TSFixedArray<GXUFONTHYPERLINKINFO>::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<GXUFONTHYPERLINKINFO>::ReallocData(
        TSFixedArray<GXUFONTHYPERLINKINFO> *this,
        unsigned int count)
{
  unsigned int m_count; // edi
  TSFixedArray<GXUFONTHYPERLINKINFO>_vtbl *v4; // edx
  const char *v5; // eax
  GXUFONTHYPERLINKINFO *v6; // eax
  const char *v7; // eax
  GXUFONTHYPERLINKINFO *v8; // esi
  int v9; // edx
  GXUFONTHYPERLINKINFO *v10; // eax
  const char *v11; // eax
  int v12; // [esp-8h] [ebp-18h]
  int v13; // [esp-8h] [ebp-18h]
  int v14; // [esp-8h] [ebp-18h]
  GXUFONTHYPERLINKINFO *oldData; // [esp+Ch] [ebp-4h]
  unsigned int counta; // [esp+18h] [ebp+8h]

  m_count = count;
  v4 = this->__vftable;
  oldData = this->m_data;
  this->m_alloc = count;
  v12 = ((int (*)(void))v4->TSBaseArray<GXUFONTHYPERLINKINFO>::MemLineNo)();
  v5 = (const char *)this->TSBaseArray<GXUFONTHYPERLINKINFO>::MemFileName(this);
  v6 = (GXUFONTHYPERLINKINFO *)SMemReAlloc(oldData, 24 * count, v5, v12, 0x10u);
  this->m_data = v6;
  if ( v6 == nullptr )
  {
    v13 = this->TSBaseArray<GXUFONTHYPERLINKINFO>::MemLineNo(this);
    v7 = (const char *)this->TSBaseArray<GXUFONTHYPERLINKINFO>::MemFileName(this);
    v8 = oldData;
    this->m_data = (GXUFONTHYPERLINKINFO *)SMemAlloc(24 * count, v7, v13, 0);
    if ( oldData != nullptr )
    {
      if ( count >= this->m_count )
        m_count = this->m_count;
      if ( m_count != 0 )
      {
        v9 = 0;
        for ( counta = m_count; counta != 0; --counta )
        {
          v10 = &this->m_data[v9];
          if ( v10 != nullptr )
          {
            qmemcpy(v10, &v8[v9], sizeof(GXUFONTHYPERLINKINFO));
            v8 = oldData;
          }
          ++v9;
        }
      }
      v14 = this->TSBaseArray<GXUFONTHYPERLINKINFO>::MemLineNo(this);
      v11 = (const char *)this->TSBaseArray<GXUFONTHYPERLINKINFO>::MemFileName(this);
      SMemFree(v8, v11, v14, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CB460  EA 0x5CB460
// DIA: TSGrowableArray<NTempest::CImVector *>::CalcChunkSize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<NTempest::CImVector *>::CalcChunkSize(
        TSGrowableArray<NTempest::CImVector *> *this,
        unsigned int count)
{
  unsigned int result; // eax
  unsigned int i; // ecx

  result = count;
  if ( count >= 0x40 )
  {
    this->m_chunk = 64;
    return 64;
  }
  else
  {
    for ( i = count & (count - 1); i != 0; i &= i - 1 )
      result = i;
    if ( result == 0 )
      return 1;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB4A0  EA 0x5CB4A0
// DIA: TSGrowableArray<NTempest::CImVector *>::RoundToChunk
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<NTempest::CImVector *>::RoundToChunk(
        TSGrowableArray<NTempest::CImVector *> *this,
        unsigned int count,
        unsigned int chunk)
{
  unsigned int v3; // esi

  v3 = count;
  if ( count % chunk != 0 )
    return chunk - count % chunk + count;
  return v3;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB4C0  EA 0x5CB4C0
// DIA: TSFixedArray<NTempest::CImVector *>::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<NTempest::CImVector *>::ReallocData(
        TSFixedArray<NTempest::CImVector *> *this,
        unsigned int count)
{
  unsigned int m_count; // ebx
  TSFixedArray<NTempest::CImVector *>_vtbl *v4; // edx
  const char *v5; // eax
  NTempest::CImVector **v6; // eax
  const char *v7; // eax
  unsigned int i; // edx
  NTempest::CImVector **v9; // eax
  const char *v10; // eax
  int v11; // [esp-8h] [ebp-18h]
  int v12; // [esp-8h] [ebp-18h]
  int v13; // [esp-8h] [ebp-18h]
  NTempest::CImVector **oldData; // [esp+Ch] [ebp-4h]

  m_count = count;
  v4 = this->__vftable;
  oldData = this->m_data;
  this->m_alloc = count;
  v11 = ((int (*)(void))v4->TSBaseArray<NTempest::CImVector *>::MemLineNo)();
  v5 = (const char *)this->TSBaseArray<NTempest::CImVector *>::MemFileName(this);
  v6 = (NTempest::CImVector **)SMemReAlloc(oldData, 4 * count, v5, v11, 0x10u);
  this->m_data = v6;
  if ( v6 == nullptr )
  {
    v12 = this->TSBaseArray<NTempest::CImVector *>::MemLineNo(this);
    v7 = (const char *)this->TSBaseArray<NTempest::CImVector *>::MemFileName(this);
    this->m_data = (NTempest::CImVector **)SMemAlloc(4 * count, v7, v12, 0);
    if ( oldData != nullptr )
    {
      if ( count >= this->m_count )
        m_count = this->m_count;
      for ( i = 0; i < m_count; ++i )
      {
        v9 = &this->m_data[i];
        if ( v9 != nullptr )
          *v9 = oldData[i];
      }
      v13 = this->TSBaseArray<NTempest::CImVector *>::MemLineNo(this);
      v10 = (const char *)this->TSBaseArray<NTempest::CImVector *>::MemFileName(this);
      SMemFree(oldData, v10, v13, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CB570  EA 0x5CB570
// DIA: TSLink<KERNNODE>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<KERNNODE> *__thiscall TSLink<KERNNODE>::NextLink(TSLink<KERNNODE> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<KERNNODE> *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<KERNNODE> *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001CB5A0  EA 0x5CB5A0
// DIA: TSList<CGxString,TSGetExplicitLink<CGxString> >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CGxString,TSGetExplicitLink<CGxString>>::InitializeTerminator(
        TSList<CGxString,TSGetExplicitLink<CGxString> > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (CGxString *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB5B0  EA 0x5CB5B0
// DIA: TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<GLYPHBITMAPDATA> *__thiscall TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *this,
        const GLYPHBITMAPDATA *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<GLYPHBITMAPDATA> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB5D0  EA 0x5CB5D0
// DIA: TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >::TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *__thiscall TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>(
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 0;
  this->m_terminator.m_next = (GLYPHBITMAPDATA *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB5F0  EA 0x5CB5F0
// DIA: TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::LinkNode(
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *this,
        GLYPHBITMAPDATA *ptr,
        unsigned int linktype,
        GLYPHBITMAPDATA *existingptr)
{
  TSLink<GLYPHBITMAPDATA> *p_m_terminator; // eax
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<GLYPHBITMAPDATA> *v8; // edx
  TSLink<GLYPHBITMAPDATA> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<GLYPHBITMAPDATA> *)((char *)ptr + this->m_linkoffset);
  else
    p_m_terminator = &this->m_terminator;
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v7 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v7 = ~m_next;
    *(_DWORD *)v7 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( existingptr != nullptr )
    v8 = (TSLink<GLYPHBITMAPDATA> *)((char *)existingptr + this->m_linkoffset);
  else
    v8 = &this->m_terminator;
  if ( linktype == 1 )
  {
    p_m_terminator->m_prevlink = v8;
    p_m_terminator->m_next = v8->m_next;
    m_linkoffset = this->m_linkoffset;
    v11 = (int)v8->m_next;
    if ( v11 > 0 )
    {
      if ( m_linkoffset < 0 )
        m_linkoffset = (char *)v8 - (char *)v8->m_prevlink->m_next;
      *(_DWORD *)(m_linkoffset + v11) = p_m_terminator;
      v8->m_next = ptr;
    }
    else
    {
      *(_DWORD *)~v11 = p_m_terminator;
      v8->m_next = ptr;
    }
  }
  else
  {
    if ( linktype != 2 )
    {
      SErrPrepareAppFatal(aStormHStplH, 2061);
      SErrDisplayAppFatal("Invalid case: %s=%u", aLinktype, linktype);
    }
    v9 = v8->m_prevlink;
    p_m_terminator->m_prevlink = v8->m_prevlink;
    p_m_terminator->m_next = v9->m_next;
    v9->m_next = ptr;
    v8->m_prevlink = p_m_terminator;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CB6C0  EA 0x5CB6C0
// DIA: TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *__thiscall TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>(
        TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *this)
{
  this->__vftable = (TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::`vftable';
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB6E0  EA 0x5CB6E0
// DIA: TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::CheckArrayBounds
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *this,
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
    v4 = ((int (__thiscall *)(TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo)(
           this,
           1,
           1,
           aIndex0x08xArra,
           index,
           m_count);
    v5 = (const char *)((int (__thiscall *)(TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int))this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName)(
                         this,
                         v4);
    SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CB720  EA 0x5CB720
// DIA: TSHashTable<CHARCODEDESC,HASHKEY_NONE>::Initialize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CHARCODEDESC,HASHKEY_NONE>::Initialize(TSHashTable<CHARCODEDESC,HASHKEY_NONE> *this)
{
  TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *p_m_slotlistarray; // edi
  unsigned int m_count; // eax
  unsigned int v3; // esi
  unsigned int m_chunk; // eax
  unsigned int v5; // eax
  unsigned int v6; // eax
  int v7; // ebx
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *v8; // esi
  int v9; // ebx
  unsigned int v10; // esi
  int v11; // eax
  const char *v12; // eax
  TSExplicitList<CHARCODEDESC,-572662307> *m_data; // ebx
  int m_linkoffset; // eax
  TSExplicitList<CHARCODEDESC,-572662307> *v15; // ebx
  unsigned int p_m_terminator; // edx
  int v17; // ecx
  int v18; // esi
  _DWORD *v19; // eax
  int v20; // edx
  int v21; // edx
  int v22; // [esp-18h] [ebp-34h]
  int v23; // [esp-14h] [ebp-30h]
  unsigned int v24; // [esp-10h] [ebp-2Ch]
  const char *v25; // [esp-Ch] [ebp-28h]
  unsigned int slot; // [esp+14h] [ebp-8h]
  unsigned int v28; // [esp+18h] [ebp-4h]
  int v29; // [esp+18h] [ebp-4h]

  p_m_slotlistarray = &this->m_slotlistarray;
  this->m_slotmask = 3;
  m_count = this->m_slotlistarray.m_count;
  v3 = 4;
  if ( m_count >= 4 )
  {
    if ( m_count > 4 )
    {
      v9 = 4;
      do
      {
        TSExplicitList<CHARCODEDESC,-572662307>::`scalar deleting destructor'(&p_m_slotlistarray->m_data[v9], 0);
        ++v3;
        ++v9;
      }
      while ( v3 < p_m_slotlistarray->m_count );
    }
  }
  else
  {
    if ( this->m_slotlistarray.m_alloc < 4 )
    {
      m_chunk = this->m_slotlistarray.m_chunk;
      if ( m_chunk == 0 )
        m_chunk = TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::CalcChunkSize(&this->m_slotlistarray, 4u);
      v5 = TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::RoundToChunk(p_m_slotlistarray, 4u, m_chunk);
      TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>::ReallocData(p_m_slotlistarray, v5);
    }
    v6 = p_m_slotlistarray->m_count;
    if ( v6 < 4 )
    {
      v7 = v6;
      v28 = 4 - v6;
      do
      {
        v8 = &p_m_slotlistarray->m_data[v7];
        if ( v8 != nullptr )
        {
          TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>(v8);
          v8->m_linkoffset = -572662307;
          TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::InitializeTerminator(v8);
        }
        ++v7;
        --v28;
      }
      while ( v28 != 0 );
    }
  }
  v10 = 0;
  p_m_slotlistarray->m_count = 4;
  slot = 0;
  v29 = 0;
  do
  {
    if ( v10 >= p_m_slotlistarray->m_count )
    {
      v11 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              v10,
              p_m_slotlistarray->m_count);
      v12 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v11);
      SErrDisplayErrorFmt(0x85100080, v12, v22, v23, v24, v25);
    }
    m_data = p_m_slotlistarray->m_data;
    m_linkoffset = m_data[v29].m_linkoffset;
    v15 = &m_data[v29];
    if ( m_linkoffset != 4 )
    {
      p_m_terminator = (unsigned int)&v15->m_terminator;
      while ( 1 )
      {
        v17 = *(_DWORD *)(p_m_terminator + 4);
        if ( v17 <= 0 )
          break;
        v18 = *(_DWORD *)(v15->m_linkoffset + v17);
        v19 = (_DWORD *)(v17 + v15->m_linkoffset);
        if ( v18 != 0 )
        {
          v20 = v19[1];
          if ( v20 > 0 )
            v21 = (int)v19 + v20 - *(_DWORD *)(v18 + 4);
          else
            v21 = ~v20;
          *(_DWORD *)v21 = v18;
          *(_DWORD *)(*v19 + 4) = v19[1];
          p_m_terminator = (unsigned int)&v15->m_terminator;
          *v19 = 0;
          v19[1] = 0;
        }
      }
      v10 = slot;
      *(_DWORD *)p_m_terminator = p_m_terminator;
      v15->m_linkoffset = 4;
      v15->m_terminator.m_next = (CHARCODEDESC *)~p_m_terminator;
    }
    slot = ++v10;
    ++v29;
  }
  while ( v10 <= this->m_slotmask );
}


// -----------------------------------------------------------------------------
// RVA 0x001CB8A0  EA 0x5CB8A0
// DIA: TSExplicitList<CHARCODEDESC,-572662307>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSExplicitList<CHARCODEDESC,-572662307> *__thiscall TSExplicitList<CHARCODEDESC,-572662307>::`scalar deleting destructor'(
        TSExplicitList<CHARCODEDESC,-572662307> *this,
        char __flags)
{
  TSExplicitList<CHARCODEDESC,-572662307> *v2; // esi
  TSLink<CHARCODEDESC> *p_m_terminator; // edi
  int m_next; // ecx
  int m_linkoffset; // eax
  int v6; // esi
  _DWORD *v7; // eax
  int v8; // edx
  int v9; // edx
  TSLink<CHARCODEDESC> *m_prevlink; // edx
  int v11; // ecx
  int v12; // ecx

  v2 = this;
  p_m_terminator = &this->m_terminator;
  while ( 1 )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next <= 0 )
      break;
    m_linkoffset = v2->m_linkoffset;
    v6 = *(_DWORD *)(v2->m_linkoffset + m_next);
    v7 = (_DWORD *)(m_next + m_linkoffset);
    if ( v6 != 0 )
    {
      v8 = v7[1];
      if ( v8 > 0 )
        v9 = (int)v7 + v8 - *(_DWORD *)(v6 + 4);
      else
        v9 = ~v8;
      *(_DWORD *)v9 = v6;
      *(_DWORD *)(*v7 + 4) = v7[1];
      *v7 = 0;
      v7[1] = 0;
    }
    v2 = this;
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    v11 = (int)p_m_terminator->m_next;
    if ( v11 > 0 )
      v12 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v11;
    else
      v12 = ~v11;
    *(_DWORD *)v12 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( (__flags & 1) != 0 && v2 != nullptr )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CB950  EA 0x5CB950
// DIA: TSHashTable<CHARCODEDESC,HASHKEY_NONE>::MonitorFullness
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSHashTable<CHARCODEDESC,HASHKEY_NONE>::MonitorFullness(
        TSHashTable<CHARCODEDESC,HASHKEY_NONE> *this,
        unsigned int slot)
{
  bool v2; // cf
  unsigned int m_fullnessIndicator; // eax
  TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *p_m_slotlistarray; // esi
  int v6; // eax
  const char *v7; // eax
  unsigned int v8; // ecx
  CHARCODEDESC *m_next; // edi
  int v10; // edi
  TSHashTable<CHARCODEDESC,HASHKEY_NONE> *v11; // eax
  unsigned int v12; // edx
  int v13; // eax
  const char *v14; // eax
  int v16; // edi
  unsigned int v17; // ebx
  int v18; // edi
  CHARCODEDESC *v19; // eax
  unsigned int v20; // edi
  unsigned int v21; // ebx
  TSExplicitList<CHARCODEDESC,-572662307> *m_data; // edi
  int m_linkoffset; // eax
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > *v24; // edi
  CHARCODEDESC *v25; // ebx
  unsigned int v26; // edi
  int v27; // [esp-18h] [ebp-38h]
  int v28; // [esp-18h] [ebp-38h]
  int v29; // [esp-14h] [ebp-34h]
  int v30; // [esp-14h] [ebp-34h]
  unsigned int v31; // [esp-10h] [ebp-30h]
  unsigned int v32; // [esp-10h] [ebp-30h]
  const char *v33; // [esp-Ch] [ebp-2Ch]
  const char *v34; // [esp-Ch] [ebp-2Ch]
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC> > v35; // [esp+Ch] [ebp-14h] BYREF
  TSHashTable<CHARCODEDESC,HASHKEY_NONE> *v36; // [esp+18h] [ebp-8h]
  unsigned int count; // [esp+1Ch] [ebp-4h]
  unsigned int slota; // [esp+28h] [ebp+8h]
  unsigned int slotb; // [esp+28h] [ebp+8h]
  unsigned int slotc; // [esp+28h] [ebp+8h]

  v2 = this->m_slotmask < 0x1FFF;
  v36 = this;
  if ( !v2 )
    return 0;
  m_fullnessIndicator = this->m_fullnessIndicator;
  if ( m_fullnessIndicator <= 3 )
    this->m_fullnessIndicator = 0;
  else
    this->m_fullnessIndicator = m_fullnessIndicator - 3;
  p_m_slotlistarray = &this->m_slotlistarray;
  if ( slot >= this->m_slotlistarray.m_count )
  {
    v6 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo)(
           &this->m_slotlistarray,
           1,
           1,
           aIndex0x08xArra,
           slot,
           this->m_slotlistarray.m_count);
    v7 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName)(
                         p_m_slotlistarray,
                         v6);
    SErrDisplayErrorFmt(0x85100080, v7, v27, v29, v31, v33);
  }
  v8 = 12 * slot;
  m_next = p_m_slotlistarray->m_data[slot].m_terminator.m_next;
  slota = 12 * slot;
  v10 = (int)m_next <= 0 ? 0 : (unsigned int)m_next;
  if ( v10 <= 0 )
    return 0;
  while ( 1 )
  {
    v11 = v36;
    v12 = v36->m_fullnessIndicator + 1;
    v36->m_fullnessIndicator = v12;
    if ( v12 > 0xD )
      break;
    if ( slot >= p_m_slotlistarray->m_count )
    {
      v13 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              slot,
              p_m_slotlistarray->m_count);
      v14 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v13);
      SErrDisplayErrorFmt(0x85100080, v14, v28, v30, v32, v34);
      v8 = slota;
    }
    v10 = *(_DWORD *)(v10 + *(int *)((char *)&p_m_slotlistarray->m_data->m_linkoffset + v8) + 4);
    if ( v10 <= 0 )
      return 0;
  }
  v11->m_fullnessIndicator = 0;
  v16 = v11->m_slotmask + 1;
  slotb = v16;
  count = 2 * v16;
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>(&v35);
  v35.m_linkoffset = -572662307;
  TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::InitializeTerminator(&v35);
  if ( v35.m_linkoffset != 4 )
  {
    TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::UnlinkAll(&v35);
    v35.m_linkoffset = 4;
    TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::InitializeTerminator(&v35);
  }
  v17 = 0;
  if ( v16 != 0 )
  {
    v18 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v17);
        v19 = p_m_slotlistarray->m_data[v18].m_terminator.m_next;
        if ( (int)v19 <= 0 )
          break;
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::LinkNode(&v35, v19, 2u, nullptr);
      }
      ++v17;
      ++v18;
    }
    while ( v17 < slotb );
  }
  v20 = count;
  TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::SetCount(p_m_slotlistarray, count);
  v21 = 0;
  if ( v20 != 0 )
  {
    slotc = 0;
    do
    {
      TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v21);
      m_data = p_m_slotlistarray->m_data;
      m_linkoffset = m_data[slotc].m_linkoffset;
      v24 = &m_data[slotc];
      if ( m_linkoffset != 4 )
      {
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::UnlinkAll(v24);
        v24->m_linkoffset = 4;
        TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::InitializeTerminator(v24);
      }
      ++v21;
      ++slotc;
    }
    while ( v21 < count );
    v20 = count;
  }
  v36->m_slotmask = v20 - 1;
  while ( (int)v35.m_terminator.m_next > 0 )
  {
    v25 = v35.m_terminator.m_next;
    v26 = v36->m_slotmask & v35.m_terminator.m_next->m_hashval;
    TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v26);
    TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::LinkNode(&p_m_slotlistarray->m_data[v26], v25, 2u, nullptr);
  }
  TSLink<CHARCODEDESC>::Unlink(&v35.m_terminator);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001CBB80  EA 0x5CBB80
// DIA: TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::CheckArrayBounds
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307>>::CheckArrayBounds(
        TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> > *this,
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
    v4 = ((int (__thiscall *)(TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))this->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo)(
           this,
           1,
           1,
           aIndex0x08xArra,
           index,
           m_count);
    v5 = (const char *)((int (__thiscall *)(TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> > *, int))this->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName)(
                         this,
                         v4);
    SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CBBC0  EA 0x5CBBC0
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::Initialize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::Initialize(TSHashTable<KERNNODE,KERNINGHASHKEY> *this)
{
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *p_m_slotlistarray; // edi
  unsigned int m_count; // eax
  unsigned int v3; // esi
  unsigned int m_chunk; // eax
  unsigned int v5; // eax
  unsigned int v6; // eax
  int v7; // ebx
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *v8; // esi
  int v9; // ebx
  unsigned int v10; // esi
  int v11; // eax
  const char *v12; // eax
  TSExplicitList<KERNNODE,-572662307> *m_data; // ebx
  int m_linkoffset; // eax
  TSExplicitList<KERNNODE,-572662307> *v15; // ebx
  unsigned int p_m_terminator; // edx
  int v17; // ecx
  int v18; // esi
  _DWORD *v19; // eax
  int v20; // edx
  int v21; // edx
  int v22; // [esp-18h] [ebp-34h]
  int v23; // [esp-14h] [ebp-30h]
  unsigned int v24; // [esp-10h] [ebp-2Ch]
  const char *v25; // [esp-Ch] [ebp-28h]
  unsigned int slot; // [esp+14h] [ebp-8h]
  unsigned int v28; // [esp+18h] [ebp-4h]
  int v29; // [esp+18h] [ebp-4h]

  p_m_slotlistarray = &this->m_slotlistarray;
  this->m_slotmask = 3;
  m_count = this->m_slotlistarray.m_count;
  v3 = 4;
  if ( m_count >= 4 )
  {
    if ( m_count > 4 )
    {
      v9 = 4;
      do
      {
        TSExplicitList<KERNNODE,-572662307>::`scalar deleting destructor'(&p_m_slotlistarray->m_data[v9], 0);
        ++v3;
        ++v9;
      }
      while ( v3 < p_m_slotlistarray->m_count );
    }
  }
  else
  {
    if ( this->m_slotlistarray.m_alloc < 4 )
    {
      m_chunk = this->m_slotlistarray.m_chunk;
      if ( m_chunk == 0 )
        m_chunk = TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::CalcChunkSize(&this->m_slotlistarray, 4u);
      v5 = TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::RoundToChunk(p_m_slotlistarray, 4u, m_chunk);
      TSFixedArray<TSExplicitList<KERNNODE,-572662307>>::ReallocData(p_m_slotlistarray, v5);
    }
    v6 = p_m_slotlistarray->m_count;
    if ( v6 < 4 )
    {
      v7 = v6;
      v28 = 4 - v6;
      do
      {
        v8 = &p_m_slotlistarray->m_data[v7];
        if ( v8 != nullptr )
        {
          TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>(v8);
          v8->m_linkoffset = -572662307;
          TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(v8);
        }
        ++v7;
        --v28;
      }
      while ( v28 != 0 );
    }
  }
  v10 = 0;
  p_m_slotlistarray->m_count = 4;
  slot = 0;
  v29 = 0;
  do
  {
    if ( v10 >= p_m_slotlistarray->m_count )
    {
      v11 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              v10,
              p_m_slotlistarray->m_count);
      v12 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v11);
      SErrDisplayErrorFmt(0x85100080, v12, v22, v23, v24, v25);
    }
    m_data = p_m_slotlistarray->m_data;
    m_linkoffset = m_data[v29].m_linkoffset;
    v15 = &m_data[v29];
    if ( m_linkoffset != 4 )
    {
      p_m_terminator = (unsigned int)&v15->m_terminator;
      while ( 1 )
      {
        v17 = *(_DWORD *)(p_m_terminator + 4);
        if ( v17 <= 0 )
          break;
        v18 = *(_DWORD *)(v15->m_linkoffset + v17);
        v19 = (_DWORD *)(v17 + v15->m_linkoffset);
        if ( v18 != 0 )
        {
          v20 = v19[1];
          if ( v20 > 0 )
            v21 = (int)v19 + v20 - *(_DWORD *)(v18 + 4);
          else
            v21 = ~v20;
          *(_DWORD *)v21 = v18;
          *(_DWORD *)(*v19 + 4) = v19[1];
          p_m_terminator = (unsigned int)&v15->m_terminator;
          *v19 = 0;
          v19[1] = 0;
        }
      }
      v10 = slot;
      *(_DWORD *)p_m_terminator = p_m_terminator;
      v15->m_linkoffset = 4;
      v15->m_terminator.m_next = (KERNNODE *)~p_m_terminator;
    }
    slot = ++v10;
    ++v29;
  }
  while ( v10 <= this->m_slotmask );
}


// -----------------------------------------------------------------------------
// RVA 0x001CBD40  EA 0x5CBD40
// DIA: TSExplicitList<KERNNODE,-572662307>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSExplicitList<KERNNODE,-572662307> *__thiscall TSExplicitList<KERNNODE,-572662307>::`scalar deleting destructor'(
        TSExplicitList<KERNNODE,-572662307> *this,
        char __flags)
{
  TSExplicitList<KERNNODE,-572662307> *v2; // esi
  TSLink<KERNNODE> *p_m_terminator; // edi
  int m_next; // ecx
  int m_linkoffset; // eax
  int v6; // esi
  _DWORD *v7; // eax
  int v8; // edx
  int v9; // edx
  TSLink<KERNNODE> *m_prevlink; // edx
  int v11; // ecx
  int v12; // ecx

  v2 = this;
  p_m_terminator = &this->m_terminator;
  while ( 1 )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next <= 0 )
      break;
    m_linkoffset = v2->m_linkoffset;
    v6 = *(_DWORD *)(v2->m_linkoffset + m_next);
    v7 = (_DWORD *)(m_next + m_linkoffset);
    if ( v6 != 0 )
    {
      v8 = v7[1];
      if ( v8 > 0 )
        v9 = (int)v7 + v8 - *(_DWORD *)(v6 + 4);
      else
        v9 = ~v8;
      *(_DWORD *)v9 = v6;
      *(_DWORD *)(*v7 + 4) = v7[1];
      *v7 = 0;
      v7[1] = 0;
    }
    v2 = this;
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    v11 = (int)p_m_terminator->m_next;
    if ( v11 > 0 )
      v12 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v11;
    else
      v12 = ~v11;
    *(_DWORD *)v12 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( (__flags & 1) != 0 && v2 != nullptr )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CBDF0  EA 0x5CBDF0
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::MonitorFullness
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::MonitorFullness(
        TSHashTable<KERNNODE,KERNINGHASHKEY> *this,
        unsigned int slot)
{
  bool v2; // cf
  unsigned int m_fullnessIndicator; // eax
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *p_m_slotlistarray; // esi
  int v6; // eax
  const char *v7; // eax
  unsigned int v8; // ecx
  KERNNODE *m_next; // edi
  int v10; // edi
  TSHashTable<KERNNODE,KERNINGHASHKEY> *v11; // eax
  unsigned int v12; // edx
  int v13; // eax
  const char *v14; // eax
  int v16; // edi
  unsigned int v17; // ebx
  int v18; // edi
  KERNNODE *v19; // eax
  unsigned int v20; // edi
  unsigned int v21; // ebx
  TSExplicitList<KERNNODE,-572662307> *m_data; // edi
  int m_linkoffset; // eax
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *v24; // edi
  KERNNODE *v25; // ebx
  unsigned int v26; // edi
  int v27; // [esp-18h] [ebp-38h]
  int v28; // [esp-18h] [ebp-38h]
  int v29; // [esp-14h] [ebp-34h]
  int v30; // [esp-14h] [ebp-34h]
  unsigned int v31; // [esp-10h] [ebp-30h]
  unsigned int v32; // [esp-10h] [ebp-30h]
  const char *v33; // [esp-Ch] [ebp-2Ch]
  const char *v34; // [esp-Ch] [ebp-2Ch]
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > v35; // [esp+Ch] [ebp-14h] BYREF
  TSHashTable<KERNNODE,KERNINGHASHKEY> *v36; // [esp+18h] [ebp-8h]
  unsigned int count; // [esp+1Ch] [ebp-4h]
  unsigned int slota; // [esp+28h] [ebp+8h]
  unsigned int slotb; // [esp+28h] [ebp+8h]
  unsigned int slotc; // [esp+28h] [ebp+8h]

  v2 = this->m_slotmask < 0x1FFF;
  v36 = this;
  if ( !v2 )
    return 0;
  m_fullnessIndicator = this->m_fullnessIndicator;
  if ( m_fullnessIndicator <= 3 )
    this->m_fullnessIndicator = 0;
  else
    this->m_fullnessIndicator = m_fullnessIndicator - 3;
  p_m_slotlistarray = &this->m_slotlistarray;
  if ( slot >= this->m_slotlistarray.m_count )
  {
    v6 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo)(
           &this->m_slotlistarray,
           1,
           1,
           aIndex0x08xArra,
           slot,
           this->m_slotlistarray.m_count);
    v7 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName)(
                         p_m_slotlistarray,
                         v6);
    SErrDisplayErrorFmt(0x85100080, v7, v27, v29, v31, v33);
  }
  v8 = 12 * slot;
  m_next = p_m_slotlistarray->m_data[slot].m_terminator.m_next;
  slota = 12 * slot;
  v10 = (int)m_next <= 0 ? 0 : (unsigned int)m_next;
  if ( v10 <= 0 )
    return 0;
  while ( 1 )
  {
    v11 = v36;
    v12 = v36->m_fullnessIndicator + 1;
    v36->m_fullnessIndicator = v12;
    if ( v12 > 0xD )
      break;
    if ( slot >= p_m_slotlistarray->m_count )
    {
      v13 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              slot,
              p_m_slotlistarray->m_count);
      v14 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v13);
      SErrDisplayErrorFmt(0x85100080, v14, v28, v30, v32, v34);
      v8 = slota;
    }
    v10 = *(_DWORD *)(v10 + *(int *)((char *)&p_m_slotlistarray->m_data->m_linkoffset + v8) + 4);
    if ( v10 <= 0 )
      return 0;
  }
  v11->m_fullnessIndicator = 0;
  v16 = v11->m_slotmask + 1;
  slotb = v16;
  count = 2 * v16;
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>(&v35);
  v35.m_linkoffset = -572662307;
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(&v35);
  if ( v35.m_linkoffset != 4 )
  {
    TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::UnlinkAll(&v35);
    v35.m_linkoffset = 4;
    TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(&v35);
  }
  v17 = 0;
  if ( v16 != 0 )
  {
    v18 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v17);
        v19 = p_m_slotlistarray->m_data[v18].m_terminator.m_next;
        if ( (int)v19 <= 0 )
          break;
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::LinkNode(&v35, v19, 2u, nullptr);
      }
      ++v17;
      ++v18;
    }
    while ( v17 < slotb );
  }
  v20 = count;
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::SetCount(p_m_slotlistarray, count);
  v21 = 0;
  if ( v20 != 0 )
  {
    slotc = 0;
    do
    {
      TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v21);
      m_data = p_m_slotlistarray->m_data;
      m_linkoffset = m_data[slotc].m_linkoffset;
      v24 = &m_data[slotc];
      if ( m_linkoffset != 4 )
      {
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::UnlinkAll(v24);
        v24->m_linkoffset = 4;
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(v24);
      }
      ++v21;
      ++slotc;
    }
    while ( v21 < count );
    v20 = count;
  }
  v36->m_slotmask = v20 - 1;
  while ( (int)v35.m_terminator.m_next > 0 )
  {
    v25 = v35.m_terminator.m_next;
    v26 = v36->m_slotmask & v35.m_terminator.m_next->m_hashval;
    TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v26);
    TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::LinkNode(&p_m_slotlistarray->m_data[v26], v25, 2u, nullptr);
  }
  TSLink<KERNNODE>::Unlink(&v35.m_terminator);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001CC020  EA 0x5CC020
// DIA: TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<KERNNODE> *__thiscall TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *this,
        const KERNNODE *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<KERNNODE> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CC040  EA 0x5CC040
// DIA: TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >::TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *__thiscall TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>(
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 0;
  this->m_terminator.m_next = (KERNNODE *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CC060  EA 0x5CC060
// DIA: TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::LinkNode(
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *this,
        KERNNODE *ptr,
        unsigned int linktype,
        KERNNODE *existingptr)
{
  TSLink<KERNNODE> *p_m_terminator; // eax
  TSLink<KERNNODE> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<KERNNODE> *v8; // edx
  TSLink<KERNNODE> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<KERNNODE> *)((char *)ptr + this->m_linkoffset);
  else
    p_m_terminator = &this->m_terminator;
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v7 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v7 = ~m_next;
    *(_DWORD *)v7 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( existingptr != nullptr )
    v8 = (TSLink<KERNNODE> *)((char *)existingptr + this->m_linkoffset);
  else
    v8 = &this->m_terminator;
  if ( linktype == 1 )
  {
    p_m_terminator->m_prevlink = v8;
    p_m_terminator->m_next = v8->m_next;
    m_linkoffset = this->m_linkoffset;
    v11 = (int)v8->m_next;
    if ( v11 > 0 )
    {
      if ( m_linkoffset < 0 )
        m_linkoffset = (char *)v8 - (char *)v8->m_prevlink->m_next;
      *(_DWORD *)(m_linkoffset + v11) = p_m_terminator;
      v8->m_next = ptr;
    }
    else
    {
      *(_DWORD *)~v11 = p_m_terminator;
      v8->m_next = ptr;
    }
  }
  else
  {
    if ( linktype != 2 )
    {
      SErrPrepareAppFatal(aStormHStplH, 2061);
      SErrDisplayAppFatal("Invalid case: %s=%u", aLinktype, linktype);
    }
    v9 = v8->m_prevlink;
    p_m_terminator->m_prevlink = v8->m_prevlink;
    p_m_terminator->m_next = v9->m_next;
    v9->m_next = ptr;
    v8->m_prevlink = p_m_terminator;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CC130  EA 0x5CC130
// DIA: TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >::UnlinkAll
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::UnlinkAll(
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *this)
{
  KERNNODE *m_next; // edx
  int v2; // edi
  _DWORD *v3; // eax
  int v4; // esi
  int v5; // esi

  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v2 = *(unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    v3 = (unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    if ( v2 != 0 )
    {
      v4 = v3[1];
      if ( v4 > 0 )
        v5 = (int)v3 + v4 - *(_DWORD *)(v2 + 4);
      else
        v5 = ~v4;
      *(_DWORD *)v5 = v2;
      *(_DWORD *)(*v3 + 4) = v3[1];
      *v3 = 0;
      v3[1] = 0;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CC180  EA 0x5CC180
// DIA: TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::CheckArrayBounds
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(
        TSBaseArray<TSExplicitList<KERNNODE,-572662307> > *this,
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
    v4 = ((int (__thiscall *)(TSBaseArray<TSExplicitList<KERNNODE,-572662307> > *, int, int, char *, unsigned int, unsigned int))this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo)(
           this,
           1,
           1,
           aIndex0x08xArra,
           index,
           m_count);
    v5 = (const char *)((int (__thiscall *)(TSBaseArray<TSExplicitList<KERNNODE,-572662307> > *, int))this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName)(
                         this,
                         v4);
    SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CC1C0  EA 0x5CC1C0
// DIA: TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::Initialize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::Initialize(
        TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *this)
{
  TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *p_m_slotlistarray; // edi
  unsigned int m_count; // eax
  unsigned int v3; // esi
  unsigned int m_chunk; // eax
  unsigned int v5; // eax
  unsigned int v6; // eax
  int v7; // ebx
  TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *v8; // esi
  int v9; // ebx
  unsigned int v10; // esi
  int v11; // eax
  const char *v12; // eax
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *m_data; // ebx
  int m_linkoffset; // eax
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v15; // ebx
  unsigned int p_m_terminator; // edx
  int v17; // ecx
  int v18; // esi
  _DWORD *v19; // eax
  int v20; // edx
  int v21; // edx
  int v22; // [esp-18h] [ebp-34h]
  int v23; // [esp-14h] [ebp-30h]
  unsigned int v24; // [esp-10h] [ebp-2Ch]
  const char *v25; // [esp-Ch] [ebp-28h]
  unsigned int slot; // [esp+14h] [ebp-8h]
  unsigned int v28; // [esp+18h] [ebp-4h]
  int v29; // [esp+18h] [ebp-4h]

  p_m_slotlistarray = &this->m_slotlistarray;
  this->m_slotmask = 3;
  m_count = this->m_slotlistarray.m_count;
  v3 = 4;
  if ( m_count >= 4 )
  {
    if ( m_count > 4 )
    {
      v9 = 4;
      do
      {
        TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>::`scalar deleting destructor'(
          &p_m_slotlistarray->m_data[v9],
          0);
        ++v3;
        ++v9;
      }
      while ( v3 < p_m_slotlistarray->m_count );
    }
  }
  else
  {
    if ( this->m_slotlistarray.m_alloc < 4 )
    {
      m_chunk = this->m_slotlistarray.m_chunk;
      if ( m_chunk == 0 )
        m_chunk = TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CalcChunkSize(
                    &this->m_slotlistarray,
                    4u);
      v5 = TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::RoundToChunk(
             p_m_slotlistarray,
             4u,
             m_chunk);
      TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::ReallocData(p_m_slotlistarray, v5);
    }
    v6 = p_m_slotlistarray->m_count;
    if ( v6 < 4 )
    {
      v7 = v6;
      v28 = 4 - v6;
      do
      {
        v8 = &p_m_slotlistarray->m_data[v7];
        if ( v8 != nullptr )
        {
          TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>(v8);
          v8->m_linkoffset = -572662307;
          TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::InitializeTerminator(v8);
        }
        ++v7;
        --v28;
      }
      while ( v28 != 0 );
    }
  }
  v10 = 0;
  p_m_slotlistarray->m_count = 4;
  slot = 0;
  v29 = 0;
  do
  {
    if ( v10 >= p_m_slotlistarray->m_count )
    {
      v11 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              v10,
              p_m_slotlistarray->m_count);
      v12 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v11);
      SErrDisplayErrorFmt(0x85100080, v12, v22, v23, v24, v25);
    }
    m_data = p_m_slotlistarray->m_data;
    m_linkoffset = m_data[v29].m_linkoffset;
    v15 = &m_data[v29];
    if ( m_linkoffset != 4 )
    {
      p_m_terminator = (unsigned int)&v15->m_terminator;
      while ( 1 )
      {
        v17 = *(_DWORD *)(p_m_terminator + 4);
        if ( v17 <= 0 )
          break;
        v18 = *(_DWORD *)(v15->m_linkoffset + v17);
        v19 = (_DWORD *)(v17 + v15->m_linkoffset);
        if ( v18 != 0 )
        {
          v20 = v19[1];
          if ( v20 > 0 )
            v21 = (int)v19 + v20 - *(_DWORD *)(v18 + 4);
          else
            v21 = ~v20;
          *(_DWORD *)v21 = v18;
          *(_DWORD *)(*v19 + 4) = v19[1];
          p_m_terminator = (unsigned int)&v15->m_terminator;
          *v19 = 0;
          v19[1] = 0;
        }
      }
      v10 = slot;
      *(_DWORD *)p_m_terminator = p_m_terminator;
      v15->m_linkoffset = 4;
      v15->m_terminator.m_next = (BATCHEDRENDERFONTDESC *)~p_m_terminator;
    }
    slot = ++v10;
    ++v29;
  }
  while ( v10 <= this->m_slotmask );
}


// -----------------------------------------------------------------------------
// RVA 0x001CC340  EA 0x5CC340
// DIA: TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *__thiscall TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>::`scalar deleting destructor'(
        TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *this,
        char __flags)
{
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v2; // esi
  TSLink<BATCHEDRENDERFONTDESC> *p_m_terminator; // edi
  int m_next; // ecx
  int m_linkoffset; // eax
  int v6; // esi
  _DWORD *v7; // eax
  int v8; // edx
  int v9; // edx
  TSLink<BATCHEDRENDERFONTDESC> *m_prevlink; // edx
  int v11; // ecx
  int v12; // ecx

  v2 = this;
  p_m_terminator = &this->m_terminator;
  while ( 1 )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next <= 0 )
      break;
    m_linkoffset = v2->m_linkoffset;
    v6 = *(_DWORD *)(v2->m_linkoffset + m_next);
    v7 = (_DWORD *)(m_next + m_linkoffset);
    if ( v6 != 0 )
    {
      v8 = v7[1];
      if ( v8 > 0 )
        v9 = (int)v7 + v8 - *(_DWORD *)(v6 + 4);
      else
        v9 = ~v8;
      *(_DWORD *)v9 = v6;
      *(_DWORD *)(*v7 + 4) = v7[1];
      *v7 = 0;
      v7[1] = 0;
    }
    v2 = this;
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    v11 = (int)p_m_terminator->m_next;
    if ( v11 > 0 )
      v12 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v11;
    else
      v12 = ~v11;
    *(_DWORD *)v12 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( (__flags & 1) != 0 && v2 != nullptr )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CC3F0  EA 0x5CC3F0
// DIA: TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::MonitorFullness
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR>::MonitorFullness(
        TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *this,
        unsigned int slot)
{
  bool v2; // cf
  unsigned int m_fullnessIndicator; // eax
  TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *p_m_slotlistarray; // esi
  int v6; // eax
  const char *v7; // eax
  unsigned int v8; // ecx
  BATCHEDRENDERFONTDESC *m_next; // edi
  int v10; // edi
  TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *v11; // eax
  unsigned int v12; // edx
  int v13; // eax
  const char *v14; // eax
  int v16; // edi
  unsigned int v17; // ebx
  int v18; // edi
  BATCHEDRENDERFONTDESC *v19; // eax
  unsigned int v20; // edi
  unsigned int v21; // ebx
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *m_data; // edi
  int m_linkoffset; // eax
  TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *v24; // edi
  BATCHEDRENDERFONTDESC *v25; // ebx
  unsigned int v26; // edi
  int v27; // [esp-18h] [ebp-38h]
  int v28; // [esp-18h] [ebp-38h]
  int v29; // [esp-14h] [ebp-34h]
  int v30; // [esp-14h] [ebp-34h]
  unsigned int v31; // [esp-10h] [ebp-30h]
  unsigned int v32; // [esp-10h] [ebp-30h]
  const char *v33; // [esp-Ch] [ebp-2Ch]
  const char *v34; // [esp-Ch] [ebp-2Ch]
  TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > v35; // [esp+Ch] [ebp-14h] BYREF
  TSHashTable<BATCHEDRENDERFONTDESC,HASHKEY_PTR> *v36; // [esp+18h] [ebp-8h]
  unsigned int count; // [esp+1Ch] [ebp-4h]
  unsigned int slota; // [esp+28h] [ebp+8h]
  unsigned int slotb; // [esp+28h] [ebp+8h]
  unsigned int slotc; // [esp+28h] [ebp+8h]

  v2 = this->m_slotmask < 0x1FFF;
  v36 = this;
  if ( !v2 )
    return 0;
  m_fullnessIndicator = this->m_fullnessIndicator;
  if ( m_fullnessIndicator <= 3 )
    this->m_fullnessIndicator = 0;
  else
    this->m_fullnessIndicator = m_fullnessIndicator - 3;
  p_m_slotlistarray = &this->m_slotlistarray;
  if ( slot >= this->m_slotlistarray.m_count )
  {
    v6 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo)(
           &this->m_slotlistarray,
           1,
           1,
           aIndex0x08xArra,
           slot,
           this->m_slotlistarray.m_count);
    v7 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName)(
                         p_m_slotlistarray,
                         v6);
    SErrDisplayErrorFmt(0x85100080, v7, v27, v29, v31, v33);
  }
  v8 = 12 * slot;
  m_next = p_m_slotlistarray->m_data[slot].m_terminator.m_next;
  slota = 12 * slot;
  v10 = (int)m_next <= 0 ? 0 : (unsigned int)m_next;
  if ( v10 <= 0 )
    return 0;
  while ( 1 )
  {
    v11 = v36;
    v12 = v36->m_fullnessIndicator + 1;
    v36->m_fullnessIndicator = v12;
    if ( v12 > 0xD )
      break;
    if ( slot >= p_m_slotlistarray->m_count )
    {
      v13 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              slot,
              p_m_slotlistarray->m_count);
      v14 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v13);
      SErrDisplayErrorFmt(0x85100080, v14, v28, v30, v32, v34);
      v8 = slota;
    }
    v10 = *(_DWORD *)(v10 + *(int *)((char *)&p_m_slotlistarray->m_data->m_linkoffset + v8) + 4);
    if ( v10 <= 0 )
      return 0;
  }
  v11->m_fullnessIndicator = 0;
  v16 = v11->m_slotmask + 1;
  slotb = v16;
  count = 2 * v16;
  TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>(&v35);
  v35.m_linkoffset = -572662307;
  TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::InitializeTerminator(&v35);
  if ( v35.m_linkoffset != 4 )
  {
    TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::UnlinkAll(&v35);
    v35.m_linkoffset = 4;
    TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::InitializeTerminator(&v35);
  }
  v17 = 0;
  if ( v16 != 0 )
  {
    v18 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v17);
        v19 = p_m_slotlistarray->m_data[v18].m_terminator.m_next;
        if ( (int)v19 <= 0 )
          break;
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::LinkNode(&v35, v19, 2u, nullptr);
      }
      ++v17;
      ++v18;
    }
    while ( v17 < slotb );
  }
  v20 = count;
  TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::SetCount(p_m_slotlistarray, count);
  v21 = 0;
  if ( v20 != 0 )
  {
    slotc = 0;
    do
    {
      TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v21);
      m_data = p_m_slotlistarray->m_data;
      m_linkoffset = m_data[slotc].m_linkoffset;
      v24 = &m_data[slotc];
      if ( m_linkoffset != 4 )
      {
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::UnlinkAll(v24);
        v24->m_linkoffset = 4;
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::InitializeTerminator(v24);
      }
      ++v21;
      ++slotc;
    }
    while ( v21 < count );
    v20 = count;
  }
  v36->m_slotmask = v20 - 1;
  while ( (int)v35.m_terminator.m_next > 0 )
  {
    v25 = v35.m_terminator.m_next;
    v26 = v36->m_slotmask & v35.m_terminator.m_next->m_hashval;
    TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v26);
    TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::LinkNode(
      &p_m_slotlistarray->m_data[v26],
      v25,
      2u,
      nullptr);
  }
  TSLink<BATCHEDRENDERFONTDESC>::Unlink(&v35.m_terminator);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001CC620  EA 0x5CC620
// DIA: TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::LinkNode(
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *this,
        BATCHEDRENDERFONTDESC *ptr,
        unsigned int linktype,
        BATCHEDRENDERFONTDESC *existingptr)
{
  TSLink<BATCHEDRENDERFONTDESC> *p_m_terminator; // eax
  TSLink<BATCHEDRENDERFONTDESC> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<BATCHEDRENDERFONTDESC> *v8; // edx
  TSLink<BATCHEDRENDERFONTDESC> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<BATCHEDRENDERFONTDESC> *)((char *)ptr + this->m_linkoffset);
  else
    p_m_terminator = &this->m_terminator;
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v7 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v7 = ~m_next;
    *(_DWORD *)v7 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( existingptr != nullptr )
    v8 = (TSLink<BATCHEDRENDERFONTDESC> *)((char *)existingptr + this->m_linkoffset);
  else
    v8 = &this->m_terminator;
  if ( linktype == 1 )
  {
    p_m_terminator->m_prevlink = v8;
    p_m_terminator->m_next = v8->m_next;
    m_linkoffset = this->m_linkoffset;
    v11 = (int)v8->m_next;
    if ( v11 > 0 )
    {
      if ( m_linkoffset < 0 )
        m_linkoffset = (char *)v8 - (char *)v8->m_prevlink->m_next;
      *(_DWORD *)(m_linkoffset + v11) = p_m_terminator;
      v8->m_next = ptr;
    }
    else
    {
      *(_DWORD *)~v11 = p_m_terminator;
      v8->m_next = ptr;
    }
  }
  else
  {
    if ( linktype != 2 )
    {
      SErrPrepareAppFatal(aStormHStplH, 2061);
      SErrDisplayAppFatal("Invalid case: %s=%u", aLinktype, linktype);
    }
    v9 = v8->m_prevlink;
    p_m_terminator->m_prevlink = v8->m_prevlink;
    p_m_terminator->m_next = v9->m_next;
    v9->m_next = ptr;
    v8->m_prevlink = p_m_terminator;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CC6F0  EA 0x5CC6F0
// DIA: TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> >::UnlinkAll
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::UnlinkAll(
        TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC> > *this)
{
  BATCHEDRENDERFONTDESC *m_next; // edx
  int v2; // edi
  _DWORD *v3; // eax
  int v4; // esi
  int v5; // esi

  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v2 = *(unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    v3 = (unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    if ( v2 != 0 )
    {
      v4 = v3[1];
      if ( v4 > 0 )
        v5 = (int)v3 + v4 - *(_DWORD *)(v2 + 4);
      else
        v5 = ~v4;
      *(_DWORD *)v5 = v2;
      *(_DWORD *)(*v3 + 4) = v3[1];
      *v3 = 0;
      v3[1] = 0;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CC740  EA 0x5CC740
// DIA: TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::Initialize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::Initialize(
        TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *this)
{
  TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *p_m_slotlistarray; // edi
  unsigned int m_count; // eax
  unsigned int v3; // esi
  unsigned int m_chunk; // eax
  unsigned int v5; // eax
  unsigned int v6; // eax
  int v7; // ebx
  TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *v8; // esi
  int v9; // ebx
  unsigned int v10; // esi
  int v11; // eax
  const char *v12; // eax
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *m_data; // ebx
  int m_linkoffset; // eax
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v15; // ebx
  unsigned int p_m_terminator; // edx
  int v17; // ecx
  int v18; // esi
  _DWORD *v19; // eax
  int v20; // edx
  int v21; // edx
  int v22; // [esp-18h] [ebp-34h]
  int v23; // [esp-14h] [ebp-30h]
  unsigned int v24; // [esp-10h] [ebp-2Ch]
  const char *v25; // [esp-Ch] [ebp-28h]
  unsigned int slot; // [esp+14h] [ebp-8h]
  unsigned int v28; // [esp+18h] [ebp-4h]
  int v29; // [esp+18h] [ebp-4h]

  p_m_slotlistarray = &this->m_slotlistarray;
  this->m_slotmask = 3;
  m_count = this->m_slotlistarray.m_count;
  v3 = 4;
  if ( m_count >= 4 )
  {
    if ( m_count > 4 )
    {
      v9 = 4;
      do
      {
        TSExplicitList<STRINGVIEWMATRICES,-572662307>::`scalar deleting destructor'(&p_m_slotlistarray->m_data[v9], 0);
        ++v3;
        ++v9;
      }
      while ( v3 < p_m_slotlistarray->m_count );
    }
  }
  else
  {
    if ( this->m_slotlistarray.m_alloc < 4 )
    {
      m_chunk = this->m_slotlistarray.m_chunk;
      if ( m_chunk == 0 )
        m_chunk = TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CalcChunkSize(
                    &this->m_slotlistarray,
                    4u);
      v5 = TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::RoundToChunk(p_m_slotlistarray, 4u, m_chunk);
      TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::ReallocData(p_m_slotlistarray, v5);
    }
    v6 = p_m_slotlistarray->m_count;
    if ( v6 < 4 )
    {
      v7 = v6;
      v28 = 4 - v6;
      do
      {
        v8 = &p_m_slotlistarray->m_data[v7];
        if ( v8 != nullptr )
        {
          TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>(v8);
          v8->m_linkoffset = -572662307;
          TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::InitializeTerminator(v8);
        }
        ++v7;
        --v28;
      }
      while ( v28 != 0 );
    }
  }
  v10 = 0;
  p_m_slotlistarray->m_count = 4;
  slot = 0;
  v29 = 0;
  do
  {
    if ( v10 >= p_m_slotlistarray->m_count )
    {
      v11 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              v10,
              p_m_slotlistarray->m_count);
      v12 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v11);
      SErrDisplayErrorFmt(0x85100080, v12, v22, v23, v24, v25);
    }
    m_data = p_m_slotlistarray->m_data;
    m_linkoffset = m_data[v29].m_linkoffset;
    v15 = &m_data[v29];
    if ( m_linkoffset != 4 )
    {
      p_m_terminator = (unsigned int)&v15->m_terminator;
      while ( 1 )
      {
        v17 = *(_DWORD *)(p_m_terminator + 4);
        if ( v17 <= 0 )
          break;
        v18 = *(_DWORD *)(v15->m_linkoffset + v17);
        v19 = (_DWORD *)(v17 + v15->m_linkoffset);
        if ( v18 != 0 )
        {
          v20 = v19[1];
          if ( v20 > 0 )
            v21 = (int)v19 + v20 - *(_DWORD *)(v18 + 4);
          else
            v21 = ~v20;
          *(_DWORD *)v21 = v18;
          *(_DWORD *)(*v19 + 4) = v19[1];
          p_m_terminator = (unsigned int)&v15->m_terminator;
          *v19 = 0;
          v19[1] = 0;
        }
      }
      v10 = slot;
      *(_DWORD *)p_m_terminator = p_m_terminator;
      v15->m_linkoffset = 4;
      v15->m_terminator.m_next = (STRINGVIEWMATRICES *)~p_m_terminator;
    }
    slot = ++v10;
    ++v29;
  }
  while ( v10 <= this->m_slotmask );
}


// -----------------------------------------------------------------------------
// RVA 0x001CC8C0  EA 0x5CC8C0
// DIA: TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >::TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *__thiscall TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>(
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 0;
  this->m_terminator.m_next = (STRINGVIEWMATRICES *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CC8E0  EA 0x5CC8E0
// DIA: TSExplicitList<STRINGVIEWMATRICES,-572662307>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSExplicitList<STRINGVIEWMATRICES,-572662307> *__thiscall TSExplicitList<STRINGVIEWMATRICES,-572662307>::`scalar deleting destructor'(
        TSExplicitList<STRINGVIEWMATRICES,-572662307> *this,
        char __flags)
{
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v2; // esi
  TSLink<STRINGVIEWMATRICES> *p_m_terminator; // edi
  int m_next; // ecx
  int m_linkoffset; // eax
  int v6; // esi
  _DWORD *v7; // eax
  int v8; // edx
  int v9; // edx
  TSLink<STRINGVIEWMATRICES> *m_prevlink; // edx
  int v11; // ecx
  int v12; // ecx

  v2 = this;
  p_m_terminator = &this->m_terminator;
  while ( 1 )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next <= 0 )
      break;
    m_linkoffset = v2->m_linkoffset;
    v6 = *(_DWORD *)(v2->m_linkoffset + m_next);
    v7 = (_DWORD *)(m_next + m_linkoffset);
    if ( v6 != 0 )
    {
      v8 = v7[1];
      if ( v8 > 0 )
        v9 = (int)v7 + v8 - *(_DWORD *)(v6 + 4);
      else
        v9 = ~v8;
      *(_DWORD *)v9 = v6;
      *(_DWORD *)(*v7 + 4) = v7[1];
      *v7 = 0;
      v7[1] = 0;
    }
    v2 = this;
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    v11 = (int)p_m_terminator->m_next;
    if ( v11 > 0 )
      v12 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v11;
    else
      v12 = ~v11;
    *(_DWORD *)v12 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( (__flags & 1) != 0 && v2 != nullptr )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CC990  EA 0x5CC990
// DIA: TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::MonitorFullness
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR>::MonitorFullness(
        TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *this,
        unsigned int slot)
{
  bool v2; // cf
  unsigned int m_fullnessIndicator; // eax
  TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *p_m_slotlistarray; // esi
  int v6; // eax
  const char *v7; // eax
  unsigned int v8; // ecx
  STRINGVIEWMATRICES *m_next; // edi
  int v10; // edi
  TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *v11; // eax
  unsigned int v12; // edx
  int v13; // eax
  const char *v14; // eax
  int v16; // edi
  unsigned int v17; // ebx
  int v18; // edi
  STRINGVIEWMATRICES *v19; // eax
  unsigned int v20; // edi
  unsigned int v21; // ebx
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *m_data; // edi
  int m_linkoffset; // eax
  TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *v24; // edi
  STRINGVIEWMATRICES *v25; // ebx
  unsigned int v26; // edi
  int v27; // [esp-18h] [ebp-38h]
  int v28; // [esp-18h] [ebp-38h]
  int v29; // [esp-14h] [ebp-34h]
  int v30; // [esp-14h] [ebp-34h]
  unsigned int v31; // [esp-10h] [ebp-30h]
  unsigned int v32; // [esp-10h] [ebp-30h]
  const char *v33; // [esp-Ch] [ebp-2Ch]
  const char *v34; // [esp-Ch] [ebp-2Ch]
  TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > v35; // [esp+Ch] [ebp-14h] BYREF
  TSHashTable<STRINGVIEWMATRICES,HASHKEY_PTR> *v36; // [esp+18h] [ebp-8h]
  unsigned int count; // [esp+1Ch] [ebp-4h]
  unsigned int slota; // [esp+28h] [ebp+8h]
  unsigned int slotb; // [esp+28h] [ebp+8h]
  unsigned int slotc; // [esp+28h] [ebp+8h]

  v2 = this->m_slotmask < 0x1FFF;
  v36 = this;
  if ( !v2 )
    return 0;
  m_fullnessIndicator = this->m_fullnessIndicator;
  if ( m_fullnessIndicator <= 3 )
    this->m_fullnessIndicator = 0;
  else
    this->m_fullnessIndicator = m_fullnessIndicator - 3;
  p_m_slotlistarray = &this->m_slotlistarray;
  if ( slot >= this->m_slotlistarray.m_count )
  {
    v6 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo)(
           &this->m_slotlistarray,
           1,
           1,
           aIndex0x08xArra,
           slot,
           this->m_slotlistarray.m_count);
    v7 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName)(
                         p_m_slotlistarray,
                         v6);
    SErrDisplayErrorFmt(0x85100080, v7, v27, v29, v31, v33);
  }
  v8 = 12 * slot;
  m_next = p_m_slotlistarray->m_data[slot].m_terminator.m_next;
  slota = 12 * slot;
  v10 = (int)m_next <= 0 ? 0 : (unsigned int)m_next;
  if ( v10 <= 0 )
    return 0;
  while ( 1 )
  {
    v11 = v36;
    v12 = v36->m_fullnessIndicator + 1;
    v36->m_fullnessIndicator = v12;
    if ( v12 > 0xD )
      break;
    if ( slot >= p_m_slotlistarray->m_count )
    {
      v13 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              slot,
              p_m_slotlistarray->m_count);
      v14 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v13);
      SErrDisplayErrorFmt(0x85100080, v14, v28, v30, v32, v34);
      v8 = slota;
    }
    v10 = *(_DWORD *)(v10 + *(int *)((char *)&p_m_slotlistarray->m_data->m_linkoffset + v8) + 4);
    if ( v10 <= 0 )
      return 0;
  }
  v11->m_fullnessIndicator = 0;
  v16 = v11->m_slotmask + 1;
  slotb = v16;
  count = 2 * v16;
  TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>(&v35);
  v35.m_linkoffset = -572662307;
  TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::InitializeTerminator(&v35);
  if ( v35.m_linkoffset != 4 )
  {
    TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::UnlinkAll(&v35);
    v35.m_linkoffset = 4;
    TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::InitializeTerminator(&v35);
  }
  v17 = 0;
  if ( v16 != 0 )
  {
    v18 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v17);
        v19 = p_m_slotlistarray->m_data[v18].m_terminator.m_next;
        if ( (int)v19 <= 0 )
          break;
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::LinkNode(&v35, v19, 2u, nullptr);
      }
      ++v17;
      ++v18;
    }
    while ( v17 < slotb );
  }
  v20 = count;
  TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::SetCount(p_m_slotlistarray, count);
  v21 = 0;
  if ( v20 != 0 )
  {
    slotc = 0;
    do
    {
      TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v21);
      m_data = p_m_slotlistarray->m_data;
      m_linkoffset = m_data[slotc].m_linkoffset;
      v24 = &m_data[slotc];
      if ( m_linkoffset != 4 )
      {
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::UnlinkAll(v24);
        v24->m_linkoffset = 4;
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::InitializeTerminator(v24);
      }
      ++v21;
      ++slotc;
    }
    while ( v21 < count );
    v20 = count;
  }
  v36->m_slotmask = v20 - 1;
  while ( (int)v35.m_terminator.m_next > 0 )
  {
    v25 = v35.m_terminator.m_next;
    v26 = v36->m_slotmask & v35.m_terminator.m_next->m_hashval;
    TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v26);
    TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::LinkNode(
      &p_m_slotlistarray->m_data[v26],
      v25,
      2u,
      nullptr);
  }
  TSLink<STRINGVIEWMATRICES>::Unlink(&v35.m_terminator);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001CCBC0  EA 0x5CCBC0
// DIA: TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >::UnlinkAll
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::UnlinkAll(
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *this)
{
  STRINGVIEWMATRICES *m_next; // edx
  int v2; // edi
  _DWORD *v3; // eax
  int v4; // esi
  int v5; // esi

  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v2 = *(unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    v3 = (unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    if ( v2 != 0 )
    {
      v4 = v3[1];
      if ( v4 > 0 )
        v5 = (int)v3 + v4 - *(_DWORD *)(v2 + 4);
      else
        v5 = ~v4;
      *(_DWORD *)v5 = v2;
      *(_DWORD *)(*v3 + 4) = v3[1];
      *v3 = 0;
      v3[1] = 0;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CCC10  EA 0x5CCC10
// DIA: TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::InitializeTerminator(
        TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES> > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (STRINGVIEWMATRICES *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CCC20  EA 0x5CCC20
// DIA: TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::CheckArrayBounds
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CheckArrayBounds(
        TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *this,
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
    v4 = ((int (__thiscall *)(TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int, int, char *, unsigned int, unsigned int))this->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo)(
           this,
           1,
           1,
           aIndex0x08xArra,
           index,
           m_count);
    v5 = (const char *)((int (__thiscall *)(TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *, int))this->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName)(
                         this,
                         v4);
    SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CCC60  EA 0x5CCC60
// DIA: TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::GrowListArray
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE>::GrowListArray(
        TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *this,
        unsigned int newarraysize)
{
  TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *v2; // edi
  unsigned int v3; // ebx
  unsigned int v4; // esi
  TSLink<GLYPHBITMAPDATA> *v5; // eax
  int v6; // esi
  TSLink<GLYPHBITMAPDATA> *v7; // edi
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // eax
  unsigned int m_count; // eax
  TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *p_m_slotlistarray; // esi
  unsigned int v11; // edi
  unsigned int v12; // eax
  int v13; // ebx
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v14; // ecx
  unsigned int v15; // ebx
  unsigned int v16; // edi
  unsigned int v17; // ebx
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *m_data; // edi
  int v19; // eax
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *v20; // edi
  TSLink<GLYPHBITMAPDATA> *v21; // eax
  GLYPHBITMAPDATA *m_next; // ebx
  unsigned int v23; // edi
  int v24; // eax
  const char *v25; // eax
  int v26; // [esp-18h] [ebp-3Ch]
  int v27; // [esp-14h] [ebp-38h]
  unsigned int v28; // [esp-10h] [ebp-34h]
  const char *v29; // [esp-Ch] [ebp-30h]
  TSExplicitList<GLYPHBITMAPDATA,-572662307> templist; // [esp+Ch] [ebp-18h] BYREF
  unsigned int oldarraysize; // [esp+18h] [ebp-Ch]
  TSHashTable<GLYPHBITMAPDATA,HASHKEY_NONE> *v32; // [esp+1Ch] [ebp-8h]
  int v33; // [esp+20h] [ebp-4h]

  v2 = this;
  v3 = 0;
  v4 = this->m_slotmask + 1;
  v32 = this;
  oldarraysize = v4;
  memset(&templist, 0, sizeof(templist));
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::InitializeTerminator(&templist);
  templist.m_linkoffset = -572662307;
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::InitializeTerminator(&templist);
  if ( templist.m_linkoffset != 4 )
  {
    while ( (int)templist.m_terminator.m_next > 0 )
    {
      v5 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(&templist, templist.m_terminator.m_next);
      TSLink<GLYPHBITMAPDATA>::Unlink(v5);
    }
    templist.m_linkoffset = 4;
    TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::InitializeTerminator(&templist);
  }
  if ( v4 != 0 )
  {
    v33 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(&v2->m_slotlistarray, v3);
        v6 = *(int *)((char *)&v2->m_slotlistarray.m_data->m_terminator.m_next + v33);
        if ( v6 <= 0 )
          break;
        v7 = (TSLink<GLYPHBITMAPDATA> *)(v6 + templist.m_linkoffset);
        if ( *(_DWORD *)(v6 + templist.m_linkoffset) != 0 )
          TSLink<GLYPHBITMAPDATA>::Unlink((TSLink<GLYPHBITMAPDATA> *)(v6 + templist.m_linkoffset));
        m_prevlink = templist.m_terminator.m_prevlink;
        v7->m_prevlink = templist.m_terminator.m_prevlink;
        v7->m_next = m_prevlink->m_next;
        m_prevlink->m_next = (GLYPHBITMAPDATA *)v6;
        templist.m_terminator.m_prevlink = v7;
        v2 = v32;
      }
      ++v3;
      v33 += 12;
    }
    while ( v3 < oldarraysize );
  }
  m_count = v2->m_slotlistarray.m_count;
  p_m_slotlistarray = &v2->m_slotlistarray;
  v11 = newarraysize;
  if ( newarraysize <= m_count )
  {
    if ( newarraysize < m_count )
    {
      v15 = newarraysize;
      v16 = newarraysize;
      do
      {
        TSExplicitList<GLYPHBITMAPDATA,-572662307>::`scalar deleting destructor'(&p_m_slotlistarray->m_data[v16], 0);
        ++v15;
        ++v16;
      }
      while ( v15 < p_m_slotlistarray->m_count );
      v11 = newarraysize;
    }
  }
  else
  {
    TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::Reserve(p_m_slotlistarray, newarraysize - m_count, 1);
    v12 = p_m_slotlistarray->m_count;
    if ( v12 < newarraysize )
    {
      v13 = v12;
      v33 = newarraysize - v12;
      do
      {
        v14 = &p_m_slotlistarray->m_data[v13];
        if ( v14 != nullptr )
          TSExplicitList<GLYPHBITMAPDATA,-572662307>::TSExplicitList<GLYPHBITMAPDATA,-572662307>(v14);
        ++v13;
        --v33;
      }
      while ( v33 != 0 );
    }
  }
  v17 = 0;
  p_m_slotlistarray->m_count = v11;
  if ( v11 != 0 )
  {
    v33 = 0;
    do
    {
      TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v17);
      m_data = p_m_slotlistarray->m_data;
      v19 = *(int *)((char *)&m_data->m_linkoffset + v33);
      v20 = (TSExplicitList<GLYPHBITMAPDATA,-572662307> *)((char *)m_data + v33);
      if ( v19 != 4 )
      {
        while ( (int)v20->m_terminator.m_next > 0 )
        {
          v21 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(v20, v20->m_terminator.m_next);
          TSLink<GLYPHBITMAPDATA>::Unlink(v21);
        }
        v20->m_linkoffset = 4;
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::InitializeTerminator(v20);
      }
      ++v17;
      v33 += 12;
    }
    while ( v17 < newarraysize );
    v11 = newarraysize;
  }
  v32->m_slotmask = v11 - 1;
  while ( (int)templist.m_terminator.m_next > 0 )
  {
    m_next = templist.m_terminator.m_next;
    v23 = v32->m_slotmask & templist.m_terminator.m_next->m_hashval;
    if ( v23 >= p_m_slotlistarray->m_count )
    {
      v24 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              v23,
              p_m_slotlistarray->m_count);
      v25 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v24);
      SErrDisplayErrorFmt(0x85100080, v25, v26, v27, v28, v29);
    }
    TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::LinkNode(
      &p_m_slotlistarray->m_data[v23],
      m_next,
      2u,
      nullptr);
  }
  TSLink<GLYPHBITMAPDATA>::Unlink(&templist.m_terminator);
}


// -----------------------------------------------------------------------------
// RVA 0x001CCE80  EA 0x5CCE80
// DIA: TSExplicitList<GLYPHBITMAPDATA,-572662307>::TSExplicitList<GLYPHBITMAPDATA,-572662307>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSExplicitList<GLYPHBITMAPDATA,-572662307> *__thiscall TSExplicitList<GLYPHBITMAPDATA,-572662307>::TSExplicitList<GLYPHBITMAPDATA,-572662307>(
        TSExplicitList<GLYPHBITMAPDATA,-572662307> *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = -572662307;
  this->m_terminator.m_next = (GLYPHBITMAPDATA *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CCEA0  EA 0x5CCEA0
// DIA: TSExplicitList<GLYPHBITMAPDATA,-572662307>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSExplicitList<GLYPHBITMAPDATA,-572662307> *__thiscall TSExplicitList<GLYPHBITMAPDATA,-572662307>::`scalar deleting destructor'(
        TSExplicitList<GLYPHBITMAPDATA,-572662307> *this,
        char __flags)
{
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v2; // esi
  TSLink<GLYPHBITMAPDATA> *p_m_terminator; // edi
  int m_next; // ecx
  int m_linkoffset; // eax
  int v6; // esi
  _DWORD *v7; // eax
  int v8; // edx
  int v9; // edx
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // edx
  int v11; // ecx
  int v12; // ecx

  v2 = this;
  p_m_terminator = &this->m_terminator;
  while ( 1 )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next <= 0 )
      break;
    m_linkoffset = v2->m_linkoffset;
    v6 = *(_DWORD *)(v2->m_linkoffset + m_next);
    v7 = (_DWORD *)(m_next + m_linkoffset);
    if ( v6 != 0 )
    {
      v8 = v7[1];
      if ( v8 > 0 )
        v9 = (int)v7 + v8 - *(_DWORD *)(v6 + 4);
      else
        v9 = ~v8;
      *(_DWORD *)v9 = v6;
      *(_DWORD *)(*v7 + 4) = v7[1];
      *v7 = 0;
      v7[1] = 0;
    }
    v2 = this;
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    v11 = (int)p_m_terminator->m_next;
    if ( v11 > 0 )
      v12 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v11;
    else
      v12 = ~v11;
    *(_DWORD *)v12 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( (__flags & 1) != 0 && v2 != nullptr )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CCF50  EA 0x5CCF50
// DIA: TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::InitializeTerminator(
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (GLYPHBITMAPDATA *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CCF60  EA 0x5CCF60
// DIA: TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::SetCount
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::SetCount(
        TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *this,
        unsigned int count)
{
  unsigned int v2; // ebx
  TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *v3; // edi
  unsigned int m_count; // eax
  unsigned int m_chunk; // ecx
  unsigned int i; // eax
  unsigned int v7; // esi
  unsigned int v8; // eax
  int v9; // edx
  int v10; // esi
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v11; // ecx
  unsigned int v12; // eax
  _DWORD *v13; // edi
  TSLink<GLYPHBITMAPDATA> *v14; // esi
  int v15; // eax
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // edi
  bool v17; // cf
  unsigned int v18; // [esp+Ch] [ebp-8h]

  v2 = count;
  v3 = this;
  m_count = this->m_count;
  if ( count > m_count )
  {
    if ( count > this->m_alloc )
    {
      m_chunk = this->m_chunk;
      if ( m_chunk == 0 )
      {
        m_chunk = count;
        if ( count >= 0x15 )
        {
          v3->m_chunk = 21;
          m_chunk = 21;
        }
        else
        {
          for ( i = count & (count - 1); i != 0; i &= i - 1 )
            m_chunk = i;
          if ( m_chunk == 0 )
            m_chunk = 1;
        }
      }
      v7 = count;
      if ( count % m_chunk != 0 )
        v7 = count + m_chunk - count % m_chunk;
      TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::ReallocData(v3, v7);
    }
    v8 = v3->m_count;
    if ( v8 < count )
    {
      v9 = v8;
      v10 = count - v8;
      do
      {
        v11 = &v3->m_data[v9];
        if ( v11 != nullptr )
        {
          v11->m_terminator.m_prevlink = nullptr;
          v11->m_terminator.m_next = nullptr;
          v11->m_terminator.m_prevlink = &v11->m_terminator;
          v11->m_linkoffset = -572662307;
          v11->m_terminator.m_next = (GLYPHBITMAPDATA *)~(unsigned int)&v11->m_terminator;
        }
        ++v9;
        --v10;
      }
      while ( v10 != 0 );
    }
    goto LABEL_18;
  }
  if ( count >= m_count )
  {
LABEL_18:
    v3->m_count = count;
    return;
  }
  v12 = 12 * count;
  v18 = 12 * count;
  while ( 1 )
  {
    v13 = (int *)((char *)&v3->m_data->m_linkoffset + v12);
    v14 = (TSLink<GLYPHBITMAPDATA> *)(v13 + 1);
    while ( 1 )
    {
      v15 = v13[2];
      if ( v15 <= 0 )
        break;
      TSLink<GLYPHBITMAPDATA>::Unlink((TSLink<GLYPHBITMAPDATA> *)(v15 + *v13));
    }
    if ( v14->m_prevlink != nullptr )
    {
      m_prevlink = v14->m_prevlink;
      TSLink<GLYPHBITMAPDATA>::NextLink(v14, -1)->m_prevlink = m_prevlink;
      v14->m_prevlink->m_next = v14->m_next;
      v14->m_prevlink = nullptr;
      v14->m_next = nullptr;
    }
    v12 = v18 + 12;
    v17 = ++count < this->m_count;
    v18 += 12;
    if ( !v17 )
      break;
    v3 = this;
  }
  this->m_count = v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CD0A0  EA 0x5CD0A0
// DIA: TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> >::SetCount
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::SetCount(
        TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *this,
        unsigned int count)
{
  unsigned int v2; // ebx
  TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *v3; // edi
  unsigned int m_count; // eax
  unsigned int m_chunk; // ecx
  unsigned int i; // eax
  unsigned int v7; // esi
  unsigned int v8; // eax
  int v9; // edx
  int v10; // esi
  TSExplicitList<CHARCODEDESC,-572662307> *v11; // ecx
  unsigned int v12; // eax
  _DWORD *v13; // edi
  TSLink<CHARCODEDESC> *v14; // esi
  int v15; // eax
  TSLink<CHARCODEDESC> *m_prevlink; // edi
  bool v17; // cf
  unsigned int v18; // [esp+Ch] [ebp-8h]

  v2 = count;
  v3 = this;
  m_count = this->m_count;
  if ( count > m_count )
  {
    if ( count > this->m_alloc )
    {
      m_chunk = this->m_chunk;
      if ( m_chunk == 0 )
      {
        m_chunk = count;
        if ( count >= 0x15 )
        {
          v3->m_chunk = 21;
          m_chunk = 21;
        }
        else
        {
          for ( i = count & (count - 1); i != 0; i &= i - 1 )
            m_chunk = i;
          if ( m_chunk == 0 )
            m_chunk = 1;
        }
      }
      v7 = count;
      if ( count % m_chunk != 0 )
        v7 = count + m_chunk - count % m_chunk;
      TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>::ReallocData(v3, v7);
    }
    v8 = v3->m_count;
    if ( v8 < count )
    {
      v9 = v8;
      v10 = count - v8;
      do
      {
        v11 = &v3->m_data[v9];
        if ( v11 != nullptr )
        {
          v11->m_terminator.m_prevlink = nullptr;
          v11->m_terminator.m_next = nullptr;
          v11->m_terminator.m_prevlink = &v11->m_terminator;
          v11->m_linkoffset = -572662307;
          v11->m_terminator.m_next = (CHARCODEDESC *)~(unsigned int)&v11->m_terminator;
        }
        ++v9;
        --v10;
      }
      while ( v10 != 0 );
    }
    goto LABEL_18;
  }
  if ( count >= m_count )
  {
LABEL_18:
    v3->m_count = count;
    return;
  }
  v12 = 12 * count;
  v18 = 12 * count;
  while ( 1 )
  {
    v13 = (int *)((char *)&v3->m_data->m_linkoffset + v12);
    v14 = (TSLink<CHARCODEDESC> *)(v13 + 1);
    while ( 1 )
    {
      v15 = v13[2];
      if ( v15 <= 0 )
        break;
      TSLink<CHARCODEDESC>::Unlink((TSLink<CHARCODEDESC> *)(v15 + *v13));
    }
    if ( v14->m_prevlink != nullptr )
    {
      m_prevlink = v14->m_prevlink;
      TSLink<CHARCODEDESC>::NextLink(v14, -1)->m_prevlink = m_prevlink;
      v14->m_prevlink->m_next = v14->m_next;
      v14->m_prevlink = nullptr;
      v14->m_next = nullptr;
    }
    v12 = v18 + 12;
    v17 = ++count < this->m_count;
    v18 += 12;
    if ( !v17 )
      break;
    v3 = this;
  }
  this->m_count = v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CD1E0  EA 0x5CD1E0
// DIA: TSHashTable<KERNNODE,KERNINGHASHKEY>::GrowListArray
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<KERNNODE,KERNINGHASHKEY>::GrowListArray(
        TSHashTable<KERNNODE,KERNINGHASHKEY> *this,
        unsigned int newarraysize)
{
  TSHashTable<KERNNODE,KERNINGHASHKEY> *v2; // edi
  unsigned int v3; // ebx
  unsigned int v4; // esi
  TSLink<KERNNODE> *v5; // eax
  int v6; // esi
  TSLink<KERNNODE> *v7; // edi
  TSLink<KERNNODE> *m_prevlink; // eax
  unsigned int m_count; // eax
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *p_m_slotlistarray; // esi
  unsigned int v11; // edi
  unsigned int v12; // eax
  int v13; // ebx
  TSExplicitList<KERNNODE,-572662307> *v14; // ecx
  unsigned int v15; // ebx
  unsigned int v16; // edi
  unsigned int v17; // ebx
  TSExplicitList<KERNNODE,-572662307> *m_data; // edi
  int v19; // eax
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *v20; // edi
  TSLink<KERNNODE> *v21; // eax
  KERNNODE *m_next; // ebx
  unsigned int v23; // edi
  int v24; // eax
  const char *v25; // eax
  int v26; // [esp-18h] [ebp-3Ch]
  int v27; // [esp-14h] [ebp-38h]
  unsigned int v28; // [esp-10h] [ebp-34h]
  const char *v29; // [esp-Ch] [ebp-30h]
  TSExplicitList<KERNNODE,-572662307> templist; // [esp+Ch] [ebp-18h] BYREF
  unsigned int oldarraysize; // [esp+18h] [ebp-Ch]
  TSHashTable<KERNNODE,KERNINGHASHKEY> *v32; // [esp+1Ch] [ebp-8h]
  int v33; // [esp+20h] [ebp-4h]

  v2 = this;
  v3 = 0;
  v4 = this->m_slotmask + 1;
  v32 = this;
  oldarraysize = v4;
  memset(&templist, 0, sizeof(templist));
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(&templist);
  templist.m_linkoffset = -572662307;
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(&templist);
  if ( templist.m_linkoffset != 4 )
  {
    while ( (int)templist.m_terminator.m_next > 0 )
    {
      v5 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(&templist, templist.m_terminator.m_next);
      TSLink<KERNNODE>::Unlink(v5);
    }
    templist.m_linkoffset = 4;
    TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(&templist);
  }
  if ( v4 != 0 )
  {
    v33 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(&v2->m_slotlistarray, v3);
        v6 = *(int *)((char *)&v2->m_slotlistarray.m_data->m_terminator.m_next + v33);
        if ( v6 <= 0 )
          break;
        v7 = (TSLink<KERNNODE> *)(v6 + templist.m_linkoffset);
        if ( *(_DWORD *)(v6 + templist.m_linkoffset) != 0 )
          TSLink<KERNNODE>::Unlink((TSLink<KERNNODE> *)(v6 + templist.m_linkoffset));
        m_prevlink = templist.m_terminator.m_prevlink;
        v7->m_prevlink = templist.m_terminator.m_prevlink;
        v7->m_next = m_prevlink->m_next;
        m_prevlink->m_next = (KERNNODE *)v6;
        templist.m_terminator.m_prevlink = v7;
        v2 = v32;
      }
      ++v3;
      v33 += 12;
    }
    while ( v3 < oldarraysize );
  }
  m_count = v2->m_slotlistarray.m_count;
  p_m_slotlistarray = &v2->m_slotlistarray;
  v11 = newarraysize;
  if ( newarraysize <= m_count )
  {
    if ( newarraysize < m_count )
    {
      v15 = newarraysize;
      v16 = newarraysize;
      do
      {
        TSExplicitList<KERNNODE,-572662307>::`scalar deleting destructor'(&p_m_slotlistarray->m_data[v16], 0);
        ++v15;
        ++v16;
      }
      while ( v15 < p_m_slotlistarray->m_count );
      v11 = newarraysize;
    }
  }
  else
  {
    TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::Reserve(p_m_slotlistarray, newarraysize - m_count, 1);
    v12 = p_m_slotlistarray->m_count;
    if ( v12 < newarraysize )
    {
      v13 = v12;
      v33 = newarraysize - v12;
      do
      {
        v14 = &p_m_slotlistarray->m_data[v13];
        if ( v14 != nullptr )
          TSExplicitList<KERNNODE,-572662307>::TSExplicitList<KERNNODE,-572662307>(v14);
        ++v13;
        --v33;
      }
      while ( v33 != 0 );
    }
  }
  v17 = 0;
  p_m_slotlistarray->m_count = v11;
  if ( v11 != 0 )
  {
    v33 = 0;
    do
    {
      TSBaseArray<TSExplicitList<KERNNODE,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v17);
      m_data = p_m_slotlistarray->m_data;
      v19 = *(int *)((char *)&m_data->m_linkoffset + v33);
      v20 = (TSExplicitList<KERNNODE,-572662307> *)((char *)m_data + v33);
      if ( v19 != 4 )
      {
        while ( (int)v20->m_terminator.m_next > 0 )
        {
          v21 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(v20, v20->m_terminator.m_next);
          TSLink<KERNNODE>::Unlink(v21);
        }
        v20->m_linkoffset = 4;
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(v20);
      }
      ++v17;
      v33 += 12;
    }
    while ( v17 < newarraysize );
    v11 = newarraysize;
  }
  v32->m_slotmask = v11 - 1;
  while ( (int)templist.m_terminator.m_next > 0 )
  {
    m_next = templist.m_terminator.m_next;
    v23 = v32->m_slotmask & templist.m_terminator.m_next->m_hashval;
    if ( v23 >= p_m_slotlistarray->m_count )
    {
      v24 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              v23,
              p_m_slotlistarray->m_count);
      v25 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v24);
      SErrDisplayErrorFmt(0x85100080, v25, v26, v27, v28, v29);
    }
    TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::LinkNode(&p_m_slotlistarray->m_data[v23], m_next, 2u, nullptr);
  }
  TSLink<KERNNODE>::Unlink(&templist.m_terminator);
}


// -----------------------------------------------------------------------------
// RVA 0x001CD400  EA 0x5CD400
// DIA: TSExplicitList<KERNNODE,-572662307>::TSExplicitList<KERNNODE,-572662307>
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSExplicitList<KERNNODE,-572662307> *__thiscall TSExplicitList<KERNNODE,-572662307>::TSExplicitList<KERNNODE,-572662307>(
        TSExplicitList<KERNNODE,-572662307> *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = -572662307;
  this->m_terminator.m_next = (KERNNODE *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CD420  EA 0x5CD420
// DIA: TSList<KERNNODE,TSGetExplicitLink<KERNNODE> >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::InitializeTerminator(
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (KERNNODE *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CD430  EA 0x5CD430
// DIA: TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >::SetCount
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::SetCount(
        TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *this,
        unsigned int count)
{
  unsigned int v2; // ebx
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *v3; // edi
  unsigned int m_count; // eax
  unsigned int m_chunk; // ecx
  unsigned int i; // eax
  unsigned int v7; // esi
  unsigned int v8; // eax
  int v9; // edx
  int v10; // esi
  TSExplicitList<KERNNODE,-572662307> *v11; // ecx
  unsigned int v12; // eax
  _DWORD *v13; // edi
  TSLink<KERNNODE> *v14; // esi
  int v15; // eax
  TSLink<KERNNODE> *m_prevlink; // edi
  bool v17; // cf
  unsigned int v18; // [esp+Ch] [ebp-8h]

  v2 = count;
  v3 = this;
  m_count = this->m_count;
  if ( count > m_count )
  {
    if ( count > this->m_alloc )
    {
      m_chunk = this->m_chunk;
      if ( m_chunk == 0 )
      {
        m_chunk = count;
        if ( count >= 0x15 )
        {
          v3->m_chunk = 21;
          m_chunk = 21;
        }
        else
        {
          for ( i = count & (count - 1); i != 0; i &= i - 1 )
            m_chunk = i;
          if ( m_chunk == 0 )
            m_chunk = 1;
        }
      }
      v7 = count;
      if ( count % m_chunk != 0 )
        v7 = count + m_chunk - count % m_chunk;
      TSFixedArray<TSExplicitList<KERNNODE,-572662307>>::ReallocData(v3, v7);
    }
    v8 = v3->m_count;
    if ( v8 < count )
    {
      v9 = v8;
      v10 = count - v8;
      do
      {
        v11 = &v3->m_data[v9];
        if ( v11 != nullptr )
        {
          v11->m_terminator.m_prevlink = nullptr;
          v11->m_terminator.m_next = nullptr;
          v11->m_terminator.m_prevlink = &v11->m_terminator;
          v11->m_linkoffset = -572662307;
          v11->m_terminator.m_next = (KERNNODE *)~(unsigned int)&v11->m_terminator;
        }
        ++v9;
        --v10;
      }
      while ( v10 != 0 );
    }
    goto LABEL_18;
  }
  if ( count >= m_count )
  {
LABEL_18:
    v3->m_count = count;
    return;
  }
  v12 = 12 * count;
  v18 = 12 * count;
  while ( 1 )
  {
    v13 = (int *)((char *)&v3->m_data->m_linkoffset + v12);
    v14 = (TSLink<KERNNODE> *)(v13 + 1);
    while ( 1 )
    {
      v15 = v13[2];
      if ( v15 <= 0 )
        break;
      TSLink<KERNNODE>::Unlink((TSLink<KERNNODE> *)(v15 + *v13));
    }
    if ( v14->m_prevlink != nullptr )
    {
      m_prevlink = v14->m_prevlink;
      TSLink<KERNNODE>::NextLink(v14, -1)->m_prevlink = m_prevlink;
      v14->m_prevlink->m_next = v14->m_next;
      v14->m_prevlink = nullptr;
      v14->m_next = nullptr;
    }
    v12 = v18 + 12;
    v17 = ++count < this->m_count;
    v18 += 12;
    if ( !v17 )
      break;
    v3 = this;
  }
  this->m_count = v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CD570  EA 0x5CD570
// DIA: TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::SetCount
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::SetCount(
        TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this,
        unsigned int count)
{
  unsigned int v2; // ebx
  TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *v3; // edi
  unsigned int m_count; // eax
  unsigned int m_chunk; // ecx
  unsigned int i; // eax
  unsigned int v7; // esi
  unsigned int v8; // eax
  int v9; // edx
  int v10; // esi
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v11; // ecx
  unsigned int v12; // eax
  _DWORD *v13; // edi
  TSLink<BATCHEDRENDERFONTDESC> *v14; // esi
  int v15; // eax
  TSLink<BATCHEDRENDERFONTDESC> *m_prevlink; // edi
  bool v17; // cf
  unsigned int v18; // [esp+Ch] [ebp-8h]

  v2 = count;
  v3 = this;
  m_count = this->m_count;
  if ( count > m_count )
  {
    if ( count > this->m_alloc )
    {
      m_chunk = this->m_chunk;
      if ( m_chunk == 0 )
      {
        m_chunk = count;
        if ( count >= 0x15 )
        {
          v3->m_chunk = 21;
          m_chunk = 21;
        }
        else
        {
          for ( i = count & (count - 1); i != 0; i &= i - 1 )
            m_chunk = i;
          if ( m_chunk == 0 )
            m_chunk = 1;
        }
      }
      v7 = count;
      if ( count % m_chunk != 0 )
        v7 = count + m_chunk - count % m_chunk;
      TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::ReallocData(v3, v7);
    }
    v8 = v3->m_count;
    if ( v8 < count )
    {
      v9 = v8;
      v10 = count - v8;
      do
      {
        v11 = &v3->m_data[v9];
        if ( v11 != nullptr )
        {
          v11->m_terminator.m_prevlink = nullptr;
          v11->m_terminator.m_next = nullptr;
          v11->m_terminator.m_prevlink = &v11->m_terminator;
          v11->m_linkoffset = -572662307;
          v11->m_terminator.m_next = (BATCHEDRENDERFONTDESC *)~(unsigned int)&v11->m_terminator;
        }
        ++v9;
        --v10;
      }
      while ( v10 != 0 );
    }
    goto LABEL_18;
  }
  if ( count >= m_count )
  {
LABEL_18:
    v3->m_count = count;
    return;
  }
  v12 = 12 * count;
  v18 = 12 * count;
  while ( 1 )
  {
    v13 = (int *)((char *)&v3->m_data->m_linkoffset + v12);
    v14 = (TSLink<BATCHEDRENDERFONTDESC> *)(v13 + 1);
    while ( 1 )
    {
      v15 = v13[2];
      if ( v15 <= 0 )
        break;
      TSLink<BATCHEDRENDERFONTDESC>::Unlink((TSLink<BATCHEDRENDERFONTDESC> *)(v15 + *v13));
    }
    if ( v14->m_prevlink != nullptr )
    {
      m_prevlink = v14->m_prevlink;
      TSLink<BATCHEDRENDERFONTDESC>::NextLink(v14, -1)->m_prevlink = m_prevlink;
      v14->m_prevlink->m_next = v14->m_next;
      v14->m_prevlink = nullptr;
      v14->m_next = nullptr;
    }
    v12 = v18 + 12;
    v17 = ++count < this->m_count;
    v18 += 12;
    if ( !v17 )
      break;
    v3 = this;
  }
  this->m_count = v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CD6B0  EA 0x5CD6B0
// DIA: TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::SetCount
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::SetCount(
        TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *this,
        unsigned int count)
{
  unsigned int v2; // ebx
  TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *v3; // edi
  unsigned int m_count; // eax
  unsigned int m_chunk; // ecx
  unsigned int i; // eax
  unsigned int v7; // esi
  unsigned int v8; // eax
  int v9; // edx
  int v10; // esi
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v11; // ecx
  unsigned int v12; // eax
  _DWORD *v13; // edi
  TSLink<STRINGVIEWMATRICES> *v14; // esi
  int v15; // eax
  TSLink<STRINGVIEWMATRICES> *m_prevlink; // edi
  bool v17; // cf
  unsigned int v18; // [esp+Ch] [ebp-8h]

  v2 = count;
  v3 = this;
  m_count = this->m_count;
  if ( count > m_count )
  {
    if ( count > this->m_alloc )
    {
      m_chunk = this->m_chunk;
      if ( m_chunk == 0 )
      {
        m_chunk = count;
        if ( count >= 0x15 )
        {
          v3->m_chunk = 21;
          m_chunk = 21;
        }
        else
        {
          for ( i = count & (count - 1); i != 0; i &= i - 1 )
            m_chunk = i;
          if ( m_chunk == 0 )
            m_chunk = 1;
        }
      }
      v7 = count;
      if ( count % m_chunk != 0 )
        v7 = count + m_chunk - count % m_chunk;
      TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::ReallocData(v3, v7);
    }
    v8 = v3->m_count;
    if ( v8 < count )
    {
      v9 = v8;
      v10 = count - v8;
      do
      {
        v11 = &v3->m_data[v9];
        if ( v11 != nullptr )
        {
          v11->m_terminator.m_prevlink = nullptr;
          v11->m_terminator.m_next = nullptr;
          v11->m_terminator.m_prevlink = &v11->m_terminator;
          v11->m_linkoffset = -572662307;
          v11->m_terminator.m_next = (STRINGVIEWMATRICES *)~(unsigned int)&v11->m_terminator;
        }
        ++v9;
        --v10;
      }
      while ( v10 != 0 );
    }
    goto LABEL_18;
  }
  if ( count >= m_count )
  {
LABEL_18:
    v3->m_count = count;
    return;
  }
  v12 = 12 * count;
  v18 = 12 * count;
  while ( 1 )
  {
    v13 = (int *)((char *)&v3->m_data->m_linkoffset + v12);
    v14 = (TSLink<STRINGVIEWMATRICES> *)(v13 + 1);
    while ( 1 )
    {
      v15 = v13[2];
      if ( v15 <= 0 )
        break;
      TSLink<STRINGVIEWMATRICES>::Unlink((TSLink<STRINGVIEWMATRICES> *)(v15 + *v13));
    }
    if ( v14->m_prevlink != nullptr )
    {
      m_prevlink = v14->m_prevlink;
      TSLink<STRINGVIEWMATRICES>::NextLink(v14, -1)->m_prevlink = m_prevlink;
      v14->m_prevlink->m_next = v14->m_next;
      v14->m_prevlink = nullptr;
      v14->m_next = nullptr;
    }
    v12 = v18 + 12;
    v17 = ++count < this->m_count;
    v18 += 12;
    if ( !v17 )
      break;
    v3 = this;
  }
  this->m_count = v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CD7F0  EA 0x5CD7F0
// DIA: TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::Reserve
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::Reserve(
        TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *this,
        unsigned int count,
        int round)
{
  unsigned int v4; // edi
  unsigned int m_chunk; // ecx
  int i; // eax
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v7; // ebx
  TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >_vtbl *v8; // edx
  const char *v9; // eax
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v10; // eax
  const char *v11; // eax
  unsigned int m_count; // eax
  int v13; // ebx
  TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *v14; // edi
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v15; // ecx
  int m_linkoffset; // edx
  TSLink<GLYPHBITMAPDATA> *v17; // eax
  const char *v18; // eax
  int v19; // [esp-8h] [ebp-14h]
  int v20; // [esp-8h] [ebp-14h]
  int v21; // [esp-8h] [ebp-14h]
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *counta; // [esp+14h] [ebp+8h]
  unsigned int rounda; // [esp+18h] [ebp+Ch]
  int roundb; // [esp+18h] [ebp+Ch]

  v4 = this->m_count + count;
  if ( v4 > this->m_alloc )
  {
    if ( round != 0 )
    {
      m_chunk = this->m_chunk;
      if ( m_chunk == 0 )
      {
        m_chunk = v4;
        if ( v4 >= 0x15 )
        {
          this->m_chunk = 21;
          m_chunk = 21;
        }
        else
        {
          for ( i = v4 & (v4 - 1); i != 0; i &= i - 1 )
            m_chunk = i;
          if ( m_chunk == 0 )
            m_chunk = 1;
        }
      }
      if ( v4 % m_chunk != 0 )
        v4 += m_chunk - v4 % m_chunk;
    }
    counta = this->m_data;
    rounda = v4;
    if ( v4 < this->m_count )
    {
      v7 = &this->m_data[v4];
      do
      {
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::UnlinkAll(v7);
        TSLink<GLYPHBITMAPDATA>::Unlink(&v7->m_terminator);
        ++v7;
        ++rounda;
      }
      while ( rounda < this->m_count );
    }
    v8 = this->__vftable;
    this->m_alloc = v4;
    v19 = v8->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName(this);
    v10 = (TSExplicitList<GLYPHBITMAPDATA,-572662307> *)SMemReAlloc(counta, 12 * v4, v9, v19, 0x10u);
    this->m_data = v10;
    if ( v10 == nullptr )
    {
      v20 = this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo(this);
      v11 = (const char *)this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName(this);
      this->m_data = (TSExplicitList<GLYPHBITMAPDATA,-572662307> *)SMemAlloc(12 * v4, v11, v20, 0);
      if ( counta != nullptr )
      {
        m_count = this->m_count;
        if ( v4 < m_count )
          m_count = v4;
        if ( m_count != 0 )
        {
          v13 = 0;
          v14 = counta;
          for ( roundb = m_count; roundb != 0; --roundb )
          {
            v15 = &this->m_data[v13];
            if ( v15 != nullptr )
            {
              v15->m_terminator.m_prevlink = nullptr;
              v15->m_terminator.m_next = nullptr;
              m_linkoffset = v14->m_linkoffset;
              v15->m_terminator.m_prevlink = &v15->m_terminator;
              v15->m_linkoffset = m_linkoffset;
              v15->m_terminator.m_next = (GLYPHBITMAPDATA *)~(unsigned int)&v15->m_terminator;
            }
            while ( (int)v14->m_terminator.m_next > 0 )
            {
              v17 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(v14, v14->m_terminator.m_next);
              TSLink<GLYPHBITMAPDATA>::Unlink(v17);
            }
            TSLink<GLYPHBITMAPDATA>::Unlink(&v14->m_terminator);
            ++v13;
            ++v14;
          }
        }
        v21 = this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo(this);
        v18 = (const char *)this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName(this);
        SMemFree(counta, v18, v21, 0);
      }
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CD970  EA 0x5CD970
// DIA: TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> >::UnlinkAll
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::UnlinkAll(
        TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA> > *this)
{
  GLYPHBITMAPDATA *m_next; // edx
  int v2; // edi
  _DWORD *v3; // eax
  int v4; // esi
  int v5; // esi

  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v2 = *(unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    v3 = (unsigned int *)((char *)&m_next->m_hashval + this->m_linkoffset);
    if ( v2 != 0 )
    {
      v4 = v3[1];
      if ( v4 > 0 )
        v5 = (int)v3 + v4 - *(_DWORD *)(v2 + 4);
      else
        v5 = ~v4;
      *(_DWORD *)v5 = v2;
      *(_DWORD *)(*v3 + 4) = v3[1];
      *v3 = 0;
      v3[1] = 0;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CD9C0  EA 0x5CD9C0
// DIA: TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >::Reserve
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::Reserve(
        TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *this,
        unsigned int count,
        int round)
{
  unsigned int v4; // edi
  unsigned int m_chunk; // ecx
  int i; // eax
  TSExplicitList<KERNNODE,-572662307> *v7; // ebx
  TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >_vtbl *v8; // edx
  const char *v9; // eax
  TSExplicitList<KERNNODE,-572662307> *v10; // eax
  const char *v11; // eax
  unsigned int m_count; // eax
  int v13; // ebx
  TSList<KERNNODE,TSGetExplicitLink<KERNNODE> > *v14; // edi
  TSExplicitList<KERNNODE,-572662307> *v15; // ecx
  int m_linkoffset; // edx
  TSLink<KERNNODE> *v17; // eax
  const char *v18; // eax
  int v19; // [esp-8h] [ebp-14h]
  int v20; // [esp-8h] [ebp-14h]
  int v21; // [esp-8h] [ebp-14h]
  TSExplicitList<KERNNODE,-572662307> *counta; // [esp+14h] [ebp+8h]
  unsigned int rounda; // [esp+18h] [ebp+Ch]
  int roundb; // [esp+18h] [ebp+Ch]

  v4 = this->m_count + count;
  if ( v4 > this->m_alloc )
  {
    if ( round != 0 )
    {
      m_chunk = this->m_chunk;
      if ( m_chunk == 0 )
      {
        m_chunk = v4;
        if ( v4 >= 0x15 )
        {
          this->m_chunk = 21;
          m_chunk = 21;
        }
        else
        {
          for ( i = v4 & (v4 - 1); i != 0; i &= i - 1 )
            m_chunk = i;
          if ( m_chunk == 0 )
            m_chunk = 1;
        }
      }
      if ( v4 % m_chunk != 0 )
        v4 += m_chunk - v4 % m_chunk;
    }
    counta = this->m_data;
    rounda = v4;
    if ( v4 < this->m_count )
    {
      v7 = &this->m_data[v4];
      do
      {
        TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::UnlinkAll(v7);
        TSLink<KERNNODE>::Unlink(&v7->m_terminator);
        ++v7;
        ++rounda;
      }
      while ( rounda < this->m_count );
    }
    v8 = this->__vftable;
    this->m_alloc = v4;
    v19 = v8->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName(this);
    v10 = (TSExplicitList<KERNNODE,-572662307> *)SMemReAlloc(counta, 12 * v4, v9, v19, 0x10u);
    this->m_data = v10;
    if ( v10 == nullptr )
    {
      v20 = this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo(this);
      v11 = (const char *)this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName(this);
      this->m_data = (TSExplicitList<KERNNODE,-572662307> *)SMemAlloc(12 * v4, v11, v20, 0);
      if ( counta != nullptr )
      {
        m_count = this->m_count;
        if ( v4 < m_count )
          m_count = v4;
        if ( m_count != 0 )
        {
          v13 = 0;
          v14 = counta;
          for ( roundb = m_count; roundb != 0; --roundb )
          {
            v15 = &this->m_data[v13];
            if ( v15 != nullptr )
            {
              v15->m_terminator.m_prevlink = nullptr;
              v15->m_terminator.m_next = nullptr;
              m_linkoffset = v14->m_linkoffset;
              v15->m_terminator.m_prevlink = &v15->m_terminator;
              v15->m_linkoffset = m_linkoffset;
              v15->m_terminator.m_next = (KERNNODE *)~(unsigned int)&v15->m_terminator;
            }
            while ( (int)v14->m_terminator.m_next > 0 )
            {
              v17 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(v14, v14->m_terminator.m_next);
              TSLink<KERNNODE>::Unlink(v17);
            }
            TSLink<KERNNODE>::Unlink(&v14->m_terminator);
            ++v13;
            ++v14;
          }
        }
        v21 = this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo(this);
        v18 = (const char *)this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName(this);
        SMemFree(counta, v18, v21, 0);
      }
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CDB40  EA 0x5CDB40
// DIA: TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307>>::ReallocData(
        TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> > *this,
        unsigned int count)
{
  unsigned int m_count; // ebx
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v4; // edi
  TSLink<GLYPHBITMAPDATA> *v5; // eax
  TSFixedArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >_vtbl *v6; // edx
  const char *v7; // eax
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v8; // eax
  const char *v9; // eax
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v10; // edi
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *v11; // ecx
  int m_linkoffset; // edx
  int m_next; // eax
  TSLink<GLYPHBITMAPDATA> *m_prevlink; // ebx
  TSLink<GLYPHBITMAPDATA> *p_m_terminator; // eax
  int v16; // edx
  int v17; // edx
  bool v18; // zf
  const char *v19; // eax
  int v20; // [esp-8h] [ebp-1Ch]
  int v21; // [esp-8h] [ebp-1Ch]
  int v22; // [esp-8h] [ebp-1Ch]
  unsigned int v23; // [esp+Ch] [ebp-8h]
  TSExplicitList<GLYPHBITMAPDATA,-572662307> *oldData; // [esp+10h] [ebp-4h]
  unsigned int index; // [esp+1Ch] [ebp+8h]

  m_count = count;
  oldData = this->m_data;
  if ( count < this->m_count )
  {
    v4 = &this->m_data[count];
    do
    {
      while ( (int)v4->m_terminator.m_next > 0 )
      {
        v5 = TSList<GLYPHBITMAPDATA,TSGetExplicitLink<GLYPHBITMAPDATA>>::Link(v4, v4->m_terminator.m_next);
        TSLink<GLYPHBITMAPDATA>::Unlink(v5);
      }
      TSLink<GLYPHBITMAPDATA>::Unlink(&v4->m_terminator);
      ++v4;
      ++count;
    }
    while ( count < this->m_count );
  }
  v6 = this->__vftable;
  this->m_alloc = m_count;
  v20 = v6->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo(this);
  v7 = (const char *)this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName(this);
  v8 = (TSExplicitList<GLYPHBITMAPDATA,-572662307> *)SMemReAlloc(oldData, 12 * m_count, v7, v20, 0x10u);
  this->m_data = v8;
  if ( v8 == nullptr )
  {
    v21 = this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName(this);
    v10 = oldData;
    this->m_data = (TSExplicitList<GLYPHBITMAPDATA,-572662307> *)SMemAlloc(12 * m_count, v9, v21, 0);
    if ( oldData != nullptr )
    {
      if ( m_count >= this->m_count )
        m_count = this->m_count;
      if ( m_count != 0 )
      {
        index = 0;
        v23 = m_count;
        do
        {
          v11 = &this->m_data[index];
          if ( v11 != nullptr )
          {
            v11->m_terminator.m_prevlink = nullptr;
            v11->m_terminator.m_next = nullptr;
            m_linkoffset = v10->m_linkoffset;
            v11->m_terminator.m_prevlink = &v11->m_terminator;
            v11->m_linkoffset = m_linkoffset;
            v11->m_terminator.m_next = (GLYPHBITMAPDATA *)~(unsigned int)&v11->m_terminator;
          }
          while ( 1 )
          {
            m_next = (int)v10->m_terminator.m_next;
            if ( m_next <= 0 )
              break;
            TSLink<GLYPHBITMAPDATA>::Unlink((TSLink<GLYPHBITMAPDATA> *)(m_next + v10->m_linkoffset));
          }
          m_prevlink = v10->m_terminator.m_prevlink;
          p_m_terminator = &v10->m_terminator;
          if ( m_prevlink != nullptr )
          {
            v16 = (int)v10->m_terminator.m_next;
            if ( v16 > 0 )
              v17 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v16;
            else
              v17 = ~v16;
            *(_DWORD *)v17 = m_prevlink;
            p_m_terminator->m_prevlink->m_next = v10->m_terminator.m_next;
            p_m_terminator->m_prevlink = nullptr;
            v10->m_terminator.m_next = nullptr;
          }
          ++v10;
          v18 = v23 == 1;
          ++index;
          --v23;
        }
        while ( !v18 );
      }
      v22 = this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemLineNo(this);
      v19 = (const char *)this->TSBaseArray<TSExplicitList<GLYPHBITMAPDATA,-572662307> >::MemFileName(this);
      SMemFree(oldData, v19, v22, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CDCB0  EA 0x5CDCB0
// DIA: NTempest::C44Matrix::C44Matrix
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
NTempest::C44Matrix *__thiscall NTempest::C44Matrix::C44Matrix(NTempest::C44Matrix *this)
{
  this->a0 = 1.0;
  this->a1 = 0.0;
  this->a2 = 0.0;
  this->a3 = 0.0;
  this->b0 = 0.0;
  this->b1 = 1.0;
  this->b2 = 0.0;
  this->b3 = 0.0;
  this->c0 = 0.0;
  this->c1 = 0.0;
  this->c2 = 1.0;
  this->c3 = 0.0;
  this->d0 = 0.0;
  this->d1 = 0.0;
  this->d2 = 0.0;
  this->d3 = 1.0;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CDCF0  EA 0x5CDCF0
// DIA: TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> >::CalcChunkSize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::CalcChunkSize(
        TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *this,
        unsigned int count)
{
  unsigned int result; // eax
  unsigned int i; // ecx

  result = count;
  if ( count >= 0x15 )
  {
    this->m_chunk = 21;
    return 21;
  }
  else
  {
    for ( i = count & (count - 1); i != 0; i &= i - 1 )
      result = i;
    if ( result == 0 )
      return 1;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001CDD30  EA 0x5CDD30
// DIA: TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> >::RoundToChunk
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307>>::RoundToChunk(
        TSGrowableArray<TSExplicitList<CHARCODEDESC,-572662307> > *this,
        unsigned int count,
        unsigned int chunk)
{
  unsigned int v3; // esi

  v3 = count;
  if ( count % chunk != 0 )
    return chunk - count % chunk + count;
  return v3;
}


// -----------------------------------------------------------------------------
// RVA 0x001CDD50  EA 0x5CDD50
// DIA: TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307> >::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307>>::ReallocData(
        TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307> > *this,
        unsigned int count)
{
  unsigned int m_count; // ebx
  TSExplicitList<CHARCODEDESC,-572662307> *v4; // edi
  TSLink<CHARCODEDESC> *v5; // eax
  TSFixedArray<TSExplicitList<CHARCODEDESC,-572662307> >_vtbl *v6; // edx
  const char *v7; // eax
  TSExplicitList<CHARCODEDESC,-572662307> *v8; // eax
  const char *v9; // eax
  TSExplicitList<CHARCODEDESC,-572662307> *v10; // edi
  TSExplicitList<CHARCODEDESC,-572662307> *v11; // ecx
  int m_linkoffset; // edx
  int m_next; // eax
  TSLink<CHARCODEDESC> *m_prevlink; // ebx
  TSLink<CHARCODEDESC> *p_m_terminator; // eax
  int v16; // edx
  int v17; // edx
  bool v18; // zf
  const char *v19; // eax
  int v20; // [esp-8h] [ebp-1Ch]
  int v21; // [esp-8h] [ebp-1Ch]
  int v22; // [esp-8h] [ebp-1Ch]
  unsigned int v23; // [esp+Ch] [ebp-8h]
  TSExplicitList<CHARCODEDESC,-572662307> *oldData; // [esp+10h] [ebp-4h]
  unsigned int index; // [esp+1Ch] [ebp+8h]

  m_count = count;
  oldData = this->m_data;
  if ( count < this->m_count )
  {
    v4 = &this->m_data[count];
    do
    {
      while ( (int)v4->m_terminator.m_next > 0 )
      {
        v5 = TSList<CHARCODEDESC,TSGetExplicitLink<CHARCODEDESC>>::Link(v4, v4->m_terminator.m_next);
        TSLink<CHARCODEDESC>::Unlink(v5);
      }
      TSLink<CHARCODEDESC>::Unlink(&v4->m_terminator);
      ++v4;
      ++count;
    }
    while ( count < this->m_count );
  }
  v6 = this->__vftable;
  this->m_alloc = m_count;
  v20 = v6->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo(this);
  v7 = (const char *)this->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName(this);
  v8 = (TSExplicitList<CHARCODEDESC,-572662307> *)SMemReAlloc(oldData, 12 * m_count, v7, v20, 0x10u);
  this->m_data = v8;
  if ( v8 == nullptr )
  {
    v21 = this->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName(this);
    v10 = oldData;
    this->m_data = (TSExplicitList<CHARCODEDESC,-572662307> *)SMemAlloc(12 * m_count, v9, v21, 0);
    if ( oldData != nullptr )
    {
      if ( m_count >= this->m_count )
        m_count = this->m_count;
      if ( m_count != 0 )
      {
        index = 0;
        v23 = m_count;
        do
        {
          v11 = &this->m_data[index];
          if ( v11 != nullptr )
          {
            v11->m_terminator.m_prevlink = nullptr;
            v11->m_terminator.m_next = nullptr;
            m_linkoffset = v10->m_linkoffset;
            v11->m_terminator.m_prevlink = &v11->m_terminator;
            v11->m_linkoffset = m_linkoffset;
            v11->m_terminator.m_next = (CHARCODEDESC *)~(unsigned int)&v11->m_terminator;
          }
          while ( 1 )
          {
            m_next = (int)v10->m_terminator.m_next;
            if ( m_next <= 0 )
              break;
            TSLink<CHARCODEDESC>::Unlink((TSLink<CHARCODEDESC> *)(m_next + v10->m_linkoffset));
          }
          m_prevlink = v10->m_terminator.m_prevlink;
          p_m_terminator = &v10->m_terminator;
          if ( m_prevlink != nullptr )
          {
            v16 = (int)v10->m_terminator.m_next;
            if ( v16 > 0 )
              v17 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v16;
            else
              v17 = ~v16;
            *(_DWORD *)v17 = m_prevlink;
            p_m_terminator->m_prevlink->m_next = v10->m_terminator.m_next;
            p_m_terminator->m_prevlink = nullptr;
            v10->m_terminator.m_next = nullptr;
          }
          ++v10;
          v18 = v23 == 1;
          ++index;
          --v23;
        }
        while ( !v18 );
      }
      v22 = this->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemLineNo(this);
      v19 = (const char *)this->TSBaseArray<TSExplicitList<CHARCODEDESC,-572662307> >::MemFileName(this);
      SMemFree(oldData, v19, v22, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CDEC0  EA 0x5CDEC0
// DIA: TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >::CalcChunkSize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::CalcChunkSize(
        TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *this,
        unsigned int count)
{
  unsigned int result; // eax
  unsigned int i; // ecx

  result = count;
  if ( count >= 0x15 )
  {
    this->m_chunk = 21;
    return 21;
  }
  else
  {
    for ( i = count & (count - 1); i != 0; i &= i - 1 )
      result = i;
    if ( result == 0 )
      return 1;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001CDF00  EA 0x5CDF00
// DIA: TSGrowableArray<TSExplicitList<KERNNODE,-572662307> >::RoundToChunk
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<KERNNODE,-572662307>>::RoundToChunk(
        TSGrowableArray<TSExplicitList<KERNNODE,-572662307> > *this,
        unsigned int count,
        unsigned int chunk)
{
  unsigned int v3; // esi

  v3 = count;
  if ( count % chunk != 0 )
    return chunk - count % chunk + count;
  return v3;
}


// -----------------------------------------------------------------------------
// RVA 0x001CDF20  EA 0x5CDF20
// DIA: TSFixedArray<TSExplicitList<KERNNODE,-572662307> >::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<KERNNODE,-572662307>>::ReallocData(
        TSFixedArray<TSExplicitList<KERNNODE,-572662307> > *this,
        unsigned int count)
{
  unsigned int m_count; // ebx
  TSExplicitList<KERNNODE,-572662307> *v4; // edi
  TSLink<KERNNODE> *v5; // eax
  TSFixedArray<TSExplicitList<KERNNODE,-572662307> >_vtbl *v6; // edx
  const char *v7; // eax
  TSExplicitList<KERNNODE,-572662307> *v8; // eax
  const char *v9; // eax
  TSExplicitList<KERNNODE,-572662307> *v10; // edi
  TSExplicitList<KERNNODE,-572662307> *v11; // ecx
  int m_linkoffset; // edx
  int m_next; // eax
  TSLink<KERNNODE> *m_prevlink; // ebx
  TSLink<KERNNODE> *p_m_terminator; // eax
  int v16; // edx
  int v17; // edx
  bool v18; // zf
  const char *v19; // eax
  int v20; // [esp-8h] [ebp-1Ch]
  int v21; // [esp-8h] [ebp-1Ch]
  int v22; // [esp-8h] [ebp-1Ch]
  unsigned int v23; // [esp+Ch] [ebp-8h]
  TSExplicitList<KERNNODE,-572662307> *oldData; // [esp+10h] [ebp-4h]
  unsigned int index; // [esp+1Ch] [ebp+8h]

  m_count = count;
  oldData = this->m_data;
  if ( count < this->m_count )
  {
    v4 = &this->m_data[count];
    do
    {
      while ( (int)v4->m_terminator.m_next > 0 )
      {
        v5 = TSList<KERNNODE,TSGetExplicitLink<KERNNODE>>::Link(v4, v4->m_terminator.m_next);
        TSLink<KERNNODE>::Unlink(v5);
      }
      TSLink<KERNNODE>::Unlink(&v4->m_terminator);
      ++v4;
      ++count;
    }
    while ( count < this->m_count );
  }
  v6 = this->__vftable;
  this->m_alloc = m_count;
  v20 = v6->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo(this);
  v7 = (const char *)this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName(this);
  v8 = (TSExplicitList<KERNNODE,-572662307> *)SMemReAlloc(oldData, 12 * m_count, v7, v20, 0x10u);
  this->m_data = v8;
  if ( v8 == nullptr )
  {
    v21 = this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName(this);
    v10 = oldData;
    this->m_data = (TSExplicitList<KERNNODE,-572662307> *)SMemAlloc(12 * m_count, v9, v21, 0);
    if ( oldData != nullptr )
    {
      if ( m_count >= this->m_count )
        m_count = this->m_count;
      if ( m_count != 0 )
      {
        index = 0;
        v23 = m_count;
        do
        {
          v11 = &this->m_data[index];
          if ( v11 != nullptr )
          {
            v11->m_terminator.m_prevlink = nullptr;
            v11->m_terminator.m_next = nullptr;
            m_linkoffset = v10->m_linkoffset;
            v11->m_terminator.m_prevlink = &v11->m_terminator;
            v11->m_linkoffset = m_linkoffset;
            v11->m_terminator.m_next = (KERNNODE *)~(unsigned int)&v11->m_terminator;
          }
          while ( 1 )
          {
            m_next = (int)v10->m_terminator.m_next;
            if ( m_next <= 0 )
              break;
            TSLink<KERNNODE>::Unlink((TSLink<KERNNODE> *)(m_next + v10->m_linkoffset));
          }
          m_prevlink = v10->m_terminator.m_prevlink;
          p_m_terminator = &v10->m_terminator;
          if ( m_prevlink != nullptr )
          {
            v16 = (int)v10->m_terminator.m_next;
            if ( v16 > 0 )
              v17 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v16;
            else
              v17 = ~v16;
            *(_DWORD *)v17 = m_prevlink;
            p_m_terminator->m_prevlink->m_next = v10->m_terminator.m_next;
            p_m_terminator->m_prevlink = nullptr;
            v10->m_terminator.m_next = nullptr;
          }
          ++v10;
          v18 = v23 == 1;
          ++index;
          --v23;
        }
        while ( !v18 );
      }
      v22 = this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemLineNo(this);
      v19 = (const char *)this->TSBaseArray<TSExplicitList<KERNNODE,-572662307> >::MemFileName(this);
      SMemFree(oldData, v19, v22, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CE090  EA 0x5CE090
// DIA: TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::CalcChunkSize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::CalcChunkSize(
        TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this,
        unsigned int count)
{
  unsigned int result; // eax
  unsigned int i; // ecx

  result = count;
  if ( count >= 0x15 )
  {
    this->m_chunk = 21;
    return 21;
  }
  else
  {
    for ( i = count & (count - 1); i != 0; i &= i - 1 )
      result = i;
    if ( result == 0 )
      return 1;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001CE0D0  EA 0x5CE0D0
// DIA: TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::RoundToChunk
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::RoundToChunk(
        TSGrowableArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this,
        unsigned int count,
        unsigned int chunk)
{
  unsigned int v3; // esi

  v3 = count;
  if ( count % chunk != 0 )
    return chunk - count % chunk + count;
  return v3;
}


// -----------------------------------------------------------------------------
// RVA 0x001CE0F0  EA 0x5CE0F0
// DIA: TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307>>::ReallocData(
        TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> > *this,
        unsigned int count)
{
  unsigned int m_count; // ebx
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v4; // edi
  TSLink<BATCHEDRENDERFONTDESC> *v5; // eax
  TSFixedArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >_vtbl *v6; // edx
  const char *v7; // eax
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v8; // eax
  const char *v9; // eax
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v10; // edi
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *v11; // ecx
  int m_linkoffset; // edx
  int m_next; // eax
  TSLink<BATCHEDRENDERFONTDESC> *m_prevlink; // ebx
  TSLink<BATCHEDRENDERFONTDESC> *p_m_terminator; // eax
  int v16; // edx
  int v17; // edx
  bool v18; // zf
  const char *v19; // eax
  int v20; // [esp-8h] [ebp-1Ch]
  int v21; // [esp-8h] [ebp-1Ch]
  int v22; // [esp-8h] [ebp-1Ch]
  unsigned int v23; // [esp+Ch] [ebp-8h]
  TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *oldData; // [esp+10h] [ebp-4h]
  unsigned int index; // [esp+1Ch] [ebp+8h]

  m_count = count;
  oldData = this->m_data;
  if ( count < this->m_count )
  {
    v4 = &this->m_data[count];
    do
    {
      while ( (int)v4->m_terminator.m_next > 0 )
      {
        v5 = TSList<BATCHEDRENDERFONTDESC,TSGetExplicitLink<BATCHEDRENDERFONTDESC>>::Link(v4, v4->m_terminator.m_next);
        TSLink<BATCHEDRENDERFONTDESC>::Unlink(v5);
      }
      TSLink<BATCHEDRENDERFONTDESC>::Unlink(&v4->m_terminator);
      ++v4;
      ++count;
    }
    while ( count < this->m_count );
  }
  v6 = this->__vftable;
  this->m_alloc = m_count;
  v20 = v6->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo(this);
  v7 = (const char *)this->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName(this);
  v8 = (TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *)SMemReAlloc(oldData, 12 * m_count, v7, v20, 0x10u);
  this->m_data = v8;
  if ( v8 == nullptr )
  {
    v21 = this->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName(this);
    v10 = oldData;
    this->m_data = (TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> *)SMemAlloc(12 * m_count, v9, v21, 0);
    if ( oldData != nullptr )
    {
      if ( m_count >= this->m_count )
        m_count = this->m_count;
      if ( m_count != 0 )
      {
        index = 0;
        v23 = m_count;
        do
        {
          v11 = &this->m_data[index];
          if ( v11 != nullptr )
          {
            v11->m_terminator.m_prevlink = nullptr;
            v11->m_terminator.m_next = nullptr;
            m_linkoffset = v10->m_linkoffset;
            v11->m_terminator.m_prevlink = &v11->m_terminator;
            v11->m_linkoffset = m_linkoffset;
            v11->m_terminator.m_next = (BATCHEDRENDERFONTDESC *)~(unsigned int)&v11->m_terminator;
          }
          while ( 1 )
          {
            m_next = (int)v10->m_terminator.m_next;
            if ( m_next <= 0 )
              break;
            TSLink<BATCHEDRENDERFONTDESC>::Unlink((TSLink<BATCHEDRENDERFONTDESC> *)(m_next + v10->m_linkoffset));
          }
          m_prevlink = v10->m_terminator.m_prevlink;
          p_m_terminator = &v10->m_terminator;
          if ( m_prevlink != nullptr )
          {
            v16 = (int)v10->m_terminator.m_next;
            if ( v16 > 0 )
              v17 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v16;
            else
              v17 = ~v16;
            *(_DWORD *)v17 = m_prevlink;
            p_m_terminator->m_prevlink->m_next = v10->m_terminator.m_next;
            p_m_terminator->m_prevlink = nullptr;
            v10->m_terminator.m_next = nullptr;
          }
          ++v10;
          v18 = v23 == 1;
          ++index;
          --v23;
        }
        while ( !v18 );
      }
      v22 = this->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemLineNo(this);
      v19 = (const char *)this->TSBaseArray<TSExplicitList<BATCHEDRENDERFONTDESC,-572662307> >::MemFileName(this);
      SMemFree(oldData, v19, v22, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CE260  EA 0x5CE260
// DIA: TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::CalcChunkSize
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::CalcChunkSize(
        TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *this,
        unsigned int count)
{
  unsigned int result; // eax
  unsigned int i; // ecx

  result = count;
  if ( count >= 0x15 )
  {
    this->m_chunk = 21;
    return 21;
  }
  else
  {
    for ( i = count & (count - 1); i != 0; i &= i - 1 )
      result = i;
    if ( result == 0 )
      return 1;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001CE2A0  EA 0x5CE2A0
// DIA: TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::RoundToChunk
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::RoundToChunk(
        TSGrowableArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *this,
        unsigned int count,
        unsigned int chunk)
{
  unsigned int v3; // esi

  v3 = count;
  if ( count % chunk != 0 )
    return chunk - count % chunk + count;
  return v3;
}


// -----------------------------------------------------------------------------
// RVA 0x001CE2C0  EA 0x5CE2C0
// DIA: TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontUtil.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307>>::ReallocData(
        TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> > *this,
        unsigned int count)
{
  unsigned int m_count; // ebx
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v4; // edi
  TSLink<STRINGVIEWMATRICES> *v5; // eax
  TSFixedArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >_vtbl *v6; // edx
  const char *v7; // eax
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v8; // eax
  const char *v9; // eax
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v10; // edi
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *v11; // ecx
  int m_linkoffset; // edx
  int m_next; // eax
  TSLink<STRINGVIEWMATRICES> *m_prevlink; // ebx
  TSLink<STRINGVIEWMATRICES> *p_m_terminator; // eax
  int v16; // edx
  int v17; // edx
  bool v18; // zf
  const char *v19; // eax
  int v20; // [esp-8h] [ebp-1Ch]
  int v21; // [esp-8h] [ebp-1Ch]
  int v22; // [esp-8h] [ebp-1Ch]
  unsigned int v23; // [esp+Ch] [ebp-8h]
  TSExplicitList<STRINGVIEWMATRICES,-572662307> *oldData; // [esp+10h] [ebp-4h]
  unsigned int index; // [esp+1Ch] [ebp+8h]

  m_count = count;
  oldData = this->m_data;
  if ( count < this->m_count )
  {
    v4 = &this->m_data[count];
    do
    {
      while ( (int)v4->m_terminator.m_next > 0 )
      {
        v5 = TSList<STRINGVIEWMATRICES,TSGetExplicitLink<STRINGVIEWMATRICES>>::Link(v4, v4->m_terminator.m_next);
        TSLink<STRINGVIEWMATRICES>::Unlink(v5);
      }
      TSLink<STRINGVIEWMATRICES>::Unlink(&v4->m_terminator);
      ++v4;
      ++count;
    }
    while ( count < this->m_count );
  }
  v6 = this->__vftable;
  this->m_alloc = m_count;
  v20 = v6->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo(this);
  v7 = (const char *)this->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName(this);
  v8 = (TSExplicitList<STRINGVIEWMATRICES,-572662307> *)SMemReAlloc(oldData, 12 * m_count, v7, v20, 0x10u);
  this->m_data = v8;
  if ( v8 == nullptr )
  {
    v21 = this->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName(this);
    v10 = oldData;
    this->m_data = (TSExplicitList<STRINGVIEWMATRICES,-572662307> *)SMemAlloc(12 * m_count, v9, v21, 0);
    if ( oldData != nullptr )
    {
      if ( m_count >= this->m_count )
        m_count = this->m_count;
      if ( m_count != 0 )
      {
        index = 0;
        v23 = m_count;
        do
        {
          v11 = &this->m_data[index];
          if ( v11 != nullptr )
          {
            v11->m_terminator.m_prevlink = nullptr;
            v11->m_terminator.m_next = nullptr;
            m_linkoffset = v10->m_linkoffset;
            v11->m_terminator.m_prevlink = &v11->m_terminator;
            v11->m_linkoffset = m_linkoffset;
            v11->m_terminator.m_next = (STRINGVIEWMATRICES *)~(unsigned int)&v11->m_terminator;
          }
          while ( 1 )
          {
            m_next = (int)v10->m_terminator.m_next;
            if ( m_next <= 0 )
              break;
            TSLink<STRINGVIEWMATRICES>::Unlink((TSLink<STRINGVIEWMATRICES> *)(m_next + v10->m_linkoffset));
          }
          m_prevlink = v10->m_terminator.m_prevlink;
          p_m_terminator = &v10->m_terminator;
          if ( m_prevlink != nullptr )
          {
            v16 = (int)v10->m_terminator.m_next;
            if ( v16 > 0 )
              v17 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v16;
            else
              v17 = ~v16;
            *(_DWORD *)v17 = m_prevlink;
            p_m_terminator->m_prevlink->m_next = v10->m_terminator.m_next;
            p_m_terminator->m_prevlink = nullptr;
            v10->m_terminator.m_next = nullptr;
          }
          ++v10;
          v18 = v23 == 1;
          ++index;
          --v23;
        }
        while ( !v18 );
      }
      v22 = this->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemLineNo(this);
      v19 = (const char *)this->TSBaseArray<TSExplicitList<STRINGVIEWMATRICES,-572662307> >::MemFileName(this);
      SMemFree(oldData, v19, v22, 0);
    }
  }
}

