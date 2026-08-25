// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\FreeType\src\base\ftinit.c
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x003CCF90  EA 0x7CCF90
// DIA: FT_Add_Default_Modules
// Compiland: .\Release_Assertions_Enabled\ftinit.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall FT_Add_Default_Modules(FT_ModuleRec_ *this)
{
  FT_LibraryRec_ *v2; // edx
  const FT_Module_Class_ *const *v4; // esi

  v2 = (FT_LibraryRec_ *)&autohint_module_class;
  v4 = ft_default_modules;
  if ( &autohint_module_class != nullptr )
  {
    do
    {
      FT_Add_Module(this, v2);
      v2 = *((FT_LibraryRec_ **)v4++ + 1);
    }
    while ( v2 != nullptr );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x003CCFC0  EA 0x7CCFC0
// DIA: FT_Init_FreeType
// Compiland: .\Release_Assertions_Enabled\ftinit.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall FT_Init_FreeType(FT_LibraryRec_ **this)
{
  FT_LibraryRec_ **v3; // eax
  int v5; // esi

  v3 = (FT_LibraryRec_ **)FT_New_Memory();
  if ( v3 == nullptr )
    return 7;
  v5 = FT_New_Library(v3, this);
  if ( v5 == 0 )
    FT_Add_Default_Modules((FT_ModuleRec_ *)*this);
  return v5;
}


// -----------------------------------------------------------------------------
// RVA 0x003CCFF0  EA 0x7CCFF0
// DIA: FT_Done_FreeType
// Compiland: .\Release_Assertions_Enabled\ftinit.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __thiscall FT_Done_FreeType(int this)
{
  if ( this != 0 )
    FT_Done_Library(this);
  return 0;
}

