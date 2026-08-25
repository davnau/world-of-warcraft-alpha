// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\FreeType\src\base\ftsystem.c
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x003E4C00  EA 0x7E4C00
// DIA: FT_New_Stream
// Compiland: .\Release_Assertions_Enabled\ftsystem.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall FT_New_Stream(FT_StreamRec_ *astream, _DWORD *a2)
{
  FILE *v6; // eax
  FILE *v7; // edi

  if ( a2 == nullptr )
    return 40;
  v6 = _fopen((const char *)astream, Mode);
  v7 = v6;
  if ( v6 == nullptr )
    return 1;
  _fseek(v6, 0, 2);
  a2[1] = _ftell(v7);
  _fseek(v7, 0, 0);
  a2[3] = v7;
  a2[4] = astream;
  a2[2] = 0;
  a2[5] = ft_io_stream;
  a2[6] = ft_close_stream;
  return 0;
}


// -----------------------------------------------------------------------------
// RVA 0x003E4C80  EA 0x7E4C80
// DIA: ft_close_stream
// Compiland: .\Release_Assertions_Enabled\ftsystem.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall ft_close_stream(int this)
{
  _fclose(*(FILE **)(this + 12));
  *(_DWORD *)(this + 12) = 0;
  *(_DWORD *)(this + 4) = 0;
  *(_DWORD *)this = 0;
}


// -----------------------------------------------------------------------------
// RVA 0x003E4CA0  EA 0x7E4CA0
// DIA: ft_io_stream
// Compiland: .\Release_Assertions_Enabled\ftsystem.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
size_t __fastcall ft_io_stream(unsigned int offset, int a2, unsigned __int8 *buffer, size_t count)
{
  FILE *v5; // esi

  v5 = *(FILE **)(offset + 12);
  _fseek(v5, a2, 0);
  return _fread(buffer, 1u, count, v5);
}


// -----------------------------------------------------------------------------
// RVA 0x003E4CD0  EA 0x7E4CD0
// DIA: FT_New_Memory
// Compiland: .\Release_Assertions_Enabled\ftsystem.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
FT_MemoryRec_ *__fastcall FT_New_Memory()
{
  FT_MemoryRec_ *result; // eax

  result = (FT_MemoryRec_ *)_malloc(0x10u);
  if ( result != nullptr )
  {
    result->user = nullptr;
    result->alloc = (void *(__fastcall *)(FT_MemoryRec_ *, int))ft_alloc;
    result->realloc = (void *(__fastcall *)(FT_MemoryRec_ *, int, int, void *))ft_realloc;
    result->free = (void (__fastcall *)(FT_MemoryRec_ *, void *))ft_free;
  }
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x003E4D00  EA 0x7E4D00
// DIA: ft_alloc
// Compiland: .\Release_Assertions_Enabled\ftsystem.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void *__fastcall ft_alloc(int size, size_t a2)
{
  return _malloc(a2);
}


// -----------------------------------------------------------------------------
// RVA 0x003E4D10  EA 0x7E4D10
// DIA: ft_realloc
// Compiland: .\Release_Assertions_Enabled\ftsystem.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void *__stdcall ft_realloc(size_t new_size, void *block)
{
  return _realloc(block, new_size);
}


// -----------------------------------------------------------------------------
// RVA 0x003E4D30  EA 0x7E4D30
// DIA: ft_free
// Compiland: .\Release_Assertions_Enabled\ftsystem.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall ft_free(void *block, void *a2)
{
  _free(a2);
}


// -----------------------------------------------------------------------------
// RVA 0x003E4D40  EA 0x7E4D40
// DIA: FT_Done_Memory
// Compiland: .\Release_Assertions_Enabled\ftsystem.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall FT_Done_Memory(void *this)
{
  _free(this);
}

