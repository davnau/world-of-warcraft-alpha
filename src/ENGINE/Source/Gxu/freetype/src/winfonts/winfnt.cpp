// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\FreeType\src\winfonts\winfnt.c
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x003D06B0  EA 0x7D06B0
// DIA: FNT_Done_Face
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __usercall FNT_Done_Face(FNT_FaceRec_ *face@<eax>, int a2@<ecx>)
{
  void **v3; // edi
  FT_MemoryRec_ *v4; // eax

  v3 = *(void ***)(a2 + 100);
  fnt_done_fonts(face);
  FT_Free(v4, v3);
  *(_DWORD *)(a2 + 28) = 0;
}


// -----------------------------------------------------------------------------
// RVA 0x003D06D0  EA 0x7D06D0
// DIA: fnt_done_fonts
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall fnt_done_fonts(_DWORD *this)
{
  FT_StreamRec_ *v3; // eax
  unsigned int v4; // esi
  unsigned int v5; // edi
  FT_MemoryRec_ *memory; // [esp+Ch] [ebp-8h]
  FT_StreamRec_ *stream; // [esp+10h] [ebp-4h]

  v3 = (FT_StreamRec_ *)this[25];
  v4 = this[34];
  v5 = v4 + 160 * this[33];
  memory = (FT_MemoryRec_ *)v3;
  for ( stream = (FT_StreamRec_ *)this[26]; v4 < v5; v4 += 160 )
    fnt_done_font(v3, (FNT_Font_ *)stream);
  FT_Free((FT_MemoryRec_ *)v3, &memory->user);
  this[33] = 0;
}


// -----------------------------------------------------------------------------
// RVA 0x003D0740  EA 0x7D0740
// DIA: fnt_done_font
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall fnt_done_font(FNT_Font_ *font, int a2)
{
  FT_StreamRec_ *v4; // eax
  _DWORD *v5; // esi

  v4 = *(FT_StreamRec_ **)(a2 + 152);
  v5 = (_DWORD *)(a2 + 152);
  if ( v4 != nullptr )
    FT_Release_Frame(v4, (unsigned __int8 **)font);
  *(_DWORD *)(a2 + 156) = 0;
  *v5 = 0;
}


// -----------------------------------------------------------------------------
// RVA 0x003D0770  EA 0x7D0770
// DIA: FNT_Init_Face
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
FNT_FaceRec_ *__fastcall FNT_Init_Face(FNT_Font_ *face, int a2, int face_index, int num_params, FT_Parameter_ *params)
{
  FT_MemoryRec_ *dll_fonts; // eax
  FNT_FaceRec_ *result; // eax
  _DWORD *v10; // edx
  FNT_FaceRec_ *font; // eax
  FNT_FaceRec_ *v12; // ebx
  int v13; // ecx
  unsigned int v14; // edi
  _WORD *v15; // ecx
  __int16 *v16; // eax
  __int16 v17; // dx
  int v18; // eax
  bool v19; // zf
  FNT_Font_ *limit; // [esp+Ch] [ebp-8h]
  FT_MemoryRec_ *memory; // [esp+10h] [ebp-4h]

  memory = *(FT_MemoryRec_ **)(a2 + 100);
  dll_fonts = (FT_MemoryRec_ *)fnt_get_dll_fonts((FNT_FaceRec_ *)memory);
  if ( dll_fonts != nullptr )
  {
    result = (FNT_FaceRec_ *)FT_Alloc(dll_fonts, (int)memory, (void **)(a2 + 136));
    if ( result != nullptr )
      return result;
    v10 = *(_DWORD **)(a2 + 136);
    *(_DWORD *)(a2 + 132) = 1;
    *v10 = 0;
    v10[39] = face->size_shift;
    font = (FNT_FaceRec_ *)fnt_load_font(nullptr, face);
    v12 = font;
    if ( font != nullptr )
      goto LABEL_20;
  }
  v13 = *(_DWORD *)(a2 + 132);
  v14 = *(_DWORD *)(a2 + 136);
  *(_DWORD *)a2 = 1;
  *(_DWORD *)(a2 + 8) = 18;
  limit = (FNT_Font_ *)(v14 + 160 * v13);
  if ( *(_WORD *)(v14 + 104) == *(_WORD *)(v14 + 106) )
    *(_DWORD *)(a2 + 8) = 22;
  if ( *(_BYTE *)(v14 + 90) != 0 )
    *(_DWORD *)(a2 + 12) |= 1u;
  if ( *(_WORD *)(v14 + 94) >= 0x320u )
    *(_DWORD *)(a2 + 12) |= 2u;
  font = (FNT_FaceRec_ *)FT_Alloc((FT_MemoryRec_ *)(a2 + 32), (int)memory, (void **)(a2 + 32));
  v12 = font;
  if ( font != nullptr )
  {
LABEL_20:
    FNT_Done_Face(font, a2);
    return v12;
  }
  else
  {
    v15 = *(_WORD **)(a2 + 32);
    *(_DWORD *)(a2 + 28) = *(_DWORD *)(a2 + 132);
    if ( v14 < (unsigned int)limit )
    {
      v16 = (__int16 *)(v14 + 100);
      do
      {
        v15[1] = *(v16 - 1);
        v17 = *v16;
        v16 += 80;
        *v15 = v17;
        v15 += 2;
      }
      while ( v16 - 50 < (__int16 *)limit );
    }
    *(_DWORD *)(a2 + 36) = 1;
    *(_WORD *)(a2 + 154) = 1;
    *(_DWORD *)(a2 + 144) = a2;
    *(_DWORD *)(a2 + 140) = a2 + 144;
    *(_DWORD *)(a2 + 92) = a2 + 144;
    *(_DWORD *)(a2 + 40) = a2 + 140;
    *(_DWORD *)(a2 + 148) = 1970170211;
    *(_WORD *)(a2 + 152) = 3;
    *(_DWORD *)(a2 + 16) = *(unsigned __int8 *)(v14 + 109) - *(unsigned __int8 *)(v14 + 108) + 1;
    v18 = *(_DWORD *)(a2 + 12);
    *(_DWORD *)(a2 + 20) = *(_DWORD *)(v14 + 120) + *(_DWORD *)(v14 + 152);
    *(_DWORD *)(a2 + 24) = aRegular;
    if ( (v18 & 2) != 0 )
    {
      v19 = (v18 & 1) == 0;
      result = nullptr;
      if ( v19 )
        *(_DWORD *)(a2 + 24) = aBold;
      else
        *(_DWORD *)(a2 + 24) = aBoldItalic;
    }
    else
    {
      v19 = (v18 & 1) == 0;
      result = nullptr;
      if ( !v19 )
        *(_DWORD *)(a2 + 24) = aItalic;
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x003D0930  EA 0x7D0930
// DIA: fnt_load_font
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __usercall fnt_load_font@<eax>(FT_StreamRec_ *stream@<eax>, const FT_Frame_Field_ *font@<ecx>, int a3@<edx>)
{
  _WORD *v5; // ebx
  int result; // eax

  v5 = (_WORD *)(a3 + 8);
  result = FT_Seek_Stream(stream, (unsigned int)font);
  if ( result == 0 )
  {
    result = FT_Read_Fields(nullptr, font, (void *)(a3 + 8));
    if ( result == 0 )
    {
      LOWORD(result) = *v5;
      if ( *v5 == 512 || (_WORD)result == 768 )
      {
        if ( (*(_BYTE *)(a3 + 76) & 1) != 0 )
        {
          return 2;
        }
        else
        {
          if ( *(_WORD *)(a3 + 98) == 0 )
          {
            LOWORD(result) = *(_WORD *)(a3 + 100);
            *(_WORD *)(a3 + 98) = result;
          }
          result = FT_Seek_Stream((FT_StreamRec_ *)result, (unsigned int)font);
          if ( result == 0 )
            return FT_Extract_Frame(nullptr, (unsigned int)font, (unsigned __int8 **)(a3 + 152));
        }
      }
      else
      {
        return 2;
      }
    }
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x003D09B0  EA 0x7D09B0
// DIA: fnt_get_dll_fonts
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall fnt_get_dll_fonts(int this)
{
  _DWORD *v2; // ebx
  unsigned int v3; // esi
  int font; // edi
  unsigned __int16 v5; // bx
  FT_StreamRec_ *v6; // eax
  FT_StreamRec_ *v7; // eax
  __int16 v8; // di
  FT_StreamRec_ *v9; // eax
  FT_StreamRec_ *v10; // eax
  unsigned int v11; // edi
  FT_StreamRec_ *v12; // eax
  FNT_Font_ *v13; // ebx
  FNT_Font_ *v14; // eax
  bool v15; // cf
  FT_StreamRec_ *v16; // eax
  unsigned __int16 ShortLE; // ax
  int v18; // eax
  unsigned int v19; // ebx
  FT_StreamRec_ *v20; // eax
  WinNE_Header_ ne_header; // [esp+Ch] [ebp-20h] BYREF
  FT_StreamRec_ *size_shift; // [esp+14h] [ebp-18h]
  FNT_Font_ *limit; // [esp+18h] [ebp-14h]
  int v25; // [esp+1Ch] [ebp-10h]
  unsigned int font_offset; // [esp+20h] [ebp-Ch]
  WinMZ_Header_ mz_header; // [esp+24h] [ebp-8h] BYREF
  int v28; // [esp+28h] [ebp-4h]

  v2 = (_DWORD *)this;
  v3 = *(_DWORD *)(this + 104);
  limit = *(FNT_Font_ **)(this + 100);
  v28 = this;
  *(_DWORD *)(this + 136) = 0;
  *(_DWORD *)(this + 132) = 0;
  font = FT_Seek_Stream(nullptr, v3);
  if ( font != 0 )
    return font;
  font = FT_Read_Fields(nullptr, (const FT_Frame_Field_ *)v3, &mz_header);
  if ( font != 0 )
    return font;
  font = 2;
  if ( mz_header.magic != 23117 )
    goto LABEL_28;
  font = FT_Seek_Stream(nullptr, v3);
  if ( font != 0 )
    return font;
  font = FT_Read_Fields(nullptr, (const FT_Frame_Field_ *)v3, &ne_header);
  if ( font != 0 )
    return font;
  font = 2;
  if ( ne_header.magic != 17742 )
    goto LABEL_28;
  v5 = 0;
  font_offset = 0;
  font = FT_Seek_Stream((FT_StreamRec_ *)mz_header.lfanew, v3);
  if ( font == 0 )
  {
    font = FT_Access_Frame(nullptr, v3);
    if ( font == 0 )
    {
      LOWORD(v6) = FT_Get_ShortLE(nullptr);
      size_shift = v6;
      LOWORD(v7) = FT_Get_ShortLE(v6);
      v8 = (__int16)v7;
      if ( (_WORD)v7 != 0 )
      {
        while ( 1 )
        {
          LOWORD(v9) = FT_Get_ShortLE(v7);
          if ( v8 == -32760 )
            break;
          v10 = (FT_StreamRec_ *)(12 * (unsigned __int16)v9 + 4);
          *(_DWORD *)(v3 + 32) += v10;
          LOWORD(v7) = FT_Get_ShortLE(v10);
          v8 = (__int16)v7;
          if ( (_WORD)v7 == 0 )
            goto LABEL_12;
        }
        v5 = (unsigned __int16)v9;
        v7 = (FT_StreamRec_ *)FT_Stream_Pos(v9);
        v11 = (unsigned int)&v7->size + *(_DWORD *)(v3 + 32) - *(_DWORD *)(v3 + 36);
      }
      else
      {
LABEL_12:
        v11 = font_offset;
      }
      FT_Forget_Frame(v7);
      if ( v5 == 0 || v11 == 0 )
        return 2;
      font = FT_Seek_Stream(v12, v3);
      if ( font == 0 )
      {
        v25 = 160 * v5;
        font_offset = v28 + 136;
        font = FT_Alloc((FT_MemoryRec_ *)(v28 + 136), (int)limit, (void **)(v28 + 136));
        if ( font == 0 )
        {
          *(_DWORD *)(v28 + 132) = v5;
          font = FT_Access_Frame(nullptr, v3);
          if ( font == 0 )
          {
            v13 = *(FNT_Font_ **)font_offset;
            v14 = (FNT_Font_ *)(*(_DWORD *)font_offset + v25);
            v15 = *(_DWORD *)font_offset < (unsigned int)v14;
            limit = v14;
            if ( v15 )
            {
              font_offset = (unsigned __int16)size_shift;
              do
              {
                LOWORD(v16) = FT_Get_ShortLE((FT_StreamRec_ *)v14);
                v13->offset = (unsigned __int16)v16 << font_offset;
                ShortLE = FT_Get_ShortLE(v16);
                v18 = ShortLE << font_offset;
                v13->size_shift = font_offset;
                ++v13;
                v13[-1].fnt_size = v18;
                v14 = limit;
                v15 = v13 < limit;
                *(_DWORD *)(v3 + 32) += 8;
              }
              while ( v15 );
            }
            FT_Forget_Frame((FT_StreamRec_ *)v14);
            v19 = *(_DWORD *)(v28 + 136);
            v20 = (FT_StreamRec_ *)(v25 + v19);
            size_shift = (FT_StreamRec_ *)(v25 + v19);
            if ( v19 < v25 + v19 )
            {
              while ( 1 )
              {
                font = fnt_load_font(v20, (const FT_Frame_Field_ *)v3, v19);
                if ( font != 0 )
                  break;
                v20 = size_shift;
                v19 += 160;
                if ( v19 >= (unsigned int)size_shift )
                  return 0;
              }
              v2 = (_DWORD *)v28;
LABEL_28:
              fnt_done_fonts(v2);
            }
          }
        }
      }
    }
  }
  return font;
}


// -----------------------------------------------------------------------------
// RVA 0x003D0C40  EA 0x7D0C40
// DIA: FNT_Set_Pixel_Size
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall FNT_Set_Pixel_Size(int this)
{
  unsigned int v2; // eax
  unsigned int v3; // edx

  v2 = *(_DWORD *)(*(_DWORD *)this + 136);
  v3 = v2 + 160 * *(_DWORD *)(*(_DWORD *)this + 132);
  *(_DWORD *)(this + 44) = 0;
  if ( v2 < v3 )
  {
    while ( *(_WORD *)(v2 + 100) != *(_WORD *)(this + 14) )
    {
      v2 += 160;
      if ( v2 >= v3 )
        return *(_DWORD *)(this + 44) != 0 ? 0 : 0x17;
    }
    *(_DWORD *)(this + 44) = v2;
    *(_DWORD *)(this + 24) = *(unsigned __int16 *)(v2 + 84) << 6;
    *(_DWORD *)(this + 28) = (*(unsigned __int16 *)(v2 + 100) - *(unsigned __int16 *)(v2 + 84)) << 6;
    *(_DWORD *)(this + 32) = *(unsigned __int16 *)(v2 + 100) << 6;
  }
  return *(_DWORD *)(this + 44) != 0 ? 0 : 0x17;
}


// -----------------------------------------------------------------------------
// RVA 0x003D0CC0  EA 0x7D0CC0
// DIA: FNT_Get_Char_Index
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall FNT_Get_Char_Index(unsigned int char_code, unsigned int a2)
{
  unsigned int result; // eax
  int v4; // ecx
  int v5; // edx
  unsigned int v6; // eax

  result = a2;
  if ( char_code != 0 )
  {
    v4 = *(_DWORD *)(*(_DWORD *)char_code + 136);
    v5 = *(unsigned __int8 *)(v4 + 108);
    v6 = result - v5;
    if ( v6 >= (unsigned int)*(unsigned __int8 *)(v4 + 109) - v5 + 1 )
      return 0;
    else
      return v6 + 1;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x003D0CF0  EA 0x7D0CF0
// DIA: FNT_Load_Glyph
// Compiland: .\Release_Assertions_Enabled\winfnt.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall FNT_Load_Glyph(FNT_SizeRec_ *size, int a2, unsigned int glyph_index, int load_flags)
{
  FNT_Font_ *v5; // eax
  int v8; // edi
  __int16 v9; // bx
  int v10; // ecx
  unsigned __int8 *fnt_frame; // edi
  unsigned __int8 *v12; // ecx
  bool v13; // zf
  int v14; // edx
  unsigned __int8 *v15; // edi
  int v16; // ecx
  int v17; // ebx
  int v18; // ecx
  struct FT_Size_InternalRec_ *internal; // eax
  unsigned __int8 *v20; // edx
  struct FT_Size_InternalRec_ *i; // ecx
  int height; // edx
  int v23; // eax
  FNT_Font_ *font; // [esp+Ch] [ebp-8h]
  unsigned int glyph_indexa; // [esp+1Ch] [ebp+8h]

  v5 = *(FNT_Font_ **)(a2 + 44);
  font = v5;
  if ( v5 == nullptr )
    return 6;
  if ( glyph_index != 0 )
    v8 = glyph_index - 1;
  else
    v8 = v5->header.default_char - v5->header.first_char;
  LOBYTE(v9) = 0;
  v10 = v8 * (2 * (v5->header.version == 768) + 4);
  fnt_frame = v5->fnt_frame;
  HIBYTE(v9) = fnt_frame[v10 + 119];
  v12 = &fnt_frame[v10 + 120];
  v13 = v5->header.version != 768;
  size[1].root.metrics.height = *(v12 - 2) | v9;
  if ( v13 )
    v14 = *(unsigned __int16 *)v12;
  else
    v14 = *v12 | ((v12[1] | ((v12[2] | ((char)v12[3] << 8)) << 8)) << 8);
  v15 = &v5->fnt_frame[v14];
  v16 = *((_DWORD *)size->root.generic.data + 25);
  v17 = (size[1].root.metrics.height + 7) >> 3;
  size[1].root.metrics.max_advance = v17;
  size[1].root.metrics.descender = v5->header.pixel_height;
  BYTE2(size[1].font) = 1;
  v18 = FT_Alloc((FT_MemoryRec_ *)&size[1].root.internal, v16, (void **)&size[1].root.internal);
  if ( v18 == 0 )
  {
    internal = size[1].root.internal;
    if ( v17 > 0 )
    {
      for ( glyph_indexa = v17; glyph_indexa != 0; --glyph_indexa )
      {
        v20 = &v15[size[1].root.metrics.descender];
        for ( i = internal; v15 < v20; i = (struct FT_Size_InternalRec_ *)((char *)i + size[1].root.metrics.max_advance) )
          *(_BYTE *)i = *v15++;
        internal = (struct FT_Size_InternalRec_ *)((char *)internal + 1);
      }
      v18 = 0;
    }
    size[2].root.generic.data = nullptr;
    *(_DWORD *)&size->root.metrics.x_ppem = 1;
    size[2].root.generic.finalizer = (void (__fastcall *)(void *))font->header.ascent;
    size[1].root.metrics.ascender = 1651078259;
    height = size[1].root.metrics.height;
    size->root.metrics.height = 0;
    v23 = (int)size[2].root.generic.finalizer << 6;
    size->root.internal = (struct FT_Size_InternalRec_ *)(height << 6);
    size->root.metrics.max_advance = v23;
    size[1].root.generic.finalizer = (void (__fastcall *)(void *))(size[1].root.metrics.height << 16);
  }
  return v18;
}

