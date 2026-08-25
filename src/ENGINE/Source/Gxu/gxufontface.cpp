// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\GxuFontFace.cpp
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x001CE430  EA 0x5CE430
// DIA: $E2
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E2_268()
{
  return _E1_268();
}


// -----------------------------------------------------------------------------
// RVA 0x001CE440  EA 0x5CE440
// DIA: $E1
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int _E1_268()
{
  INFINITY_268 = INFINITY;
  return 2139095040;
}


// -----------------------------------------------------------------------------
// RVA 0x001CE450  EA 0x5CE450
// DIA: $E26
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E26_42()
{
  _E23_47();
  return _E25_33();
}


// -----------------------------------------------------------------------------
// RVA 0x001CE460  EA 0x5CE460
// DIA: $E23
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void _E23_47()
{
  int m_next; // ecx
  int v1; // edx
  _DWORD *v2; // eax
  int v3; // esi
  int v4; // esi

  s_faceHash.m_fulllist.m_terminator.m_prevlink = &s_faceHash.m_fulllist.m_terminator;
  m_next = ~(unsigned int)&s_faceHash.m_fulllist.m_terminator;
  s_faceHash.m_fulllist.m_linkoffset = -572662307;
  s_faceHash.m_fulllist.m_terminator.m_next = (FACEDATA *)~(unsigned int)&s_faceHash.m_fulllist.m_terminator;
  s_faceHash.m_slotlistarray.m_alloc = 0;
  s_faceHash.m_slotlistarray.m_count = 0;
  s_faceHash.m_slotlistarray.m_data = nullptr;
  s_faceHash.m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<FACEDATA,-572662307> >_vtbl *)&TSGrowableArray<TSExplicitList<FACEDATA,-572662307>>::`vftable';
  s_faceHash.m_slotlistarray.m_chunk = 0;
  s_faceHash.__vftable = (TSHashTable<FACEDATA,HASHKEY_STRI>_vtbl *)&TSHashTable<FACEDATA,HASHKEY_STRI>::`vftable';
  s_faceHash.m_fullnessIndicator = 0;
  while ( m_next > 0 )
  {
    v1 = *(_DWORD *)(s_faceHash.m_fulllist.m_linkoffset + m_next);
    v2 = (_DWORD *)(m_next + s_faceHash.m_fulllist.m_linkoffset);
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
      m_next = (int)s_faceHash.m_fulllist.m_terminator.m_next;
    }
  }
  s_faceHash.m_fulllist.m_terminator.m_next = (FACEDATA *)~(unsigned int)&s_faceHash.m_fulllist.m_terminator;
  s_faceHash.m_fulllist.m_linkoffset = 20;
  s_faceHash.m_fulllist.m_terminator.m_prevlink = &s_faceHash.m_fulllist.m_terminator;
  s_faceHash.m_slotmask = -1;
}


// -----------------------------------------------------------------------------
// RVA 0x001CE520  EA 0x5CE520
// DIA: $E25
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E25_33()
{
  return atexit(_E24_27);
}


// -----------------------------------------------------------------------------
// RVA 0x001CE530  EA 0x5CE530
// DIA: $E24
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E24_27()
{
  unsigned int v0; // edi
  int m_next; // eax
  TSLink<FACEDATA> *v2; // eax
  unsigned int m_count; // ecx
  FACEDATA *v4; // ebx
  int v5; // eax
  const char *v6; // eax
  int m_linkoffset; // esi
  int v8; // eax
  TSLink<FACEDATA> *v9; // esi
  unsigned int v10; // esi
  int v11; // edi
  char *v12; // esi
  int v13; // [esp-1Ch] [ebp-28h]
  int v14; // [esp-18h] [ebp-24h]
  unsigned int v15; // [esp-14h] [ebp-20h]
  const char *v16; // [esp-10h] [ebp-1Ch]
  int v17; // [esp+8h] [ebp-4h]

  v0 = 0;
  s_faceHash.__vftable = (TSHashTable<FACEDATA,HASHKEY_STRI>_vtbl *)&TSHashTable<FACEDATA,HASHKEY_STRI>::`vftable';
  s_faceHash.m_fullnessIndicator = 0;
  while ( 1 )
  {
    m_next = (int)s_faceHash.m_fulllist.m_terminator.m_next;
    if ( (int)s_faceHash.m_fulllist.m_terminator.m_next <= 0 )
      break;
    v2 = TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::Link(
           &s_faceHash.m_fulllist,
           s_faceHash.m_fulllist.m_terminator.m_next);
    TSLink<FACEDATA>::Unlink(v2);
  }
  m_count = s_faceHash.m_slotlistarray.m_count;
  if ( s_faceHash.m_slotlistarray.m_count != 0 )
  {
    v17 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(&s_faceHash.m_slotlistarray, v0);
        v4 = s_faceHash.m_slotlistarray.m_data[v17].m_terminator.m_next;
        if ( (int)v4 <= 0 )
          break;
        if ( v0 >= s_faceHash.m_slotlistarray.m_count )
        {
          v5 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))s_faceHash.m_slotlistarray.TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo)(
                 &s_faceHash.m_slotlistarray,
                 1,
                 1,
                 aIndex0x08xArra,
                 v0,
                 s_faceHash.m_slotlistarray.m_count);
          v6 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *, int))s_faceHash.m_slotlistarray.TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName)(
                               &s_faceHash.m_slotlistarray,
                               v5);
          SErrDisplayErrorFmt(0x85100080, v6, v13, v14, v15, v16);
        }
        m_linkoffset = s_faceHash.m_slotlistarray.m_data[v17].m_linkoffset;
        v8 = *(int *)((char *)&v4->__vftable + m_linkoffset);
        v9 = (TSLink<FACEDATA> *)((char *)v4 + m_linkoffset);
        if ( v8 != 0 )
        {
          TSLink<FACEDATA>::NextLink(v9, -1)->m_prevlink = v9->m_prevlink;
          v9->m_prevlink->m_next = v9->m_next;
          v9->m_prevlink = nullptr;
          v9->m_next = nullptr;
        }
      }
      m_count = s_faceHash.m_slotlistarray.m_count;
      ++v0;
      ++v17;
    }
    while ( v0 < s_faceHash.m_slotlistarray.m_count );
    m_next = (int)s_faceHash.m_fulllist.m_terminator.m_next;
  }
  v10 = 0;
  s_faceHash.m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<FACEDATA,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<FACEDATA,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v11 = 0;
    do
    {
      TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::~TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>(&s_faceHash.m_slotlistarray.m_data[v11]);
      ++v10;
      ++v11;
    }
    while ( v10 < s_faceHash.m_slotlistarray.m_count );
    m_next = (int)s_faceHash.m_fulllist.m_terminator.m_next;
  }
  if ( s_faceHash.m_slotlistarray.m_data != nullptr )
  {
    SMemFree(s_faceHash.m_slotlistarray.m_data, aAvTsexplicitli_46, -2, 0);
LABEL_19:
    m_next = (int)s_faceHash.m_fulllist.m_terminator.m_next;
  }
  while ( m_next > 0 )
  {
    v12 = (char *)(s_faceHash.m_fulllist.m_linkoffset + m_next);
    if ( *(_DWORD *)(s_faceHash.m_fulllist.m_linkoffset + m_next) != 0 )
    {
      TSLink<FACEDATA>::NextLink((TSLink<FACEDATA> *)(s_faceHash.m_fulllist.m_linkoffset + m_next), -1)->m_prevlink = *(TSLink<FACEDATA> **)v12;
      *(_DWORD *)(*(_DWORD *)v12 + 4) = *((_DWORD *)v12 + 1);
      *(_DWORD *)v12 = 0;
      *((_DWORD *)v12 + 1) = 0;
      goto LABEL_19;
    }
  }
  if ( s_faceHash.m_fulllist.m_terminator.m_prevlink != nullptr )
  {
    *(_DWORD *)~m_next = s_faceHash.m_fulllist.m_terminator.m_prevlink;
    s_faceHash.m_fulllist.m_terminator.m_prevlink->m_next = s_faceHash.m_fulllist.m_terminator.m_next;
    s_faceHash.m_fulllist.m_terminator.m_prevlink = nullptr;
    s_faceHash.m_fulllist.m_terminator.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CE700  EA 0x5CE700
// DIA: FontFaceGetHandle
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CHandleObject *__fastcall FontFaceGetHandle(FT_LibraryRec_ *library, FT_Open_Args_ *a2)
{
  char *v3; // ebx
  unsigned int v4; // ebx
  unsigned int v5; // esi
  FACEDATA *v6; // edi
  unsigned int v7; // esi
  CHandleObject *v8; // esi
  FACEDATA *v9; // eax
  FACEDATA *v10; // esi
  unsigned int v12; // ebx
  unsigned int v13; // edi
  char *m_str; // eax
  char *v15; // ebx
  HOBJECT__ *v16; // eax
  unsigned int size; // [esp+10h] [ebp-10h] BYREF
  FT_FaceRec_ *theFace; // [esp+14h] [ebp-Ch] BYREF
  char *string2; // [esp+18h] [ebp-8h]
  void *data; // [esp+1Ch] [ebp-4h] BYREF

  v3 = (char *)library;
  string2 = (char *)library;
  if ( a2 == nullptr || library == nullptr || LOBYTE(library->memory) == 0 )
    return nullptr;
  if ( s_faceHash.m_slotmask == -1 )
  {
LABEL_10:
    data = nullptr;
    v8 = nullptr;
    if ( SFile::Load(nullptr, v3, &data, &size, 0, 3u, nullptr) != 0 )
    {
      if ( data == nullptr )
        return v8;
      if ( FT_New_Memory_Face(a2, (int)data, size, nullptr, &theFace) == 0
        && theFace != nullptr
        && FT_Select_Charmap(theFace, 1970170211) == 0 )
      {
        v9 = (FACEDATA *)SMemAlloc(0x2Cu, aHface, -2, 0);
        if ( v9 != nullptr )
        {
          v9->m_linktoslot.m_prevlink = nullptr;
          v9->m_linktoslot.m_next = nullptr;
          v9->m_linktofull.m_prevlink = nullptr;
          v9->m_linktofull.m_next = nullptr;
          v9->m_key.m_str = nullptr;
          v9->m_refcount = 0;
          v9->data = nullptr;
          v9->face = nullptr;
          v9->__vftable = (FACEDATA_vtbl *)&FACEDATA::`vftable';
          v10 = v9;
        }
        else
        {
          v10 = nullptr;
          SErrDisplayError(0x85100000, aDBuildBuildwow_238, 82, aFacedata, 0, 1u);
        }
        v10->data = data;
        v10->face = theFace;
        v12 = SStrHashHT(v3);
        if ( s_faceHash.m_slotmask == -1 )
          TSHashTable<FACEDATA,HASHKEY_STRI>::Initialize(&s_faceHash);
        v13 = s_faceHash.m_slotmask & v12;
        if ( TSHashTable<FACEDATA,HASHKEY_STRI>::MonitorFullness(&s_faceHash, s_faceHash.m_slotmask & v12) != 0 )
          v13 = s_faceHash.m_slotmask & v12;
        TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(&s_faceHash.m_slotlistarray, v13);
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::LinkNode(
          &s_faceHash.m_slotlistarray.m_data[v13],
          v10,
          2u,
          nullptr);
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::LinkNode(&s_faceHash.m_fulllist, v10, 2u, nullptr);
        v10->m_hashval = v12;
        m_str = v10->m_key.m_str;
        v15 = string2;
        if ( m_str != string2 )
        {
          if ( m_str != nullptr )
            SMemFree(m_str, aStormHStplH, 2650, 0);
          v10->m_key.m_str = SStrDupA(v15, aStormHStplH, 0xA5Bu);
        }
        data = nullptr;
        theFace = nullptr;
        v16 = HandleCreate((HOBJECT__ *)v10);
        v10->selfReference = (HFACE__ *)v16;
        v8 = HandleDuplicate((int)v16);
      }
    }
    if ( data != nullptr )
      SFile::Unload(data);
    return v8;
  }
  v4 = SStrHashHT((const char *)library);
  v5 = v4 & s_faceHash.m_slotmask;
  TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(
    &s_faceHash.m_slotlistarray,
    v4 & s_faceHash.m_slotmask);
  v6 = (int)s_faceHash.m_slotlistarray.m_data[v5].m_terminator.m_next <= 0
     ? nullptr
     : s_faceHash.m_slotlistarray.m_data[v5].m_terminator.m_next;
  if ( (int)v6 <= 0 )
  {
LABEL_9:
    v3 = string2;
    goto LABEL_10;
  }
  while ( v6->m_hashval != v4 || SStrCmpI(v6->m_key.m_str, string2, 0x7FFFFFFFu) != 0 )
  {
    v7 = v4 & s_faceHash.m_slotmask;
    TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(
      &s_faceHash.m_slotlistarray,
      v4 & s_faceHash.m_slotmask);
    v6 = *(FACEDATA **)((char *)&v6->m_refcount + s_faceHash.m_slotlistarray.m_data[v7].m_linkoffset);
    if ( (int)v6 <= 0 )
      goto LABEL_9;
  }
  if ( v6->selfReference == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_238, 51, aFacedataSelfre, 0, 1u);
  return HandleDuplicate((int)v6->selfReference);
}


// -----------------------------------------------------------------------------
// RVA 0x001CE990  EA 0x5CE990
// DIA: FACEDATA::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
FACEDATA *__thiscall FACEDATA::`scalar deleting destructor'(FACEDATA *this, char __flags)
{
  HFACE__ *selfReference; // eax
  FT_FaceRec_ *face; // ecx
  char *m_str; // eax
  TSLink<FACEDATA> *p_m_linktofull; // eax
  TSLink<FACEDATA> *m_prevlink; // edi
  int m_next; // edx
  int v9; // edx
  TSLink<FACEDATA> *v10; // edi
  TSLink<FACEDATA> *p_m_linktoslot; // eax
  int v12; // edx
  int v13; // edx

  selfReference = this->selfReference;
  this->__vftable = (FACEDATA_vtbl *)&FACEDATA::`vftable';
  if ( selfReference != nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_238, 21, aSelfreference, 0, 1u);
  face = this->face;
  if ( face != nullptr )
    FT_Done_Face((int)face);
  if ( this->data != nullptr )
    SFile::Unload(this->data);
  this->__vftable = (FACEDATA_vtbl *)&CHandleObject::`vftable';
  m_str = this->m_key.m_str;
  if ( m_str != nullptr )
    SMemFree(m_str, aStormHStplH, 2635, 0);
  p_m_linktofull = &this->m_linktofull;
  m_prevlink = this->m_linktofull.m_prevlink;
  if ( m_prevlink != nullptr )
  {
    m_next = (int)this->m_linktofull.m_next;
    if ( m_next > 0 )
      v9 = (char *)p_m_linktofull - (char *)m_prevlink->m_next + m_next;
    else
      v9 = ~m_next;
    *(_DWORD *)v9 = m_prevlink;
    p_m_linktofull->m_prevlink->m_next = this->m_linktofull.m_next;
    p_m_linktofull->m_prevlink = nullptr;
    this->m_linktofull.m_next = nullptr;
  }
  v10 = this->m_linktoslot.m_prevlink;
  p_m_linktoslot = &this->m_linktoslot;
  if ( v10 != nullptr )
  {
    v12 = (int)this->m_linktoslot.m_next;
    if ( v12 > 0 )
      v13 = (char *)p_m_linktoslot - (char *)v10->m_next + v12;
    else
      v13 = ~v12;
    *(_DWORD *)v13 = v10;
    p_m_linktoslot->m_prevlink->m_next = this->m_linktoslot.m_next;
    p_m_linktoslot->m_prevlink = nullptr;
    this->m_linktoslot.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 )
    SMemFree(this, filename, -1, 0);
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CEA80  EA 0x5CEA80
// DIA: FontFaceGetFace
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
FT_FaceRec_ *__thiscall FontFaceGetFace(int this)
{
  if ( this == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_238, 104);
    SErrDisplayAppFatal(aHandle);
  }
  if ( *(_DWORD *)(this + 40) == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_238, 107, aDataptrSelfref, 0, 1u);
  return *(FT_FaceRec_ **)(this + 36);
}


// -----------------------------------------------------------------------------
// RVA 0x001CEAE0  EA 0x5CEAE0
// DIA: FontFaceCloseHandle
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __usercall FontFaceCloseHandle(HFACE__ *handle@<eax>, int a2@<ecx>)
{
  unsigned int v3; // edi
  HOBJECT__ *v4; // eax

  if ( a2 == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_238, 113);
    SErrDisplayAppFatal(aHandle);
  }
  HandleClose(handle);
  v3 = *(_DWORD *)(a2 + 4);
  if ( v3 == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_238, 120, aRefcount1, 0, 1u);
  v4 = *(HOBJECT__ **)(a2 + 40);
  if ( v4 == nullptr )
    v4 = (HOBJECT__ *)SErrDisplayError(0x85100000, aDBuildBuildwow_238, 121, aDataptrSelfref, 0, 1u);
  if ( v3 <= 1 )
  {
    *(_DWORD *)(a2 + 40) = 0;
    HandleClose(v4);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CEB70  EA 0x5CEB70
// DIA: FontFaceGetFontName
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
const char *__thiscall FontFaceGetFontName(int this)
{
  if ( this == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_238, 136);
    SErrDisplayAppFatal(aHandle);
  }
  return *(const char **)(this + 28);
}


// -----------------------------------------------------------------------------
// RVA 0x001CEBA0  EA 0x5CEBA0
// DIA: TSHashTable<FACEDATA,HASHKEY_STRI>::InternalDelete
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<FACEDATA,HASHKEY_STRI>::InternalDelete(
        TSHashTable<FACEDATA,HASHKEY_STRI> *this,
        FACEDATA *ptr)
{
  ((void (__thiscall *)(FACEDATA *, _DWORD))ptr->dtr_FACEDATA)(ptr, 0);
  SMemFree(ptr, aAufacedata, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001CEBD0  EA 0x5CEBD0
// DIA: TSHashTable<FACEDATA,HASHKEY_STRI>::InternalNew
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
FACEDATA *__thiscall TSHashTable<FACEDATA,HASHKEY_STRI>::InternalNew(
        TSHashTable<FACEDATA,HASHKEY_STRI> *this,
        TSExplicitList<FACEDATA,-572662307> *listptr,
        unsigned int extrabytes,
        unsigned int flags)
{
  FACEDATA *result; // eax
  TSLink<FACEDATA> *p_m_terminator; // ecx
  TSLink<FACEDATA> *m_prevlink; // edi
  int m_next; // esi
  int v8; // esi
  int v9; // esi
  int m_linkoffset; // edi

  result = (FACEDATA *)SMemAlloc(extrabytes + 44, aAufacedata, -2, flags | 8);
  if ( result != nullptr )
  {
    result->m_linktoslot.m_prevlink = nullptr;
    result->m_linktoslot.m_next = nullptr;
    result->m_linktofull.m_prevlink = nullptr;
    result->m_linktofull.m_next = nullptr;
    result->m_key.m_str = nullptr;
    result->m_refcount = 0;
    result->data = nullptr;
    result->face = nullptr;
    result->__vftable = (FACEDATA_vtbl *)&FACEDATA::`vftable';
    p_m_terminator = (TSLink<FACEDATA> *)((char *)result + listptr->m_linkoffset);
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
// RVA 0x001CEC90  EA 0x5CEC90
// DIA: TSHashTable<FACEDATA,HASHKEY_STRI>::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<FACEDATA,HASHKEY_STRI>::Destroy(TSHashTable<FACEDATA,HASHKEY_STRI> *this)
{
  TSHashTable<FACEDATA,HASHKEY_STRI> *v1; // edi
  int m_next; // eax
  unsigned int v3; // ebx
  _DWORD *v4; // esi
  int v5; // eax
  const char *v6; // eax
  int v7; // eax
  const char *v8; // eax
  int v9; // edi
  int v10; // eax
  TSLink<FACEDATA> *v11; // edi
  unsigned int m_count; // eax
  unsigned int v13; // esi
  int v14; // ebx
  TSExplicitList<FACEDATA,-572662307> *m_data; // eax
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
    TSLink<FACEDATA>::Unlink((TSLink<FACEDATA> *)(m_next + v1->m_fulllist.m_linkoffset));
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
        v11 = (TSLink<FACEDATA> *)(v24 + v9);
        if ( v10 != 0 )
        {
          TSLink<FACEDATA>::NextLink(v11, -1)->m_prevlink = v11->m_prevlink;
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
  v1->m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<FACEDATA,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<FACEDATA,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v14 = 0;
    do
    {
      TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::~TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>(&v1->m_slotlistarray.m_data[v14]);
      ++v13;
      ++v14;
    }
    while ( v13 < v1->m_slotlistarray.m_count );
  }
  m_data = v1->m_slotlistarray.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_46, -2, 0);
  v1->m_slotlistarray.m_alloc = 0;
  v1->m_slotlistarray.m_count = 0;
  v1->m_slotlistarray.m_data = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001CEE00  EA 0x5CEE00
// DIA: TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::MemFileName(
        TSBaseArray<TSExplicitList<FACEDATA,-572662307> > *this)
{
  return aAvTsexplicitli_46;
}


// -----------------------------------------------------------------------------
// RVA 0x001CEE10  EA 0x5CEE10
// DIA: TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::MemLineNo(
        TSBaseArray<TSExplicitList<FACEDATA,-572662307> > *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CEE20  EA 0x5CEE20
// DIA: TSHashTable<FACEDATA,HASHKEY_STRI>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashTable<FACEDATA,HASHKEY_STRI> *__thiscall TSHashTable<FACEDATA,HASHKEY_STRI>::`scalar deleting destructor'(
        TSHashTable<FACEDATA,HASHKEY_STRI> *this,
        char __flags)
{
  TSHashTable<FACEDATA,HASHKEY_STRI> *v2; // esi
  unsigned int v3; // ebx
  TSExplicitList<FACEDATA,-572662307> *p_m_fulllist; // edi
  TSLink<FACEDATA> *v5; // eax
  TSBaseArray<TSExplicitList<FACEDATA,-572662307> > *p_m_slotlistarray; // esi
  int m_next; // edi
  TSLink<FACEDATA> **p_m_prevlink; // edi
  int v9; // eax
  int v12; // [esp+10h] [ebp-4h]

  v2 = this;
  v3 = 0;
  this->__vftable = (TSHashTable<FACEDATA,HASHKEY_STRI>_vtbl *)&TSHashTable<FACEDATA,HASHKEY_STRI>::`vftable';
  this->m_fullnessIndicator = 0;
  p_m_fulllist = &this->m_fulllist;
  while ( (int)p_m_fulllist->m_terminator.m_next > 0 )
  {
    v5 = TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::Link(p_m_fulllist, p_m_fulllist->m_terminator.m_next);
    TSLink<FACEDATA>::Unlink(v5);
  }
  if ( v2->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &v2->m_slotlistarray;
    v12 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        m_next = (int)p_m_slotlistarray->m_data[v12].m_terminator.m_next;
        if ( m_next <= 0 )
          break;
        TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        TSLink<FACEDATA>::Unlink((TSLink<FACEDATA> *)(m_next + p_m_slotlistarray->m_data[v12].m_linkoffset));
      }
      ++v3;
      ++v12;
    }
    while ( v3 < this->m_slotlistarray.m_count );
    v2 = this;
  }
  TSFixedArray<TSExplicitList<FACEDATA,-572662307>>::~TSFixedArray<TSExplicitList<FACEDATA,-572662307>>(&v2->m_slotlistarray);
  p_m_prevlink = &v2->m_fulllist.m_terminator.m_prevlink;
  while ( 1 )
  {
    v9 = (int)v2->m_fulllist.m_terminator.m_next;
    if ( v9 <= 0 )
      break;
    TSLink<FACEDATA>::Unlink((TSLink<FACEDATA> *)(v9 + v2->m_fulllist.m_linkoffset));
  }
  if ( *p_m_prevlink != nullptr )
  {
    TSLink<FACEDATA>::NextLink(&v2->m_fulllist.m_terminator, -1)->m_prevlink = *p_m_prevlink;
    (*p_m_prevlink)->m_next = v2->m_fulllist.m_terminator.m_next;
    *p_m_prevlink = nullptr;
    v2->m_fulllist.m_terminator.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 )
    SMemFree((void *)v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001CEF10  EA 0x5CEF10
// DIA: $E5
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E5_265()
{
  return _E4_265();
}


// -----------------------------------------------------------------------------
// RVA 0x001CEF20  EA 0x5CEF20
// DIA: $E4
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E4_265()
{
  TWO_PI_265 = 3.1415927 + 3.1415927;
}


// -----------------------------------------------------------------------------
// RVA 0x001CEF30  EA 0x5CEF30
// DIA: $E8
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E8_265()
{
  return _E7_265();
}


// -----------------------------------------------------------------------------
// RVA 0x001CEF40  EA 0x5CEF40
// DIA: $E7
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E7_265()
{
  OO_TWO_PI_265 = 1.0 / TWO_PI_265;
}


// -----------------------------------------------------------------------------
// RVA 0x001CEF60  EA 0x5CEF60
// DIA: TSLink<FACEDATA>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<FACEDATA>::Unlink(TSLink<FACEDATA> *this)
{
  TSLink<FACEDATA> *m_prevlink; // esi
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
// RVA 0x001CEFA0  EA 0x5CEFA0
// DIA: TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >::~TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::~TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>(
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *this)
{
  TSLink<FACEDATA> *p_m_terminator; // ebx
  FACEDATA *m_next; // edx
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // esi
  int v6; // esi
  TSLink<FACEDATA> *m_prevlink; // edx
  int v8; // ecx
  int v9; // ecx

  p_m_terminator = &this->m_terminator;
  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v3 = *(int *)((char *)&m_next->__vftable + this->m_linkoffset);
    v4 = (FACEDATA_vtbl **)((char *)&m_next->__vftable + this->m_linkoffset);
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
// RVA 0x001CF020  EA 0x5CF020
// DIA: TSFixedArray<TSExplicitList<FACEDATA,-572662307> >::~TSFixedArray<TSExplicitList<FACEDATA,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<FACEDATA,-572662307>>::~TSFixedArray<TSExplicitList<FACEDATA,-572662307>>(
        TSFixedArray<TSExplicitList<FACEDATA,-572662307> > *this)
{
  unsigned int m_count; // ecx
  TSExplicitList<FACEDATA,-572662307> *v3; // esi
  TSLink<FACEDATA> *v4; // eax
  bool v5; // cf
  TSExplicitList<FACEDATA,-572662307> *m_data; // ebx
  unsigned int index; // [esp+4h] [ebp-8h]
  int v8; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<TSExplicitList<FACEDATA,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<FACEDATA,-572662307>>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v8 = 0;
    do
    {
      v3 = &this->m_data[v8];
      while ( (int)v3->m_terminator.m_next > 0 )
      {
        v4 = TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::Link(v3, v3->m_terminator.m_next);
        TSLink<FACEDATA>::Unlink(v4);
      }
      TSLink<FACEDATA>::Unlink(&v3->m_terminator);
      v5 = ++index < this->m_count;
      ++v8;
    }
    while ( v5 );
  }
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_46, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001CF0B0  EA 0x5CF0B0
// DIA: TSLink<FACEDATA>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<FACEDATA> *__thiscall TSLink<FACEDATA>::NextLink(TSLink<FACEDATA> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<FACEDATA> *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<FACEDATA> *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001CF0E0  EA 0x5CF0E0
// DIA: TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<FACEDATA> *__thiscall TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::Link(
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *this,
        const FACEDATA *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<FACEDATA> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CF100  EA 0x5CF100
// DIA: TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::LinkNode(
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *this,
        FACEDATA *ptr,
        unsigned int linktype,
        FACEDATA *existingptr)
{
  TSLink<FACEDATA> *p_m_terminator; // eax
  TSLink<FACEDATA> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<FACEDATA> *v8; // edx
  TSLink<FACEDATA> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<FACEDATA> *)((char *)ptr + this->m_linkoffset);
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
    v8 = (TSLink<FACEDATA> *)((char *)existingptr + this->m_linkoffset);
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
// RVA 0x001CF1D0  EA 0x5CF1D0
// DIA: TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::CheckArrayBounds
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(
        TSBaseArray<TSExplicitList<FACEDATA,-572662307> > *this,
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
    v4 = ((int (__thiscall *)(TSBaseArray<TSExplicitList<FACEDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo)(
           this,
           1,
           1,
           aIndex0x08xArra,
           index,
           m_count);
    v5 = (const char *)((int (__thiscall *)(TSBaseArray<TSExplicitList<FACEDATA,-572662307> > *, int))this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName)(
                         this,
                         v4);
    SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CF210  EA 0x5CF210
// DIA: TSHashTable<FACEDATA,HASHKEY_STRI>::Initialize
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<FACEDATA,HASHKEY_STRI>::Initialize(TSHashTable<FACEDATA,HASHKEY_STRI> *this)
{
  TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *p_m_slotlistarray; // edi
  unsigned int m_count; // eax
  unsigned int v3; // esi
  unsigned int m_chunk; // eax
  unsigned int v5; // eax
  unsigned int v6; // eax
  int v7; // ebx
  TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *v8; // esi
  int v9; // ebx
  unsigned int v10; // esi
  int v11; // eax
  const char *v12; // eax
  TSExplicitList<FACEDATA,-572662307> *m_data; // ebx
  int m_linkoffset; // eax
  TSExplicitList<FACEDATA,-572662307> *v15; // ebx
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
        TSExplicitList<FACEDATA,-572662307>::`scalar deleting destructor'(&p_m_slotlistarray->m_data[v9], 0);
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
        m_chunk = TSGrowableArray<TSExplicitList<FACEDATA,-572662307>>::CalcChunkSize(&this->m_slotlistarray, 4u);
      v5 = TSGrowableArray<TSExplicitList<FACEDATA,-572662307>>::RoundToChunk(p_m_slotlistarray, 4u, m_chunk);
      TSFixedArray<TSExplicitList<FACEDATA,-572662307>>::ReallocData(p_m_slotlistarray, v5);
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
          TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>(v8);
          v8->m_linkoffset = -572662307;
          TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::InitializeTerminator(v8);
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
      v11 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              v10,
              p_m_slotlistarray->m_count);
      v12 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v11);
      SErrDisplayErrorFmt(0x85100080, v12, v22, v23, v24, v25);
    }
    m_data = p_m_slotlistarray->m_data;
    m_linkoffset = m_data[v29].m_linkoffset;
    v15 = &m_data[v29];
    if ( m_linkoffset != 12 )
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
      v15->m_linkoffset = 12;
      v15->m_terminator.m_next = (FACEDATA *)~p_m_terminator;
    }
    slot = ++v10;
    ++v29;
  }
  while ( v10 <= this->m_slotmask );
}


// -----------------------------------------------------------------------------
// RVA 0x001CF390  EA 0x5CF390
// DIA: TSExplicitList<FACEDATA,-572662307>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSExplicitList<FACEDATA,-572662307> *__thiscall TSExplicitList<FACEDATA,-572662307>::`scalar deleting destructor'(
        TSExplicitList<FACEDATA,-572662307> *this,
        char __flags)
{
  TSExplicitList<FACEDATA,-572662307> *v2; // esi
  TSLink<FACEDATA> *p_m_terminator; // edi
  int m_next; // ecx
  int m_linkoffset; // eax
  int v6; // esi
  _DWORD *v7; // eax
  int v8; // edx
  int v9; // edx
  TSLink<FACEDATA> *m_prevlink; // edx
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
// RVA 0x001CF440  EA 0x5CF440
// DIA: TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >::TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *__thiscall TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>(
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 0;
  this->m_terminator.m_next = (FACEDATA *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CF460  EA 0x5CF460
// DIA: TSHashTable<FACEDATA,HASHKEY_STRI>::MonitorFullness
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSHashTable<FACEDATA,HASHKEY_STRI>::MonitorFullness(
        TSHashTable<FACEDATA,HASHKEY_STRI> *this,
        unsigned int slot)
{
  bool v2; // cf
  unsigned int m_fullnessIndicator; // eax
  TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *p_m_slotlistarray; // esi
  int v6; // eax
  const char *v7; // eax
  unsigned int v8; // ecx
  FACEDATA *m_next; // edi
  int v10; // edi
  TSHashTable<FACEDATA,HASHKEY_STRI> *v11; // eax
  unsigned int v12; // edx
  int v13; // eax
  const char *v14; // eax
  int v16; // eax
  unsigned int v17; // ebx
  unsigned int v18; // edi
  FACEDATA *v19; // eax
  unsigned int m_count; // eax
  unsigned int v21; // eax
  int v22; // edi
  TSExplicitList<FACEDATA,-572662307> *v23; // ecx
  int v24; // edi
  unsigned int v25; // edi
  unsigned int v26; // edi
  FACEDATA *v27; // ebx
  int v28; // [esp-18h] [ebp-3Ch]
  int v29; // [esp-18h] [ebp-3Ch]
  int v30; // [esp-14h] [ebp-38h]
  int v31; // [esp-14h] [ebp-38h]
  unsigned int v32; // [esp-10h] [ebp-34h]
  unsigned int v33; // [esp-10h] [ebp-34h]
  const char *v34; // [esp-Ch] [ebp-30h]
  const char *v35; // [esp-Ch] [ebp-30h]
  TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > v36; // [esp+Ch] [ebp-18h] BYREF
  int v37; // [esp+18h] [ebp-Ch]
  unsigned int v38; // [esp+1Ch] [ebp-8h]
  TSHashTable<FACEDATA,HASHKEY_STRI> *v39; // [esp+20h] [ebp-4h]
  unsigned int slota; // [esp+2Ch] [ebp+8h]
  unsigned int slotb; // [esp+2Ch] [ebp+8h]
  unsigned int slotc; // [esp+2Ch] [ebp+8h]
  unsigned int slotd; // [esp+2Ch] [ebp+8h]

  v2 = this->m_slotmask < 0x1FFF;
  v39 = this;
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
    v6 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo)(
           &this->m_slotlistarray,
           1,
           1,
           aIndex0x08xArra,
           slot,
           this->m_slotlistarray.m_count);
    v7 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName)(
                         p_m_slotlistarray,
                         v6);
    SErrDisplayErrorFmt(0x85100080, v7, v28, v30, v32, v34);
  }
  v8 = 12 * slot;
  m_next = p_m_slotlistarray->m_data[slot].m_terminator.m_next;
  slota = 12 * slot;
  v10 = (int)m_next <= 0 ? 0 : (unsigned int)m_next;
  if ( v10 <= 0 )
    return 0;
  while ( 1 )
  {
    v11 = v39;
    v12 = v39->m_fullnessIndicator + 1;
    v39->m_fullnessIndicator = v12;
    if ( v12 > 0xD )
      break;
    if ( slot >= p_m_slotlistarray->m_count )
    {
      v13 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              slot,
              p_m_slotlistarray->m_count);
      v14 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v13);
      SErrDisplayErrorFmt(0x85100080, v14, v29, v31, v33, v35);
      v8 = slota;
    }
    v10 = *(_DWORD *)(v10 + *(int *)((char *)&p_m_slotlistarray->m_data->m_linkoffset + v8) + 4);
    if ( v10 <= 0 )
      return 0;
  }
  v11->m_fullnessIndicator = 0;
  v16 = v11->m_slotmask + 1;
  v17 = 2 * v16;
  v38 = v16;
  v37 = 2 * v16;
  TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>(&v36);
  v36.m_linkoffset = -572662307;
  TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::InitializeTerminator(&v36);
  if ( v36.m_linkoffset != 12 )
  {
    TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::UnlinkAll(&v36);
    v36.m_linkoffset = 12;
    TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::InitializeTerminator(&v36);
  }
  v18 = 0;
  if ( v38 != 0 )
  {
    slotb = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v18);
        v19 = p_m_slotlistarray->m_data[slotb].m_terminator.m_next;
        if ( (int)v19 <= 0 )
          break;
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::LinkNode(&v36, v19, 2u, nullptr);
      }
      ++v18;
      ++slotb;
    }
    while ( v18 < v38 );
  }
  m_count = p_m_slotlistarray->m_count;
  if ( v17 <= m_count )
  {
    if ( v17 < m_count )
    {
      v24 = v37;
      do
      {
        TSExplicitList<FACEDATA,-572662307>::`scalar deleting destructor'(&p_m_slotlistarray->m_data[v24], 0);
        ++v17;
        ++v24;
      }
      while ( v17 < p_m_slotlistarray->m_count );
      v17 = v37;
    }
  }
  else
  {
    TSGrowableArray<TSExplicitList<FACEDATA,-572662307>>::Reserve(p_m_slotlistarray, v17 - m_count, 1);
    v21 = p_m_slotlistarray->m_count;
    if ( v21 < v17 )
    {
      v22 = v21;
      slotc = v17 - v21;
      do
      {
        v23 = &p_m_slotlistarray->m_data[v22];
        if ( v23 != nullptr )
          TSExplicitList<FACEDATA,-572662307>::TSExplicitList<FACEDATA,-572662307>(v23);
        ++v22;
        --slotc;
      }
      while ( slotc != 0 );
    }
  }
  v25 = 0;
  p_m_slotlistarray->m_count = v17;
  if ( v17 != 0 )
  {
    slotd = 0;
    do
    {
      TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v25);
      TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::ChangeLinkOffset(&p_m_slotlistarray->m_data[slotd], 12);
      ++v25;
      ++slotd;
    }
    while ( v25 < v17 );
  }
  v39->m_slotmask = v17 - 1;
  while ( (int)v36.m_terminator.m_next > 0 )
  {
    v26 = v39->m_slotmask & v36.m_terminator.m_next->m_hashval;
    v27 = v36.m_terminator.m_next;
    TSBaseArray<TSExplicitList<FACEDATA,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v26);
    TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::LinkNode(&p_m_slotlistarray->m_data[v26], v27, 2u, nullptr);
  }
  TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::~TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>(&v36);
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001CF6D0  EA 0x5CF6D0
// DIA: TSExplicitList<FACEDATA,-572662307>::TSExplicitList<FACEDATA,-572662307>
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSExplicitList<FACEDATA,-572662307> *__thiscall TSExplicitList<FACEDATA,-572662307>::TSExplicitList<FACEDATA,-572662307>(
        TSExplicitList<FACEDATA,-572662307> *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = -572662307;
  this->m_terminator.m_next = (FACEDATA *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001CF6F0  EA 0x5CF6F0
// DIA: TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >::ChangeLinkOffset
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::ChangeLinkOffset(
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *this,
        int linkoffset)
{
  TSLink<FACEDATA> *p_m_terminator; // ebx
  FACEDATA *m_next; // edx
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
      v4 = *(int *)((char *)&m_next->__vftable + this->m_linkoffset);
      v5 = (FACEDATA_vtbl **)((char *)&m_next->__vftable + this->m_linkoffset);
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
    this->m_terminator.m_next = (FACEDATA *)~(unsigned int)p_m_terminator;
    this->m_linkoffset = linkoffset;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CF750  EA 0x5CF750
// DIA: TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >::UnlinkAll
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::UnlinkAll(
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *this)
{
  FACEDATA *m_next; // edx
  int v2; // edi
  _DWORD *v3; // eax
  int v4; // esi
  int v5; // esi

  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v2 = *(int *)((char *)&m_next->__vftable + this->m_linkoffset);
    v3 = (FACEDATA_vtbl **)((char *)&m_next->__vftable + this->m_linkoffset);
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
// RVA 0x001CF7A0  EA 0x5CF7A0
// DIA: TSList<FACEDATA,TSGetExplicitLink<FACEDATA> >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::InitializeTerminator(
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (FACEDATA *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001CF7B0  EA 0x5CF7B0
// DIA: TSGrowableArray<TSExplicitList<FACEDATA,-572662307> >::Reserve
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<FACEDATA,-572662307>>::Reserve(
        TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *this,
        unsigned int count,
        int round)
{
  unsigned int v4; // edi
  unsigned int m_chunk; // ecx
  int i; // eax
  TSExplicitList<FACEDATA,-572662307> *v7; // ebx
  TSGrowableArray<TSExplicitList<FACEDATA,-572662307> >_vtbl *v8; // edx
  const char *v9; // eax
  TSExplicitList<FACEDATA,-572662307> *v10; // eax
  const char *v11; // eax
  unsigned int m_count; // eax
  int v13; // ebx
  TSList<FACEDATA,TSGetExplicitLink<FACEDATA> > *v14; // edi
  TSExplicitList<FACEDATA,-572662307> *v15; // ecx
  int m_linkoffset; // edx
  TSLink<FACEDATA> *v17; // eax
  const char *v18; // eax
  int v19; // [esp-8h] [ebp-14h]
  int v20; // [esp-8h] [ebp-14h]
  int v21; // [esp-8h] [ebp-14h]
  TSExplicitList<FACEDATA,-572662307> *counta; // [esp+14h] [ebp+8h]
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
        TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::UnlinkAll(v7);
        TSLink<FACEDATA>::Unlink(&v7->m_terminator);
        ++v7;
        ++rounda;
      }
      while ( rounda < this->m_count );
    }
    v8 = this->__vftable;
    this->m_alloc = v4;
    v19 = v8->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName(this);
    v10 = (TSExplicitList<FACEDATA,-572662307> *)SMemReAlloc(counta, 12 * v4, v9, v19, 0x10u);
    this->m_data = v10;
    if ( v10 == nullptr )
    {
      v20 = this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo(this);
      v11 = (const char *)this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName(this);
      this->m_data = (TSExplicitList<FACEDATA,-572662307> *)SMemAlloc(12 * v4, v11, v20, 0);
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
              v15->m_terminator.m_next = (FACEDATA *)~(unsigned int)&v15->m_terminator;
            }
            while ( (int)v14->m_terminator.m_next > 0 )
            {
              v17 = TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::Link(v14, v14->m_terminator.m_next);
              TSLink<FACEDATA>::Unlink(v17);
            }
            TSLink<FACEDATA>::Unlink(&v14->m_terminator);
            ++v13;
            ++v14;
          }
        }
        v21 = this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo(this);
        v18 = (const char *)this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName(this);
        SMemFree(counta, v18, v21, 0);
      }
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001CF930  EA 0x5CF930
// DIA: TSGrowableArray<TSExplicitList<FACEDATA,-572662307> >::CalcChunkSize
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<FACEDATA,-572662307>>::CalcChunkSize(
        TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *this,
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
// RVA 0x001CF970  EA 0x5CF970
// DIA: TSGrowableArray<TSExplicitList<FACEDATA,-572662307> >::RoundToChunk
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<TSExplicitList<FACEDATA,-572662307>>::RoundToChunk(
        TSGrowableArray<TSExplicitList<FACEDATA,-572662307> > *this,
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
// RVA 0x001CF990  EA 0x5CF990
// DIA: TSFixedArray<TSExplicitList<FACEDATA,-572662307> >::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuFontFace.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<FACEDATA,-572662307>>::ReallocData(
        TSFixedArray<TSExplicitList<FACEDATA,-572662307> > *this,
        unsigned int count)
{
  unsigned int m_count; // ebx
  TSExplicitList<FACEDATA,-572662307> *v4; // edi
  TSLink<FACEDATA> *v5; // eax
  TSFixedArray<TSExplicitList<FACEDATA,-572662307> >_vtbl *v6; // edx
  const char *v7; // eax
  TSExplicitList<FACEDATA,-572662307> *v8; // eax
  const char *v9; // eax
  TSExplicitList<FACEDATA,-572662307> *v10; // edi
  TSExplicitList<FACEDATA,-572662307> *v11; // ecx
  int m_linkoffset; // edx
  int m_next; // eax
  TSLink<FACEDATA> *m_prevlink; // ebx
  TSLink<FACEDATA> *p_m_terminator; // eax
  int v16; // edx
  int v17; // edx
  bool v18; // zf
  const char *v19; // eax
  int v20; // [esp-8h] [ebp-1Ch]
  int v21; // [esp-8h] [ebp-1Ch]
  int v22; // [esp-8h] [ebp-1Ch]
  unsigned int v23; // [esp+Ch] [ebp-8h]
  TSExplicitList<FACEDATA,-572662307> *oldData; // [esp+10h] [ebp-4h]
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
        v5 = TSList<FACEDATA,TSGetExplicitLink<FACEDATA>>::Link(v4, v4->m_terminator.m_next);
        TSLink<FACEDATA>::Unlink(v5);
      }
      TSLink<FACEDATA>::Unlink(&v4->m_terminator);
      ++v4;
      ++count;
    }
    while ( count < this->m_count );
  }
  v6 = this->__vftable;
  this->m_alloc = m_count;
  v20 = v6->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo(this);
  v7 = (const char *)this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName(this);
  v8 = (TSExplicitList<FACEDATA,-572662307> *)SMemReAlloc(oldData, 12 * m_count, v7, v20, 0x10u);
  this->m_data = v8;
  if ( v8 == nullptr )
  {
    v21 = this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName(this);
    v10 = oldData;
    this->m_data = (TSExplicitList<FACEDATA,-572662307> *)SMemAlloc(12 * m_count, v9, v21, 0);
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
            v11->m_terminator.m_next = (FACEDATA *)~(unsigned int)&v11->m_terminator;
          }
          while ( 1 )
          {
            m_next = (int)v10->m_terminator.m_next;
            if ( m_next <= 0 )
              break;
            TSLink<FACEDATA>::Unlink((TSLink<FACEDATA> *)(m_next + v10->m_linkoffset));
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
      v22 = this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemLineNo(this);
      v19 = (const char *)this->TSBaseArray<TSExplicitList<FACEDATA,-572662307> >::MemFileName(this);
      SMemFree(oldData, v19, v22, 0);
    }
  }
}

