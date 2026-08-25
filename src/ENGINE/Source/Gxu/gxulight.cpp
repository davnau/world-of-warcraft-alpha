// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\GxuLight.cpp
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x001B6DB0  EA 0x5B6DB0
// DIA: $E25
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E25_30()
{
  return _E24_24();
}


// -----------------------------------------------------------------------------
// RVA 0x001B6DC0  EA 0x5B6DC0
// DIA: $E24
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int _E24_24()
{
  s_bucketSize = 1024.0;
  return 1149239296;
}


// -----------------------------------------------------------------------------
// RVA 0x001B6DD0  EA 0x5B6DD0
// DIA: $E28
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E28_70()
{
  return _E27_65();
}


// -----------------------------------------------------------------------------
// RVA 0x001B6DE0  EA 0x5B6DE0
// DIA: $E27
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void _E27_65()
{
  s_halfBucket = 1024.0 * 0.5;
}


// -----------------------------------------------------------------------------
// RVA 0x001B6E00  EA 0x5B6E00
// DIA: $E34
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E34_67()
{
  _E30_64();
  return _E33_63();
}


// -----------------------------------------------------------------------------
// RVA 0x001B6E10  EA 0x5B6E10
// DIA: $E30
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E30_64()
{
  CGxuLight::s_lights.m_linkoffset = 0;
  CGxuLight::s_lights.m_terminator.m_prevlink = &CGxuLight::s_lights.m_terminator;
  CGxuLight::s_lights.m_terminator.m_next = (CGxuLight *)~(unsigned int)&CGxuLight::s_lights.m_terminator;
  return ~(unsigned int)&CGxuLight::s_lights.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001B6E40  EA 0x5B6E40
// DIA: $E33
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E33_63()
{
  return atexit(_E31_67);
}


// -----------------------------------------------------------------------------
// RVA 0x001B6E50  EA 0x5B6E50
// DIA: $E31
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E31_67()
{
  CGxuLight *m_next; // eax
  TSLink<CGxuLight> *m_prevlink; // esi
  int v2; // edx
  int v3; // eax

  if ( (_S32_57 & 1) == 0 )
  {
    _S32_57 |= 1u;
LABEL_3:
    m_next = CGxuLight::s_lights.m_terminator.m_next;
    while ( (int)CGxuLight::s_lights.m_terminator.m_next > 0 )
    {
      m_prevlink = CGxuLight::s_lights.m_terminator.m_next->m_link.m_prevlink;
      if ( CGxuLight::s_lights.m_terminator.m_next->m_link.m_prevlink != nullptr )
      {
        v2 = (int)CGxuLight::s_lights.m_terminator.m_next->m_link.m_next;
        if ( v2 > 0 )
          *(_DWORD *)((char *)CGxuLight::s_lights.m_terminator.m_next - (char *)m_prevlink->m_next + v2) = m_prevlink;
        else
          *(_DWORD *)~v2 = m_prevlink;
        m_next->m_link.m_prevlink->m_next = m_next->m_link.m_next;
        m_next->m_link.m_prevlink = nullptr;
        m_next->m_link.m_next = nullptr;
        goto LABEL_3;
      }
    }
    if ( CGxuLight::s_lights.m_terminator.m_prevlink != nullptr )
    {
      if ( (int)CGxuLight::s_lights.m_terminator.m_next > 0 )
        v3 = (int)CGxuLight::s_lights.m_terminator.m_next
           + (char *)&CGxuLight::s_lights.m_terminator
           - (char *)CGxuLight::s_lights.m_terminator.m_prevlink->m_next;
      else
        v3 = ~(int)CGxuLight::s_lights.m_terminator.m_next;
      *(_DWORD *)v3 = CGxuLight::s_lights.m_terminator.m_prevlink;
      CGxuLight::s_lights.m_terminator.m_prevlink->m_next = CGxuLight::s_lights.m_terminator.m_next;
      CGxuLight::s_lights.m_terminator.m_prevlink = nullptr;
      CGxuLight::s_lights.m_terminator.m_next = nullptr;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B6EF0  EA 0x5B6EF0
// DIA: $E39
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E39_62()
{
  _E36_60();
  return _E38_37();
}


// -----------------------------------------------------------------------------
// RVA 0x001B6F00  EA 0x5B6F00
// DIA: $E36
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E36_60()
{
  CGxuLight::s_lightsFreeList.m_linkoffset = 0;
  CGxuLight::s_lightsFreeList.m_terminator.m_prevlink = &CGxuLight::s_lightsFreeList.m_terminator;
  CGxuLight::s_lightsFreeList.m_terminator.m_next = (CGxuLight *)~(unsigned int)&CGxuLight::s_lightsFreeList.m_terminator;
  return ~(unsigned int)&CGxuLight::s_lightsFreeList.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001B6F30  EA 0x5B6F30
// DIA: $E38
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E38_37()
{
  return atexit(_E37_64);
}


// -----------------------------------------------------------------------------
// RVA 0x001B6F40  EA 0x5B6F40
// DIA: $E37
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E37_64()
{
  CGxuLight *m_next; // eax
  TSLink<CGxuLight> *m_prevlink; // esi
  int v2; // edx
  int v3; // eax

  if ( (_S32_57 & 2) == 0 )
  {
    _S32_57 |= 2u;
LABEL_3:
    m_next = CGxuLight::s_lightsFreeList.m_terminator.m_next;
    while ( (int)CGxuLight::s_lightsFreeList.m_terminator.m_next > 0 )
    {
      m_prevlink = CGxuLight::s_lightsFreeList.m_terminator.m_next->m_link.m_prevlink;
      if ( CGxuLight::s_lightsFreeList.m_terminator.m_next->m_link.m_prevlink != nullptr )
      {
        v2 = (int)CGxuLight::s_lightsFreeList.m_terminator.m_next->m_link.m_next;
        if ( v2 > 0 )
          *(_DWORD *)((char *)CGxuLight::s_lightsFreeList.m_terminator.m_next - (char *)m_prevlink->m_next + v2) = m_prevlink;
        else
          *(_DWORD *)~v2 = m_prevlink;
        m_next->m_link.m_prevlink->m_next = m_next->m_link.m_next;
        m_next->m_link.m_prevlink = nullptr;
        m_next->m_link.m_next = nullptr;
        goto LABEL_3;
      }
    }
    if ( CGxuLight::s_lightsFreeList.m_terminator.m_prevlink != nullptr )
    {
      if ( (int)CGxuLight::s_lightsFreeList.m_terminator.m_next > 0 )
        v3 = (int)CGxuLight::s_lightsFreeList.m_terminator.m_next
           + (char *)&CGxuLight::s_lightsFreeList.m_terminator
           - (char *)CGxuLight::s_lightsFreeList.m_terminator.m_prevlink->m_next;
      else
        v3 = ~(int)CGxuLight::s_lightsFreeList.m_terminator.m_next;
      *(_DWORD *)v3 = CGxuLight::s_lightsFreeList.m_terminator.m_prevlink;
      CGxuLight::s_lightsFreeList.m_terminator.m_prevlink->m_next = CGxuLight::s_lightsFreeList.m_terminator.m_next;
      CGxuLight::s_lightsFreeList.m_terminator.m_prevlink = nullptr;
      CGxuLight::s_lightsFreeList.m_terminator.m_next = nullptr;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B6FE0  EA 0x5B6FE0
// DIA: $E44
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E44_36()
{
  _E41_34();
  return _E43_63();
}


// -----------------------------------------------------------------------------
// RVA 0x001B6FF0  EA 0x5B6FF0
// DIA: $E41
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E41_34()
{
  CGxuLight::s_linksFreeList.m_linkoffset = 12;
  CGxuLight::s_linksFreeList.m_terminator.m_prevlink = &CGxuLight::s_linksFreeList.m_terminator;
  CGxuLight::s_linksFreeList.m_terminator.m_next = (CGxuLightLink *)~(unsigned int)&CGxuLight::s_linksFreeList.m_terminator;
  return ~(unsigned int)&CGxuLight::s_linksFreeList.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7020  EA 0x5B7020
// DIA: $E43
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E43_63()
{
  return atexit(_E42_59);
}


// -----------------------------------------------------------------------------
// RVA 0x001B7030  EA 0x5B7030
// DIA: $E42
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E42_59()
{
  int v0; // edx
  _DWORD *v1; // eax
  int v2; // esi
  int v3; // ecx

  if ( (_S32_57 & 4) == 0 )
  {
    _S32_57 |= 4u;
    while ( (int)CGxuLight::s_linksFreeList.m_terminator.m_next > 0 )
    {
      v0 = *(int *)((char *)&CGxuLight::s_linksFreeList.m_terminator.m_next->m_light
                  + CGxuLight::s_linksFreeList.m_linkoffset);
      v1 = (CGxuLight **)((char *)&CGxuLight::s_linksFreeList.m_terminator.m_next->m_light
                        + CGxuLight::s_linksFreeList.m_linkoffset);
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
    if ( CGxuLight::s_linksFreeList.m_terminator.m_prevlink != nullptr )
    {
      if ( (int)CGxuLight::s_linksFreeList.m_terminator.m_next > 0 )
        v3 = (int)CGxuLight::s_linksFreeList.m_terminator.m_next
           + (char *)&CGxuLight::s_linksFreeList.m_terminator
           - (char *)CGxuLight::s_linksFreeList.m_terminator.m_prevlink->m_next;
      else
        v3 = ~(int)CGxuLight::s_linksFreeList.m_terminator.m_next;
      *(_DWORD *)v3 = CGxuLight::s_linksFreeList.m_terminator.m_prevlink;
      CGxuLight::s_linksFreeList.m_terminator.m_prevlink->m_next = CGxuLight::s_linksFreeList.m_terminator.m_next;
      CGxuLight::s_linksFreeList.m_terminator.m_prevlink = nullptr;
      CGxuLight::s_linksFreeList.m_terminator.m_next = nullptr;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B70E0  EA 0x5B70E0
// DIA: $E49
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E49_61()
{
  _E46_61();
  return _E48_63();
}


// -----------------------------------------------------------------------------
// RVA 0x001B70F0  EA 0x5B70F0
// DIA: $E46
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E46_61()
{
  s_dirLightList.m_linktoslot.m_prevlink = nullptr;
  s_dirLightList.m_linktoslot.m_next = nullptr;
  s_dirLightList.m_linktofull.m_prevlink = nullptr;
  s_dirLightList.m_linktofull.m_next = nullptr;
  s_dirLightList.m_key.m_key = 0;
  s_dirLightList.m_links.m_linkoffset = 20;
  s_dirLightList.m_links.m_terminator.m_prevlink = &s_dirLightList.m_links.m_terminator;
  s_dirLightList.m_links.m_terminator.m_next = (CGxuLightLink *)~(unsigned int)&s_dirLightList.m_links.m_terminator;
  return ~(unsigned int)&s_dirLightList.m_links.m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7130  EA 0x5B7130
// DIA: $E48
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E48_63()
{
  return atexit(_E47_37);
}


// -----------------------------------------------------------------------------
// RVA 0x001B7140  EA 0x5B7140
// DIA: $E47
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E47_37()
{
  char *v0; // esi
  int v1; // eax
  int v2; // ecx
  int v3; // ecx

  while ( (int)s_dirLightList.m_links.m_terminator.m_next > 0 )
  {
    v0 = (char *)s_dirLightList.m_links.m_terminator.m_next + s_dirLightList.m_links.m_linkoffset;
    if ( *(CGxuLight **)((char *)&s_dirLightList.m_links.m_terminator.m_next->m_light
                       + s_dirLightList.m_links.m_linkoffset) != nullptr )
    {
      TSLink<CGxuLightLink>::NextLink(
        (TSLink<CGxuLightLink> *)((char *)s_dirLightList.m_links.m_terminator.m_next
                                + s_dirLightList.m_links.m_linkoffset),
        -1)->m_prevlink = *(TSLink<CGxuLightLink> **)v0;
      *(_DWORD *)(*(_DWORD *)v0 + 4) = *((_DWORD *)v0 + 1);
      *(_DWORD *)v0 = 0;
      *((_DWORD *)v0 + 1) = 0;
    }
  }
  if ( s_dirLightList.m_links.m_terminator.m_prevlink != nullptr )
  {
    if ( (int)s_dirLightList.m_links.m_terminator.m_next > 0 )
      v1 = (int)s_dirLightList.m_links.m_terminator.m_next
         + (char *)&s_dirLightList.m_links.m_terminator
         - (char *)s_dirLightList.m_links.m_terminator.m_prevlink->m_next;
    else
      v1 = ~(int)s_dirLightList.m_links.m_terminator.m_next;
    *(_DWORD *)v1 = s_dirLightList.m_links.m_terminator.m_prevlink;
    s_dirLightList.m_links.m_terminator.m_prevlink->m_next = s_dirLightList.m_links.m_terminator.m_next;
    s_dirLightList.m_links.m_terminator.m_prevlink = nullptr;
    s_dirLightList.m_links.m_terminator.m_next = nullptr;
  }
  if ( s_dirLightList.m_linktofull.m_prevlink != nullptr )
  {
    if ( (int)s_dirLightList.m_linktofull.m_next > 0 )
      v2 = (int)s_dirLightList.m_linktofull.m_next
         + (char *)&s_dirLightList.m_linktofull
         - (char *)s_dirLightList.m_linktofull.m_prevlink->m_next;
    else
      v2 = ~(int)s_dirLightList.m_linktofull.m_next;
    *(_DWORD *)v2 = s_dirLightList.m_linktofull.m_prevlink;
    s_dirLightList.m_linktofull.m_prevlink->m_next = s_dirLightList.m_linktofull.m_next;
    s_dirLightList.m_linktofull.m_prevlink = nullptr;
    s_dirLightList.m_linktofull.m_next = nullptr;
  }
  if ( s_dirLightList.m_linktoslot.m_prevlink != nullptr )
  {
    if ( (int)s_dirLightList.m_linktoslot.m_next > 0 )
      v3 = (int)s_dirLightList.m_linktoslot.m_next
         + (char *)&s_dirLightList.m_linktoslot
         - (char *)s_dirLightList.m_linktoslot.m_prevlink->m_next;
    else
      v3 = ~(int)s_dirLightList.m_linktoslot.m_next;
    *(_DWORD *)v3 = s_dirLightList.m_linktoslot.m_prevlink;
    s_dirLightList.m_linktoslot.m_prevlink->m_next = s_dirLightList.m_linktoslot.m_next;
    s_dirLightList.m_linktoslot.m_prevlink = nullptr;
    s_dirLightList.m_linktoslot.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B7240  EA 0x5B7240
// DIA: $E2
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E2_264()
{
  return _E1_264();
}


// -----------------------------------------------------------------------------
// RVA 0x001B7250  EA 0x5B7250
// DIA: $E1
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int _E1_264()
{
  INFINITY_264 = INFINITY;
  return 2139095040;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7260  EA 0x5B7260
// DIA: $E55
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E55_56()
{
  _E51_59();
  return _E54_64();
}


// -----------------------------------------------------------------------------
// RVA 0x001B7270  EA 0x5B7270
// DIA: $E51
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E51_59()
{
  unsigned int result; // eax

  TSList<CLightList,TSGetExplicitLink<CLightList>>::TSList<CLightList,TSGetExplicitLink<CLightList>>(&CLightList::s_lightHashTable.m_fulllist);
  CLightList::s_lightHashTable.m_fulllist.m_linkoffset = -572662307;
  TSList<CLightList,TSGetExplicitLink<CLightList>>::InitializeTerminator(&CLightList::s_lightHashTable.m_fulllist);
  TSFixedArray<TSExplicitList<CLightList,-572662307>>::TSFixedArray<TSExplicitList<CLightList,-572662307>>(&CLightList::s_lightHashTable.m_slotlistarray);
  CLightList::s_lightHashTable.m_slotlistarray.TSHashTable<CLightList,HASHKEY_DWORD>::__vftable = (TSGrowableArray<TSExplicitList<CLightList,-572662307> >_vtbl *)&TSGrowableArray<TSExplicitList<CLightList,-572662307>>::`vftable';
  CLightList::s_lightHashTable.m_slotlistarray.m_chunk = 0;
  CLightList::s_lightHashTable.__vftable = (TSHashTableReuse<CLightList,HASHKEY_DWORD,1>_vtbl *)&TSHashTable<CLightList,HASHKEY_DWORD>::`vftable';
  CLightList::s_lightHashTable.m_fullnessIndicator = 0;
  TSList<CLightList,TSGetExplicitLink<CLightList>>::ChangeLinkOffset(&CLightList::s_lightHashTable.m_fulllist, 12);
  CLightList::s_lightHashTable.m_slotmask = -1;
  CLightList::s_lightHashTable.m_reuseList.m_terminator.m_prevlink = nullptr;
  CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next = nullptr;
  CLightList::s_lightHashTable.m_reuseList.m_linkoffset = 0;
  TSList<CLightList,TSGetExplicitLink<CLightList>>::InitializeTerminator(&CLightList::s_lightHashTable.m_reuseList);
  CLightList::s_lightHashTable.m_reuseList.m_linkoffset = -572662307;
  CLightList::s_lightHashTable.m_reuseList.m_terminator.m_prevlink = &CLightList::s_lightHashTable.m_reuseList.m_terminator;
  CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next = (CLightList *)~(unsigned int)&CLightList::s_lightHashTable.m_reuseList.m_terminator;
  CLightList::s_lightHashTable.m_chunkList.m_terminator.m_prevlink = nullptr;
  CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next = nullptr;
  CLightList::s_lightHashTable.m_chunkList.m_linkoffset = 0;
  TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>>::InitializeTerminator(&CLightList::s_lightHashTable.m_chunkList);
  result = ~(unsigned int)&CLightList::s_lightHashTable.m_chunkList.m_terminator;
  CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next = (TSHashObjectChunk<CLightList,HASHKEY_DWORD> *)~(unsigned int)&CLightList::s_lightHashTable.m_chunkList.m_terminator;
  CLightList::s_lightHashTable.m_chunkList.m_linkoffset = 20;
  CLightList::s_lightHashTable.m_chunkList.m_terminator.m_prevlink = &CLightList::s_lightHashTable.m_chunkList.m_terminator;
  CLightList::s_lightHashTable.__vftable = (TSHashTableReuse<CLightList,HASHKEY_DWORD,1>_vtbl *)&TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::`vftable';
  CLightList::s_lightHashTable.m_chunkSize = 16;
  if ( CLightList::s_lightHashTable.m_reuseList.m_linkoffset != 4 )
  {
    while ( 1 )
    {
      result = (unsigned int)CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next;
      if ( (int)CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next <= 0 )
        break;
      TSLink<CLightList>::Unlink((TSLink<CLightList> *)((char *)CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next
                                                      + CLightList::s_lightHashTable.m_reuseList.m_linkoffset));
    }
    CLightList::s_lightHashTable.m_reuseList.m_linkoffset = 4;
    CLightList::s_lightHashTable.m_reuseList.m_terminator.m_prevlink = &CLightList::s_lightHashTable.m_reuseList.m_terminator;
    CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next = (CLightList *)~(unsigned int)&CLightList::s_lightHashTable.m_reuseList.m_terminator;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001B73A0  EA 0x5B73A0
// DIA: $E54
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E54_64()
{
  return atexit(_E52_62);
}


// -----------------------------------------------------------------------------
// RVA 0x001B73B0  EA 0x5B73B0
// DIA: $E52
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E52_62()
{
  TSHashObjectChunk<CLightList,HASHKEY_DWORD> *m_next; // eax
  unsigned int v1; // edi
  TSHashObjectChunk<CLightList,HASHKEY_DWORD> *v2; // esi
  CLightList *v3; // esi
  int v4; // ebx
  CLightList *v5; // esi
  TSLink<CLightList> *v6; // eax

  if ( (_S53_46 & 1) == 0 )
  {
    _S53_46 |= 1u;
    CLightList::s_lightHashTable.__vftable = (TSHashTableReuse<CLightList,HASHKEY_DWORD,1>_vtbl *)&TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::`vftable';
    while ( 1 )
    {
      m_next = CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next;
      v1 = 0;
      if ( (int)CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next <= 0 )
        break;
      v2 = CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next;
      if ( CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next == nullptr )
        break;
      TSHashObjectChunk<CLightList,HASHKEY_DWORD>::~TSHashObjectChunk<CLightList,HASHKEY_DWORD>(CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next);
      SMemFree(v2, aAvTshashobject_7, -2, 0);
    }
    while ( 1 )
    {
      v3 = CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next;
      if ( (int)CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next <= 0 )
        break;
      CLightList::`scalar deleting destructor'(CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next, 0);
      SMemFree(v3, aAuclightlist, -2, 0);
      m_next = CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next;
    }
    CLightList::s_lightHashTable.m_chunkSize = 16;
    while ( (int)m_next > 0 )
    {
      TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>::Unlink((TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *)((char *)m_next + CLightList::s_lightHashTable.m_chunkList.m_linkoffset));
      v3 = CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next;
      m_next = CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next;
    }
    if ( CLightList::s_lightHashTable.m_chunkList.m_terminator.m_prevlink == nullptr )
      goto LABEL_14;
    TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>::NextLink(
      &CLightList::s_lightHashTable.m_chunkList.m_terminator,
      -1)->m_prevlink = CLightList::s_lightHashTable.m_chunkList.m_terminator.m_prevlink;
    CLightList::s_lightHashTable.m_chunkList.m_terminator.m_prevlink->m_next = CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next;
    CLightList::s_lightHashTable.m_chunkList.m_terminator.m_prevlink = nullptr;
    CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next = nullptr;
    while ( 1 )
    {
      v3 = CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next;
LABEL_14:
      if ( (int)v3 <= 0 )
        break;
      TSLink<CLightList>::Unlink((TSLink<CLightList> *)((char *)v3
                                                      + CLightList::s_lightHashTable.m_reuseList.m_linkoffset));
    }
    if ( CLightList::s_lightHashTable.m_reuseList.m_terminator.m_prevlink != nullptr )
    {
      TSLink<CLightList>::NextLink(&CLightList::s_lightHashTable.m_reuseList.m_terminator, -1)->m_prevlink = CLightList::s_lightHashTable.m_reuseList.m_terminator.m_prevlink;
      CLightList::s_lightHashTable.m_reuseList.m_terminator.m_prevlink->m_next = CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next;
      CLightList::s_lightHashTable.m_reuseList.m_terminator.m_prevlink = nullptr;
      CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next = nullptr;
    }
    CLightList::s_lightHashTable.__vftable = (TSHashTableReuse<CLightList,HASHKEY_DWORD,1>_vtbl *)&TSHashTable<CLightList,HASHKEY_DWORD>::`vftable';
    CLightList::s_lightHashTable.m_fullnessIndicator = 0;
    TSList<CLightList,TSGetExplicitLink<CLightList>>::UnlinkAll(&CLightList::s_lightHashTable.m_fulllist);
    if ( CLightList::s_lightHashTable.m_slotlistarray.m_count != 0 )
    {
      v4 = 0;
      do
      {
        while ( 1 )
        {
          TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(
            &CLightList::s_lightHashTable.m_slotlistarray,
            v1);
          v5 = CLightList::s_lightHashTable.m_slotlistarray.m_data[v4].m_terminator.m_next;
          if ( (int)v5 <= 0 )
            break;
          TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(
            &CLightList::s_lightHashTable.m_slotlistarray,
            v1);
          v6 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(
                 &CLightList::s_lightHashTable.m_slotlistarray.m_data[v4],
                 v5);
          TSLink<CLightList>::Unlink(v6);
        }
        ++v1;
        ++v4;
      }
      while ( v1 < CLightList::s_lightHashTable.m_slotlistarray.m_count );
    }
    TSFixedArray<TSExplicitList<CLightList,-572662307>>::~TSFixedArray<TSExplicitList<CLightList,-572662307>>(&CLightList::s_lightHashTable.m_slotlistarray);
    TSList<CLightList,TSGetExplicitLink<CLightList>>::~TSList<CLightList,TSGetExplicitLink<CLightList>>(&CLightList::s_lightHashTable.m_fulllist);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B7570  EA 0x5B7570
// DIA: $E60
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E60_59()
{
  _E57_51();
  return _E59_22();
}


// -----------------------------------------------------------------------------
// RVA 0x001B7580  EA 0x5B7580
// DIA: $E57
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int _E57_51()
{
  unsigned int expand; // edx
  unsigned int iused; // eax
  unsigned int iallocated; // ecx
  unsigned int v3; // esi
  unsigned int result; // eax

  NTempest::CMemBlock::CMemBlock(&s_lightPriQ.NTempest::CDynTable<CGxuLight *>, 0x80u, 0, aPaucgxulight, -2);
  expand = 32;
  iused = 0;
  iallocated = 32;
  s_lightPriQ.expand = 32;
  s_lightPriQ.iallocated = 32;
  s_lightPriQ.iused = 0;
  s_lightPriQ.__vftable = (NTempest::CPriorityQ<CGxuLight *,CGxuLight>_vtbl *)&NTempest::CPriorityQ<CGxuLight *,CGxuLight>::`vftable';
  if ( s_lightPriQ.mem_ == nullptr )
  {
    SErrDisplayError(0x85100000, aTempestCpriori, 177, aCpriorityqTBCo, 0, 1u);
    if ( s_lightPriQ.mem_ == nullptr )
      SErrDisplayError(0x85100000, aTempestCdyntab, 304, aIsvalid, 0, 1u);
    iallocated = s_lightPriQ.iallocated;
    expand = s_lightPriQ.expand;
    iused = s_lightPriQ.iused;
  }
  if ( iused + 1 <= iallocated )
    goto LABEL_11;
  if ( expand != 0 )
  {
    v3 = iused - iallocated + 1;
    if ( expand > v3 )
      v3 = expand;
    if ( NTempest::CMemBlock::Resize(&s_lightPriQ.NTempest::CDynTable<CGxuLight *>, 4 * (v3 + iallocated), 1u) != 0 )
    {
      s_lightPriQ.iallocated += v3;
      iused = s_lightPriQ.iused;
LABEL_11:
      result = iused + 1;
      s_lightPriQ.iused = result;
      return result;
    }
  }
  return SErrDisplayError(0x85100000, aTempestCpriori, 178, aGrow, 0, 1u);
}


// -----------------------------------------------------------------------------
// RVA 0x001B7680  EA 0x5B7680
// DIA: $E59
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E59_22()
{
  return atexit(_E58_58);
}


// -----------------------------------------------------------------------------
// RVA 0x001B7690  EA 0x5B7690
// DIA: $E58
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E58_58()
{
  s_lightPriQ.__vftable = (NTempest::CPriorityQ<CGxuLight *,CGxuLight>_vtbl *)&NTempest::CPriorityQ<CGxuLight *,CGxuLight>::`vftable';
  NTempest::CMemBlock::~CMemBlock(&s_lightPriQ.NTempest::CDynTable<CGxuLight *>);
}


// -----------------------------------------------------------------------------
// RVA 0x001B76B0  EA 0x5B76B0
// DIA: $E65
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E65_16()
{
  _E62_20();
  return _E64_46();
}


// -----------------------------------------------------------------------------
// RVA 0x001B76C0  EA 0x5B76C0
// DIA: $E62
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int _E62_20()
{
  s_lightsToUse.m_alloc = 0;
  s_lightsToUse.m_count = 0;
  s_lightsToUse.m_data = nullptr;
  s_lightsToUse.__vftable = (TSGrowableArray<CGxuLight *>_vtbl *)&TSGrowableArray<CGxuLight *>::`vftable';
  s_lightsToUse.m_chunk = 0;
  return 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001B76F0  EA 0x5B76F0
// DIA: $E64
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E64_46()
{
  return atexit(_E63_42);
}


// -----------------------------------------------------------------------------
// RVA 0x001B7700  EA 0x5B7700
// DIA: $E63
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E63_42()
{
  s_lightsToUse.__vftable = (TSGrowableArray<CGxuLight *>_vtbl *)&TSFixedArray<CGxuLight *>::`vftable';
  if ( s_lightsToUse.m_data != nullptr )
    SMemFree(s_lightsToUse.m_data, aPaucgxulight, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001B7730  EA 0x5B7730
// DIA: $E70
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E70_46()
{
  _E67_54();
  return _E69_42();
}


// -----------------------------------------------------------------------------
// RVA 0x001B7740  EA 0x5B7740
// DIA: $E67
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void _E67_54()
{
  s_cameraWorldPos.x = 0.0;
  s_cameraWorldPos.y = 0.0;
  s_cameraWorldPos.z = 0.0;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7760  EA 0x5B7760
// DIA: $E69
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
int _E69_42()
{
  return atexit(_E68_24);
}


// -----------------------------------------------------------------------------
// RVA 0x001B7770  EA 0x5B7770
// DIA: $E68
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E68_24()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7780  EA 0x5B7780
// DIA: GxuLightFuncsSet
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuLightFuncsSet(
        void (__fastcall *shutDownFunc)(),
        void (__fastcall *a2)(),
        unsigned int (__fastcall *createFunc)(),
        void (__fastcall *destroyFunc)(unsigned int),
        CGxLight *(__fastcall *lockFunc)(unsigned int),
        void (__fastcall *unlockFunc)(unsigned int),
        void (__fastcall *selectFunc)(NTempest::C3Vector, const NTempest::C3Vector *, unsigned int),
        int (__fastcall *enableFunc)(unsigned int),
        void (__fastcall *enableSetFunc)(unsigned int, int),
        void (__fastcall *setMaxLightsFunc)(unsigned int),
        float (__fastcall *bucketSizeFunc)(),
        void (__fastcall *bucketSizeSetFunc)(float),
        void (__fastcall *resetCacheFunc)())
{
  if ( shutDownFunc != nullptr )
    GxuLightInitialize = shutDownFunc;
  if ( a2 != nullptr )
    GxuLightShutdown = a2;
  if ( createFunc != nullptr )
    GxuLightCreate = createFunc;
  if ( destroyFunc != nullptr )
    GxuLightDestroy = destroyFunc;
  if ( lockFunc != nullptr )
    GxuLightLock = lockFunc;
  if ( unlockFunc != nullptr )
    GxuLightUnlock = unlockFunc;
  if ( selectFunc != nullptr )
    GxuLightSelect = selectFunc;
  if ( enableFunc != nullptr )
    GxuLightEnable = enableFunc;
  if ( enableSetFunc != nullptr )
    GxuLightEnableSet = enableSetFunc;
  if ( setMaxLightsFunc != nullptr )
    GxuLightSetMaxLights = setMaxLightsFunc;
  if ( bucketSizeFunc != nullptr )
    GxuLightBucketSize = bucketSizeFunc;
  if ( bucketSizeSetFunc != nullptr )
    GxuLightBucketSizeSet = bucketSizeSetFunc;
  if ( resetCacheFunc != nullptr )
    GxuLightResetCache = resetCacheFunc;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7820  EA 0x5B7820
// DIA: IGxuLightInitialize
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void IGxuLightInitialize()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7830  EA 0x5B7830
// DIA: IGxuLightShutdown
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int IGxuLightShutdown()
{
  unsigned int result; // eax
  unsigned int v1; // esi
  int v2; // edi
  TSHashObjectChunk<CLightList,HASHKEY_DWORD> *m_next; // esi
  CLightList *v4; // esi
  CGxuLight *v5; // esi
  CGxuLightLink *v6; // esi

  if ( (int)CGxuLight::s_lights.m_terminator.m_next > 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_243, 118, aCgxulightSLigh, 0, 1u);
  CLightList::s_lightHashTable.m_fullnessIndicator = 0;
  TSList<CLightList,TSGetExplicitLink<CLightList>>::UnlinkAll(&CLightList::s_lightHashTable.m_fulllist);
  result = CLightList::s_lightHashTable.m_slotlistarray.m_count;
  v1 = 0;
  if ( CLightList::s_lightHashTable.m_slotlistarray.m_count != 0 )
  {
    v2 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(
          &CLightList::s_lightHashTable.m_slotlistarray,
          v1);
        if ( (int)CLightList::s_lightHashTable.m_slotlistarray.m_data[v2].m_terminator.m_next <= 0 )
          break;
        ((void (__thiscall *)(TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *, CLightList *))CLightList::s_lightHashTable.TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::Destroy)(
          &CLightList::s_lightHashTable,
          CLightList::s_lightHashTable.m_slotlistarray.m_data[v2].m_terminator.m_next);
      }
      result = CLightList::s_lightHashTable.m_slotlistarray.m_count;
      ++v1;
      ++v2;
    }
    while ( v1 < CLightList::s_lightHashTable.m_slotlistarray.m_count );
  }
  while ( 1 )
  {
    m_next = CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next;
    if ( (int)CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next <= 0 )
      break;
    TSHashObjectChunk<CLightList,HASHKEY_DWORD>::`scalar deleting destructor'(
      CLightList::s_lightHashTable.m_chunkList.m_terminator.m_next,
      0);
    result = SMemFree(m_next, aAvTshashobject_7, -2, 0);
  }
  while ( 1 )
  {
    v4 = CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next;
    if ( (int)CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next <= 0 )
      break;
    CLightList::`scalar deleting destructor'(CLightList::s_lightHashTable.m_reuseList.m_terminator.m_next, 0);
    result = SMemFree(v4, aAuclightlist, -2, 0);
  }
  CLightList::s_lightHashTable.m_chunkSize = 16;
  while ( 1 )
  {
    v5 = CGxuLight::s_lightsFreeList.m_terminator.m_next;
    if ( (int)CGxuLight::s_lightsFreeList.m_terminator.m_next <= 0 )
      break;
    TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::~TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>(&CGxuLight::s_lightsFreeList.m_terminator.m_next->m_links);
    CGxLight::~CGxLight(&v5->m_light);
    TSLinkedNode<CGxuLight>::~TSLinkedNode<CGxuLight>(v5);
    result = SMemFree(v5, aAucgxulight, -2, 0);
  }
  while ( 1 )
  {
    v6 = CGxuLight::s_linksFreeList.m_terminator.m_next;
    if ( (int)CGxuLight::s_linksFreeList.m_terminator.m_next <= 0 )
      break;
    TSLink<CGxuLightLink>::Unlink(&CGxuLight::s_linksFreeList.m_terminator.m_next->m_listLink);
    TSLink<CGxuLightLink>::Unlink(&v6->m_lightLink);
    result = SMemFree(v6, aAvcgxulightlin, -2, 0);
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7970  EA 0x5B7970
// DIA: IGxuLightCreate
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CGxuLight *__fastcall IGxuLightCreate()
{
  CGxuLight *m_next; // esi
  CGxuLight *v1; // eax
  TSLink<CGxuLight> *m_prevlink; // edx
  int v4; // ecx
  int v5; // ecx
  TSLink<CGxuLight> *v6; // eax

  m_next = CGxuLight::s_lightsFreeList.m_terminator.m_next;
  if ( (int)CGxuLight::s_lightsFreeList.m_terminator.m_next > 0 )
  {
    m_prevlink = CGxuLight::s_lightsFreeList.m_terminator.m_next->m_link.m_prevlink;
    if ( CGxuLight::s_lightsFreeList.m_terminator.m_next->m_link.m_prevlink != nullptr )
    {
      v4 = (int)CGxuLight::s_lightsFreeList.m_terminator.m_next->m_link.m_next;
      if ( v4 > 0 )
        v5 = (char *)CGxuLight::s_lightsFreeList.m_terminator.m_next - (char *)m_prevlink->m_next + v4;
      else
        v5 = ~v4;
      *(_DWORD *)v5 = m_prevlink;
      m_next->m_link.m_prevlink->m_next = m_next->m_link.m_next;
      m_next->m_link.m_prevlink = nullptr;
      m_next->m_link.m_next = nullptr;
    }
    if ( m_next->m_link.m_prevlink != nullptr )
    {
      TSLink<CGxuLight>::NextLink(&m_next->m_link, -1)->m_prevlink = m_next->m_link.m_prevlink;
      m_next->m_link.m_prevlink->m_next = m_next->m_link.m_next;
      m_next->m_link.m_prevlink = nullptr;
      m_next->m_link.m_next = nullptr;
    }
    v6 = CGxuLight::s_lights.m_terminator.m_prevlink;
    m_next->m_link.m_prevlink = CGxuLight::s_lights.m_terminator.m_prevlink;
    m_next->m_link.m_next = v6->m_next;
    v6->m_next = m_next;
    CGxuLight::s_lights.m_terminator.m_prevlink = &m_next->m_link;
    if ( m_next != (CGxuLight *)-8 )
      CGxLight::CGxLight(&m_next->m_light);
  }
  else
  {
    v1 = (CGxuLight *)SMemAlloc(0x5Cu, aAucgxulight, -2, 8u);
    m_next = v1;
    if ( v1 != nullptr )
    {
      v1->m_link.m_prevlink = nullptr;
      v1->m_link.m_next = nullptr;
      CGxLight::CGxLight(&v1->m_light);
      m_next->m_lockCount = 0;
      TSExplicitList<CGxuLightLink,12>::TSExplicitList<CGxuLightLink,12>(&m_next->m_links);
    }
    else
    {
      m_next = nullptr;
    }
    TSList<CGxuLight,TSGetLink<CGxuLight>>::LinkNode(&CGxuLight::s_lights, m_next, 2u, nullptr);
    if ( m_next == nullptr )
    {
      SErrDisplayError(0x85100000, aDBuildBuildwow_243, 132, (const char *)aLight, 0, 1u);
      return nullptr;
    }
  }
  return m_next;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7A60  EA 0x5B7A60
// DIA: IGxuLightDestroy
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall IGxuLightDestroy(TSLink<CGxuLight> *this)
{
  TSLink<CGxuLight> *m_prevlink; // edx
  int m_next; // ecx
  int v5; // ecx
  CGxuLightLink *i; // ebx
  TSLink<CGxuLightLink> **p_m_prevlink; // esi
  TSLink<CGxuLightLink> **v8; // esi
  CLightList *m_list; // esi
  TSLink<CGxuLight> *v10; // eax
  CGxuLightLink *v11; // [esp+4h] [ebp-4h]

  if ( this == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_243, 147, (const char *)aLight, 0, 1u);
  s_forceSettingLights = 1;
  m_prevlink = this->m_prevlink;
  if ( this->m_prevlink != nullptr )
  {
    m_next = (int)this->m_next;
    if ( m_next > 0 )
      v5 = (char *)this - (char *)m_prevlink->m_next + m_next;
    else
      v5 = ~m_next;
    *(_DWORD *)v5 = m_prevlink;
    this->m_prevlink->m_next = this->m_next;
    this->m_prevlink = nullptr;
    this->m_next = nullptr;
  }
  for ( i = (int)this[11].m_prevlink <= 0 ? nullptr : (CGxuLightLink *)this[11].m_prevlink; (int)i > 0; i = v11 )
  {
    p_m_prevlink = &i->m_lightLink.m_prevlink;
    v11 = *(CGxuLightLink **)((char *)&this[10].m_prevlink->m_next + (unsigned int)i);
    if ( i->m_lightLink.m_prevlink != nullptr )
    {
      TSLink<CGxuLightLink>::NextLink(&i->m_lightLink, -1)->m_prevlink = *p_m_prevlink;
      (*p_m_prevlink)->m_next = i->m_lightLink.m_next;
      *p_m_prevlink = nullptr;
      i->m_lightLink.m_next = nullptr;
    }
    v8 = &i->m_listLink.m_prevlink;
    if ( i->m_listLink.m_prevlink != nullptr )
    {
      TSLink<CGxuLightLink>::NextLink(&i->m_listLink, -1)->m_prevlink = *v8;
      (*v8)->m_next = i->m_listLink.m_next;
      *v8 = nullptr;
      i->m_listLink.m_next = nullptr;
    }
    if ( ((int)this[1].m_prevlink & 2) != 0 )
    {
      m_list = i->m_list;
      if ( (int)m_list->m_links.m_terminator.m_next > 0 )
      {
        TSHashTable<CLightList,HASHKEY_DWORD>::Unlink(
          &CLightList::s_lightHashTable.TSHashTable<CLightList,HASHKEY_DWORD>,
          i->m_list);
        ((void (__thiscall *)(TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *, CLightList *))CLightList::s_lightHashTable.TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::Destroy)(
          &CLightList::s_lightHashTable,
          m_list);
      }
    }
    TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::LinkNode(&CGxuLight::s_linksFreeList, i, 2u, nullptr);
  }
  if ( this->m_prevlink != nullptr )
  {
    TSLink<CGxuLight>::NextLink(this, -1)->m_prevlink = this->m_prevlink;
    this->m_prevlink->m_next = this->m_next;
    this->m_prevlink = nullptr;
    this->m_next = nullptr;
  }
  v10 = CGxuLight::s_lightsFreeList.m_terminator.m_prevlink;
  this->m_prevlink = CGxuLight::s_lightsFreeList.m_terminator.m_prevlink;
  this->m_next = v10->m_next;
  v10->m_next = (CGxuLight *)this;
  CGxuLight::s_lightsFreeList.m_terminator.m_prevlink = this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B7BC0  EA 0x5B7BC0
// DIA: IGxuLightLock
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
CGxLight *__thiscall IGxuLightLock(int this)
{
  if ( this == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_243, 164, (const char *)aLight, 0, 1u);
  ++*(_DWORD *)(this + 76);
  return (CGxLight *)(this + 8);
}


// -----------------------------------------------------------------------------
// RVA 0x001B7BF0  EA 0x5B7BF0
// DIA: IGxuLightUnlock
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall IGxuLightUnlock(CGxuLight *this)
{
  CGxuLight *v2; // esi
  CGxuLightLink *i; // edi
  CLightList *m_list; // ebx
  int m_light; // eax
  CGxuLightLink *v6; // edi
  TSLink<CGxuLightLink> *p_m_terminator; // ebx
  TSLink<CGxuLightLink> *v8; // eax
  int j; // ecx
  int v10; // ecx
  unsigned int v11; // ebx
  unsigned int v12; // edi
  CLightList *m_next; // edi
  unsigned int v14; // esi
  CLightList *v15; // eax
  float v16; // ecx
  float m_quadraticAttenuation; // edx
  NTempest::C3Vector *v18; // eax
  long double v19; // st7
  long double v20; // st6
  CGxuLightLink *v21; // esi
  CGxuLight *v22; // ecx
  const CGxuLightLink *v23; // ebx
  TSLink<CGxuLightLink> *v24; // eax
  TSLink<CGxuLightLink> *v25; // edi
  TSLink<CGxuLightLink> *v26; // eax
  TSLink<CGxuLightLink> *m_prevlink; // ecx
  float v28; // [esp+0h] [ebp-74h]
  float v29; // [esp+0h] [ebp-74h]
  float v30; // [esp+4h] [ebp-70h]
  float v31; // [esp+4h] [ebp-70h]
  float z; // [esp+8h] [ebp-6Ch]
  float za; // [esp+8h] [ebp-6Ch]
  NTempest::C3Vector l; // [esp+18h] [ebp-5Ch] BYREF
  NTempest::C3Vector pos; // [esp+24h] [ebp-50h] BYREF
  NTempest::C3Vector max; // [esp+30h] [ebp-44h] BYREF
  NTempest::C3Vector min; // [esp+3Ch] [ebp-38h] BYREF
  float m_linearAttenuation; // [esp+48h] [ebp-2Ch]
  float v39; // [esp+4Ch] [ebp-28h]
  float v40; // [esp+50h] [ebp-24h]
  int v41; // [esp+54h] [ebp-20h]
  int v42; // [esp+58h] [ebp-1Ch]
  float v43; // [esp+5Ch] [ebp-18h]
  int v44; // [esp+60h] [ebp-14h]
  float fitness; // [esp+64h] [ebp-10h]
  int y; // [esp+68h] [ebp-Ch]
  CGxuLight *v47; // [esp+6Ch] [ebp-8h]
  int x; // [esp+70h] [ebp-4h]

  v2 = this;
  v47 = this;
  if ( this == nullptr )
    SErrDisplayError(0x85100000, aDBuildBuildwow_243, 173, (const char *)aLight, 0, 1u);
  if ( v2->m_lockCount == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_243, 174, aLightMLockcoun, 0, 1u);
  s_forceSettingLights = 1;
  --v2->m_lockCount;
  for ( i = (int)v2->m_links.m_terminator.m_next <= 0 ? nullptr : v2->m_links.m_terminator.m_next;
        (int)i > 0;
        i = (CGxuLightLink *)LODWORD(v43) )
  {
    v43 = *(float *)&TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::Link(&v2->m_links, i)->m_next;
    TSLink<CGxuLightLink>::Unlink(&i->m_lightLink);
    TSLink<CGxuLightLink>::Unlink(&i->m_listLink);
    if ( (*(_BYTE *)&v2->m_light & 2) != 0 )
    {
      m_list = i->m_list;
      if ( (int)m_list->m_links.m_terminator.m_next > 0 )
      {
        TSHashTable<CLightList,HASHKEY_DWORD>::Unlink(
          &CLightList::s_lightHashTable.TSHashTable<CLightList,HASHKEY_DWORD>,
          i->m_list);
        ((void (__thiscall *)(TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *, CLightList *))CLightList::s_lightHashTable.TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::Destroy)(
          &CLightList::s_lightHashTable,
          m_list);
      }
    }
    TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::LinkNode(&CGxuLight::s_linksFreeList, i, 2u, nullptr);
  }
  m_light = (int)v2->m_light;
  if ( (m_light & 1) != 0 )
  {
    if ( (m_light & 2) != 0 )
    {
      *(float *)&x = (v2->m_light.m_dirIntensity + v2->m_light.m_ambIntensity) * 20.0;
      if ( v2->m_light.m_linearAttenuation != 0.0 )
        *(float *)&x = *(float *)&x / v2->m_light.m_linearAttenuation;
      z = v2->m_light.m_dir.z - *(float *)&x;
      v30 = v2->m_light.m_dir.y - *(float *)&x;
      v28 = v2->m_light.m_dir.x - *(float *)&x;
      NTempest::C3Vector::C3Vector(&min, v28, v30, z);
      za = *(float *)&x + v2->m_light.m_dir.z;
      v31 = *(float *)&x + v2->m_light.m_dir.y;
      v29 = *(float *)&x + v2->m_light.m_dir.x;
      NTempest::C3Vector::C3Vector(&max, v29, v31, za);
      v43 = min.y / s_bucketSize;
      y = (int)(v43 - OneHalfOffset_0);
      v43 = min.x / s_bucketSize;
      v42 = (int)(v43 - OneHalfOffset_0);
      *(float *)&v44 = max.y / s_bucketSize;
      LODWORD(v43) = (int)(*(float *)&v44 - OneHalfOffset_0);
      fitness = max.x / s_bucketSize;
      v44 = (int)(fitness - OneHalfOffset_0);
      for ( j = y; j <= SLODWORD(v43); y = j )
      {
        x = v42;
        if ( v42 <= v44 )
        {
          v10 = j << 16;
          v41 = v10;
          v40 = (float)y;
          while ( 1 )
          {
            LODWORD(fitness) = v10 | (unsigned __int16)x;
            v11 = LODWORD(fitness) % 0x1FFF;
            if ( CLightList::s_lightHashTable.m_slotmask == -1
              || (v12 = v11 & CLightList::s_lightHashTable.m_slotmask,
                  TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(
                    &CLightList::s_lightHashTable.m_slotlistarray,
                    v11 & CLightList::s_lightHashTable.m_slotmask),
                  (int)(m_next = CLightList::s_lightHashTable.m_slotlistarray.m_data[v12].m_terminator.m_next) <= 0) )
            {
LABEL_32:
              v15 = TSHashTable<CLightList,HASHKEY_DWORD>::InternalNewNode(
                      &CLightList::s_lightHashTable.TSHashTable<CLightList,HASHKEY_DWORD>,
                      v11,
                      0,
                      0);
              v16 = fitness;
              v15->m_hashval = v11;
              *(float *)&v15->m_key.m_key = v16;
              m_next = v15;
            }
            else
            {
              while ( m_next->m_hashval != v11 || m_next->m_key.m_key != LODWORD(fitness) )
              {
                v14 = v11 & CLightList::s_lightHashTable.m_slotmask;
                TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(
                  &CLightList::s_lightHashTable.m_slotlistarray,
                  v11 & CLightList::s_lightHashTable.m_slotmask);
                m_next = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(
                           &CLightList::s_lightHashTable.m_slotlistarray.m_data[v14],
                           m_next)->m_next;
                v2 = v47;
                if ( (int)m_next <= 0 )
                  goto LABEL_32;
              }
            }
            m_quadraticAttenuation = v2->m_light.m_quadraticAttenuation;
            m_linearAttenuation = v2->m_light.m_linearAttenuation;
            v39 = m_quadraticAttenuation;
            pos.x = (double)x * s_bucketSize + s_halfBucket;
            pos.y = v40 * s_bucketSize + s_halfBucket;
            pos.z = min.z;
            v18 = NTempest::operator-(&l, &pos.x, &v2->m_light.m_dir);
            fitness = NTempest::C3Vector::SquaredMag(v18);
            v19 = NTempest::CMath::sqrt_(fitness);
            v20 = v2->m_light.m_dirIntensity / (v19 * v19 * v39 + v19 * m_linearAttenuation + 1.0)
                + v2->m_light.m_ambIntensity;
            v21 = CGxuLight::s_linksFreeList.m_terminator.m_next;
            fitness = v20;
            if ( (int)CGxuLight::s_linksFreeList.m_terminator.m_next <= 0 )
              v21 = TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::NewNode(
                      &CGxuLight::s_linksFreeList,
                      2u,
                      0,
                      0);
            TSLink<CGxuLightLink>::Unlink(&v21->m_lightLink);
            TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::LinkNode(&v47->m_links, v21, 2u, nullptr);
            v22 = v47;
            v21->m_fitness = fitness;
            v21->m_light = v22;
            v21->m_list = m_next;
            v23 = m_next->m_links.m_terminator.m_next;
            if ( (int)v23 <= 0 )
            {
LABEL_38:
              TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::LinkNode(&m_next->m_links, v21, 2u, nullptr);
            }
            else
            {
              while ( fitness < (double)v23->m_fitness )
              {
                v23 = v23->m_listLink.m_next;
                if ( (int)v23 <= 0 )
                  goto LABEL_38;
              }
              LODWORD(fitness) = &m_next->m_links;
              v24 = TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::Link(&m_next->m_links, v21);
              v25 = v24;
              if ( v24->m_prevlink != nullptr )
                TSLink<CGxuLightLink>::Unlink(v24);
              v26 = TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::Link(
                      (TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> > *)LODWORD(fitness),
                      v23);
              m_prevlink = v26->m_prevlink;
              v25->m_prevlink = v26->m_prevlink;
              v25->m_next = m_prevlink->m_next;
              m_prevlink->m_next = v21;
              v26->m_prevlink = v25;
            }
            v2 = v47;
            if ( ++x > v44 )
              break;
            v10 = v41;
          }
          j = y;
        }
        ++j;
      }
    }
    else
    {
      v6 = CGxuLight::s_linksFreeList.m_terminator.m_next;
      if ( (int)CGxuLight::s_linksFreeList.m_terminator.m_next <= 0 )
        v6 = TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::NewNode(&CGxuLight::s_linksFreeList, 2u, 0, 0);
      TSLink<CGxuLightLink>::Unlink(&v6->m_lightLink);
      TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::LinkNode(&v2->m_links, v6, 2u, nullptr);
      if ( v6 != nullptr )
        p_m_terminator = (TSLink<CGxuLightLink> *)((char *)v6 + s_dirLightList.m_links.m_linkoffset);
      else
        p_m_terminator = &s_dirLightList.m_links.m_terminator;
      if ( p_m_terminator->m_prevlink != nullptr )
        TSLink<CGxuLightLink>::Unlink(p_m_terminator);
      v8 = s_dirLightList.m_links.m_terminator.m_prevlink;
      p_m_terminator->m_prevlink = s_dirLightList.m_links.m_terminator.m_prevlink;
      p_m_terminator->m_next = v8->m_next;
      v8->m_next = v6;
      s_dirLightList.m_links.m_terminator.m_prevlink = p_m_terminator;
      v6->m_light = v2;
      v6->m_list = &s_dirLightList;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B8040  EA 0x5B8040
// DIA: IGxuLightSelect
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall IGxuLightSelect(const NTempest::C3Vector *maxLightsToUse, NTempest::C3Vector worldPos)
{
  ACTIVITY v3; // edx
  unsigned int v4; // esi
  CLightList *m_next; // edi
  unsigned int v6; // esi
  CLightList *v7; // ebx
  const CGxLight *v8; // esi
  CGxuLightLink *i; // edi
  int v10; // edi
  bool v11; // zf
  int v12; // [esp+Ch] [ebp-Ch]
  float v13; // [esp+10h] [ebp-8h]
  int v14; // [esp+10h] [ebp-8h]
  float activitya; // [esp+28h] [ebp+10h]
  ACTIVITY activity; // [esp+28h] [ebp+10h]

  if ( (int)CGxuLight::s_lights.m_terminator.m_next > 0 )
  {
    ActivityBegin(ACTIVITY_LIGHTING);
    v13 = worldPos.y / s_bucketSize;
    v12 = (int)(v13 - OneHalfOffset_0);
    activitya = worldPos.x / s_bucketSize;
    v14 = (int)(activitya - OneHalfOffset_0);
    v3 = ((unsigned int)(v12 << 16) | (unsigned __int64)(unsigned __int16)v14) % 0x1FFF;
    activity = v3;
    if ( CLightList::s_lightHashTable.m_slotmask == -1
      || (v4 = v3 & CLightList::s_lightHashTable.m_slotmask,
          TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(
            &CLightList::s_lightHashTable.m_slotlistarray,
            v3 & CLightList::s_lightHashTable.m_slotmask),
          (int)(m_next = (int)CLightList::s_lightHashTable.m_slotlistarray.m_data[v4].m_terminator.m_next <= 0
                       ? nullptr
                       : CLightList::s_lightHashTable.m_slotlistarray.m_data[v4].m_terminator.m_next) <= 0) )
    {
LABEL_7:
      v7 = nullptr;
    }
    else
    {
      while ( m_next->m_hashval != activity || m_next->m_key.m_key != ((v12 << 16) | (unsigned __int16)v14) )
      {
        v6 = activity & CLightList::s_lightHashTable.m_slotmask;
        TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(
          &CLightList::s_lightHashTable.m_slotlistarray,
          activity & CLightList::s_lightHashTable.m_slotmask);
        m_next = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(
                   &CLightList::s_lightHashTable.m_slotlistarray.m_data[v6],
                   m_next)->m_next;
        if ( (int)m_next <= 0 )
          goto LABEL_7;
      }
      v7 = m_next;
    }
    ++s_selectionCount;
    v8 = nullptr;
    for ( i = (int)s_dirLightList.m_links.m_terminator.m_next <= 0
            ? nullptr
            : s_dirLightList.m_links.m_terminator.m_next; (int)i > 0; v8 = (const CGxLight *)((char *)v8 + 1) )
    {
      GxLightSet(v8, &i->m_light->m_light, *maxLightsToUse);
      i->m_light->m_selectionCount = s_selectionCount;
      i->m_light->m_hwLight = (unsigned int)v8;
      i = *(CGxuLightLink **)((char *)&i->m_list + s_dirLightList.m_links.m_linkoffset);
    }
    if ( v7 != nullptr
      && (v10 = (int)v7->m_links.m_terminator.m_next <= 0 ? 0 : (int)v7->m_links.m_terminator.m_next) > 0 )
    {
      while ( 1 )
      {
        v11 = v8 == (const CGxLight *)8;
        if ( (unsigned int)v8 >= 8 )
          break;
        GxLightSet(v8, (const CGxLight *)(*(_DWORD *)v10 + 8), *maxLightsToUse);
        *(_DWORD *)(*(_DWORD *)v10 + 72) = s_selectionCount;
        *(_DWORD *)(*(_DWORD *)v10 + 68) = v8;
        v10 = *(_DWORD *)(v10 + v7->m_links.m_linkoffset + 4);
        v8 = (const CGxLight *)((char *)v8 + 1);
        if ( v10 <= 0 )
          goto LABEL_14;
      }
    }
    else
    {
LABEL_14:
      v11 = v8 == (const CGxLight *)8;
    }
    if ( !v11 )
    {
      do
      {
        GxLightEnable((unsigned int)v8, 0);
        v8 = (const CGxLight *)((char *)v8 + 1);
      }
      while ( v8 != (const CGxLight *)8 );
    }
    s_forceSettingLights = 0;
    s_lastLightsHash = ((unsigned int)(v12 << 16) | (unsigned __int64)(unsigned __int16)v14) % 0x1FFF;
    ActivityEnd(12);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B8240  EA 0x5B8240
// DIA: IGxuLightEnableSet
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall IGxuLightEnableSet(unsigned int enable, unsigned __int8 a2)
{
  CGxLight *v5; // eax

  if ( enable == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_243, 428, (const char *)aLight, 0, 1u);
  v5 = GxuLightLock(enable);
  *(_DWORD *)v5 ^= (a2 ^ (unsigned __int8)*(_DWORD *)v5) & 1;
  GxuLightUnlock(enable);
}


// -----------------------------------------------------------------------------
// RVA 0x001B8290  EA 0x5B8290
// DIA: IGxuLightEnable
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall IGxuLightEnable(unsigned int this)
{
  int v3; // esi

  if ( this == 0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_243, 440, (const char *)aLight, 0, 1u);
  v3 = (int)(*(_DWORD *)GxuLightLock(this) << 31) >> 31;
  GxuLightUnlock(this);
  return v3;
}


// -----------------------------------------------------------------------------
// RVA 0x001B82E0  EA 0x5B82E0
// DIA: IGxuLightSetMaxLights
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall IGxuLightSetMaxLights(unsigned int a1)
{
  s_maxLightsToUse = a1;
  if ( a1 >= 8 )
    s_maxLightsToUse = 8;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8300  EA 0x5B8300
// DIA: IGxuLightBucketSize
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __fastcall IGxuLightBucketSize()
{
  return s_bucketSize;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8310  EA 0x5B8310
// DIA: IGxuLightBucketSizeSet
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall IGxuLightBucketSizeSet(float bucketSize)
{
  if ( bucketSize <= 0.0 )
    SErrDisplayError(0x85100000, aDBuildBuildwow_243, 464, aBucketsize00f, 0, 1u);
  s_bucketSize = bucketSize;
  s_halfBucket = bucketSize * 0.5;
  GxuLightResetCache();
}


// -----------------------------------------------------------------------------
// RVA 0x001B8370  EA 0x5B8370
// DIA: IGxuLightResetCache
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void IGxuLightResetCache()
{
  s_forceSettingLights = 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8380  EA 0x5B8380
// DIA: TSLink<CGxuLightLink>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<CGxuLightLink>::Unlink(TSLink<CGxuLightLink> *this)
{
  TSLink<CGxuLightLink> *m_prevlink; // esi
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
// RVA 0x001B83C0  EA 0x5B83C0
// DIA: TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> >::~TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> >
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::~TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>(
        TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> > *this)
{
  TSLink<CGxuLightLink> *p_m_terminator; // ebx
  CGxuLightLink *m_next; // edx
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // esi
  int v6; // esi
  TSLink<CGxuLightLink> *m_prevlink; // edx
  int v8; // ecx
  int v9; // ecx

  p_m_terminator = &this->m_terminator;
  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v3 = *(int *)((char *)&m_next->m_light + this->m_linkoffset);
    v4 = (CGxuLight **)((char *)&m_next->m_light + this->m_linkoffset);
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
// RVA 0x001B8440  EA 0x5B8440
// DIA: TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::LinkNode(
        TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> > *this,
        CGxuLightLink *ptr,
        unsigned int linktype,
        CGxuLightLink *existingptr)
{
  TSLink<CGxuLightLink> *p_m_terminator; // eax
  TSLink<CGxuLightLink> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<CGxuLightLink> *v8; // edx
  TSLink<CGxuLightLink> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<CGxuLightLink> *)((char *)ptr + this->m_linkoffset);
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
    v8 = (TSLink<CGxuLightLink> *)((char *)existingptr + this->m_linkoffset);
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
// RVA 0x001B8510  EA 0x5B8510
// DIA: TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> >::NewNode
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CGxuLightLink *__thiscall TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::NewNode(
        TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> > *this,
        unsigned int location,
        unsigned int extrabytes,
        unsigned int flags)
{
  CGxuLightLink *result; // eax
  TSLink<CGxuLightLink> *p_m_terminator; // ecx
  TSLink<CGxuLightLink> *m_prevlink; // edi
  int m_next; // esi
  int v9; // esi
  TSLink<CGxuLightLink> *v10; // edx
  TSLink<CGxuLightLink> *v11; // esi
  int v12; // esi
  int m_linkoffset; // edi

  result = (CGxuLightLink *)SMemAlloc(extrabytes + 28, aAvcgxulightlin, -2, flags | 8);
  if ( result != nullptr )
  {
    result->m_lightLink.m_prevlink = nullptr;
    result->m_lightLink.m_next = nullptr;
    result->m_listLink.m_prevlink = nullptr;
    result->m_listLink.m_next = nullptr;
  }
  else
  {
    result = nullptr;
  }
  if ( location != 0 )
  {
    if ( result != nullptr )
      p_m_terminator = (TSLink<CGxuLightLink> *)((char *)result + this->m_linkoffset);
    else
      p_m_terminator = &this->m_terminator;
    m_prevlink = p_m_terminator->m_prevlink;
    if ( p_m_terminator->m_prevlink != nullptr )
    {
      m_next = (int)p_m_terminator->m_next;
      if ( m_next > 0 )
        v9 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
      else
        v9 = ~m_next;
      *(_DWORD *)v9 = m_prevlink;
      p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
      p_m_terminator->m_prevlink = nullptr;
      p_m_terminator->m_next = nullptr;
    }
    v10 = &this->m_terminator;
    if ( location == 1 )
    {
      p_m_terminator->m_prevlink = v10;
      p_m_terminator->m_next = this->m_terminator.m_next;
      v12 = (int)this->m_terminator.m_next;
      m_linkoffset = this->m_linkoffset;
      if ( v12 > 0 )
      {
        if ( m_linkoffset < 0 )
          m_linkoffset = (char *)v10 - (char *)v10->m_prevlink->m_next;
        *(_DWORD *)(m_linkoffset + v12) = p_m_terminator;
        this->m_terminator.m_next = result;
      }
      else
      {
        *(_DWORD *)~v12 = p_m_terminator;
        this->m_terminator.m_next = result;
      }
    }
    else
    {
      if ( location != 2 )
      {
        SErrPrepareAppFatal(aStormHStplH, 2061);
        SErrDisplayAppFatal("Invalid case: %s=%u", aLinktype, location);
      }
      v11 = v10->m_prevlink;
      p_m_terminator->m_prevlink = v10->m_prevlink;
      p_m_terminator->m_next = v11->m_next;
      v11->m_next = result;
      v10->m_prevlink = p_m_terminator;
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8610  EA 0x5B8610
// DIA: TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::InternalDelete
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::InternalDelete(
        TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *this,
        CLightList *ptr)
{
  TSLink<CLightList> *p_m_terminator; // eax
  TSLink<CLightList> *m_prevlink; // esi
  int m_next; // edx
  int v6; // edx
  TSLink<CLightList> *v7; // esi

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<CLightList> *)((char *)ptr + this->m_fulllist.m_linkoffset);
  else
    p_m_terminator = &this->m_fulllist.m_terminator;
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next > 0 )
      v6 = (char *)p_m_terminator - (char *)m_prevlink->m_next + m_next;
    else
      v6 = ~m_next;
    *(_DWORD *)v6 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  if ( ptr != nullptr )
    v7 = (TSLink<CLightList> *)((char *)ptr + this->m_reuseList.m_linkoffset);
  else
    v7 = &this->m_reuseList.m_terminator;
  if ( v7->m_prevlink != nullptr )
  {
    TSLink<CLightList>::NextLink(v7, -1)->m_prevlink = v7->m_prevlink;
    v7->m_prevlink->m_next = v7->m_next;
    v7->m_prevlink = nullptr;
    v7->m_next = nullptr;
  }
  v7->m_prevlink = &this->m_reuseList.m_terminator;
  v7->m_next = this->m_reuseList.m_terminator.m_next;
  TSLink<CLightList>::NextLink(&this->m_reuseList.m_terminator, this->m_reuseList.m_linkoffset)->m_prevlink = v7;
  this->m_reuseList.m_terminator.m_next = ptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001B86C0  EA 0x5B86C0
// DIA: TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::InternalNew
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CLightList *__thiscall TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::InternalNew(
        TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *this,
        TSExplicitList<CLightList,-572662307> *listptr,
        unsigned int extrabytes,
        unsigned int flags)
{
  CLightList *m_next; // ebx
  TSHashObjectChunk<CLightList,HASHKEY_DWORD> *v6; // esi
  TSGrowableArray<CLightList> *v7; // eax
  TSHashObjectChunk<CLightList,HASHKEY_DWORD> *v8; // esi
  unsigned int m_chunkSize; // edx
  unsigned int v10; // ebx
  unsigned int m_chunk; // eax
  unsigned int v12; // eax
  unsigned int m_count; // eax
  CLightList *v14; // ecx
  TSLink<CLightList> *p_m_terminator; // esi

  if ( extrabytes != 0 )
    SErrDisplayError(0x85100000, aStormHStplH, 3229, aExtrabytes, 0, 1u);
  m_next = this->m_reuseList.m_terminator.m_next;
  if ( (int)m_next > 0 )
    goto LABEL_20;
  while ( 1 )
  {
    v6 = this->m_chunkList.m_terminator.m_next;
    if ( (int)v6 > 0 )
    {
      if ( v6->m_array.m_alloc < v6->m_array.m_count )
        SErrDisplayError(0x85100000, aStormHStplH, 1018, aMAllocMCount, 0, 1u);
      if ( v6->m_array.m_alloc != v6->m_array.m_count )
        break;
    }
    v7 = (TSGrowableArray<CLightList> *)SMemAlloc(0x1Cu, aAvTshashobject_7, -2, 8u);
    v8 = (TSHashObjectChunk<CLightList,HASHKEY_DWORD> *)v7;
    if ( v7 != nullptr )
    {
      TSGrowableArray<CLightList>::TSGrowableArray<CLightList>(v7);
      v8->m_link.m_prevlink = nullptr;
      v8->m_link.m_next = nullptr;
    }
    else
    {
      v8 = nullptr;
    }
    TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>>::LinkNode(
      &this->m_chunkList,
      v8,
      1u,
      nullptr);
    m_chunkSize = this->m_chunkSize;
    if ( m_chunkSize + v8->m_array.m_count > v8->m_array.m_alloc )
      TSFixedArray<CLightList>::ReallocData(&v8->m_array, m_chunkSize + v8->m_array.m_count);
    this->m_chunkSize *= 2;
  }
  v10 = v6->m_array.m_count + 1;
  if ( v10 > v6->m_array.m_alloc )
  {
    m_chunk = v6->m_array.m_chunk;
    if ( m_chunk == 0 )
      m_chunk = TSGrowableArray<CLightList>::CalcChunkSize(&v6->m_array, v6->m_array.m_count + 1);
    v12 = TSGrowableArray<CLightList>::RoundToChunk(&v6->m_array, v10, m_chunk);
    TSFixedArray<CLightList>::ReallocData(&v6->m_array, v12);
  }
  m_count = v6->m_array.m_count;
  v14 = &v6->m_array.m_data[m_count];
  v6->m_array.m_count = m_count + 1;
  if ( v14 != nullptr )
  {
    m_next = CLightList::CLightList(v14);
    if ( m_next != nullptr )
    {
LABEL_20:
      p_m_terminator = (TSLink<CLightList> *)((char *)m_next + listptr->m_linkoffset);
      goto LABEL_23;
    }
  }
  else
  {
    m_next = nullptr;
  }
  p_m_terminator = &listptr->m_terminator;
LABEL_23:
  if ( p_m_terminator->m_prevlink != nullptr )
    TSLink<CLightList>::Unlink(p_m_terminator);
  p_m_terminator->m_prevlink = &listptr->m_terminator;
  p_m_terminator->m_next = listptr->m_terminator.m_next;
  TSLink<CLightList>::NextLink(&listptr->m_terminator, listptr->m_linkoffset)->m_prevlink = p_m_terminator;
  listptr->m_terminator.m_next = m_next;
  return m_next;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8820  EA 0x5B8820
// DIA: CLightList::CLightList
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CLightList *__thiscall CLightList::CLightList(CLightList *this)
{
  CLightList *result; // eax
  TSLink<CGxuLightLink> *p_m_terminator; // ecx

  result = this;
  this->m_linktoslot.m_prevlink = nullptr;
  this->m_linktoslot.m_next = nullptr;
  this->m_linktofull.m_prevlink = nullptr;
  this->m_linktofull.m_next = nullptr;
  this->m_key.m_key = 0;
  p_m_terminator = &this->m_links.m_terminator;
  p_m_terminator->m_prevlink = nullptr;
  p_m_terminator->m_next = nullptr;
  p_m_terminator->m_prevlink = p_m_terminator;
  result->m_links.m_linkoffset = 20;
  result->m_links.m_terminator.m_next = (CGxuLightLink *)~(unsigned int)p_m_terminator;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8850  EA 0x5B8850
// DIA: TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::Destroy(
        TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *this)
{
  CLightList *m_next; // eax
  unsigned int v3; // esi
  int v4; // ebx
  TSHashObjectChunk<CLightList,HASHKEY_DWORD> *v5; // esi
  CLightList *v6; // esi

  this->m_fullnessIndicator = 0;
  while ( 1 )
  {
    m_next = this->m_fulllist.m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    TSLink<CLightList>::Unlink((TSLink<CLightList> *)((char *)m_next + this->m_fulllist.m_linkoffset));
  }
  v3 = 0;
  if ( this->m_slotlistarray.m_count != 0 )
  {
    v4 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v3);
        if ( (int)this->m_slotlistarray.m_data[v4].m_terminator.m_next <= 0 )
          break;
        ((void (__thiscall *)(TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *, CLightList *))this->TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::Destroy)(
          this,
          this->m_slotlistarray.m_data[v4].m_terminator.m_next);
      }
      ++v3;
      ++v4;
    }
    while ( v3 < this->m_slotlistarray.m_count );
  }
  while ( 1 )
  {
    v5 = this->m_chunkList.m_terminator.m_next;
    if ( (int)v5 <= 0 )
      break;
    TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>::Unlink(&v5->m_link);
    TSFixedArray<CLightList>::~TSFixedArray<CLightList>(&v5->m_array);
    SMemFree(v5, aAvTshashobject_7, -2, 0);
  }
  while ( 1 )
  {
    v6 = this->m_reuseList.m_terminator.m_next;
    if ( (int)v6 <= 0 )
      break;
    TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::~TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>(&v6->m_links);
    TSHashObject<CLightList,HASHKEY_DWORD>::~TSHashObject<CLightList,HASHKEY_DWORD>(v6);
    SMemFree(v6, aAuclightlist, -2, 0);
  }
  this->m_chunkSize = 16;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8910  EA 0x5B8910
// DIA: TSHashObject<CLightList,HASHKEY_DWORD>::~TSHashObject<CLightList,HASHKEY_DWORD>
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashObject<CLightList,HASHKEY_DWORD>::~TSHashObject<CLightList,HASHKEY_DWORD>(
        TSHashObject<CLightList,HASHKEY_DWORD> *this)
{
  TSLink<CLightList> *p_m_linktofull; // eax
  TSLink<CLightList> *m_prevlink; // edi
  int m_next; // esi
  int v4; // esi
  TSLink<CLightList> *v5; // esi
  TSLink<CLightList> *p_m_linktoslot; // eax
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
// RVA 0x001B8980  EA 0x5B8980
// DIA: TSHashTable<CLightList,HASHKEY_DWORD>::InternalDelete
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CLightList,HASHKEY_DWORD>::InternalDelete(
        TSHashTable<CLightList,HASHKEY_DWORD> *this,
        CLightList *ptr)
{
  TSLink<CGxuLightLink> *v2; // eax

  while ( (int)ptr->m_links.m_terminator.m_next > 0 )
  {
    v2 = TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::Link(&ptr->m_links, ptr->m_links.m_terminator.m_next);
    TSLink<CGxuLightLink>::Unlink(v2);
  }
  TSLink<CGxuLightLink>::Unlink(&ptr->m_links.m_terminator);
  TSLink<CLightList>::Unlink(&ptr->m_linktofull);
  TSLink<CLightList>::Unlink(&ptr->m_linktoslot);
  SMemFree(ptr, aAuclightlist, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001B89E0  EA 0x5B89E0
// DIA: TSHashTable<CLightList,HASHKEY_DWORD>::InternalNew
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CLightList *__thiscall TSHashTable<CLightList,HASHKEY_DWORD>::InternalNew(
        TSHashTable<CLightList,HASHKEY_DWORD> *this,
        TSExplicitList<CLightList,-572662307> *listptr,
        unsigned int extrabytes,
        unsigned int flags)
{
  CLightList *v4; // eax
  TSLink<CLightList> *p_m_terminator; // esi
  int m_next; // ecx
  int m_linkoffset; // edx
  CLightList *flagsa; // [esp+1Ch] [ebp+10h]

  v4 = (CLightList *)SMemAlloc(extrabytes + 36, aAuclightlist, -2, flags | 8);
  if ( v4 != nullptr )
  {
    v4->m_linktoslot.m_prevlink = nullptr;
    v4->m_linktoslot.m_next = nullptr;
    v4->m_linktofull.m_prevlink = nullptr;
    v4->m_linktofull.m_next = nullptr;
    v4->m_key.m_key = 0;
    v4->m_links.m_terminator.m_prevlink = nullptr;
    v4->m_links.m_terminator.m_next = nullptr;
    v4->m_links.m_terminator.m_prevlink = &v4->m_links.m_terminator;
    v4->m_links.m_linkoffset = 20;
    v4->m_links.m_terminator.m_next = (CGxuLightLink *)~(unsigned int)&v4->m_links.m_terminator;
    flagsa = v4;
    p_m_terminator = (TSLink<CLightList> *)((char *)v4 + listptr->m_linkoffset);
  }
  else
  {
    flagsa = nullptr;
    p_m_terminator = &listptr->m_terminator;
  }
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    TSLink<CLightList>::NextLink(p_m_terminator, -1)->m_prevlink = p_m_terminator->m_prevlink;
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
// RVA 0x001B8AB0  EA 0x5B8AB0
// DIA: TSHashTable<CLightList,HASHKEY_DWORD>::Destroy
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CLightList,HASHKEY_DWORD>::Destroy(TSHashTable<CLightList,HASHKEY_DWORD> *this)
{
  TSHashTable<CLightList,HASHKEY_DWORD> *v1; // edi
  int m_next; // eax
  unsigned int v3; // ebx
  _DWORD *v4; // esi
  int v5; // eax
  const char *v6; // eax
  int v7; // eax
  const char *v8; // eax
  int v9; // edi
  int v10; // eax
  TSLink<CLightList> *v11; // edi
  unsigned int m_count; // eax
  unsigned int v13; // esi
  int v14; // ebx
  TSExplicitList<CLightList,-572662307> *m_data; // eax
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
    TSLink<CLightList>::Unlink((TSLink<CLightList> *)(m_next + v1->m_fulllist.m_linkoffset));
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
        v11 = (TSLink<CLightList> *)(v24 + v9);
        if ( v10 != 0 )
        {
          TSLink<CLightList>::NextLink(v11, -1)->m_prevlink = v11->m_prevlink;
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
  v1->m_slotlistarray.__vftable = (TSGrowableArray<TSExplicitList<CLightList,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<CLightList,-572662307>>::`vftable';
  if ( m_count != 0 )
  {
    v14 = 0;
    do
    {
      TSList<CLightList,TSGetExplicitLink<CLightList>>::~TSList<CLightList,TSGetExplicitLink<CLightList>>(&v1->m_slotlistarray.m_data[v14]);
      ++v13;
      ++v14;
    }
    while ( v13 < v1->m_slotlistarray.m_count );
  }
  m_data = v1->m_slotlistarray.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_52, -2, 0);
  v1->m_slotlistarray.m_alloc = 0;
  v1->m_slotlistarray.m_count = 0;
  v1->m_slotlistarray.m_data = nullptr;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8C20  EA 0x5B8C20
// DIA: TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<TSExplicitList<CLightList,-572662307>>::MemFileName(
        TSBaseArray<TSExplicitList<CLightList,-572662307> > *this)
{
  return aAvTsexplicitli_52;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8C30  EA 0x5B8C30
// DIA: TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<TSExplicitList<CLightList,-572662307>>::MemLineNo(
        TSBaseArray<TSExplicitList<CLightList,-572662307> > *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8C40  EA 0x5B8C40
// DIA: TSLinkedNode<CGxuLight>::~TSLinkedNode<CGxuLight>
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLinkedNode<CGxuLight>::~TSLinkedNode<CGxuLight>(TSLinkedNode<CGxuLight> *this)
{
  TSLink<CGxuLight> *m_prevlink; // esi
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
// RVA 0x001B8C80  EA 0x5B8C80
// DIA: TSExplicitList<CGxuLightLink,12>::TSExplicitList<CGxuLightLink,12>
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSExplicitList<CGxuLightLink,12> *__thiscall TSExplicitList<CGxuLightLink,12>::TSExplicitList<CGxuLightLink,12>(
        TSExplicitList<CGxuLightLink,12> *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 12;
  this->m_terminator.m_next = (CGxuLightLink *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8CA0  EA 0x5B8CA0
// DIA: TSList<CGxuLight,TSGetLink<CGxuLight> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CGxuLight,TSGetLink<CGxuLight>>::LinkNode(
        TSList<CGxuLight,TSGetLink<CGxuLight> > *this,
        CGxuLight *ptr,
        unsigned int linktype,
        CGxuLight *existingptr)
{
  CGxuLight *p_m_terminator; // eax
  TSLink<CGxuLight> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  CGxuLight *v8; // edx
  TSLink<CGxuLight> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  p_m_terminator = ptr;
  if ( ptr == nullptr )
    p_m_terminator = (CGxuLight *)&this->m_terminator;
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
    v8 = (CGxuLight *)&this->m_terminator;
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
// RVA 0x001B8D70  EA 0x5B8D70
// DIA: TSBaseArray<CGxuLight *>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<CGxuLight *>::MemFileName(TSBaseArray<CGxuLight *> *this)
{
  return aPaucgxulight;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8D80  EA 0x5B8D80
// DIA: TSBaseArray<CGxuLight *>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<CGxuLight *>::MemLineNo(TSBaseArray<CGxuLight *> *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8D90  EA 0x5B8D90
// DIA: CLightList::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
CLightList *__thiscall CLightList::`scalar deleting destructor'(CLightList *this, char __flags)
{
  TSLink<CGxuLightLink> *p_m_terminator; // esi
  int m_next; // eax
  TSLink<CLightList> *p_m_linktofull; // esi
  TSLink<CLightList> *p_m_linktoslot; // esi

  p_m_terminator = &this->m_links.m_terminator;
  while ( 1 )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next <= 0 )
      break;
    TSLink<CGxuLightLink>::Unlink((TSLink<CGxuLightLink> *)(m_next + this->m_links.m_linkoffset));
  }
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    TSLink<CGxuLightLink>::NextLink(p_m_terminator, -1)->m_prevlink = p_m_terminator->m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  p_m_linktofull = &this->m_linktofull;
  if ( this->m_linktofull.m_prevlink != nullptr )
  {
    TSLink<CLightList>::NextLink(&this->m_linktofull, -1)->m_prevlink = p_m_linktofull->m_prevlink;
    p_m_linktofull->m_prevlink->m_next = this->m_linktofull.m_next;
    p_m_linktofull->m_prevlink = nullptr;
    this->m_linktofull.m_next = nullptr;
  }
  p_m_linktoslot = &this->m_linktoslot;
  if ( this->m_linktoslot.m_prevlink != nullptr )
  {
    TSLink<CLightList>::NextLink(&this->m_linktoslot, -1)->m_prevlink = p_m_linktoslot->m_prevlink;
    p_m_linktoslot->m_prevlink->m_next = this->m_linktoslot.m_next;
    p_m_linktoslot->m_prevlink = nullptr;
    this->m_linktoslot.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 && this != nullptr )
    SMemFree(this, filename, -1, 0);
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8E60  EA 0x5B8E60
// DIA: TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *__thiscall TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::`scalar deleting destructor'(
        TSHashTableReuse<CLightList,HASHKEY_DWORD,1> *this,
        char __flags)
{
  TSHashObjectChunk<CLightList,HASHKEY_DWORD> *m_next; // esi
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *v4; // eax
  TSLink<CLightList> *v5; // eax

  this->__vftable = (TSHashTableReuse<CLightList,HASHKEY_DWORD,1>_vtbl *)&TSHashTableReuse<CLightList,HASHKEY_DWORD,1>::`vftable';
  while ( 1 )
  {
    m_next = this->m_chunkList.m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    TSHashObjectChunk<CLightList,HASHKEY_DWORD>::`scalar deleting destructor'(m_next, 0);
    SMemFree(m_next, aAvTshashobject_7, -2, 0);
  }
  TSList<CLightList,TSGetExplicitLink<CLightList>>::Clear(&this->m_reuseList);
  this->m_chunkSize = 16;
  while ( (int)this->m_chunkList.m_terminator.m_next > 0 )
  {
    v4 = TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>>::Link(
           &this->m_chunkList,
           this->m_chunkList.m_terminator.m_next);
    TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>::Unlink(v4);
  }
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>::Unlink(&this->m_chunkList.m_terminator);
  while ( (int)this->m_reuseList.m_terminator.m_next > 0 )
  {
    v5 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(
           &this->m_reuseList,
           this->m_reuseList.m_terminator.m_next);
    TSLink<CLightList>::Unlink(v5);
  }
  TSLink<CLightList>::Unlink(&this->m_reuseList.m_terminator);
  this->__vftable = (TSHashTableReuse<CLightList,HASHKEY_DWORD,1>_vtbl *)&TSHashTable<CLightList,HASHKEY_DWORD>::`vftable';
  TSHashTable<CLightList,HASHKEY_DWORD>::InternalClear(&this->TSHashTable<CLightList,HASHKEY_DWORD>, 1);
  TSFixedArray<TSExplicitList<CLightList,-572662307>>::~TSFixedArray<TSExplicitList<CLightList,-572662307>>(&this->m_slotlistarray);
  TSList<CLightList,TSGetExplicitLink<CLightList>>::UnlinkAll(&this->m_fulllist);
  TSLink<CLightList>::Unlink(&this->m_fulllist.m_terminator);
  if ( (__flags & 1) != 0 )
    SMemFree((void *)this, filename, -1, 0);
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B8F40  EA 0x5B8F40
// DIA: TSHashTable<CLightList,HASHKEY_DWORD>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashTable<CLightList,HASHKEY_DWORD> *__thiscall TSHashTable<CLightList,HASHKEY_DWORD>::`scalar deleting destructor'(
        TSHashTable<CLightList,HASHKEY_DWORD> *this,
        char __flags)
{
  TSHashTable<CLightList,HASHKEY_DWORD> *v2; // esi
  unsigned int v3; // ebx
  TSExplicitList<CLightList,-572662307> *p_m_fulllist; // edi
  TSLink<CLightList> *v5; // eax
  TSBaseArray<TSExplicitList<CLightList,-572662307> > *p_m_slotlistarray; // esi
  int m_next; // edi
  TSLink<CLightList> **p_m_prevlink; // edi
  int v9; // eax
  int v12; // [esp+10h] [ebp-4h]

  v2 = this;
  v3 = 0;
  this->__vftable = (TSHashTable<CLightList,HASHKEY_DWORD>_vtbl *)&TSHashTable<CLightList,HASHKEY_DWORD>::`vftable';
  this->m_fullnessIndicator = 0;
  p_m_fulllist = &this->m_fulllist;
  while ( (int)p_m_fulllist->m_terminator.m_next > 0 )
  {
    v5 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(p_m_fulllist, p_m_fulllist->m_terminator.m_next);
    TSLink<CLightList>::Unlink(v5);
  }
  if ( v2->m_slotlistarray.m_count != 0 )
  {
    p_m_slotlistarray = &v2->m_slotlistarray;
    v12 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        m_next = (int)p_m_slotlistarray->m_data[v12].m_terminator.m_next;
        if ( m_next <= 0 )
          break;
        TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v3);
        TSLink<CLightList>::Unlink((TSLink<CLightList> *)(m_next + p_m_slotlistarray->m_data[v12].m_linkoffset));
      }
      ++v3;
      ++v12;
    }
    while ( v3 < this->m_slotlistarray.m_count );
    v2 = this;
  }
  TSFixedArray<TSExplicitList<CLightList,-572662307>>::~TSFixedArray<TSExplicitList<CLightList,-572662307>>(&v2->m_slotlistarray);
  p_m_prevlink = &v2->m_fulllist.m_terminator.m_prevlink;
  while ( 1 )
  {
    v9 = (int)v2->m_fulllist.m_terminator.m_next;
    if ( v9 <= 0 )
      break;
    TSLink<CLightList>::Unlink((TSLink<CLightList> *)(v9 + v2->m_fulllist.m_linkoffset));
  }
  if ( *p_m_prevlink != nullptr )
  {
    TSLink<CLightList>::NextLink(&v2->m_fulllist.m_terminator, -1)->m_prevlink = *p_m_prevlink;
    (*p_m_prevlink)->m_next = v2->m_fulllist.m_terminator.m_next;
    *p_m_prevlink = nullptr;
    v2->m_fulllist.m_terminator.m_next = nullptr;
  }
  if ( (__flags & 1) != 0 )
    SMemFree(v2, filename, -1, 0);
  return v2;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9030  EA 0x5B9030
// DIA: NTempest::CPriorityQ<CGxuLight *,CGxuLight>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
NTempest::CPriorityQ<CGxuLight *,CGxuLight> *__thiscall NTempest::CPriorityQ<CGxuLight *,CGxuLight>::`vector deleting destructor'(
        NTempest::CPriorityQ<CGxuLight *,CGxuLight> *this,
        char __flags)
{
  this->__vftable = (NTempest::CPriorityQ<CGxuLight *,CGxuLight>_vtbl *)&NTempest::CPriorityQ<CGxuLight *,CGxuLight>::`vftable';
  NTempest::CMemBlock::~CMemBlock(&this->NTempest::CDynTable<CGxuLight *>);
  if ( (__flags & 1) != 0 )
    SMemFree((void *)this, filename, -1, 0);
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9060  EA 0x5B9060
// DIA: TSLink<CGxuLightLink>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<CGxuLightLink> *__thiscall TSLink<CGxuLightLink>::NextLink(TSLink<CGxuLightLink> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<CGxuLightLink> *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<CGxuLightLink> *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001B9090  EA 0x5B9090
// DIA: TSLink<CLightList>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<CLightList>::Unlink(TSLink<CLightList> *this)
{
  TSLink<CLightList> *m_prevlink; // esi
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
// RVA 0x001B90D0  EA 0x5B90D0
// DIA: TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<CGxuLightLink> *__thiscall TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::Link(
        TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> > *this,
        const CGxuLightLink *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<CGxuLightLink> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001B90F0  EA 0x5B90F0
// DIA: TSHashTable<CLightList,HASHKEY_DWORD>::InternalClear
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CLightList,HASHKEY_DWORD>::InternalClear(
        TSHashTable<CLightList,HASHKEY_DWORD> *this,
        int warn)
{
  unsigned int v3; // ebx
  CLightList *m_next; // eax
  TSLink<CLightList> *v5; // esi
  TSGrowableArray<TSExplicitList<CLightList,-572662307> > *p_m_slotlistarray; // esi
  int v7; // eax
  const char *v8; // eax
  CLightList *v9; // edi
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
    v5 = (TSLink<CLightList> *)((char *)m_next + this->m_fulllist.m_linkoffset);
    if ( v5->m_prevlink != nullptr )
    {
      TSLink<CLightList>::NextLink(v5, -1)->m_prevlink = v5->m_prevlink;
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
          v7 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CLightList,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo)(
                 p_m_slotlistarray,
                 1,
                 1,
                 aIndex0x08xArra,
                 v3,
                 p_m_slotlistarray->m_count);
          v8 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CLightList,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName)(
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
            v10 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CLightList,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo)(
                    p_m_slotlistarray,
                    1,
                    1,
                    aIndex0x08xArra,
                    v3,
                    p_m_slotlistarray->m_count);
            v11 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CLightList,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName)(
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
          this->TSHashTable<CLightList,HASHKEY_DWORD>::InternalDelete(this, v9);
        }
      }
      ++v26;
      ++v3;
    }
    while ( v3 < this->m_slotlistarray.m_count );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B9230  EA 0x5B9230
// DIA: TSHashTable<CLightList,HASHKEY_DWORD>::InternalNewNode
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CLightList *__thiscall TSHashTable<CLightList,HASHKEY_DWORD>::InternalNewNode(
        TSHashTable<CLightList,HASHKEY_DWORD> *this,
        unsigned int hashval,
        unsigned int extrabytes,
        unsigned int flags)
{
  unsigned int v5; // ebx
  TSExplicitList<CLightList,-572662307> *m_data; // edi
  int m_linkoffset; // eax
  TSList<CLightList,TSGetExplicitLink<CLightList> > *v8; // edi
  TSLink<CLightList> *p_m_terminator; // ebx
  TSLink<CLightList> *v10; // eax
  unsigned int v11; // edi
  unsigned int m_fullnessIndicator; // eax
  CLightList *m_next; // ebx
  unsigned int v14; // eax
  TSGrowableArray<TSExplicitList<CLightList,-572662307> > *p_m_slotlistarray; // ebx
  int v16; // eax
  const char *v17; // eax
  CLightList *v18; // eax
  CLightList *v19; // ebx
  TSLink<CLightList> *v20; // edi
  TSLink<CLightList> *m_prevlink; // eax
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
    TSGrowableArray<TSExplicitList<CLightList,-572662307>>::SetCount(&this->m_slotlistarray, 4u);
    v5 = 0;
    v28 = 0;
    v29 = 0;
    do
    {
      TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v5);
      m_data = this->m_slotlistarray.m_data;
      m_linkoffset = m_data[v29].m_linkoffset;
      v8 = &m_data[v29];
      if ( m_linkoffset != 4 )
      {
        p_m_terminator = &v8->m_terminator;
        while ( (int)v8->m_terminator.m_next > 0 )
        {
          v10 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(v8, v8->m_terminator.m_next);
          TSLink<CLightList>::Unlink(v10);
        }
        p_m_terminator->m_prevlink = p_m_terminator;
        v8->m_terminator.m_next = (CLightList *)~(unsigned int)p_m_terminator;
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
    TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v11);
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
        TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(&this->m_slotlistarray, v11);
        m_next = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(&this->m_slotlistarray.m_data[v11], m_next)->m_next;
        if ( (int)m_next <= 0 )
          goto LABEL_18;
      }
      v27 = 2 * this->m_slotmask + 2;
      this->m_fullnessIndicator = 0;
      TSHashTable<CLightList,HASHKEY_DWORD>::GrowListArray(this, v27);
      v11 = this->m_slotmask & hashval;
    }
  }
LABEL_18:
  p_m_slotlistarray = &this->m_slotlistarray;
  if ( v11 >= this->m_slotlistarray.m_count )
  {
    v16 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CLightList,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo)(
            &this->m_slotlistarray,
            1,
            1,
            aIndex0x08xArra,
            v11,
            this->m_slotlistarray.m_count);
    v17 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CLightList,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName)(
                          &this->m_slotlistarray,
                          v16);
    SErrDisplayErrorFmt(0x85100080, v17, v23, v24, v25, v26);
  }
  v18 = this->TSHashTable<CLightList,HASHKEY_DWORD>::InternalNew(
          this,
          &this->m_slotlistarray.m_data[v11],
          extrabytes,
          flags);
  v19 = v18;
  if ( v18 != nullptr )
    v20 = (TSLink<CLightList> *)((char *)v18 + this->m_fulllist.m_linkoffset);
  else
    v20 = &this->m_fulllist.m_terminator;
  if ( v20->m_prevlink != nullptr )
  {
    TSLink<CLightList>::NextLink(v20, -1)->m_prevlink = v20->m_prevlink;
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
// RVA 0x001B9400  EA 0x5B9400
// DIA: TSHashTable<CLightList,HASHKEY_DWORD>::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CLightList,HASHKEY_DWORD>::Unlink(
        TSHashTable<CLightList,HASHKEY_DWORD> *this,
        CLightList *ptr)
{
  TSLink<CLightList> *m_prevlink; // esi
  TSLink<CLightList> *p_m_linktoslot; // eax
  int m_next; // edx
  int v5; // edx
  TSLink<CLightList> *v6; // esi
  TSLink<CLightList> *p_m_linktofull; // eax
  int v8; // edx
  int v9; // edx

  if ( ptr->m_linktoslot.m_next != nullptr )
  {
    m_prevlink = ptr->m_linktoslot.m_prevlink;
    p_m_linktoslot = &ptr->m_linktoslot;
    if ( m_prevlink != nullptr )
    {
      m_next = (int)ptr->m_linktoslot.m_next;
      if ( m_next > 0 )
        v5 = (char *)p_m_linktoslot - (char *)m_prevlink->m_next + m_next;
      else
        v5 = ~m_next;
      *(_DWORD *)v5 = m_prevlink;
      p_m_linktoslot->m_prevlink->m_next = ptr->m_linktoslot.m_next;
      p_m_linktoslot->m_prevlink = nullptr;
      ptr->m_linktoslot.m_next = nullptr;
    }
    v6 = ptr->m_linktofull.m_prevlink;
    p_m_linktofull = &ptr->m_linktofull;
    if ( v6 != nullptr )
    {
      v8 = (int)ptr->m_linktofull.m_next;
      if ( v8 > 0 )
        v9 = (char *)p_m_linktofull - (char *)v6->m_next + v8;
      else
        v9 = ~v8;
      *(_DWORD *)v9 = v6;
      ptr->m_linktofull.m_prevlink->m_next = ptr->m_linktofull.m_next;
      p_m_linktofull->m_prevlink = nullptr;
      ptr->m_linktofull.m_next = nullptr;
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B9480  EA 0x5B9480
// DIA: TSList<CLightList,TSGetExplicitLink<CLightList> >::~TSList<CLightList,TSGetExplicitLink<CLightList> >
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CLightList,TSGetExplicitLink<CLightList>>::~TSList<CLightList,TSGetExplicitLink<CLightList>>(
        TSList<CLightList,TSGetExplicitLink<CLightList> > *this)
{
  TSLink<CLightList> *p_m_terminator; // ebx
  CLightList *m_next; // edx
  int v3; // edi
  _DWORD *v4; // eax
  int v5; // esi
  int v6; // esi
  TSLink<CLightList> *m_prevlink; // edx
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
// RVA 0x001B9500  EA 0x5B9500
// DIA: TSList<CLightList,TSGetExplicitLink<CLightList> >::ChangeLinkOffset
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CLightList,TSGetExplicitLink<CLightList>>::ChangeLinkOffset(
        TSList<CLightList,TSGetExplicitLink<CLightList> > *this,
        int linkoffset)
{
  TSLink<CLightList> *p_m_terminator; // ebx
  CLightList *m_next; // edx
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
    this->m_terminator.m_next = (CLightList *)~(unsigned int)p_m_terminator;
    this->m_linkoffset = linkoffset;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B9560  EA 0x5B9560
// DIA: TSFixedArray<TSExplicitList<CLightList,-572662307> >::~TSFixedArray<TSExplicitList<CLightList,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<CLightList,-572662307>>::~TSFixedArray<TSExplicitList<CLightList,-572662307>>(
        TSFixedArray<TSExplicitList<CLightList,-572662307> > *this)
{
  unsigned int m_count; // ecx
  TSExplicitList<CLightList,-572662307> *v3; // esi
  TSLink<CLightList> *v4; // eax
  bool v5; // cf
  TSExplicitList<CLightList,-572662307> *m_data; // ebx
  unsigned int index; // [esp+4h] [ebp-8h]
  int v8; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<TSExplicitList<CLightList,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<CLightList,-572662307>>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v8 = 0;
    do
    {
      v3 = &this->m_data[v8];
      while ( (int)v3->m_terminator.m_next > 0 )
      {
        v4 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(v3, v3->m_terminator.m_next);
        TSLink<CLightList>::Unlink(v4);
      }
      TSLink<CLightList>::Unlink(&v3->m_terminator);
      v5 = ++index < this->m_count;
      ++v8;
    }
    while ( v5 );
  }
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAvTsexplicitli_52, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001B95F0  EA 0x5B95F0
// DIA: $E5
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E5_261()
{
  return _E4_261();
}


// -----------------------------------------------------------------------------
// RVA 0x001B9600  EA 0x5B9600
// DIA: $E4
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E4_261()
{
  TWO_PI_261 = 3.1415927 + 3.1415927;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9610  EA 0x5B9610
// DIA: $E8
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E8_261()
{
  return _E7_261();
}


// -----------------------------------------------------------------------------
// RVA 0x001B9620  EA 0x5B9620
// DIA: $E7
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E7_261()
{
  OO_TWO_PI_261 = 1.0 / TWO_PI_261;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9640  EA 0x5B9640
// DIA: TSLink<CGxuLight>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
CGxuLight *__thiscall TSLink<CGxuLight>::NextLink(TSLink<CGxuLight> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (CGxuLight *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (CGxuLight *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001B9670  EA 0x5B9670
// DIA: TSLink<CLightList>::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<CLightList> *__thiscall TSLink<CLightList>::NextLink(TSLink<CLightList> *this, int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<CLightList> *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<CLightList> *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001B96A0  EA 0x5B96A0
// DIA: TSList<CLightList,TSGetExplicitLink<CLightList> >::Link
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<CLightList> *__thiscall TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(
        TSList<CLightList,TSGetExplicitLink<CLightList> > *this,
        const CLightList *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<CLightList> *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001B96C0  EA 0x5B96C0
// DIA: TSList<CLightList,TSGetExplicitLink<CLightList> >::TSList<CLightList,TSGetExplicitLink<CLightList> >
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSList<CLightList,TSGetExplicitLink<CLightList> > *__thiscall TSList<CLightList,TSGetExplicitLink<CLightList>>::TSList<CLightList,TSGetExplicitLink<CLightList>>(
        TSList<CLightList,TSGetExplicitLink<CLightList> > *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = 0;
  this->m_terminator.m_next = (CLightList *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B96E0  EA 0x5B96E0
// DIA: TSList<CLightList,TSGetExplicitLink<CLightList> >::Clear
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CLightList,TSGetExplicitLink<CLightList>>::Clear(
        TSList<CLightList,TSGetExplicitLink<CLightList> > *this)
{
  CLightList *m_next; // esi
  TSLink<CGxuLightLink> *v2; // eax

  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    while ( (int)m_next->m_links.m_terminator.m_next > 0 )
    {
      v2 = TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::Link(
             &m_next->m_links,
             m_next->m_links.m_terminator.m_next);
      TSLink<CGxuLightLink>::Unlink(v2);
    }
    TSLink<CGxuLightLink>::Unlink(&m_next->m_links.m_terminator);
    TSLink<CLightList>::Unlink(&m_next->m_linktofull);
    TSLink<CLightList>::Unlink(&m_next->m_linktoslot);
    SMemFree(m_next, aAuclightlist, -2, 0);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B9750  EA 0x5B9750
// DIA: TSList<CLightList,TSGetExplicitLink<CLightList> >::UnlinkAll
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CLightList,TSGetExplicitLink<CLightList>>::UnlinkAll(
        TSList<CLightList,TSGetExplicitLink<CLightList> > *this)
{
  CLightList *m_next; // edx
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
// RVA 0x001B97A0  EA 0x5B97A0
// DIA: TSBaseArray<TSExplicitList<CLightList,-572662307> >::CheckArrayBounds
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(
        TSBaseArray<TSExplicitList<CLightList,-572662307> > *this,
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
    v4 = ((int (__thiscall *)(TSBaseArray<TSExplicitList<CLightList,-572662307> > *, int, int, char *, unsigned int, unsigned int))this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo)(
           this,
           1,
           1,
           aIndex0x08xArra,
           index,
           m_count);
    v5 = (const char *)((int (__thiscall *)(TSBaseArray<TSExplicitList<CLightList,-572662307> > *, int))this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName)(
                         this,
                         v4);
    SErrDisplayErrorFmt(0x85100080, v5, v6, v7, v8, v9);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B97E0  EA 0x5B97E0
// DIA: TSGrowableArray<CLightList>::TSGrowableArray<CLightList>
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSGrowableArray<CLightList> *__thiscall TSGrowableArray<CLightList>::TSGrowableArray<CLightList>(
        TSGrowableArray<CLightList> *this)
{
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  this->__vftable = (TSGrowableArray<CLightList>_vtbl *)&TSGrowableArray<CLightList>::`vftable';
  this->m_chunk = 0;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9800  EA 0x5B9800
// DIA: TSBaseArray<CLightList>::MemFileName
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSBaseArray<CLightList>::MemFileName(TSBaseArray<CLightList> *this)
{
  return aAuclightlist;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9810  EA 0x5B9810
// DIA: TSBaseArray<CLightList>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSBaseArray<CLightList>::MemLineNo(TSBaseArray<CLightList> *this)
{
  return -2;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9820  EA 0x5B9820
// DIA: TSFixedArray<CLightList>::~TSFixedArray<CLightList>
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<CLightList>::~TSFixedArray<CLightList>(TSFixedArray<CLightList> *this)
{
  unsigned int m_count; // ecx
  CLightList *v3; // esi
  bool v4; // cf
  CLightList *m_data; // ebx
  unsigned int index; // [esp+4h] [ebp-8h]
  int v7; // [esp+8h] [ebp-4h]

  m_count = this->m_count;
  this->__vftable = (TSFixedArray<CLightList>_vtbl *)&TSFixedArray<CLightList>::`vftable';
  index = 0;
  if ( m_count != 0 )
  {
    v7 = 0;
    do
    {
      v3 = &this->m_data[v7];
      TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::UnlinkAll(&v3->m_links);
      TSLink<CGxuLightLink>::Unlink(&v3->m_links.m_terminator);
      TSLink<CLightList>::Unlink(&v3->m_linktofull);
      TSLink<CLightList>::Unlink(&v3->m_linktoslot);
      v4 = ++index < this->m_count;
      ++v7;
    }
    while ( v4 );
  }
  m_data = this->m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAuclightlist, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001B98A0  EA 0x5B98A0
// DIA: TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> >::UnlinkAll
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::UnlinkAll(
        TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> > *this)
{
  CGxuLightLink *m_next; // edx
  int v2; // edi
  _DWORD *v3; // eax
  int v4; // esi
  int v5; // esi

  while ( 1 )
  {
    m_next = this->m_terminator.m_next;
    if ( (int)m_next <= 0 )
      break;
    v2 = *(int *)((char *)&m_next->m_light + this->m_linkoffset);
    v3 = (CGxuLight **)((char *)&m_next->m_light + this->m_linkoffset);
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
// RVA 0x001B98F0  EA 0x5B98F0
// DIA: TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>>::LinkNode(
        TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > > *this,
        TSHashObjectChunk<CLightList,HASHKEY_DWORD> *ptr,
        unsigned int linktype,
        TSHashObjectChunk<CLightList,HASHKEY_DWORD> *existingptr)
{
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *p_m_terminator; // eax
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *v8; // edx
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *)((char *)ptr + this->m_linkoffset);
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
    v8 = (TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *)((char *)existingptr + this->m_linkoffset);
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
// RVA 0x001B99C0  EA 0x5B99C0
// DIA: TSHashObjectChunk<CLightList,HASHKEY_DWORD>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSHashObjectChunk<CLightList,HASHKEY_DWORD> *__thiscall TSHashObjectChunk<CLightList,HASHKEY_DWORD>::`scalar deleting destructor'(
        TSHashObjectChunk<CLightList,HASHKEY_DWORD> *this,
        char __flags)
{
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *p_m_link; // eax
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *m_prevlink; // edi
  int v5; // ebx
  int m_next; // edx
  int v7; // ecx
  unsigned int m_count; // eax
  unsigned int v9; // edi
  CLightList *m_data; // eax

  p_m_link = &this->m_link;
  m_prevlink = this->m_link.m_prevlink;
  v5 = 0;
  if ( m_prevlink != nullptr )
  {
    m_next = (int)this->m_link.m_next;
    if ( m_next > 0 )
      v7 = m_next + (char *)p_m_link - (char *)m_prevlink->m_next;
    else
      v7 = ~m_next;
    *(_DWORD *)v7 = m_prevlink;
    p_m_link->m_prevlink->m_next = p_m_link->m_next;
    p_m_link->m_prevlink = nullptr;
    p_m_link->m_next = nullptr;
  }
  m_count = this->m_array.m_count;
  v9 = 0;
  this->m_array.__vftable = (TSGrowableArray<CLightList>_vtbl *)&TSFixedArray<CLightList>::`vftable';
  if ( m_count != 0 )
  {
    do
    {
      CLightList::~CLightList(&this->m_array.m_data[v5]);
      ++v9;
      ++v5;
    }
    while ( v9 < this->m_array.m_count );
  }
  m_data = this->m_array.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAuclightlist, -2, 0);
  if ( (__flags & 1) != 0 )
    SMemFree(this, filename, -1, 0);
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9A50  EA 0x5B9A50
// DIA: CLightList::~CLightList
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __thiscall CLightList::~CLightList(CLightList *this)
{
  TSLink<CGxuLightLink> *p_m_terminator; // ecx
  int m_next; // edx
  int m_linkoffset; // eax
  int v4; // edi
  _DWORD *v5; // eax
  int v6; // esi
  TSLink<CGxuLightLink> *m_prevlink; // esi
  int v8; // edx
  int v9; // edx
  TSLink<CLightList> *v10; // edi
  TSLink<CLightList> *p_m_linktofull; // eax
  int v12; // esi
  int v13; // esi
  TSLink<CLightList> *v14; // esi
  TSLink<CLightList> *p_m_linktoslot; // eax
  int v16; // edx
  int v17; // edx

  p_m_terminator = &this->m_links.m_terminator;
  while ( 1 )
  {
    m_next = (int)p_m_terminator->m_next;
    if ( m_next <= 0 )
      break;
    m_linkoffset = this->m_links.m_linkoffset;
    v4 = *(_DWORD *)(m_linkoffset + m_next);
    v5 = (_DWORD *)(m_next + m_linkoffset);
    if ( v4 != 0 )
    {
      v6 = v5[1];
      if ( v6 > 0 )
        *(_DWORD *)((char *)v5 + v6 - *(_DWORD *)(v4 + 4)) = v4;
      else
        *(_DWORD *)~v6 = v4;
      *(_DWORD *)(*v5 + 4) = v5[1];
      *v5 = 0;
      v5[1] = 0;
    }
  }
  m_prevlink = p_m_terminator->m_prevlink;
  if ( p_m_terminator->m_prevlink != nullptr )
  {
    v8 = (int)p_m_terminator->m_next;
    if ( v8 > 0 )
      v9 = (char *)p_m_terminator - (char *)m_prevlink->m_next + v8;
    else
      v9 = ~v8;
    *(_DWORD *)v9 = m_prevlink;
    p_m_terminator->m_prevlink->m_next = p_m_terminator->m_next;
    p_m_terminator->m_prevlink = nullptr;
    p_m_terminator->m_next = nullptr;
  }
  v10 = this->m_linktofull.m_prevlink;
  p_m_linktofull = &this->m_linktofull;
  if ( v10 != nullptr )
  {
    v12 = (int)this->m_linktofull.m_next;
    if ( v12 > 0 )
      v13 = (char *)p_m_linktofull - (char *)v10->m_next + v12;
    else
      v13 = ~v12;
    *(_DWORD *)v13 = v10;
    this->m_linktofull.m_prevlink->m_next = this->m_linktofull.m_next;
    p_m_linktofull->m_prevlink = nullptr;
    this->m_linktofull.m_next = nullptr;
  }
  v14 = this->m_linktoslot.m_prevlink;
  p_m_linktoslot = &this->m_linktoslot;
  if ( v14 != nullptr )
  {
    v16 = (int)this->m_linktoslot.m_next;
    if ( v16 > 0 )
      v17 = (char *)p_m_linktoslot - (char *)v14->m_next + v16;
    else
      v17 = ~v16;
    *(_DWORD *)v17 = v14;
    this->m_linktoslot.m_prevlink->m_next = this->m_linktoslot.m_next;
    p_m_linktoslot->m_prevlink = nullptr;
    this->m_linktoslot.m_next = nullptr;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001B9B40  EA 0x5B9B40
// DIA: TSHashObjectChunk<CLightList,HASHKEY_DWORD>::~TSHashObjectChunk<CLightList,HASHKEY_DWORD>
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashObjectChunk<CLightList,HASHKEY_DWORD>::~TSHashObjectChunk<CLightList,HASHKEY_DWORD>(
        TSHashObjectChunk<CLightList,HASHKEY_DWORD> *this)
{
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *m_prevlink; // esi
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *p_m_link; // eax
  unsigned int v4; // ebx
  int m_next; // edx
  int v6; // edx
  bool v7; // zf
  CLightList *v8; // esi
  CLightList *m_data; // edi
  int v10; // [esp+Ch] [ebp-4h]

  m_prevlink = this->m_link.m_prevlink;
  p_m_link = &this->m_link;
  v4 = 0;
  if ( m_prevlink != nullptr )
  {
    m_next = (int)this->m_link.m_next;
    if ( m_next > 0 )
      v6 = (char *)p_m_link - (char *)m_prevlink->m_next + m_next;
    else
      v6 = ~m_next;
    *(_DWORD *)v6 = m_prevlink;
    this->m_link.m_prevlink->m_next = this->m_link.m_next;
    p_m_link->m_prevlink = nullptr;
    this->m_link.m_next = nullptr;
  }
  v7 = this->m_array.m_count == 0;
  this->m_array.__vftable = (TSGrowableArray<CLightList>_vtbl *)&TSFixedArray<CLightList>::`vftable';
  if ( !v7 )
  {
    v10 = 0;
    do
    {
      v8 = &this->m_array.m_data[v10];
      TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::~TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>(&v8->m_links);
      TSLink<CLightList>::Unlink(&v8->m_linktofull);
      TSLink<CLightList>::Unlink(&v8->m_linktoslot);
      ++v4;
      ++v10;
    }
    while ( v4 < this->m_array.m_count );
  }
  m_data = this->m_array.m_data;
  if ( m_data != nullptr )
    SMemFree(m_data, aAuclightlist, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001B9BD0  EA 0x5B9BD0
// DIA: TSHashTable<CLightList,HASHKEY_DWORD>::GrowListArray
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSHashTable<CLightList,HASHKEY_DWORD>::GrowListArray(
        TSHashTable<CLightList,HASHKEY_DWORD> *this,
        unsigned int newarraysize)
{
  TSHashTable<CLightList,HASHKEY_DWORD> *v2; // edi
  unsigned int v3; // ebx
  unsigned int v4; // esi
  TSLink<CLightList> *v5; // eax
  int v6; // esi
  TSLink<CLightList> *v7; // edi
  TSLink<CLightList> *m_prevlink; // eax
  unsigned int m_count; // eax
  TSGrowableArray<TSExplicitList<CLightList,-572662307> > *p_m_slotlistarray; // esi
  unsigned int v11; // edi
  unsigned int v12; // eax
  int v13; // ebx
  TSExplicitList<CLightList,-572662307> *v14; // ecx
  unsigned int v15; // ebx
  unsigned int v16; // edi
  unsigned int v17; // ebx
  TSExplicitList<CLightList,-572662307> *m_data; // edi
  int v19; // eax
  TSList<CLightList,TSGetExplicitLink<CLightList> > *v20; // edi
  TSLink<CLightList> *v21; // eax
  CLightList *m_next; // ebx
  unsigned int v23; // edi
  int v24; // eax
  const char *v25; // eax
  int v26; // [esp-18h] [ebp-3Ch]
  int v27; // [esp-14h] [ebp-38h]
  unsigned int v28; // [esp-10h] [ebp-34h]
  const char *v29; // [esp-Ch] [ebp-30h]
  TSExplicitList<CLightList,-572662307> templist; // [esp+Ch] [ebp-18h] BYREF
  unsigned int oldarraysize; // [esp+18h] [ebp-Ch]
  TSHashTable<CLightList,HASHKEY_DWORD> *v32; // [esp+1Ch] [ebp-8h]
  int v33; // [esp+20h] [ebp-4h]

  v2 = this;
  v3 = 0;
  v4 = this->m_slotmask + 1;
  v32 = this;
  oldarraysize = v4;
  memset(&templist, 0, sizeof(templist));
  TSList<CLightList,TSGetExplicitLink<CLightList>>::InitializeTerminator(&templist);
  templist.m_linkoffset = -572662307;
  TSList<CLightList,TSGetExplicitLink<CLightList>>::InitializeTerminator(&templist);
  if ( templist.m_linkoffset != 4 )
  {
    while ( (int)templist.m_terminator.m_next > 0 )
    {
      v5 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(&templist, templist.m_terminator.m_next);
      TSLink<CLightList>::Unlink(v5);
    }
    templist.m_linkoffset = 4;
    TSList<CLightList,TSGetExplicitLink<CLightList>>::InitializeTerminator(&templist);
  }
  if ( v4 != 0 )
  {
    v33 = 0;
    do
    {
      while ( 1 )
      {
        TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(&v2->m_slotlistarray, v3);
        v6 = *(int *)((char *)&v2->m_slotlistarray.m_data->m_terminator.m_next + v33);
        if ( v6 <= 0 )
          break;
        v7 = (TSLink<CLightList> *)(v6 + templist.m_linkoffset);
        if ( *(_DWORD *)(v6 + templist.m_linkoffset) != 0 )
          TSLink<CLightList>::Unlink((TSLink<CLightList> *)(v6 + templist.m_linkoffset));
        m_prevlink = templist.m_terminator.m_prevlink;
        v7->m_prevlink = templist.m_terminator.m_prevlink;
        v7->m_next = m_prevlink->m_next;
        m_prevlink->m_next = (CLightList *)v6;
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
        TSExplicitList<CLightList,-572662307>::`scalar deleting destructor'(&p_m_slotlistarray->m_data[v16], 0);
        ++v15;
        ++v16;
      }
      while ( v15 < p_m_slotlistarray->m_count );
      v11 = newarraysize;
    }
  }
  else
  {
    TSGrowableArray<TSExplicitList<CLightList,-572662307>>::Reserve(p_m_slotlistarray, newarraysize - m_count, 1);
    v12 = p_m_slotlistarray->m_count;
    if ( v12 < newarraysize )
    {
      v13 = v12;
      v33 = newarraysize - v12;
      do
      {
        v14 = &p_m_slotlistarray->m_data[v13];
        if ( v14 != nullptr )
          TSExplicitList<CLightList,-572662307>::TSExplicitList<CLightList,-572662307>(v14);
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
      TSBaseArray<TSExplicitList<CLightList,-572662307>>::CheckArrayBounds(p_m_slotlistarray, v17);
      m_data = p_m_slotlistarray->m_data;
      v19 = *(int *)((char *)&m_data->m_linkoffset + v33);
      v20 = (TSExplicitList<CLightList,-572662307> *)((char *)m_data + v33);
      if ( v19 != 4 )
      {
        while ( (int)v20->m_terminator.m_next > 0 )
        {
          v21 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(v20, v20->m_terminator.m_next);
          TSLink<CLightList>::Unlink(v21);
        }
        v20->m_linkoffset = 4;
        TSList<CLightList,TSGetExplicitLink<CLightList>>::InitializeTerminator(v20);
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
      v24 = ((int (__thiscall *)(TSGrowableArray<TSExplicitList<CLightList,-572662307> > *, int, int, char *, unsigned int, unsigned int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo)(
              p_m_slotlistarray,
              1,
              1,
              aIndex0x08xArra,
              v23,
              p_m_slotlistarray->m_count);
      v25 = (const char *)((int (__thiscall *)(TSGrowableArray<TSExplicitList<CLightList,-572662307> > *, int))p_m_slotlistarray->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName)(
                            p_m_slotlistarray,
                            v24);
      SErrDisplayErrorFmt(0x85100080, v25, v26, v27, v28, v29);
    }
    TSList<CLightList,TSGetExplicitLink<CLightList>>::LinkNode(&p_m_slotlistarray->m_data[v23], m_next, 2u, nullptr);
  }
  TSLink<CLightList>::Unlink(&templist.m_terminator);
}


// -----------------------------------------------------------------------------
// RVA 0x001B9DF0  EA 0x5B9DF0
// DIA: TSExplicitList<CLightList,-572662307>::TSExplicitList<CLightList,-572662307>
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSExplicitList<CLightList,-572662307> *__thiscall TSExplicitList<CLightList,-572662307>::TSExplicitList<CLightList,-572662307>(
        TSExplicitList<CLightList,-572662307> *this)
{
  this->m_terminator.m_prevlink = nullptr;
  this->m_terminator.m_next = nullptr;
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_linkoffset = -572662307;
  this->m_terminator.m_next = (CLightList *)~(unsigned int)&this->m_terminator;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9E10  EA 0x5B9E10
// DIA: TSList<CLightList,TSGetExplicitLink<CLightList> >::LinkNode
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CLightList,TSGetExplicitLink<CLightList>>::LinkNode(
        TSList<CLightList,TSGetExplicitLink<CLightList> > *this,
        CLightList *ptr,
        unsigned int linktype,
        CLightList *existingptr)
{
  TSLink<CLightList> *p_m_terminator; // eax
  TSLink<CLightList> *m_prevlink; // edi
  int m_next; // esi
  int v7; // esi
  TSLink<CLightList> *v8; // edx
  TSLink<CLightList> *v9; // ecx
  int m_linkoffset; // esi
  int v11; // ecx

  if ( ptr != nullptr )
    p_m_terminator = (TSLink<CLightList> *)((char *)ptr + this->m_linkoffset);
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
    v8 = (TSLink<CLightList> *)((char *)existingptr + this->m_linkoffset);
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
// RVA 0x001B9EE0  EA 0x5B9EE0
// DIA: TSExplicitList<CLightList,-572662307>::`scalar deleting destructor'
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
TSExplicitList<CLightList,-572662307> *__thiscall TSExplicitList<CLightList,-572662307>::`scalar deleting destructor'(
        TSExplicitList<CLightList,-572662307> *this,
        char __flags)
{
  TSExplicitList<CLightList,-572662307> *v2; // esi
  TSLink<CLightList> *p_m_terminator; // edi
  int m_next; // ecx
  int m_linkoffset; // eax
  int v6; // esi
  _DWORD *v7; // eax
  int v8; // edx
  int v9; // edx
  TSLink<CLightList> *m_prevlink; // edx
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
// RVA 0x001B9F90  EA 0x5B9F90
// DIA: TSList<CLightList,TSGetExplicitLink<CLightList> >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<CLightList,TSGetExplicitLink<CLightList>>::InitializeTerminator(
        TSList<CLightList,TSGetExplicitLink<CLightList> > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (CLightList *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001B9FA0  EA 0x5B9FA0
// DIA: TSGrowableArray<TSExplicitList<CLightList,-572662307> >::SetCount
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<CLightList,-572662307>>::SetCount(
        TSGrowableArray<TSExplicitList<CLightList,-572662307> > *this,
        unsigned int count)
{
  unsigned int v2; // ebx
  TSGrowableArray<TSExplicitList<CLightList,-572662307> > *v3; // edi
  unsigned int m_count; // eax
  unsigned int m_chunk; // ecx
  unsigned int i; // eax
  unsigned int v7; // esi
  unsigned int v8; // eax
  int v9; // edx
  int v10; // esi
  TSExplicitList<CLightList,-572662307> *v11; // ecx
  unsigned int v12; // eax
  _DWORD *v13; // edi
  TSLink<CLightList> *v14; // esi
  int v15; // eax
  TSLink<CLightList> *m_prevlink; // edi
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
      TSFixedArray<TSExplicitList<CLightList,-572662307>>::ReallocData(v3, v7);
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
          v11->m_terminator.m_next = (CLightList *)~(unsigned int)&v11->m_terminator;
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
    v14 = (TSLink<CLightList> *)(v13 + 1);
    while ( 1 )
    {
      v15 = v13[2];
      if ( v15 <= 0 )
        break;
      TSLink<CLightList>::Unlink((TSLink<CLightList> *)(v15 + *v13));
    }
    if ( v14->m_prevlink != nullptr )
    {
      m_prevlink = v14->m_prevlink;
      TSLink<CLightList>::NextLink(v14, -1)->m_prevlink = m_prevlink;
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
// RVA 0x001BA0E0  EA 0x5BA0E0
// DIA: TSFixedArray<TSExplicitList<CLightList,-572662307> >::TSFixedArray<TSExplicitList<CLightList,-572662307> >
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSFixedArray<TSExplicitList<CLightList,-572662307> > *__thiscall TSFixedArray<TSExplicitList<CLightList,-572662307>>::TSFixedArray<TSExplicitList<CLightList,-572662307>>(
        TSFixedArray<TSExplicitList<CLightList,-572662307> > *this)
{
  this->__vftable = (TSFixedArray<TSExplicitList<CLightList,-572662307> >_vtbl *)&TSFixedArray<TSExplicitList<CLightList,-572662307>>::`vftable';
  this->m_alloc = 0;
  this->m_count = 0;
  this->m_data = nullptr;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001BA100  EA 0x5BA100
// DIA: TSGrowableArray<CLightList>::CalcChunkSize
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<CLightList>::CalcChunkSize(
        TSGrowableArray<CLightList> *this,
        unsigned int count)
{
  unsigned int result; // eax
  unsigned int i; // ecx

  result = count;
  if ( count >= 8 )
  {
    this->m_chunk = 8;
    return 8;
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
// RVA 0x001BA140  EA 0x5BA140
// DIA: TSGrowableArray<CLightList>::RoundToChunk
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
unsigned int __thiscall TSGrowableArray<CLightList>::RoundToChunk(
        TSGrowableArray<CLightList> *this,
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
// RVA 0x001BA160  EA 0x5BA160
// DIA: TSFixedArray<CLightList>::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<CLightList>::ReallocData(TSFixedArray<CLightList> *this, unsigned int count)
{
  unsigned int m_count; // ebx
  TSLink<CLightList> *p_m_linktofull; // edi
  TSFixedArray<CLightList>_vtbl *v5; // edx
  const char *v6; // eax
  CLightList *v7; // eax
  const char *v8; // eax
  TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> > *p_m_links; // edi
  int v10; // eax
  _DWORD *v11; // eax
  int m_linkoffset; // edx
  TSLink<CGxuLightLink> *v13; // eax
  const char *v14; // eax
  int v15; // [esp-8h] [ebp-1Ch]
  int v16; // [esp-8h] [ebp-1Ch]
  int v17; // [esp-8h] [ebp-1Ch]
  CLightList *oldData; // [esp+10h] [ebp-4h]
  unsigned int index; // [esp+1Ch] [ebp+8h]

  m_count = count;
  oldData = this->m_data;
  if ( count < this->m_count )
  {
    p_m_linktofull = &this->m_data[count].m_linktofull;
    do
    {
      TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::~TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>((TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink> > *)&p_m_linktofull[1].m_next);
      TSLink<CLightList>::Unlink(p_m_linktofull);
      TSLink<CLightList>::Unlink(p_m_linktofull - 1);
      p_m_linktofull = (TSLink<CLightList> *)((char *)p_m_linktofull + 36);
      ++count;
    }
    while ( count < this->m_count );
  }
  v5 = this->__vftable;
  this->m_alloc = m_count;
  v15 = v5->TSBaseArray<CLightList>::MemLineNo(this);
  v6 = (const char *)this->TSBaseArray<CLightList>::MemFileName(this);
  v7 = (CLightList *)SMemReAlloc(oldData, 36 * m_count, v6, v15, 0x10u);
  this->m_data = v7;
  if ( v7 == nullptr )
  {
    v16 = this->TSBaseArray<CLightList>::MemLineNo(this);
    v8 = (const char *)this->TSBaseArray<CLightList>::MemFileName(this);
    this->m_data = (CLightList *)SMemAlloc(36 * m_count, v8, v16, 0);
    if ( oldData != nullptr )
    {
      if ( m_count >= this->m_count )
        m_count = this->m_count;
      if ( m_count != 0 )
      {
        p_m_links = &oldData->m_links;
        v10 = -24 - (_DWORD)oldData;
        index = m_count;
        while ( 1 )
        {
          v11 = (unsigned int *)((char *)&this->m_data->m_hashval + (unsigned int)p_m_links + v10);
          if ( v11 != nullptr )
          {
            v11[1] = 0;
            v11[2] = 0;
            v11[3] = 0;
            v11[4] = 0;
            v11[5] = 0;
            v11[7] = 0;
            v11[8] = 0;
            m_linkoffset = p_m_links->m_linkoffset;
            v11[7] = v11 + 7;
            v11[6] = m_linkoffset;
            v11[8] = ~(unsigned int)(v11 + 7);
          }
          while ( (int)p_m_links->m_terminator.m_next > 0 )
          {
            v13 = TSList<CGxuLightLink,TSGetExplicitLink<CGxuLightLink>>::Link(
                    p_m_links,
                    p_m_links->m_terminator.m_next);
            TSLink<CGxuLightLink>::Unlink(v13);
          }
          TSLink<CGxuLightLink>::Unlink(&p_m_links->m_terminator);
          TSLink<CLightList>::Unlink((TSLink<CLightList> *)&p_m_links[-1]);
          TSLink<CLightList>::Unlink((TSLink<CLightList> *)&p_m_links[-2].m_terminator);
          p_m_links += 3;
          if ( --index == 0 )
            break;
          v10 = -24 - (_DWORD)oldData;
        }
      }
      v17 = this->TSBaseArray<CLightList>::MemLineNo(this);
      v14 = (const char *)this->TSBaseArray<CLightList>::MemFileName(this);
      SMemFree(oldData, v14, v17, 0);
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BA2C0  EA 0x5BA2C0
// DIA: TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> >::NextLink
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *__thiscall TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>::NextLink(
        TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *this,
        int linkoffset)
{
  int m_next; // eax
  int v4; // edx

  m_next = (int)this->m_next;
  if ( m_next <= 0 )
    return (TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *)~m_next;
  v4 = linkoffset;
  if ( linkoffset < 0 )
    v4 = (char *)this - (char *)this->m_prevlink->m_next;
  return (TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *)(v4 + m_next);
}


// -----------------------------------------------------------------------------
// RVA 0x001BA2F0  EA 0x5BA2F0
// DIA: TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> >::Unlink
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>::Unlink(
        TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *this)
{
  TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *m_prevlink; // esi
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
// RVA 0x001BA330  EA 0x5BA330
// DIA: TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > >::InitializeTerminator
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>>::InitializeTerminator(
        TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > > *this)
{
  this->m_terminator.m_prevlink = &this->m_terminator;
  this->m_terminator.m_next = (TSHashObjectChunk<CLightList,HASHKEY_DWORD> *)~(unsigned int)&this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BA340  EA 0x5BA340
// DIA: TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > >::Link
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *__thiscall TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD>>>::Link(
        TSList<TSHashObjectChunk<CLightList,HASHKEY_DWORD>,TSGetExplicitLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > > *this,
        const TSHashObjectChunk<CLightList,HASHKEY_DWORD> *ptr)
{
  if ( ptr != nullptr )
    return (TSLink<TSHashObjectChunk<CLightList,HASHKEY_DWORD> > *)((char *)ptr + this->m_linkoffset);
  else
    return &this->m_terminator;
}


// -----------------------------------------------------------------------------
// RVA 0x001BA360  EA 0x5BA360
// DIA: TSGrowableArray<TSExplicitList<CLightList,-572662307> >::Reserve
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSGrowableArray<TSExplicitList<CLightList,-572662307>>::Reserve(
        TSGrowableArray<TSExplicitList<CLightList,-572662307> > *this,
        unsigned int count,
        int round)
{
  unsigned int v4; // edi
  unsigned int m_chunk; // ecx
  int i; // eax
  TSExplicitList<CLightList,-572662307> *v7; // ebx
  TSGrowableArray<TSExplicitList<CLightList,-572662307> >_vtbl *v8; // edx
  const char *v9; // eax
  TSExplicitList<CLightList,-572662307> *v10; // eax
  const char *v11; // eax
  unsigned int m_count; // eax
  int v13; // ebx
  TSList<CLightList,TSGetExplicitLink<CLightList> > *v14; // edi
  TSExplicitList<CLightList,-572662307> *v15; // ecx
  int m_linkoffset; // edx
  TSLink<CLightList> *v17; // eax
  const char *v18; // eax
  int v19; // [esp-8h] [ebp-14h]
  int v20; // [esp-8h] [ebp-14h]
  int v21; // [esp-8h] [ebp-14h]
  TSExplicitList<CLightList,-572662307> *counta; // [esp+14h] [ebp+8h]
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
        TSList<CLightList,TSGetExplicitLink<CLightList>>::UnlinkAll(v7);
        TSLink<CLightList>::Unlink(&v7->m_terminator);
        ++v7;
        ++rounda;
      }
      while ( rounda < this->m_count );
    }
    v8 = this->__vftable;
    this->m_alloc = v4;
    v19 = v8->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName(this);
    v10 = (TSExplicitList<CLightList,-572662307> *)SMemReAlloc(counta, 12 * v4, v9, v19, 0x10u);
    this->m_data = v10;
    if ( v10 == nullptr )
    {
      v20 = this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo(this);
      v11 = (const char *)this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName(this);
      this->m_data = (TSExplicitList<CLightList,-572662307> *)SMemAlloc(12 * v4, v11, v20, 0);
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
              v15->m_terminator.m_next = (CLightList *)~(unsigned int)&v15->m_terminator;
            }
            while ( (int)v14->m_terminator.m_next > 0 )
            {
              v17 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(v14, v14->m_terminator.m_next);
              TSLink<CLightList>::Unlink(v17);
            }
            TSLink<CLightList>::Unlink(&v14->m_terminator);
            ++v13;
            ++v14;
          }
        }
        v21 = this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo(this);
        v18 = (const char *)this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName(this);
        SMemFree(counta, v18, v21, 0);
      }
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BA4E0  EA 0x5BA4E0
// DIA: TSFixedArray<TSExplicitList<CLightList,-572662307> >::ReallocData
// Compiland: .\Release_Assertions_Enabled\GxuLight.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void __thiscall TSFixedArray<TSExplicitList<CLightList,-572662307>>::ReallocData(
        TSFixedArray<TSExplicitList<CLightList,-572662307> > *this,
        unsigned int count)
{
  unsigned int m_count; // ebx
  TSExplicitList<CLightList,-572662307> *v4; // edi
  TSLink<CLightList> *v5; // eax
  TSFixedArray<TSExplicitList<CLightList,-572662307> >_vtbl *v6; // edx
  const char *v7; // eax
  TSExplicitList<CLightList,-572662307> *v8; // eax
  const char *v9; // eax
  TSExplicitList<CLightList,-572662307> *v10; // edi
  TSExplicitList<CLightList,-572662307> *v11; // ecx
  int m_linkoffset; // edx
  int m_next; // eax
  TSLink<CLightList> *m_prevlink; // ebx
  TSLink<CLightList> *p_m_terminator; // eax
  int v16; // edx
  int v17; // edx
  bool v18; // zf
  const char *v19; // eax
  int v20; // [esp-8h] [ebp-1Ch]
  int v21; // [esp-8h] [ebp-1Ch]
  int v22; // [esp-8h] [ebp-1Ch]
  unsigned int v23; // [esp+Ch] [ebp-8h]
  TSExplicitList<CLightList,-572662307> *oldData; // [esp+10h] [ebp-4h]
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
        v5 = TSList<CLightList,TSGetExplicitLink<CLightList>>::Link(v4, v4->m_terminator.m_next);
        TSLink<CLightList>::Unlink(v5);
      }
      TSLink<CLightList>::Unlink(&v4->m_terminator);
      ++v4;
      ++count;
    }
    while ( count < this->m_count );
  }
  v6 = this->__vftable;
  this->m_alloc = m_count;
  v20 = v6->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo(this);
  v7 = (const char *)this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName(this);
  v8 = (TSExplicitList<CLightList,-572662307> *)SMemReAlloc(oldData, 12 * m_count, v7, v20, 0x10u);
  this->m_data = v8;
  if ( v8 == nullptr )
  {
    v21 = this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo(this);
    v9 = (const char *)this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName(this);
    v10 = oldData;
    this->m_data = (TSExplicitList<CLightList,-572662307> *)SMemAlloc(12 * m_count, v9, v21, 0);
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
            v11->m_terminator.m_next = (CLightList *)~(unsigned int)&v11->m_terminator;
          }
          while ( 1 )
          {
            m_next = (int)v10->m_terminator.m_next;
            if ( m_next <= 0 )
              break;
            TSLink<CLightList>::Unlink((TSLink<CLightList> *)(m_next + v10->m_linkoffset));
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
      v22 = this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemLineNo(this);
      v19 = (const char *)this->TSBaseArray<TSExplicitList<CLightList,-572662307> >::MemFileName(this);
      SMemFree(oldData, v19, v22, 0);
    }
  }
}

