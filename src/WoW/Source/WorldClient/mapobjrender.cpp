#include "WoW/Source/WorldClient/mapobjrender.h"

#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Services/texture.h"

#include <algorithm>

void CMapObj::Render(const NTempest::C44Matrix& worldTransform) const {
    if (!loaded) return;
    GxRsPush();
    GxXformPush(GxXform_World, worldTransform);
    GxVertexShaderSelect(GxVS_PassThru);
    GxRsSet(GxRs_DepthTest, 1);
    GxRsSet(GxRs_PolygonOffset, 0.0F);
    GxRsSet(GxRs_TexBlend0, static_cast<int>(GxTexBlend_Mod));
    GxRsSet(GxRs_TexBlend1, static_cast<int>(GxTexBlend_Mod));

    for (const CMapObjGroup& group : groups) {
        // CMapObj::ExtRender seeds portal traversal only from exterior groups
        // (SMOGroupInfo::flags bit 0x8) and always-visible groups (0x10000).
        // Rendering every loaded group exposes the complete underground WMO
        // whenever an outdoor entrance definition is loaded.
        if ((group.infoFlags & (0x8U | 0x10000U)) == 0U)
            continue;
        if (!group.loaded || group.vertices.empty() || group.indices.empty())
            continue;
        GxPrimLockVertexPtrs(
            static_cast<unsigned>(group.vertices.size()),
            group.vertices.data(), sizeof(NTempest::C3Vector),
            group.normals.data(), sizeof(NTempest::C3Vector),
            group.vertexColors.data(), sizeof(NTempest::CImVector),
            nullptr, 0,
            group.textureCoordinates.data(), sizeof(NTempest::C2Vector),
            group.lightmapCoordinates.data(), sizeof(NTempest::C2Vector));
        for (const CMapObjBatch& batch : group.batches) {
            if (batch.texture >= materials.size() ||
                batch.startIndex > group.indices.size() ||
                batch.count > group.indices.size() - batch.startIndex)
                continue;
            const CMapObjMaterial& material = materials[batch.texture];
            CGxTex* texture = material.diffuse
                ? TextureGetGxTex(material.diffuse, 1, nullptr) : nullptr;
            CGxTex* lightmap = batch.lightMap < group.lightmapTextures.size()
                ? group.lightmapTextures[batch.lightMap].texture : nullptr;
            GxRsSet(GxRs_Texture0, texture);
            GxRsSet(GxRs_Texture1, lightmap);
            GxRsSet(GxRs_Blend, static_cast<EGxBlend>(
                (std::min)(material.blendMode,
                           static_cast<std::uint32_t>(GxBlends_Last - 1U))));
            GxRsSet(GxRs_DepthWrite,
                    material.blendMode <= static_cast<std::uint32_t>(
                        GxBlend_AlphaKey));
            GxRsSet(GxRs_Lighting,
                    lightmap ? 0 : (material.flags & 0x1U) == 0U);
            // Recovered exterior and interior WMO batch paths enable fog
            // unless material flag 0x2 explicitly opts the batch out.
            GxRsSet(GxRs_Fog, (material.flags & 0x2U) == 0U);
            // Legacy WMO geometry is clockwise after the WoW-to-D3D11 axis
            // conversion.  State 2 keeps culling enabled while selecting that
            // front-face convention; flag 0x4 remains the authored two-sided
            // material override.
            GxRsSet(GxRs_Culling,
                    !forceTwoSided && (material.flags & 0x4U) == 0U ? 2 : 0);
            GxRsSet(GxRs_MatDiffuse, NTempest::CImVector(0xffffffffU));
            GxRsSet(GxRs_MatEmissive,
                    (material.flags & 0x10U) != 0U
                        ? material.frameSidnColor
                        : NTempest::CImVector(0x00000000U));
            GxPrimDrawElements(GxPrim_Triangles, batch.count,
                               group.indices.data() + batch.startIndex);
        }
        GxPrimUnlockVertexPtrs();
    }
    GxRsSet(GxRs_Texture1, static_cast<CGxTex*>(nullptr));
    GxXformPop(GxXform_World);
    GxRsPop();
}

void CMapObj::CullBatch() { }
void CMapObj::ExtRender() { }
void CMapObj::IntRender() { }
void CMapObj::LocateViewer() { }
void CMapObj::PrepareUpdate() { }
void CMapObj::RRenderThruPortals() { }
void CMapObj::RTransformPortal() { }
void CMapObj::RenderAlways() { }
void CMapObj::RenderExteriorWater_0() { }
void CMapObj::RenderGroup() { }
void CMapObj::RenderGroupBsp() { }
void CMapObj::RenderGroupColorTex() { }
void CMapObj::RenderGroupColorTex_Ext() { }
void CMapObj::RenderGroupColorTex_Int() { }
void CMapObj::RenderGroupLightTex() { }
void CMapObj::RenderGroupLightmap() { }
void CMapObj::RenderGroupLightmapTex() { }
void CMapObj::RenderGroupLightmapTex_Ext() { }
void CMapObj::RenderGroupLightmapTex_Int() { }
void CMapObj::RenderGroupNormals() { }
void CMapObj::RenderGroupTex() { }
void CMapObj::RenderGroup_Ext() { }
void CMapObj::RenderGroup_Int() { }
void CMapObj::RenderInteriorWater_0() { }
void CMapObj::RenderLiquid_0() { }
void CMapObj::RenderMagma() { }
void CMapObj::RenderPortals() { }
void CMapObj::RenderWaterIndices_0() { }
void CMapObj::SetGroupRenderCallback() { }
void CMapObj::StabPortals() { }
void CMapObj::UpdateMaterials() { }
void CMapObjGroup::ExtGxBufFillVertex() { }
void CMapObjGroup::GxBufFillIndex() { }
void CMapObjGroup::IntGxBufFillVertex() { }
