// Recovered by pdb_hexrays_export.py
// PDB source path: D:\build\buildWoW\engine\Source\Gxu\Gxu.cpp
// Hex-Rays output is reconstruction material, not original source text.


// -----------------------------------------------------------------------------
// RVA 0x001BA650  EA 0x5BA650
// DIA: GxuXformCreateProjection
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall GxuXformCreateProjection(int this, float fovyInRadians, float aspect, float minZ, float maxZ)
{
  double v7; // st7
  long double v8; // st7
  double v9; // st7
  float v10; // [esp+10h] [ebp-4h]

  if ( fovyInRadians <= 0.0 || fovyInRadians >= 3.1415927 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 83);
    SErrDisplayAppFatal(aFovyinradians0);
  }
  if ( aspect <= 0.0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 84);
    SErrDisplayAppFatal(aAspect00f);
  }
  if ( minZ >= (double)maxZ )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 85);
    SErrDisplayAppFatal(aMinzMaxz_0);
  }
  v7 = aspect * aspect + 1.0;
  v10 = v7;
  if ( v7 < 0.0 )
    SErrDisplayErrorFmt(0x85100000, aTempestCmathH, 412, 0, 1u, "\"%s\", %s = %f", aX0f, asc_832D0C, v10);
  *(_DWORD *)(this + 16) = 0;
  *(_DWORD *)(this + 32) = 0;
  *(_DWORD *)(this + 48) = 0;
  *(_DWORD *)(this + 4) = 0;
  *(_DWORD *)(this + 36) = 0;
  *(_DWORD *)(this + 52) = 0;
  *(_DWORD *)(this + 8) = 0;
  *(_DWORD *)(this + 24) = 0;
  *(_DWORD *)(this + 12) = 0;
  *(_DWORD *)(this + 28) = 0;
  *(_DWORD *)(this + 60) = 0;
  *(_DWORD *)(this + 44) = 1065353216;
  v8 = tan(fovyInRadians / sqrt(v10) * 0.5) * minZ;
  *(float *)this = minZ / (aspect * v8);
  *(float *)(this + 20) = minZ / v8;
  v9 = maxZ - minZ;
  *(float *)(this + 40) = (minZ + maxZ) / v9;
  *(float *)(this + 56) = minZ * maxZ * -2.0 / v9;
}


// -----------------------------------------------------------------------------
// RVA 0x001BA7E0  EA 0x5BA7E0
// DIA: GxuXformCreateOrtho
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __thiscall GxuXformCreateOrtho(int this, float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
  double v8; // st7
  double v9; // st7
  double v10; // st7

  if ( minX == maxX )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 136);
    SErrDisplayAppFatal(aMinxMaxx_0);
  }
  if ( minY == maxY )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 137);
    SErrDisplayAppFatal(aMinyMaxy_0);
  }
  if ( minZ >= (double)maxZ )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 138);
    SErrDisplayAppFatal(aMinzMaxz_0);
  }
  v8 = maxX - minX;
  *(_DWORD *)(this + 16) = 0;
  *(_DWORD *)(this + 32) = 0;
  *(_DWORD *)(this + 4) = 0;
  *(_DWORD *)(this + 36) = 0;
  *(_DWORD *)(this + 8) = 0;
  *(_DWORD *)(this + 24) = 0;
  *(_DWORD *)(this + 12) = 0;
  *(_DWORD *)(this + 28) = 0;
  *(_DWORD *)(this + 44) = 0;
  *(_DWORD *)(this + 60) = 1065353216;
  *(float *)this = 2.0 / v8;
  *(float *)(this + 48) = -((minX + maxX) / v8);
  v9 = maxY - minY;
  *(float *)(this + 20) = 2.0 / v9;
  *(float *)(this + 52) = -((minY + maxY) / v9);
  v10 = maxZ - minZ;
  *(float *)(this + 40) = 2.0 / v10;
  *(float *)(this + 56) = -((minZ + maxZ) / v10);
}


// -----------------------------------------------------------------------------
// RVA 0x001BA910  EA 0x5BA910
// DIA: GxuXformCreateOrtho
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuXformCreateOrtho(NTempest::C44Matrix *dst, int a2)
{
  GxuXformCreateOrtho(a2, dst->a0, dst->a1, dst->a2, dst->a3, dst->b0, dst->b1);
}


// -----------------------------------------------------------------------------
// RVA 0x001BA930  EA 0x5BA930
// DIA: GxuXformCreateLookAtSgCompat
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuXformCreateLookAtSgCompat(
        const NTempest::C3Vector *center,
        float *a2,
        const NTempest::C3Vector *up,
        NTempest::C44Matrix *dst)
{
  char v8; // c0
  long double v9; // st7
  long double v10; // st7
  long double v11; // st7
  float v12; // edx
  double v13; // st7
  float v14; // ecx
  double v15; // st7
  float v16; // edx
  double v17; // st7
  float v18; // eax
  float v19; // ecx
  double v20; // st7
  float v21; // edx
  double v22; // st7
  float v23; // eax
  float x; // [esp+0h] [ebp-50h]
  float xa; // [esp+0h] [ebp-50h]
  float xb; // [esp+0h] [ebp-50h]
  float xc; // [esp+0h] [ebp-50h]
  float y; // [esp+4h] [ebp-4Ch]
  float ya; // [esp+4h] [ebp-4Ch]
  float yb; // [esp+4h] [ebp-4Ch]
  float yc; // [esp+4h] [ebp-4Ch]
  float z; // [esp+8h] [ebp-48h]
  float za; // [esp+8h] [ebp-48h]
  float zb; // [esp+8h] [ebp-48h]
  float zc; // [esp+8h] [ebp-48h]
  float zd; // [esp+8h] [ebp-48h]
  float ze; // [esp+8h] [ebp-48h]
  float zf; // [esp+8h] [ebp-48h]
  NTempest::C3Vector move; // [esp+18h] [ebp-38h] BYREF
  NTempest::C3Vector yv; // [esp+24h] [ebp-2Ch] BYREF
  NTempest::C3Vector xv; // [esp+30h] [ebp-20h] BYREF
  NTempest::C3Vector zv; // [esp+3Ch] [ebp-14h] BYREF
  float v43; // [esp+48h] [ebp-8h]
  float v44; // [esp+4Ch] [ebp-4h]
  float dsta; // [esp+5Ch] [ebp+Ch]

  dst->d3 = 1.0;
  dst->c2 = 1.0;
  dst->b1 = 1.0;
  dst->a0 = 1.0;
  dst->d2 = 0.0;
  dst->d1 = 0.0;
  dst->d0 = 0.0;
  dst->c3 = 0.0;
  dst->c1 = 0.0;
  dst->c0 = 0.0;
  dst->b3 = 0.0;
  dst->b2 = 0.0;
  dst->b0 = 0.0;
  dst->a3 = 0.0;
  dst->a2 = 0.0;
  dst->a1 = 0.0;
  z = a2[2] - center->z;
  y = a2[1] - center->y;
  x = *a2 - center->x;
  NTempest::C3Vector::C3Vector(&zv, x, y, z);
  v44 = zv.z * zv.z;
  dsta = zv.y * zv.y;
  v43 = zv.x * zv.x;
  if ( dsta + v44 + v43 < 0.0099999998 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 216);
    SErrDisplayAppFatal(aZvSquaredmag00);
  }
  if ( v8 != 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 217);
    SErrDisplayAppFatal(aUpSquaredmag00);
  }
  za = dsta + v44 + v43;
  v9 = 1.0 / NTempest::CMath::sqrt_(za);
  zv.x = zv.x * v9;
  zv.y = zv.y * v9;
  zv.z = v9 * zv.z;
  zb = zv.x * up->y - zv.y * up->x;
  ya = zv.z * up->x - zv.x * up->z;
  xa = zv.y * up->z - zv.z * up->y;
  NTempest::C3Vector::C3Vector(&xv, xa, ya, zb);
  zc = xv.z * xv.z + xv.y * xv.y + xv.x * xv.x;
  v10 = 1.0 / NTempest::CMath::sqrt_(zc);
  xv.x = xv.x * v10;
  xv.y = xv.y * v10;
  xv.z = v10 * xv.z;
  zd = zv.y * xv.x - xv.y * zv.x;
  yb = xv.z * zv.x - zv.z * xv.x;
  xb = zv.z * xv.y - xv.z * zv.y;
  NTempest::C3Vector::C3Vector(&yv, xb, yb, zd);
  ze = yv.z * yv.z + yv.y * yv.y + yv.x * yv.x;
  v11 = 1.0 / NTempest::CMath::sqrt_(ze);
  yv.x = yv.x * v11;
  yv.y = yv.y * v11;
  yv.z = v11 * yv.z;
  v12 = yv.x;
  move.z = zv.x;
  v13 = xv.y;
  dst->a0 = xv.x;
  v14 = move.z;
  move.x = v13;
  v15 = yv.y;
  dst->a1 = v12;
  v16 = move.x;
  move.y = v15;
  v17 = zv.y;
  dst->a2 = v14;
  v18 = move.y;
  move.z = v17;
  v19 = move.z;
  move.x = xv.z;
  v20 = yv.z;
  dst->b0 = v16;
  v21 = move.x;
  dst->b1 = v18;
  dst->b2 = v19;
  move.y = v20;
  v22 = zv.z;
  v23 = move.y;
  dst->c0 = v21;
  dst->c1 = v23;
  move.z = v22;
  dst->c2 = move.z;
  zf = -center->z;
  yc = -center->y;
  xc = -center->x;
  NTempest::C3Vector::C3Vector(&move, xc, yc, zf);
  NTempest::C44Matrix::Translate(dst, &move);
}


// -----------------------------------------------------------------------------
// RVA 0x001BAC20  EA 0x5BAC20
// DIA: $E2
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E2_265()
{
  return _E1_265();
}


// -----------------------------------------------------------------------------
// RVA 0x001BAC30  EA 0x5BAC30
// DIA: $E1
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int _E1_265()
{
  INFINITY_265 = INFINITY;
  return 2139095040;
}


// -----------------------------------------------------------------------------
// RVA 0x001BAC40  EA 0x5BAC40
// DIA: GxuXformCreateLookAtXXX
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuXformCreateLookAtXXX(
        const NTempest::C3Vector *center,
        float *a2,
        const NTempest::C3Vector *up,
        NTempest::C44Matrix *dst)
{
  char v8; // c0
  long double v9; // st7
  long double v10; // st7
  long double v11; // st7
  float v12; // edx
  double v13; // st7
  float v14; // ecx
  double v15; // st7
  float v16; // edx
  double v17; // st7
  float v18; // eax
  float v19; // ecx
  double v20; // st7
  float v21; // edx
  double v22; // st7
  float v23; // eax
  float x; // [esp+0h] [ebp-50h]
  float xa; // [esp+0h] [ebp-50h]
  float xb; // [esp+0h] [ebp-50h]
  float xc; // [esp+0h] [ebp-50h]
  float y; // [esp+4h] [ebp-4Ch]
  float ya; // [esp+4h] [ebp-4Ch]
  float yb; // [esp+4h] [ebp-4Ch]
  float yc; // [esp+4h] [ebp-4Ch]
  float z; // [esp+8h] [ebp-48h]
  float za; // [esp+8h] [ebp-48h]
  float zb; // [esp+8h] [ebp-48h]
  float zc; // [esp+8h] [ebp-48h]
  float zd; // [esp+8h] [ebp-48h]
  float ze; // [esp+8h] [ebp-48h]
  float zf; // [esp+8h] [ebp-48h]
  NTempest::C3Vector move; // [esp+18h] [ebp-38h] BYREF
  NTempest::C3Vector yv; // [esp+24h] [ebp-2Ch] BYREF
  NTempest::C3Vector xv; // [esp+30h] [ebp-20h] BYREF
  NTempest::C3Vector zv; // [esp+3Ch] [ebp-14h] BYREF
  float v43; // [esp+48h] [ebp-8h]
  float v44; // [esp+4Ch] [ebp-4h]
  float dsta; // [esp+5Ch] [ebp+Ch]

  dst->d3 = 1.0;
  dst->c2 = 1.0;
  dst->b1 = 1.0;
  dst->a0 = 1.0;
  dst->d2 = 0.0;
  dst->d1 = 0.0;
  dst->d0 = 0.0;
  dst->c3 = 0.0;
  dst->c1 = 0.0;
  dst->c0 = 0.0;
  dst->b3 = 0.0;
  dst->b2 = 0.0;
  dst->b0 = 0.0;
  dst->a3 = 0.0;
  dst->a2 = 0.0;
  dst->a1 = 0.0;
  z = a2[2] - center->z;
  y = a2[1] - center->y;
  x = *a2 - center->x;
  NTempest::C3Vector::C3Vector(&zv, x, y, z);
  v44 = zv.z * zv.z;
  dsta = zv.y * zv.y;
  v43 = zv.x * zv.x;
  if ( dsta + v44 + v43 < 0.0099999998 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 256);
    SErrDisplayAppFatal(aZvSquaredmag00);
  }
  if ( v8 != 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 257);
    SErrDisplayAppFatal(aUpSquaredmag00);
  }
  za = dsta + v44 + v43;
  v9 = 1.0 / NTempest::CMath::sqrt_(za);
  zv.x = zv.x * v9;
  zv.y = zv.y * v9;
  zv.z = v9 * zv.z;
  zb = zv.y * up->x - zv.x * up->y;
  ya = zv.x * up->z - zv.z * up->x;
  xa = zv.z * up->y - zv.y * up->z;
  NTempest::C3Vector::C3Vector(&xv, xa, ya, zb);
  zc = xv.z * xv.z + xv.y * xv.y + xv.x * xv.x;
  v10 = 1.0 / NTempest::CMath::sqrt_(zc);
  xv.x = xv.x * v10;
  xv.y = xv.y * v10;
  xv.z = v10 * xv.z;
  zd = xv.y * zv.x - zv.y * xv.x;
  yb = zv.z * xv.x - xv.z * zv.x;
  xb = xv.z * zv.y - zv.z * xv.y;
  NTempest::C3Vector::C3Vector(&yv, xb, yb, zd);
  ze = yv.z * yv.z + yv.y * yv.y + yv.x * yv.x;
  v11 = 1.0 / NTempest::CMath::sqrt_(ze);
  yv.x = yv.x * v11;
  yv.y = yv.y * v11;
  yv.z = v11 * yv.z;
  v12 = yv.x;
  move.z = zv.x;
  v13 = xv.y;
  dst->a0 = xv.x;
  v14 = move.z;
  move.x = v13;
  v15 = yv.y;
  dst->a1 = v12;
  v16 = move.x;
  move.y = v15;
  v17 = zv.y;
  dst->a2 = v14;
  v18 = move.y;
  move.z = v17;
  v19 = move.z;
  move.x = xv.z;
  v20 = yv.z;
  dst->b0 = v16;
  v21 = move.x;
  dst->b1 = v18;
  dst->b2 = v19;
  move.y = v20;
  v22 = zv.z;
  v23 = move.y;
  dst->c0 = v21;
  dst->c1 = v23;
  move.z = v22;
  dst->c2 = move.z;
  zf = -center->z;
  yc = -center->y;
  xc = -center->x;
  NTempest::C3Vector::C3Vector(&move, xc, yc, zf);
  NTempest::C44Matrix::Translate(dst, &move);
}


// -----------------------------------------------------------------------------
// RVA 0x001BAF30  EA 0x5BAF30
// DIA: GxuXformCalcFrustumCorners
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuXformCalcFrustumCorners(
        const NTempest::C44Matrix *proj,
        NTempest::C44Matrix *edx0,
        NTempest::C3Vector *corners)
{
  const NTempest::C44Matrix *v6; // eax
  float v7; // eax
  float v8; // edx
  const NTempest::C44Matrix *v9; // eax
  float v10; // eax
  float v11; // edx
  const NTempest::C44Matrix *v12; // eax
  float v13; // eax
  float v14; // edx
  const NTempest::C44Matrix *v15; // eax
  float v16; // eax
  float v17; // edx
  const NTempest::C44Matrix *v18; // eax
  float v19; // eax
  float v20; // edx
  const NTempest::C44Matrix *v21; // eax
  float v22; // eax
  float v23; // edx
  const NTempest::C44Matrix *v24; // eax
  float v25; // eax
  float v26; // edx
  const NTempest::C44Matrix *v27; // eax
  double v28; // st7
  const NTempest::C44Matrix *v29; // eax
  NTempest::C3Vector *v30; // esi
  float y; // edx
  float z; // ecx
  const NTempest::C4Vector *v33; // edx
  const NTempest::C44Matrix *v34; // eax
  double a2; // st7
  float v36; // eax
  float v37; // eax
  const NTempest::C44Matrix *v38; // eax
  double v39; // st7
  float v40; // ecx
  float v41; // ecx
  const NTempest::C44Matrix *v42; // eax
  double v43; // st7
  float v44; // edx
  float v45; // edx
  double v46; // st7
  const NTempest::C44Matrix *v47; // eax
  double v48; // st7
  float v49; // ecx
  const NTempest::C44Matrix *v50; // eax
  double v51; // st7
  float v52; // edx
  float v53; // edx
  const NTempest::C44Matrix *v54; // eax
  double v55; // st7
  float v56; // eax
  float v57; // eax
  const NTempest::C44Matrix *v58; // eax
  double v59; // st7
  float v60; // eax
  float v61; // ecx
  float ___ReturnUdt; // [esp+0h] [ebp-104h]
  float ___ReturnUdta; // [esp+0h] [ebp-104h]
  NTempest::C44Matrix projInv; // [esp+Ch] [ebp-F8h] BYREF
  NTempest::C44Matrix viewInv; // [esp+4Ch] [ebp-B8h] BYREF
  NTempest::C44Matrix inv; // [esp+8Ch] [ebp-78h] BYREF
  _BYTE v67[16]; // [esp+CCh] [ebp-38h] BYREF
  const NTempest::C4Vector *v68; // [esp+DCh] [ebp-28h]
  float zMin; // [esp+E0h] [ebp-24h]
  float zMax; // [esp+E4h] [ebp-20h]
  float v71; // [esp+E8h] [ebp-1Ch] BYREF
  float v72; // [esp+ECh] [ebp-18h]
  float v73; // [esp+F0h] [ebp-14h]
  float v74; // [esp+F4h] [ebp-10h]
  NTempest::C3Vector v75; // [esp+F8h] [ebp-Ch] BYREF
  float cornersa; // [esp+10Ch] [ebp+8h]

  ___ReturnUdt = NTempest::C44Matrix::Determinant(proj);
  NTempest::C44Matrix::Inverse(proj, &viewInv, ___ReturnUdt);
  ___ReturnUdta = NTempest::C44Matrix::Determinant(edx0);
  NTempest::C44Matrix::Inverse(edx0, &projInv, ___ReturnUdta);
  NTempest::operator*(&inv, &projInv, &viewInv);
  if ( fabs(edx0->d3 - 1.0) >= 0.00000023841858 )
  {
    v28 = -(edx0->d2 / (edx0->c2 + 1.0));
    zMin = v28;
    zMax = -(edx0->d2 / (edx0->c2 - 1.0));
    v71 = -v28;
    v72 = v71;
    *(float *)&v68 = v71;
    v73 = v71;
    v74 = v28;
    v29 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v29->a0, v29->a1, v29->a2);
    v30 = corners;
    y = v75.y;
    corners->x = v75.x;
    z = v75.z;
    corners->y = y;
    v33 = v68;
    corners->z = z;
    v71 = *(float *)&v33;
    v72 = zMin;
    v73 = *(float *)&v33;
    v74 = zMin;
    v34 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    v75.x = v34->a0;
    v75.y = v34->a1;
    a2 = v34->a2;
    v36 = v75.y;
    corners[1].x = v75.x;
    corners[1].y = v36;
    v37 = zMin;
    v75.z = a2;
    corners[1].z = v75.z;
    v72 = v37;
    v73 = *(float *)&v68;
    v71 = v37;
    v74 = v37;
    v38 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    v75.x = v38->a0;
    v75.y = v38->a1;
    v39 = v38->a2;
    v40 = v75.y;
    corners[2].x = v75.x;
    corners[2].y = v40;
    v41 = zMin;
    v75.z = v39;
    corners[2].z = v75.z;
    v72 = *(float *)&v68;
    v71 = v41;
    v74 = v41;
    v73 = *(float *)&v68;
    v42 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    v75.x = v42->a0;
    v75.y = v42->a1;
    v43 = v42->a2;
    v44 = v75.y;
    corners[3].x = v75.x;
    corners[3].y = v44;
    v45 = zMax;
    v75.z = v43;
    v46 = zMax;
    corners[3].z = v75.z;
    v71 = -v46;
    cornersa = v71;
    v73 = v45;
    v72 = v71;
    v74 = v45;
    v47 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    v75.x = v47->a0;
    v75.y = v47->a1;
    v48 = v47->a2;
    v49 = v75.y;
    v30[4].x = v75.x;
    v30[4].y = v49;
    v75.z = v48;
    v30[4].z = v75.z;
    v71 = cornersa;
    v72 = zMax;
    v73 = zMax;
    v74 = zMax;
    v50 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    v75.x = v50->a0;
    v75.y = v50->a1;
    v51 = v50->a2;
    v52 = v75.y;
    v30[5].x = v75.x;
    v30[5].y = v52;
    v53 = zMax;
    v75.z = v51;
    v30[5].z = v75.z;
    v72 = v53;
    v71 = v53;
    v73 = v53;
    v74 = v53;
    v54 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    v75.x = v54->a0;
    v75.y = v54->a1;
    v55 = v54->a2;
    v56 = v75.y;
    v30[6].x = v75.x;
    v30[6].y = v56;
    v57 = zMax;
    v75.z = v55;
    v30[6].z = v75.z;
    v72 = cornersa;
    v73 = v57;
    v71 = v57;
    v74 = v57;
    v58 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    v75.x = v58->a0;
    v75.y = v58->a1;
    v59 = v58->a2;
    v60 = v75.y;
    v30 += 7;
    v30->x = v75.x;
    v75.z = v59;
    v61 = v75.z;
    v30->y = v60;
    v30->z = v61;
  }
  else
  {
    v71 = -1.0;
    v72 = -1.0;
    v73 = -1.0;
    v74 = 1.0;
    v6 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v6->a0, v6->a1, v6->a2);
    v7 = v75.y;
    corners->x = v75.x;
    v8 = v75.z;
    corners->y = v7;
    corners->z = v8;
    v71 = -1.0;
    v72 = 1.0;
    v73 = -1.0;
    v74 = 1.0;
    v9 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v9->a0, v9->a1, v9->a2);
    v10 = v75.y;
    corners[1].x = v75.x;
    v11 = v75.z;
    corners[1].y = v10;
    corners[1].z = v11;
    v71 = 1.0;
    v72 = 1.0;
    v73 = -1.0;
    v74 = 1.0;
    v12 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v12->a0, v12->a1, v12->a2);
    v13 = v75.y;
    corners[2].x = v75.x;
    v14 = v75.z;
    corners[2].y = v13;
    corners[2].z = v14;
    v71 = 1.0;
    v72 = -1.0;
    v73 = -1.0;
    v74 = 1.0;
    v15 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v15->a0, v15->a1, v15->a2);
    v16 = v75.y;
    corners[3].x = v75.x;
    v17 = v75.z;
    corners[3].y = v16;
    corners[3].z = v17;
    v71 = -1.0;
    v72 = -1.0;
    v73 = 1.0;
    v74 = 1.0;
    v18 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v18->a0, v18->a1, v18->a2);
    v19 = v75.y;
    corners[4].x = v75.x;
    v20 = v75.z;
    corners[4].y = v19;
    corners[4].z = v20;
    v71 = -1.0;
    v72 = 1.0;
    v73 = 1.0;
    v74 = 1.0;
    v21 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v21->a0, v21->a1, v21->a2);
    v22 = v75.y;
    corners[5].x = v75.x;
    v23 = v75.z;
    corners[5].y = v22;
    corners[5].z = v23;
    v71 = 1.0;
    v72 = 1.0;
    v73 = 1.0;
    v74 = 1.0;
    v24 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v24->a0, v24->a1, v24->a2);
    v25 = v75.y;
    corners[6].x = v75.x;
    v26 = v75.z;
    corners[6].y = v25;
    corners[6].z = v26;
    v71 = 1.0;
    v72 = -1.0;
    v73 = 1.0;
    v74 = 1.0;
    v27 = NTempest::operator*((const NTempest::C44Matrix *)v67, &v71, &inv);
    NTempest::C3Vector::C3Vector(&v75, v27->a0, v27->a1, v27->a2);
    corners[7] = v75;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BB4B0  EA 0x5BB4B0
// DIA: GxuXformCalcFrustumPlanes
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuXformCalcFrustumPlanes(NTempest::C44Matrix *planes, NTempest::C4Vector *a2)
{
  float v5; // ecx
  float v6; // eax
  float v7; // ecx
  float v8; // eax
  float v9; // edx
  float v10; // eax
  float v11; // edx
  float v12; // ecx
  float v13; // edx
  float v14; // ecx
  float v15; // eax
  float v16; // ecx
  const NTempest::C4Vector *v17; // eax
  NTempest::C4Vector *v18; // eax
  const NTempest::C4Vector *v19; // eax
  int i; // esi
  float x; // [esp+0h] [ebp-5Ch]
  float xa; // [esp+0h] [ebp-5Ch]
  float xb; // [esp+0h] [ebp-5Ch]
  float xc; // [esp+0h] [ebp-5Ch]
  float xd; // [esp+0h] [ebp-5Ch]
  float xe; // [esp+0h] [ebp-5Ch]
  float y; // [esp+4h] [ebp-58h]
  float ya; // [esp+4h] [ebp-58h]
  float yb; // [esp+4h] [ebp-58h]
  float yc; // [esp+4h] [ebp-58h]
  float yd; // [esp+4h] [ebp-58h]
  float ye; // [esp+4h] [ebp-58h]
  float z; // [esp+8h] [ebp-54h]
  float za; // [esp+8h] [ebp-54h]
  float zb; // [esp+8h] [ebp-54h]
  float zc; // [esp+8h] [ebp-54h]
  float zd; // [esp+8h] [ebp-54h]
  float ze; // [esp+8h] [ebp-54h]
  float w; // [esp+Ch] [ebp-50h]
  float wa; // [esp+Ch] [ebp-50h]
  float wb; // [esp+Ch] [ebp-50h]
  float wc; // [esp+Ch] [ebp-50h]
  float wd; // [esp+Ch] [ebp-50h]
  float we; // [esp+Ch] [ebp-50h]
  NTempest::C4Vector result; // [esp+18h] [ebp-44h] BYREF
  NTempest::C4Vector v46; // [esp+28h] [ebp-34h] BYREF
  NTempest::C4Vector l; // [esp+38h] [ebp-24h] BYREF
  NTempest::C4Vector v48; // [esp+48h] [ebp-14h] BYREF
  float mag; // [esp+58h] [ebp-4h]

  NTempest::C4Vector::C4Vector(&l, planes->a3, planes->b3, planes->c3, planes->d3);
  NTempest::C4Vector::C4Vector(&v48, planes->a0, planes->b0, planes->c0, planes->d0);
  w = v48.w - l.w;
  z = v48.z - l.z;
  y = v48.y - l.y;
  x = v48.x - l.x;
  NTempest::C4Vector::C4Vector(&v46, x, y, z, w);
  v5 = v46.y;
  a2->x = v46.x;
  v6 = v46.z;
  a2->y = v5;
  v7 = v46.w;
  a2->z = v6;
  a2->w = v7;
  NTempest::C4Vector::C4Vector(&v48, planes->a3, planes->b3, planes->c3, planes->d3);
  NTempest::C4Vector::C4Vector(&v46, planes->a0, planes->b0, planes->c0, planes->d0);
  wa = -v46.w;
  za = -v46.z;
  ya = -v46.y;
  xa = -v46.x;
  NTempest::C4Vector::C4Vector(&l, xa, ya, za, wa);
  wb = l.w - v48.w;
  zb = l.z - v48.z;
  yb = l.y - v48.y;
  xb = l.x - v48.x;
  NTempest::C4Vector::C4Vector(&result, xb, yb, zb, wb);
  v8 = result.y;
  a2[1].x = result.x;
  v9 = result.z;
  a2[1].y = v8;
  v10 = result.w;
  a2[1].z = v9;
  a2[1].w = v10;
  NTempest::C4Vector::C4Vector(&v46, planes->a3, planes->b3, planes->c3, planes->d3);
  NTempest::C4Vector::C4Vector(&result, planes->a1, planes->b1, planes->c1, planes->d1);
  wc = result.w - v46.w;
  zc = result.z - v46.z;
  yc = result.y - v46.y;
  xc = result.x - v46.x;
  NTempest::C4Vector::C4Vector(&l, xc, yc, zc, wc);
  v11 = l.y;
  a2[2].x = l.x;
  v12 = l.z;
  a2[2].y = v11;
  v13 = l.w;
  a2[2].z = v12;
  a2[2].w = v13;
  NTempest::C4Vector::C4Vector(&l, planes->a3, planes->b3, planes->c3, planes->d3);
  NTempest::C4Vector::C4Vector(&result, planes->a1, planes->b1, planes->c1, planes->d1);
  wd = -result.w;
  zd = -result.z;
  yd = -result.y;
  xd = -result.x;
  NTempest::C4Vector::C4Vector(&v46, xd, yd, zd, wd);
  we = v46.w - l.w;
  ze = v46.z - l.z;
  ye = v46.y - l.y;
  xe = v46.x - l.x;
  NTempest::C4Vector::C4Vector(&v48, xe, ye, ze, we);
  v14 = v48.y;
  a2[3].x = v48.x;
  v15 = v48.z;
  a2[3].y = v14;
  v16 = v48.w;
  a2[3].z = v15;
  a2[3].w = v16;
  NTempest::C4Vector::C4Vector(&result, planes->a3, planes->b3, planes->c3, planes->d3);
  v17 = NTempest::C4Vector::C4Vector(&v46, planes->a2, planes->b2, planes->c2, planes->d2);
  a2[4] = *NTempest::operator-(v17, &l);
  NTempest::C44Matrix::Col3(planes, &result);
  v18 = NTempest::C44Matrix::Col2(planes, &l);
  v19 = NTempest::C4Vector::operator-(v18, &v46);
  a2[5] = *NTempest::operator-(v19, &v48);
  for ( i = 6; i != 0; --i )
  {
    mag = a2->z * a2->z + a2->x * a2->x + a2->y * a2->y;
    mag = 1.0 / NTempest::CMath::sqrt_(mag);
    NTempest::C4Vector::operator*=(a2++, mag);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BB800  EA 0x5BB800
// DIA: GxuXformCalcFrustumBounds
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuXformCalcFrustumBounds(
        const NTempest::C44Matrix *proj,
        NTempest::C44Matrix *a2,
        NTempest::C3Vector *minBound,
        NTempest::C3Vector *maxBound)
{
  float *p_z; // eax
  int i; // esi
  float x; // eax
  float y; // ecx
  float z; // edi
  float *p_y; // ecx
  int j; // edi
  double v12; // st7
  double v13; // st7
  double v14; // st7
  double v15; // st7
  double v16; // st7
  double v17; // st7
  NTempest::C3Vector corners[8]; // [esp+8h] [ebp-60h] BYREF

  p_z = &corners[0].z;
  for ( i = 8; i != 0; --i )
  {
    *(p_z - 2) = 0.0;
    *(p_z - 1) = 0.0;
    *p_z = 0.0;
    p_z += 3;
  }
  GxuXformCalcFrustumCorners(proj, a2, corners);
  x = corners[0].x;
  y = corners[0].y;
  z = corners[0].z;
  *maxBound = corners[0];
  minBound->x = x;
  minBound->y = y;
  minBound->z = z;
  p_y = &corners[0].y;
  for ( j = 8; j != 0; --j )
  {
    if ( *(p_y - 1) <= (double)minBound->x )
      v12 = *(p_y - 1);
    else
      v12 = minBound->x;
    minBound->x = v12;
    if ( *p_y <= (double)minBound->y )
      v13 = *p_y;
    else
      v13 = minBound->y;
    minBound->y = v13;
    if ( p_y[1] <= (double)minBound->z )
      v14 = p_y[1];
    else
      v14 = minBound->z;
    minBound->z = v14;
    if ( *(p_y - 1) >= (double)maxBound->x )
      v15 = *(p_y - 1);
    else
      v15 = maxBound->x;
    maxBound->x = v15;
    if ( *p_y >= (double)maxBound->y )
      v16 = *p_y;
    else
      v16 = maxBound->y;
    maxBound->y = v16;
    if ( p_y[1] >= (double)maxBound->z )
      v17 = p_y[1];
    else
      v17 = maxBound->z;
    p_y += 3;
    maxBound->z = v17;
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BB900  EA 0x5BB900
// DIA: GxuXformCalc2dScreenCoords
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuXformCalc2dScreenCoords(const NTempest::C3Vector *src, int a2, NTempest::C3Vector *dst)
{
  double v6; // st7
  double v7; // st6
  int v8; // ecx
  float *p_y; // eax
  int v10; // edx
  double v11; // st5
  double v12; // st4
  double v13; // st5
  double v14; // st4
  double v15; // st5
  NTempest::C44Matrix viewProj; // [esp+8h] [ebp-68h] BYREF
  NTempest::CRect winRect; // [esp+48h] [ebp-28h] BYREF
  NTempest::C3Vector vpMax; // [esp+58h] [ebp-18h] BYREF
  NTempest::C3Vector vpMin; // [esp+64h] [ebp-Ch] BYREF

  memset((void *)&winRect, 0, sizeof(winRect));
  memset((void *)&vpMin, 0, sizeof(vpMin));
  memset((void *)&vpMax, 0, sizeof(vpMax));
  viewProj.a0 = 1.0;
  memset(&viewProj.a1, 0, 16);
  viewProj.b1 = 1.0;
  memset(&viewProj.b2, 0, 16);
  viewProj.c2 = 1.0;
  memset(&viewProj.c3, 0, 16);
  viewProj.d3 = 1.0;
  GxCapsWindowSize(&winRect);
  GxXformViewport((CBoundingBox *)&vpMin, &vpMax.x, &vpMin.y, &vpMax.y, &vpMin.z, &vpMax.z);
  GxXformViewProj(&viewProj);
  if ( src != nullptr )
  {
    v6 = vpMax.x - vpMin.x;
    v7 = vpMax.y - vpMin.y;
    vpMax.z = 1.0;
    v8 = a2;
    p_y = &dst->y;
    v10 = a2 - (_DWORD)dst;
    do
    {
      v8 += 12;
      p_y += 3;
      src = (const NTempest::C3Vector *)((char *)src - 1);
      *(p_y - 4) = viewProj.a0 * *(float *)(v8 - 12) + viewProj.b0 * *(float *)((char *)p_y + v10 - 12) + viewProj.d0;
      v11 = viewProj.a1 * *(float *)(v8 - 12);
      v12 = viewProj.b1 * *(float *)((char *)p_y + v10 - 12);
      *(p_y - 2) = 1.0;
      *(p_y - 3) = v11 + v12 + viewProj.d1;
      v13 = (*(p_y - 4) + 1.0) * v6 * 0.5 + vpMin.x;
      *(p_y - 4) = v13;
      v14 = (*(p_y - 3) + 1.0) * v7 * 0.5 + vpMin.y;
      *(p_y - 3) = v14;
      vpMax.x = v13 * winRect.r;
      v15 = v14 * winRect.b;
      *(p_y - 4) = vpMax.x;
      vpMax.y = winRect.b - v15;
      *(p_y - 3) = vpMax.y;
      *(p_y - 2) = 1.0;
    }
    while ( src != nullptr );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BBAC0  EA 0x5BBAC0
// DIA: GxuTexScale
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuTexScale(
        EGxTexFormat srcFormat,
        int a2,
        unsigned int srcW,
        unsigned int srcH,
        unsigned int srcStrideInBytes,
        char *dstPixels,
        EGxTexFormat dstFormat,
        unsigned int dstW,
        unsigned int dstH,
        unsigned int dstStrideInBytes)
{
  EGxTexFormat v12; // ebx
  unsigned int v13; // edi
  unsigned int v14; // eax
  char *v15; // edx
  unsigned int v16; // esi
  bool v17; // zf
  unsigned int v18; // edi
  unsigned int v19; // eax
  char *v20; // edx
  unsigned int v21; // esi
  unsigned int srcWa; // [esp+18h] [ebp+8h]
  unsigned int srcWb; // [esp+18h] [ebp+8h]
  unsigned int srcHa; // [esp+1Ch] [ebp+Ch]
  unsigned int srcHb; // [esp+1Ch] [ebp+Ch]
  EGxTexFormat dstFormata; // [esp+28h] [ebp+18h]
  EGxTexFormat dstFormatb; // [esp+28h] [ebp+18h]

  v12 = srcFormat;
  if ( a2 != dstFormat )
    SErrDisplayError(0x85100000, aDBuildBuildwow_242, 507, aSrcformatDstfo, 0, 1u);
  if ( v12 == GxTex_Unknown )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 510);
    SErrDisplayAppFatal(aSrcpixels);
  }
  if ( a2 >= 8 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 511);
    SErrDisplayAppFatal(aSrcformatGxtex);
  }
  if ( dstPixels == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 512);
    SErrDisplayAppFatal(aDstpixels);
  }
  if ( dstFormat >= GxTexFormats_Last )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 513);
    SErrDisplayAppFatal(aDstformatGxtex);
  }
  if ( a2 == 1 && dstFormat == GxTex_Argb8888 )
  {
    v13 = dstW;
    dstFormata = (srcW << 16) / dstW;
    srcHa = (srcH << 16) / dstH;
    v14 = 0;
    v15 = dstPixels;
    srcWa = dstH;
    do
    {
      v16 = 0;
      if ( v13 != 0 )
      {
        do
        {
          *(_DWORD *)v15 = *(_DWORD *)(v12 + srcStrideInBytes * HIWORD(v14) + 4 * HIWORD(v16));
          v15 += 4;
          v16 += dstFormata;
          --v13;
        }
        while ( v13 != 0 );
        v13 = dstW;
        v12 = srcFormat;
      }
      v14 += srcHa;
      v15 = &dstPixels[dstStrideInBytes];
      v17 = srcWa == 1;
      dstPixels += dstStrideInBytes;
      --srcWa;
    }
    while ( !v17 );
  }
  else
  {
    v18 = dstW;
    dstFormatb = (srcW << 16) / dstW;
    srcHb = (srcH << 16) / dstH;
    v19 = 0;
    v20 = dstPixels;
    srcWb = dstH;
    do
    {
      v21 = 0;
      if ( v18 != 0 )
      {
        do
        {
          *(_WORD *)v20 = *(_WORD *)(v12 + srcStrideInBytes * HIWORD(v19) + 2 * HIWORD(v21));
          v20 += 2;
          v21 += dstFormatb;
          --v18;
        }
        while ( v18 != 0 );
        v18 = dstW;
        v12 = srcFormat;
      }
      v19 += srcHb;
      v20 = &dstPixels[dstStrideInBytes];
      v17 = srcWb == 1;
      dstPixels += dstStrideInBytes;
      --srcWb;
    }
    while ( !v17 );
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BBCD0  EA 0x5BBCD0
// DIA: GxuUpdateSingleColorTexture
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuUpdateSingleColorTexture(
        unsigned int w,
        int a2,
        unsigned int h,
        unsigned int d,
        unsigned int mipLevel,
        void *userArg,
        unsigned int *texelStrideInBytes,
        const void **texels)
{
  if ( (__S19__1__GxuUpdateSingleColorTexture__YIXW4EGxTexCommand__IIIIPAXAAIAAPBX_Z_4EA & 1) == 0 )
  {
    __S19__1__GxuUpdateSingleColorTexture__YIXW4EGxTexCommand__IIIIPAXAAIAAPBX_Z_4EA |= 1u;
    memset(image, 0, sizeof(image));
    atexit(_E20_41);
  }
  if ( w != 0 )
  {
    if ( w == 1 )
    {
      *texelStrideInBytes = 4 * a2;
      *texels = image;
    }
  }
  else
  {
    memset32(image, (int)userArg, 0x40u);
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BBD50  EA 0x5BBD50
// DIA: $E20
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E20_41()
{
  ;
}


// -----------------------------------------------------------------------------
// RVA 0x001BBD60  EA 0x5BBD60
// DIA: GxuTestRayAndSphere
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuTestRayAndSphere(
        const NTempest::C3Vector *rayDirection,
        float *a2,
        const NTempest::C3Vector *sphereCenter,
        float sphereRadius,
        float *distance)
{
  double v6; // st7
  double v7; // st6
  float v9; // [esp+8h] [ebp-18h]
  float v10; // [esp+14h] [ebp-Ch]
  float v11; // [esp+18h] [ebp-8h]
  float v12; // [esp+1Ch] [ebp-4h]

  *distance = INFINITY_265;
  if ( fabs(a2[1] * a2[1] + a2[2] * a2[2] + *a2 * *a2 - 1.0) >= 0.0009765625 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 606);
    SErrDisplayAppFatal(aIsunitvectorRa);
  }
  v6 = (sphereCenter->x - rayDirection->x) * *a2
     + (sphereCenter->z - rayDirection->z) * a2[2]
     + (sphereCenter->y - rayDirection->y) * a2[1];
  if ( v6 < -sphereRadius )
    return 0;
  v12 = v6 * a2[2];
  v9 = v6 * *a2 + rayDirection->x;
  v10 = v9 - sphereCenter->x;
  v11 = v6 * a2[1] + rayDirection->y - sphereCenter->y;
  v7 = v12 + rayDirection->z - sphereCenter->z;
  if ( sphereRadius * sphereRadius < v7 * v7 + v11 * v11 + v10 * v10 )
    return 0;
  *distance = v6;
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BBE90  EA 0x5BBE90
// DIA: GxuTestSphereAndFrustumPlanes
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuTestSphereAndFrustumPlanes(const NTempest::C4Vector *planes, int a2, float sphereRadius)
{
  int v4; // esi
  float *i; // ecx
  float x; // [esp+8h] [ebp-10h]
  float y; // [esp+Ch] [ebp-Ch]
  float z; // [esp+10h] [ebp-8h]

  x = planes->x;
  z = planes->z;
  v4 = 0;
  y = planes->y;
  for ( i = (float *)(a2 + 8); y * *(i - 1) + x * *(i - 2) + z * *i + i[1] - sphereRadius <= 0.0; i += 4 )
  {
    if ( ++v4 == 6 )
      return 1;
  }
  return 0;
}


// -----------------------------------------------------------------------------
// RVA 0x001BBF00  EA 0x5BBF00
// DIA: GxuTestRayAndTriangle
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
int __fastcall GxuTestRayAndTriangle(
        const NTempest::C3Vector *rayDirection,
        float *a2,
        const NTempest::C3Vector *v0,
        const NTempest::C3Vector *v1,
        const NTempest::C3Vector *v2,
        float *distance)
{
  float v10; // edx
  float v11; // eax
  long double v12; // st7
  double v13; // st7
  double v14; // st7
  float x; // [esp+0h] [ebp-60h]
  float xa; // [esp+0h] [ebp-60h]
  float xb; // [esp+0h] [ebp-60h]
  float xc; // [esp+0h] [ebp-60h]
  float xd; // [esp+0h] [ebp-60h]
  float y; // [esp+4h] [ebp-5Ch]
  float ya; // [esp+4h] [ebp-5Ch]
  float yb; // [esp+4h] [ebp-5Ch]
  float yc; // [esp+4h] [ebp-5Ch]
  float yd; // [esp+4h] [ebp-5Ch]
  float z; // [esp+8h] [ebp-58h]
  float za; // [esp+8h] [ebp-58h]
  float zb; // [esp+8h] [ebp-58h]
  float zc; // [esp+8h] [ebp-58h]
  float zd; // [esp+8h] [ebp-58h]
  NTempest::C3Vector v31; // [esp+18h] [ebp-48h] BYREF
  NTempest::C3Vector q; // [esp+24h] [ebp-3Ch] BYREF
  NTempest::C3Vector p; // [esp+30h] [ebp-30h] BYREF
  NTempest::C3Vector e2; // [esp+3Ch] [ebp-24h] BYREF
  NTempest::C3Vector e1; // [esp+48h] [ebp-18h] BYREF
  NTempest::C3Vector s; // [esp+54h] [ebp-Ch] BYREF
  float u; // [esp+68h] [ebp+8h]
  float f; // [esp+6Ch] [ebp+Ch]

  *distance = INFINITY_265;
  v10 = *a2;
  v11 = a2[1];
  v31.z = a2[2];
  v31.x = v10;
  v31.y = v11;
  if ( fabs(NTempest::C3Vector::SquaredMag(&v31) - 1.0) >= 0.0009765625 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 663);
    SErrDisplayAppFatal(aIsunitvectorRa);
  }
  z = v1->z - v0->z;
  y = v1->y - v0->y;
  x = v1->x - v0->x;
  NTempest::C3Vector::C3Vector(&e1, x, y, z);
  za = v2->z - v0->z;
  ya = v2->y - v0->y;
  xa = v2->x - v0->x;
  NTempest::C3Vector::C3Vector(&e2, xa, ya, za);
  zb = e2.y * *a2 - e2.x * a2[1];
  yb = e2.x * a2[2] - e2.z * *a2;
  xb = e2.z * a2[1] - e2.y * a2[2];
  NTempest::C3Vector::C3Vector(&p, xb, yb, zb);
  v12 = p.z * e1.z + p.y * e1.y + p.x * e1.x;
  if ( fabs(v12) < 0.00000023841858 )
    return 0;
  f = 1.0 / v12;
  zc = rayDirection->z - v0->z;
  yc = rayDirection->y - v0->y;
  xc = rayDirection->x - v0->x;
  NTempest::C3Vector::C3Vector(&s, xc, yc, zc);
  v13 = (s.z * p.z + s.y * p.y + s.x * p.x) * f;
  u = v13;
  if ( v13 < 0.0 )
    return 0;
  if ( u > 1.0 )
    return 0;
  zd = e1.y * s.x - s.y * e1.x;
  yd = s.z * e1.x - e1.z * s.x;
  xd = s.y * e1.z - s.z * e1.y;
  NTempest::C3Vector::C3Vector(&q, xd, yd, zd);
  v14 = (q.y * a2[1] + q.x * *a2 + q.z * a2[2]) * f;
  if ( v14 < 0.0 || u + v14 > 1.0 )
    return 0;
  *distance = (q.y * e2.y + q.z * e2.z + q.x * e2.x) * f;
  return 1;
}


// -----------------------------------------------------------------------------
// RVA 0x001BC160  EA 0x5BC160
// DIA: GxuTestRayAndMesh
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
BOOL __fastcall GxuTestRayAndMesh(
        const NTempest::C3Vector *rayDirection,
        float *a2,
        float modelToWorldMatrices,
        unsigned int matrixCount,
        float posCount,
        const NTempest::C3Vector *pos,
        unsigned int posStride,
        unsigned int boneCount,
        const unsigned __int8 *bone,
        float *boneStride,
        EGxPrim primType,
        unsigned int indexCount,
        const unsigned __int16 *indices,
        float *distance,
        float *primIntersected)
{
  unsigned int v16; // ebx
  float v17; // eax
  float v18; // ecx
  float v19; // edx
  unsigned int v20; // edi
  const NTempest::C3Vector *v21; // esi
  unsigned int m_count; // ecx
  unsigned int v23; // ecx
  unsigned int v24; // edx
  NTempest::C3Vector *v25; // eax
  const NTempest::C3Vector *v26; // eax
  unsigned int v27; // edi
  int v28; // eax
  const char *v29; // eax
  _DWORD *v30; // esi
  const NTempest::C34Matrix *v31; // eax
  unsigned int v32; // ecx
  float *v33; // edx
  unsigned int x_low; // edi
  int v35; // eax
  const char *v36; // eax
  unsigned __int8 *v37; // esi
  const NTempest::C34Matrix *v38; // eax
  float *v39; // ecx
  const unsigned __int8 *v40; // edx
  const unsigned __int16 *v41; // edi
  float v42; // esi
  unsigned int v43; // ebx
  unsigned int v44; // eax
  int v45; // eax
  const char *v46; // eax
  unsigned int v47; // edi
  int v48; // eax
  const char *v49; // eax
  unsigned int v50; // esi
  int v51; // eax
  const char *v52; // eax
  float v53; // eax
  const unsigned __int16 *v54; // esi
  unsigned int v55; // ebx
  unsigned int v56; // eax
  int v57; // eax
  const char *v58; // eax
  unsigned int v59; // edi
  int v60; // eax
  const char *v61; // eax
  unsigned int v62; // esi
  int v63; // eax
  const char *v64; // eax
  float v65; // eax
  bool v66; // cf
  unsigned int v67; // eax
  unsigned __int16 *v68; // esi
  unsigned int v69; // ebx
  unsigned int v70; // eax
  int v71; // eax
  const char *v72; // eax
  unsigned int v73; // edi
  int v74; // eax
  const char *v75; // eax
  unsigned int v76; // esi
  int v77; // eax
  const char *v78; // eax
  float *v79; // eax
  bool v80; // zf
  int v82; // [esp-20h] [ebp-6Ch]
  int v83; // [esp-20h] [ebp-6Ch]
  int v84; // [esp-20h] [ebp-6Ch]
  int v85; // [esp-20h] [ebp-6Ch]
  int v86; // [esp-20h] [ebp-6Ch]
  int v87; // [esp-20h] [ebp-6Ch]
  int v88; // [esp-20h] [ebp-6Ch]
  int v89; // [esp-20h] [ebp-6Ch]
  int v90; // [esp-20h] [ebp-6Ch]
  int v91; // [esp-20h] [ebp-6Ch]
  int v92; // [esp-20h] [ebp-6Ch]
  int v93; // [esp-1Ch] [ebp-68h]
  int v94; // [esp-1Ch] [ebp-68h]
  int v95; // [esp-1Ch] [ebp-68h]
  int v96; // [esp-1Ch] [ebp-68h]
  int v97; // [esp-1Ch] [ebp-68h]
  int v98; // [esp-1Ch] [ebp-68h]
  int v99; // [esp-1Ch] [ebp-68h]
  int v100; // [esp-1Ch] [ebp-68h]
  int v101; // [esp-1Ch] [ebp-68h]
  int v102; // [esp-1Ch] [ebp-68h]
  int v103; // [esp-1Ch] [ebp-68h]
  unsigned int v104; // [esp-18h] [ebp-64h]
  unsigned int v105; // [esp-18h] [ebp-64h]
  unsigned int v106; // [esp-18h] [ebp-64h]
  unsigned int v107; // [esp-18h] [ebp-64h]
  unsigned int v108; // [esp-18h] [ebp-64h]
  unsigned int v109; // [esp-18h] [ebp-64h]
  unsigned int v110; // [esp-18h] [ebp-64h]
  unsigned int v111; // [esp-18h] [ebp-64h]
  unsigned int v112; // [esp-18h] [ebp-64h]
  unsigned int v113; // [esp-18h] [ebp-64h]
  unsigned int v114; // [esp-18h] [ebp-64h]
  const char *v115; // [esp-14h] [ebp-60h]
  const char *v116; // [esp-14h] [ebp-60h]
  const char *v117; // [esp-14h] [ebp-60h]
  const char *v118; // [esp-14h] [ebp-60h]
  const char *v119; // [esp-14h] [ebp-60h]
  const char *v120; // [esp-14h] [ebp-60h]
  const char *v121; // [esp-14h] [ebp-60h]
  const char *v122; // [esp-14h] [ebp-60h]
  const char *v123; // [esp-14h] [ebp-60h]
  const char *v124; // [esp-14h] [ebp-60h]
  const char *v125; // [esp-14h] [ebp-60h]
  NTempest::C34Matrix identity; // [esp+4h] [ebp-48h] BYREF
  _DWORD v127[3]; // [esp+34h] [ebp-18h] BYREF
  const NTempest::C3Vector *v128; // [esp+40h] [ebp-Ch]
  float *v129; // [esp+44h] [ebp-8h]
  const NTempest::C3Vector *v130; // [esp+48h] [ebp-4h]

  *distance = INFINITY_265;
  v128 = rayDirection;
  v16 = 0;
  v129 = a2;
  *primIntersected = 0.0;
  v17 = *a2;
  v18 = a2[1];
  v19 = a2[2];
  *(float *)v127 = v17;
  *(float *)&v127[1] = v18;
  *(float *)&v127[2] = v19;
  if ( fabs(v17 * v17 + v18 * v18 + v19 * v19 - 1.0) >= 0.0009765625 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 746);
    SErrDisplayAppFatal(aIsunitvectorRa);
  }
  v20 = LODWORD(posCount);
  if ( posCount == 0.0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 747);
    SErrDisplayAppFatal(aPoscount);
  }
  v21 = pos;
  if ( pos == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 748);
    SErrDisplayAppFatal(aPos);
  }
  if ( primType != GxPrim_Triangles && primType != GxPrim_TriangleStrip && primType != GxPrim_TriangleFan )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 749);
    SErrDisplayAppFatal(aPrimtypeGxprim_0);
  }
  if ( indexCount < 3 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 750);
    SErrDisplayAppFatal(aIndexcount3);
  }
  if ( (__S22__BH___GxuTestRayAndMesh__YIHABVC3Vector_NTempest__0PBVC34Matrix_3_IIPBV23_IIPBEIW4EGxPrim__IPBGAAMAAI_Z_4EA
      & 1) == 0 )
  {
    __S22__BH___GxuTestRayAndMesh__YIHABVC3Vector_NTempest__0PBVC34Matrix_3_IIPBV23_IIPBEIW4EGxPrim__IPBGAAMAAI_Z_4EA |= 1u;
    tmpVtx.m_alloc = 0;
    tmpVtx.m_count = 0;
    tmpVtx.m_data = nullptr;
    tmpVtx.__vftable = (TSFixedArray_<NTempest::C3Vector,1199076692,759>_vtbl *)&TSFixedArray_<NTempest::C3Vector,1199076692,759>::`vftable';
    atexit(_E23_44);
  }
  identity.a0 = 1.0;
  memset(&identity.a1, 0, 12);
  identity.b1 = 1.0;
  memset(&identity.b2, 0, 12);
  identity.c2 = 1.0;
  memset(&identity.d0, 0, 12);
  if ( modelToWorldMatrices == 0.0 )
    modelToWorldMatrices = COERCE_FLOAT(&identity);
  m_count = tmpVtx.m_count;
  if ( v20 > tmpVtx.m_count )
  {
    TSFixedArray<NTempest::C3Vector>::ReallocData(&tmpVtx.TSFixedArray<NTempest::C3Vector>, v20);
    if ( tmpVtx.m_count < v20 )
    {
      v23 = tmpVtx.m_count;
      v24 = v20 - tmpVtx.m_count;
      do
      {
        v25 = &tmpVtx.m_data[v23];
        if ( &tmpVtx.m_data[v23] != nullptr )
        {
          v25->x = 0.0;
          v25->y = 0.0;
          v25->z = 0.0;
        }
        ++v23;
        --v24;
      }
      while ( v24 != 0 );
    }
    m_count = v20;
    tmpVtx.m_count = v20;
  }
  v26 = (const NTempest::C3Vector *)bone;
  if ( bone != nullptr )
  {
    bone = nullptr;
    v130 = v21;
    pos = v26;
    while ( 1 )
    {
      if ( boneStride != nullptr && v16 >= boneCount )
      {
        SErrDisplayError(0x85100000, aDBuildBuildwow_242, 779, aBonestrideVndx, v16 < boneCount, 1u);
        m_count = tmpVtx.m_count;
      }
      x_low = LOBYTE(pos->x);
      if ( x_low >= matrixCount )
      {
        SErrDisplayError(0x85100000, aDBuildBuildwow_242, 783, aIdMatrixcount, 0, 1u);
        m_count = tmpVtx.m_count;
      }
      if ( v16 >= m_count )
      {
        v35 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                &tmpVtx,
                1,
                1,
                aIndex0x08xArra,
                v16,
                m_count);
        v36 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                              &tmpVtx,
                              v35);
        SErrDisplayErrorFmt(0x85100080, v36, v83, v94, v105, v116);
      }
      v37 = (unsigned __int8 *)&bone[(unsigned int)tmpVtx.m_data];
      v38 = NTempest::operator*(
              (const NTempest::C34Matrix *)v127,
              &v130->x,
              (const NTempest::C34Matrix *)(LODWORD(modelToWorldMatrices) + 48 * (unsigned __int8)x_low));
      *(float *)v37 = v38->a0;
      v39 = boneStride;
      *((_DWORD *)v37 + 1) = LODWORD(v38->a1);
      v40 = bone;
      *((_DWORD *)v37 + 2) = LODWORD(v38->a2);
      bone = v40 + 12;
      ++v16;
      pos = (const NTempest::C3Vector *)((char *)pos + (_DWORD)v39);
      v130 = (const NTempest::C3Vector *)((char *)v130 + posStride);
      if ( v16 == LODWORD(posCount) )
        break;
      m_count = tmpVtx.m_count;
    }
  }
  else
  {
    v27 = 0;
    boneStride = &v21->x;
    while ( 1 )
    {
      if ( v27 >= m_count )
      {
        v28 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                &tmpVtx,
                1,
                1,
                aIndex0x08xArra,
                v27,
                m_count);
        v29 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                              &tmpVtx,
                              v28);
        SErrDisplayErrorFmt(0x85100080, v29, v82, v93, v104, v115);
      }
      v30 = (_DWORD *)((char *)&tmpVtx.m_data->x + v16);
      v31 = NTempest::operator*(
              (const NTempest::C34Matrix *)v127,
              boneStride,
              (const NTempest::C34Matrix *)LODWORD(modelToWorldMatrices));
      *v30 = LODWORD(v31->a0);
      v32 = posStride;
      v30[1] = LODWORD(v31->a1);
      v33 = boneStride;
      v30[2] = LODWORD(v31->a2);
      ++v27;
      v16 += 12;
      boneStride = (float *)((char *)v33 + v32);
      if ( v27 == LODWORD(posCount) )
        break;
      m_count = tmpVtx.m_count;
    }
  }
  switch ( primType )
  {
    case GxPrim_Triangles:
      v67 = indexCount;
      posCount = 0.0;
      if ( indexCount != 0 )
      {
        indexCount = (unsigned int)(indices + 1);
        indices = (const unsigned __int16 *)((v67 - 1) / 3 + 1);
        do
        {
          v68 = (unsigned __int16 *)indexCount;
          v69 = *(unsigned __int16 *)(indexCount + 2);
          v70 = tmpVtx.m_count;
          if ( v69 >= tmpVtx.m_count )
          {
            v71 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                    &tmpVtx,
                    1,
                    1,
                    aIndex0x08xArra,
                    v69,
                    tmpVtx.m_count);
            v72 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                  &tmpVtx,
                                  v71);
            SErrDisplayErrorFmt(0x85100080, v72, v90, v101, v112, v123);
            v70 = tmpVtx.m_count;
          }
          v73 = *v68;
          if ( v73 >= v70 )
          {
            v74 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                    &tmpVtx,
                    1,
                    1,
                    aIndex0x08xArra,
                    v73,
                    v70);
            v75 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                  &tmpVtx,
                                  v74);
            SErrDisplayErrorFmt(0x85100080, v75, v91, v102, v113, v124);
            v70 = tmpVtx.m_count;
          }
          v76 = *(unsigned __int16 *)(indexCount - 2);
          if ( v76 >= v70 )
          {
            v77 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                    &tmpVtx,
                    1,
                    1,
                    aIndex0x08xArra,
                    v76,
                    v70);
            v78 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                  &tmpVtx,
                                  v77);
            SErrDisplayErrorFmt(0x85100080, v78, v92, v103, v114, v125);
          }
          if ( GxuTestRayAndTriangle(
                 v128,
                 v129,
                 &tmpVtx.m_data[v76],
                 &tmpVtx.m_data[v73],
                 &tmpVtx.m_data[v69],
                 &modelToWorldMatrices) != 0
            && modelToWorldMatrices >= 0.0
            && modelToWorldMatrices < (double)*distance )
          {
            v79 = primIntersected;
            *distance = modelToWorldMatrices;
            *v79 = posCount;
          }
          v80 = indices == (const unsigned __int16 *)1;
          indexCount += 6;
          ++LODWORD(posCount);
          indices = (const unsigned __int16 *)((char *)indices - 1);
        }
        while ( !v80 );
      }
      break;
    case GxPrim_TriangleStrip:
      LODWORD(posCount) = 2;
      if ( indexCount > 2 )
      {
        v54 = indices;
        for ( modelToWorldMatrices = *(float *)&indices; ; v54 = (const unsigned __int16 *)LODWORD(modelToWorldMatrices) )
        {
          v55 = v54[2];
          v56 = tmpVtx.m_count;
          if ( v55 >= tmpVtx.m_count )
          {
            v57 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                    &tmpVtx,
                    1,
                    1,
                    aIndex0x08xArra,
                    v55,
                    tmpVtx.m_count);
            v58 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                  &tmpVtx,
                                  v57);
            SErrDisplayErrorFmt(0x85100080, v58, v87, v98, v109, v120);
            v56 = tmpVtx.m_count;
          }
          v59 = v54[1];
          if ( v59 >= v56 )
          {
            v60 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                    &tmpVtx,
                    1,
                    1,
                    aIndex0x08xArra,
                    v59,
                    v56);
            v61 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                  &tmpVtx,
                                  v60);
            SErrDisplayErrorFmt(0x85100080, v61, v88, v99, v110, v121);
            v56 = tmpVtx.m_count;
          }
          v62 = (unsigned __int16)*(_WORD *)LODWORD(modelToWorldMatrices);
          if ( v62 >= v56 )
          {
            v63 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                    &tmpVtx,
                    1,
                    1,
                    aIndex0x08xArra,
                    v62,
                    v56);
            v64 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                  &tmpVtx,
                                  v63);
            SErrDisplayErrorFmt(0x85100080, v64, v89, v100, v111, v122);
          }
          if ( GxuTestRayAndTriangle(
                 v128,
                 v129,
                 &tmpVtx.m_data[v62],
                 &tmpVtx.m_data[v59],
                 &tmpVtx.m_data[v55],
                 (float *)&indices) != 0
            && *(float *)&indices >= 0.0
            && *(float *)&indices < (double)*distance )
          {
            v65 = posCount;
            *distance = *(float *)&indices;
            *(_DWORD *)primIntersected = LODWORD(v65) - 2;
          }
          v66 = ++LODWORD(posCount) < indexCount;
          LODWORD(modelToWorldMatrices) += 2;
          if ( !v66 )
            break;
        }
      }
      break;
    case GxPrim_TriangleFan:
      for ( LODWORD(modelToWorldMatrices) = 2; LODWORD(modelToWorldMatrices) < indexCount; ++LODWORD(modelToWorldMatrices) )
      {
        v41 = indices;
        v42 = modelToWorldMatrices;
        v43 = indices[LODWORD(modelToWorldMatrices)];
        v44 = tmpVtx.m_count;
        if ( v43 >= tmpVtx.m_count )
        {
          v45 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                  &tmpVtx,
                  1,
                  1,
                  aIndex0x08xArra,
                  v43,
                  tmpVtx.m_count);
          v46 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                &tmpVtx,
                                v45);
          SErrDisplayErrorFmt(0x85100080, v46, v84, v95, v106, v117);
          v44 = tmpVtx.m_count;
        }
        v47 = v41[LODWORD(v42) - 1];
        if ( v47 >= v44 )
        {
          v48 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                  &tmpVtx,
                  1,
                  1,
                  aIndex0x08xArra,
                  v47,
                  v44);
          v49 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                &tmpVtx,
                                v48);
          SErrDisplayErrorFmt(0x85100080, v49, v85, v96, v107, v118);
          v44 = tmpVtx.m_count;
        }
        v50 = *indices;
        if ( v50 >= v44 )
        {
          v51 = ((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int, int, char *, unsigned int, unsigned int))tmpVtx.TSBaseArray<NTempest::C3Vector>::MemLineNo)(
                  &tmpVtx,
                  1,
                  1,
                  aIndex0x08xArra,
                  v50,
                  v44);
          v52 = (const char *)((int (__thiscall *)(TSFixedArray_<NTempest::C3Vector,1199076692,759> *, int))tmpVtx.TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo)(
                                &tmpVtx,
                                v51);
          SErrDisplayErrorFmt(0x85100080, v52, v86, v97, v108, v119);
        }
        if ( GxuTestRayAndTriangle(v128, v129, &tmpVtx.m_data[v50], &tmpVtx.m_data[v47], &tmpVtx.m_data[v43], &posCount) != 0
          && posCount >= 0.0
          && posCount < (double)*distance )
        {
          v53 = modelToWorldMatrices;
          *distance = posCount;
          *(_DWORD *)primIntersected = LODWORD(v53) - 2;
        }
      }
      break;
    default:
      break;
  }
  return *distance != INFINITY_265;
}


// -----------------------------------------------------------------------------
// RVA 0x001BC970  EA 0x5BC970
// DIA: $E23
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
void __cdecl _E23_44()
{
  tmpVtx.__vftable = (TSFixedArray_<NTempest::C3Vector,1199076692,759>_vtbl *)&TSFixedArray<NTempest::C3Vector>::`vftable';
  if ( tmpVtx.m_data != nullptr )
    SMemFree((void *)tmpVtx.m_data, aAvc3vectorNtem, -2, 0);
}


// -----------------------------------------------------------------------------
// RVA 0x001BC9A0  EA 0x5BC9A0
// DIA: GxuTestRayAndRigidMeshInModelSpace
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
BOOL __fastcall GxuTestRayAndRigidMeshInModelSpace(
        const NTempest::C3Vector *rayDirection,
        float *a2,
        unsigned int posCount,
        const NTempest::C3Vector *pos,
        EGxPrim primType,
        unsigned int indexCount,
        const unsigned __int16 *indices,
        float *distance,
        unsigned int *primIntersected)
{
  unsigned int v12; // edi
  const unsigned __int16 *v13; // ebx
  const unsigned __int16 *v14; // edi
  unsigned int *v15; // edx
  const unsigned __int16 *v16; // edi
  bool v17; // zf
  unsigned int ndx; // [esp+20h] [ebp+8h]
  unsigned int ndxa; // [esp+20h] [ebp+8h]
  unsigned int prim; // [esp+2Ch] [ebp+14h]

  *distance = INFINITY_265;
  *primIntersected = 0;
  if ( fabs(a2[1] * a2[1] + a2[2] * a2[2] + *a2 * *a2 - 1.0) >= 0.0009765625 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 885);
    SErrDisplayAppFatal(aIsunitvectorRa);
  }
  if ( posCount == 0 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 886);
    SErrDisplayAppFatal(aPoscount);
  }
  if ( pos == nullptr )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 887);
    SErrDisplayAppFatal(aPos);
  }
  if ( primType != GxPrim_Triangles && primType != GxPrim_TriangleStrip && primType != GxPrim_TriangleFan )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 888);
    SErrDisplayAppFatal(aPrimtypeGxprim_0);
  }
  if ( indexCount < 3 )
  {
    SErrPrepareAppFatal(aDBuildBuildwow_242, 889);
    SErrDisplayAppFatal(aIndexcount3);
  }
  if ( primType == GxPrim_Triangles )
  {
    prim = 0;
    if ( indexCount != 0 )
    {
      v16 = indices;
      ndxa = (indexCount - 1) / 3 + 1;
      do
      {
        if ( GxuTestRayAndTriangle(rayDirection, a2, &pos[*v16], &pos[v16[1]], &pos[v16[2]], (float *)&indices) != 0
          && *(float *)&indices < (double)*distance )
        {
          *distance = *(float *)&indices;
          *primIntersected = prim;
        }
        v16 += 3;
        v17 = ndxa == 1;
        ++prim;
        --ndxa;
      }
      while ( !v17 );
    }
  }
  else if ( primType == GxPrim_TriangleStrip )
  {
    ndx = 2;
    v14 = indices;
    do
    {
      if ( GxuTestRayAndTriangle(rayDirection, a2, &pos[*v14], &pos[v14[1]], &pos[v14[2]], (float *)&indices) != 0
        && *(float *)&indices < (double)*distance )
      {
        v15 = primIntersected;
        *distance = *(float *)&indices;
        *v15 = ndx - 2;
      }
      ++v14;
      ++ndx;
    }
    while ( ndx < indexCount );
  }
  else
  {
    v12 = 2;
    v13 = indices;
    do
    {
      if ( GxuTestRayAndTriangle(rayDirection, a2, &pos[*v13], &pos[v13[v12 - 1]], &pos[v13[v12]], (float *)&indices) != 0
        && *(float *)&indices < (double)*distance )
      {
        *distance = *(float *)&indices;
        *primIntersected = v12 - 2;
      }
      ++v12;
    }
    while ( v12 < indexCount );
  }
  return *distance != INFINITY_265;
}


// -----------------------------------------------------------------------------
// RVA 0x001BCC50  EA 0x5BCC50
// DIA: GxuClipCalcCode
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
unsigned int __fastcall GxuClipCalcCode(const NTempest::C44Matrix *pos, float *a2)
{
  float v3; // eax
  float cc[6]; // [esp+0h] [ebp-28h] BYREF
  NTempest::C4Vector clipVert; // [esp+18h] [ebp-10h] BYREF

  cc[2] = *a2;
  v3 = a2[1];
  cc[4] = a2[2];
  cc[3] = v3;
  cc[5] = 1.0;
  NTempest::operator*((const NTempest::C44Matrix *)&clipVert, &cc[2], pos);
  cc[0] = clipVert.w - clipVert.x;
  cc[1] = clipVert.x + clipVert.w;
  cc[2] = clipVert.w - clipVert.y;
  cc[3] = clipVert.y + clipVert.w;
  cc[4] = clipVert.w - clipVert.z;
  cc[5] = clipVert.z + clipVert.w;
  return (LODWORD(cc[5]) & 0x83FFFFFF
        | ((LODWORD(cc[4]) & 0x87FFFFFF
          | ((LODWORD(cc[3]) & 0x8FFFFFFF
            | ((LODWORD(cc[2]) & 0x9FFFFFFF | (((LODWORD(cc[0]) >> 1) | LODWORD(cc[1]) & 0xBFFFFFFF) >> 1)) >> 1)) >> 1)) >> 1)) >> 26;
}


// -----------------------------------------------------------------------------
// RVA 0x001BCD00  EA 0x5BCD00
// DIA: GxuSnapTexelsToPixels
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall GxuSnapTexelsToPixels(NTempest::C2Vector *tex, int a2, int texW, int texH)
{
  float *p_z; // eax
  int i; // ecx
  float *v8; // eax
  int j; // ecx
  float *v10; // eax
  int k; // ecx
  float v12; // edx
  const CGxCaps *v13; // eax
  float *v14; // edi
  int m; // esi
  double v16; // st7
  double v17; // st6
  __int16 v18; // fps
  double v19; // st7
  bool v20; // c0
  char v21; // c2
  bool v22; // c3
  float *p_y; // ecx
  int v26; // edx
  double v27; // st7
  double v28; // st6
  double v29; // st5
  double v30; // st4
  double v31; // st7
  char v32; // cl
  long double v33; // st7
  double n; // st7
  unsigned int v35; // esi
  unsigned int v36; // edi
  int v37; // edx
  float *v38; // ecx
  double v39; // st7
  int v40; // esi
  double v41; // st5
  double v42; // st5
  double v44; // st5
  unsigned __int8 v45; // c0
  unsigned __int8 v46; // c2
  double v47; // st5
  unsigned int v48; // eax
  int ii; // ecx
  float v50; // edi
  int v51; // ecx
  float *v52; // eax
  int v53; // edx
  double v54; // st6
  double v55; // st5
  double v56; // st5
  float x; // [esp+0h] [ebp-198h]
  float y; // [esp+4h] [ebp-194h]
  float y_4; // [esp+8h] [ebp-190h]
  float y_4a; // [esp+8h] [ebp-190h]
  CGxCaps caps; // [esp+18h] [ebp-180h] BYREF
  NTempest::C3Vector posScr[4]; // [esp+80h] [ebp-118h] BYREF
  NTempest::C33Matrix fromPixToTexM33; // [esp+B0h] [ebp-E8h] BYREF
  NTempest::C33Matrix texM33; // [esp+D4h] [ebp-C4h] BYREF
  NTempest::C33Matrix iposM33; // [esp+F8h] [ebp-A0h] BYREF
  NTempest::C3Vector iposScr[4]; // [esp+11Ch] [ebp-7Ch] BYREF
  int v67; // [esp+14Ch] [ebp-4Ch]
  NTempest::C3Vector texScr[4]; // [esp+150h] [ebp-48h] BYREF
  const CGxCaps *v69; // [esp+180h] [ebp-18h]
  __int64 v70; // [esp+184h] [ebp-14h]
  NTempest::C3Vector iposCntr; // [esp+18Ch] [ebp-Ch] BYREF
  float pixArea; // [esp+1A0h] [ebp+8h]
  float pixAreaa; // [esp+1A0h] [ebp+8h]
  float pixAreab; // [esp+1A0h] [ebp+8h]
  float texArea; // [esp+1A4h] [ebp+Ch]
  float texAreaa; // [esp+1A4h] [ebp+Ch]

  HIDWORD(v70) = tex;
  v67 = a2;
  p_z = &posScr[0].z;
  for ( i = 4; i != 0; --i )
  {
    *(p_z - 2) = 0.0;
    *(p_z - 1) = 0.0;
    *p_z = 0.0;
    p_z += 3;
  }
  v8 = &iposScr[0].z;
  for ( j = 4; j != 0; --j )
  {
    *(v8 - 2) = 0.0;
    *(v8 - 1) = 0.0;
    *v8 = 0.0;
    v8 += 3;
  }
  memset((void *)&iposCntr, 0, sizeof(iposCntr));
  v10 = &texScr[0].z;
  for ( k = 4; k != 0; --k )
  {
    *(v10 - 2) = 0.0;
    *(v10 - 1) = 0.0;
    *v10 = 0.0;
    v10 += 3;
  }
  iposM33.a0 = 1.0;
  memset(&iposM33.a1, 0, 12);
  iposM33.b1 = 1.0;
  memset(&iposM33.b2, 0, 12);
  iposM33.c2 = 1.0;
  texM33.a0 = 1.0;
  memset(&texM33.a1, 0, 12);
  texM33.b1 = 1.0;
  memset(&texM33.b2, 0, 12);
  texM33.c2 = 1.0;
  caps = *GxCaps();
  GxuXformCalc2dScreenCoords((const NTempest::C3Vector *)4, SHIDWORD(v70), posScr);
  v12 = *(float *)(a2 + 4);
  v13 = *(const CGxCaps **)a2;
  v69 = v13;
  v70 = LODWORD(v12);
  LODWORD(texScr[1].x) = v13;
  texScr[1].y = v12;
  texScr[1].z = 0.0;
  LODWORD(texScr[0].x) = v13;
  texScr[0].y = v12;
  texScr[0].z = 0.0;
  v14 = (float *)a2;
  for ( m = 0; m != 4; ++m )
  {
    PixSnap(v13, (const NTempest::C3Vector *)&caps, &iposScr[m]);
    v16 = iposScr[m].x * 0.25;
    v17 = iposScr[m].y * 0.25;
    *((float *)&v70 + 1) = iposScr[m].z * 0.25;
    iposCntr.x = v16 + iposCntr.x;
    iposCntr.y = v17 + iposCntr.y;
    iposCntr.z = iposCntr.z + *((float *)&v70 + 1);
    if ( *v14 <= (double)texScr[0].x )
      texScr[0].x = *v14;
    if ( v14[1] <= (double)texScr[0].y )
      texScr[0].y = v14[1];
    if ( *v14 >= (double)texScr[1].x )
      texScr[1].x = *v14;
    v19 = v14[1];
    v20 = v19 < texScr[1].y;
    v21 = 0;
    v22 = v19 == texScr[1].y;
    LOWORD(v13) = v18;
    if ( v19 >= texScr[1].y )
      texScr[1].y = v14[1];
    v14 += 2;
  }
  p_y = &iposScr[0].y;
  v26 = 4;
  v70 = (unsigned int)texH;
  texArea = (texScr[1].y - texScr[0].y) * (texScr[1].x - texScr[0].x) * (double)texW * (double)texH;
  do
  {
    if ( *(p_y - 1) > (double)iposCntr.x )
      *(p_y - 1) = *(p_y - 1) - 1.0;
    if ( *p_y > (double)iposCntr.y )
      *p_y = *p_y - 1.0;
    p_y += 3;
    --v26;
  }
  while ( v26 != 0 );
  v27 = iposScr[2].x - iposScr[1].x;
  v28 = iposScr[2].y - iposScr[1].y;
  v29 = iposScr[2].z - iposScr[1].z;
  v30 = iposScr[0].x - iposScr[1].x;
  iposCntr.y = iposScr[0].y - iposScr[1].y;
  iposCntr.z = iposScr[0].z - iposScr[1].z;
  y_4 = v28 * v30 - iposCntr.y * v27;
  y = iposCntr.z * v27 - v29 * v30;
  x = iposCntr.y * v29 - iposCntr.z * v28;
  NTempest::C3Vector::C3Vector(&iposCntr, x, y, y_4);
  v31 = iposCntr.x * iposCntr.x + iposCntr.z * iposCntr.z + iposCntr.y * iposCntr.y;
  pixArea = v31;
  if ( v31 >= 1.0 )
  {
    if ( pixArea < 0.0 )
      SErrDisplayErrorFmt(0x85100000, aTempestCmathH, 412, 0, 1u, "\"%s\", %s = %f", aX0f, asc_832D0C, pixArea);
    v32 = 0;
    v33 = sqrt(pixArea);
    pixAreaa = (v33 + v33) * 1.5;
    for ( n = texArea; pixAreaa <= n; ++v32 )
      n = n * 0.5;
    v35 = (unsigned int)texW >> v32;
    v36 = (unsigned int)texH >> v32;
    if ( v35 != 0 && v36 != 0 )
    {
      LODWORD(v70) = v36;
      texAreaa = (float)v35;
      v37 = 0;
      v38 = &texScr[0].y;
      v39 = (texScr[1].x + texScr[0].x) * texAreaa * 0.5;
      *((float *)&v70 + 1) = (float)v36;
      do
      {
        v40 = v67;
        v41 = texAreaa * *(float *)(v67 + 8 * v37);
        *(v38 - 1) = v41;
        pixAreab = *((float *)&v70 + 1) * *(float *)(v40 + 8 * v37 + 4);
        *v38 = pixAreab;
        if ( v41 >= v39 )
          v42 = v41 - 0.5;
        else
          v42 = v41 + 0.5;
        *(v38 - 1) = v42;
        v44 = pixAreab;
        if ( (v45 | v46) != 0 )
          v47 = v44 + 0.5;
        else
          v47 = v44 - 0.5;
        *v38 = v47;
        ++v37;
        v38 += 3;
      }
      while ( v37 != 4 );
      v48 = 0;
      for ( ii = 3; ii != 0; --ii )
      {
        *(float *)((char *)&iposM33.a0 + v48) = iposScr[v48 / 0xC].x;
        *(float *)((char *)&iposM33.a1 + v48) = iposScr[v48 / 0xC].y;
        *(float *)((char *)&texM33.a0 + v48) = texScr[v48 / 0xC].x;
        v50 = texScr[v48 / 0xC].y;
        *(float *)((char *)&iposM33.a2 + v48) = 1.0;
        *(float *)((char *)&texM33.a1 + v48) = v50;
        *(float *)((char *)&texM33.a2 + v48) = 1.0;
        v48 += 12;
      }
      y_4a = NTempest::C33Matrix::Determinant(&iposM33);
      NTempest::C33Matrix::Inverse(&iposM33, (NTempest::C33Matrix *)&texScr[1], y_4a);
      NTempest::operator*(&fromPixToTexM33, &texScr[1].x, &texM33);
      v51 = v40;
      v52 = &posScr[0].y;
      v53 = 4;
      v54 = 1.0 / *((float *)&v70 + 1);
      do
      {
        v52 += 3;
        v51 += 8;
        --v53;
        iposCntr.x = fromPixToTexM33.c0 * *(v52 - 2) + fromPixToTexM33.a0 * *(v52 - 4) + fromPixToTexM33.b0 * *(v52 - 3);
        v55 = fromPixToTexM33.c1 * *(v52 - 2);
        *(float *)(v51 - 8) = iposCntr.x;
        iposCntr.y = v55 + fromPixToTexM33.a1 * *(v52 - 4) + fromPixToTexM33.b1 * *(v52 - 3);
        v56 = 1.0 / texAreaa * *(float *)(v51 - 8);
        *(float *)(v51 - 4) = iposCntr.y;
        *(float *)(v51 - 8) = v56;
        *(float *)(v51 - 4) = v54 * *(float *)(v51 - 4);
      }
      while ( v53 != 0 );
    }
  }
}


// -----------------------------------------------------------------------------
// RVA 0x001BD230  EA 0x5BD230
// DIA: PixSnap
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
void __fastcall PixSnap(const NTempest::C3Vector *src, float *a2, NTempest::C3Vector *dst)
{
  float v5; // [esp+4h] [ebp-10h]
  float v6; // [esp+4h] [ebp-10h]
  float v7; // [esp+10h] [ebp-4h]
  float v8; // [esp+10h] [ebp-4h]

  if ( LODWORD(src->y) != 0 )
  {
    if ( LODWORD(src->z) != 0 )
    {
      v8 = OglFloor(a2[1]);
      v6 = OglFloor(*a2);
      dst->x = v6;
      dst->y = v8;
      dst->z = 1.0;
      return;
    }
  }
  else if ( LODWORD(src->z) != 0 )
  {
    v7 = D3dCeil(a2[1]);
    v5 = D3dCeil(*a2);
    dst->x = v5;
    dst->y = v7;
    dst->z = 1.0;
    return;
  }
  SErrDisplayError(0x85100000, aDBuildBuildwow_242, 64, value, 0, 1u);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD2F0  EA 0x5BD2F0
// DIA: D3dCeil
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __fastcall D3dCeil(float f)
{
  double v3; // st7

  v3 = _floor(f * 16.0 + 0.5);
  return _ceil(v3 * 0.0625);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD330  EA 0x5BD330
// DIA: OglFloor
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_line_record
// -----------------------------------------------------------------------------
double __fastcall OglFloor(float f)
{
  double v3; // st7

  v3 = _floor(f * 16.0 + 0.5);
  return _ceil(v3 * 0.0625);
}


// -----------------------------------------------------------------------------
// RVA 0x001BD370  EA 0x5BD370
// DIA: TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemFileName
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
char *__thiscall TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemFileName(
        TSFixedArray_<NTempest::C3Vector,1199076692,759> *this)
{
  return TSFixedArray_<NTempest::C3Vector,1199076692,759>::s_name;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD380  EA 0x5BD380
// DIA: TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
int __thiscall TSFixedArray_<NTempest::C3Vector,1199076692,759>::MemLineNo(
        TSFixedArray_<NTempest::C3Vector,1199076692,759> *this)
{
  return 759;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD390  EA 0x5BD390
// DIA: $E5
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E5_262()
{
  return _E4_262();
}


// -----------------------------------------------------------------------------
// RVA 0x001BD3A0  EA 0x5BD3A0
// DIA: $E4
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E4_262()
{
  TWO_PI_262 = 3.1415927 + 3.1415927;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD3B0  EA 0x5BD3B0
// DIA: $E8
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: compiland_primary_cpp
// -----------------------------------------------------------------------------
// attributes: thunk
int _E8_262()
{
  return _E7_262();
}


// -----------------------------------------------------------------------------
// RVA 0x001BD3C0  EA 0x5BD3C0
// DIA: $E7
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
void _E7_262()
{
  OO_TWO_PI_262 = 1.0 / TWO_PI_262;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD3E0  EA 0x5BD3E0
// DIA: NTempest::C4Vector::operator*=
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
NTempest::C4Vector *__thiscall NTempest::C4Vector::operator*=(NTempest::C4Vector *this, float a)
{
  this->x = a * this->x;
  this->y = a * this->y;
  this->z = a * this->z;
  this->w = a * this->w;
  return this;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD410  EA 0x5BD410
// DIA: NTempest::C4Vector::operator-
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
NTempest::C4Vector *__thiscall NTempest::C4Vector::operator-(NTempest::C4Vector *this, NTempest::C4Vector *result)
{
  double v3; // st7
  double v4; // st6
  double v5; // st5

  v3 = -this->w;
  v4 = -this->z;
  v5 = -this->y;
  result->x = -this->x;
  result->y = v5;
  result->z = v4;
  result->w = v3;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD440  EA 0x5BD440
// DIA: NTempest::operator-
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
NTempest::C4Vector *__fastcall NTempest::operator-(const NTempest::C4Vector *l, float *a2, const NTempest::C4Vector *r)
{
  double v5; // st7
  double v6; // st6
  double v7; // st5

  v5 = a2[3] - r->w;
  v6 = a2[2] - r->z;
  v7 = a2[1] - r->y;
  l->x = *a2 - r->x;
  l->y = v7;
  l->z = v6;
  l->w = v5;
  return l;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD470  EA 0x5BD470
// DIA: NTempest::C44Matrix::Col2
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
NTempest::C4Vector *__thiscall NTempest::C44Matrix::Col2(NTempest::C44Matrix *this, NTempest::C4Vector *result)
{
  double d2; // st7
  double c2; // st6
  double b2; // st5

  d2 = this->d2;
  c2 = this->c2;
  b2 = this->b2;
  result->x = this->a2;
  result->y = b2;
  result->z = c2;
  result->w = d2;
  return result;
}


// -----------------------------------------------------------------------------
// RVA 0x001BD4A0  EA 0x5BD4A0
// DIA: NTempest::C44Matrix::Col3
// Compiland: .\Release_Assertions_Enabled\Gxu.obj
// Mapping: dia_header_line+compiland_cpp
// -----------------------------------------------------------------------------
NTempest::C4Vector *__thiscall NTempest::C44Matrix::Col3(NTempest::C44Matrix *this, NTempest::C4Vector *result)
{
  double d3; // st7
  double c3; // st6
  double b3; // st5

  d3 = this->d3;
  c3 = this->c3;
  b3 = this->b3;
  result->x = this->a3;
  result->y = b3;
  result->z = c3;
  result->w = d3;
  return result;
}

