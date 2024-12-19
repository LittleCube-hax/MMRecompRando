#include "modding.h"
#include "global.h"

#include "z64player.h"

// temporary OoT tunic colors
Color_RGB8 sTunicColors[] = {
    { 30, 105, 27 },    // PLAYER_TUNIC_KOKIRI
    { 100, 20, 0 },     // PLAYER_TUNIC_GORON
    { 0, 60, 100 },     // PLAYER_TUNIC_ZORA
};

Color_RGB8* humanTunicColor = &sTunicColors[0];

extern Vtx object_link_childVtx_007900[];
extern u64 gLinkHumanSkinTLUT[];
extern u64 gLinkHumanMouthTLUT[];
extern u64 gLinkHumanEarTex[];
extern u64 gLinkHumanBeltTex[];
extern u64 gLinkHumanBeltClaspTex[];
extern u64 object_link_child_Tex_005400[];
extern u64 object_link_child_Tex_005500[];
extern u64 object_link_child_Tex_005540[];
extern u64 object_link_child_Tex_005C40[];
extern u64 object_link_child_Tex_005D40[];
extern u64 object_link_child_Tex_005D80[];
extern u64 object_link_child_Tex_006B00[];

Gfx gLinkHumanRightThighDL[] = {
    gsSPMatrix(0x0D000000, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[80], 10, 0),
    gsSPMatrix(0x0D000040, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005D80, G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[90], 21, 10),
    gsSP2Triangles(2, 3, 10, 0, 11, 4, 5, 0),
    gsSP2Triangles(12, 13, 6, 0, 14, 15, 7, 0),
    gsSP2Triangles(16, 8, 6, 0, 17, 3, 0, 0),
    gsSP2Triangles(4, 18, 19, 0, 6, 20, 21, 0),
    gsSP2Triangles(6, 7, 22, 0, 23, 9, 4, 0),
    gsSP2Triangles(5, 24, 25, 0, 26, 5, 2, 0),
    gsSP2Triangles(7, 9, 27, 0, 8, 28, 1, 0),
    gsSP1Triangle(0, 29, 30, 0),
    gsSPVertex(&object_link_childVtx_007900[111], 9, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 0, 4, 0),
    gsSP2Triangles(5, 6, 3, 0, 3, 7, 5, 0),
    gsSP2Triangles(2, 4, 0, 0, 4, 7, 3, 0),
    gsSP1Triangle(6, 5, 8, 0),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(object_link_child_Tex_005500, G_IM_FMT_CI, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[120], 10, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 2, 0),
    gsSP2Triangles(4, 0, 5, 0, 0, 2, 5, 0),
    gsSP2Triangles(6, 4, 5, 0, 1, 7, 3, 0),
    gsSP2Triangles(7, 8, 3, 0, 7, 6, 8, 0),
    gsSP2Triangles(6, 9, 8, 0, 5, 9, 6, 0),
    gsSPEndDisplayList(),
};

Gfx gLinkHumanLeftThighDL[] = {
    gsSPMatrix(0x0D000000, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[210], 10, 0),
    gsSPMatrix(0x0D000100, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005D80, G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[220], 21, 10),
    gsSP2Triangles(10, 5, 4, 0, 1, 0, 11, 0),
    gsSP2Triangles(3, 12, 13, 0, 6, 14, 15, 0),
    gsSP2Triangles(3, 2, 16, 0, 8, 5, 17, 0),
    gsSP2Triangles(18, 19, 0, 0, 20, 21, 3, 0),
    gsSP2Triangles(22, 6, 3, 0, 0, 7, 23, 0),
    gsSP2Triangles(24, 25, 1, 0, 4, 1, 26, 0),
    gsSP2Triangles(27, 7, 6, 0, 9, 28, 2, 0),
    gsSP1Triangle(29, 30, 8, 0),
    gsSPVertex(&object_link_childVtx_007900[241], 9, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 2, 4, 0),
    gsSP2Triangles(4, 5, 6, 0, 6, 7, 4, 0),
    gsSP2Triangles(2, 3, 0, 0, 4, 7, 3, 0),
    gsSP1Triangle(8, 6, 5, 0),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(object_link_child_Tex_005500, G_IM_FMT_CI, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[250], 10, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
    gsSP2Triangles(4, 2, 5, 0, 4, 0, 2, 0),
    gsSP2Triangles(4, 5, 6, 0, 3, 7, 1, 0),
    gsSP2Triangles(3, 8, 7, 0, 8, 6, 7, 0),
    gsSP2Triangles(8, 9, 6, 0, 6, 9, 4, 0),
    gsSPEndDisplayList(),
};

Gfx gLinkHumanWaistDL[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005D80, G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[260], 23, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(3, 5, 6, 0, 5, 4, 7, 0),
    gsSP2Triangles(7, 8, 5, 0, 9, 10, 11, 0),
    gsSP2Triangles(2, 9, 12, 0, 9, 13, 12, 0),
    gsSP2Triangles(10, 14, 11, 0, 4, 15, 7, 0),
    gsSP2Triangles(6, 16, 3, 0, 17, 1, 0, 0),
    gsSP2Triangles(15, 17, 0, 0, 2, 12, 0, 0),
    gsSP2Triangles(0, 7, 15, 0, 11, 13, 9, 0),
    gsSP2Triangles(12, 13, 18, 0, 19, 8, 20, 0),
    gsSP2Triangles(20, 12, 18, 0, 20, 21, 19, 0),
    gsSP2Triangles(20, 22, 21, 0, 8, 19, 5, 0),
    gsSP1Triangle(18, 22, 20, 0),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(gLinkHumanBeltClaspTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 16, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 5, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPVertex(&object_link_childVtx_007900[283], 7, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 3, 4, 0),
    gsSP2Triangles(2, 5, 0, 0, 4, 5, 2, 0),
    gsSP1Triangle(4, 6, 5, 0),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(gLinkHumanBeltTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 16, 0, G_TX_MIRROR | G_TX_CLAMP, G_TX_MIRROR
                         | G_TX_CLAMP, 3, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[290], 10, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 3, 0, 0),
    gsSP2Triangles(1, 4, 5, 0, 5, 2, 1, 0),
    gsSP2Triangles(5, 4, 6, 0, 6, 7, 5, 0),
    gsSP2Triangles(7, 6, 8, 0, 8, 9, 7, 0),
    gsSPEndDisplayList(),
};

Gfx gLinkHumanCollarDL[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005D40, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[551], 20, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(4, 0, 5, 0, 0, 6, 5, 0),
    gsSP2Triangles(2, 6, 0, 0, 7, 8, 9, 0),
    gsSP2Triangles(9, 10, 11, 0, 8, 10, 9, 0),
    gsSP2Triangles(12, 13, 14, 0, 15, 13, 12, 0),
    gsSP2Triangles(12, 14, 16, 0, 4, 5, 17, 0),
    gsSP2Triangles(18, 19, 17, 0, 17, 19, 4, 0),
    gsSPEndDisplayList(),
};

Gfx gLinkHumanTorsoDL[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005C40, G_IM_FMT_I, G_IM_SIZ_8b, 16, 16, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[819], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 0, 0),
    gsSP2Triangles(5, 6, 7, 0, 8, 9, 10, 0),
    gsSP2Triangles(11, 12, 13, 0, 1, 14, 7, 0),
    gsSP2Triangles(4, 3, 15, 0, 10, 13, 8, 0),
    gsSP2Triangles(14, 9, 7, 0, 16, 6, 17, 0),
    gsSP2Triangles(16, 2, 6, 0, 18, 8, 19, 0),
    gsSP2Triangles(18, 5, 8, 0, 20, 21, 3, 0),
    gsSP2Triangles(20, 22, 21, 0, 0, 20, 3, 0),
    gsSP2Triangles(2, 7, 6, 0, 5, 18, 23, 0),
    gsSP2Triangles(2, 20, 0, 0, 2, 22, 20, 0),
    gsSP2Triangles(2, 16, 22, 0, 17, 6, 23, 0),
    gsSP2Triangles(6, 5, 23, 0, 7, 9, 5, 0),
    gsSP2Triangles(11, 15, 12, 0, 15, 11, 4, 0),
    gsSP2Triangles(8, 5, 9, 0, 10, 9, 14, 0),
    gsSP2Triangles(7, 2, 1, 0, 19, 8, 13, 0),
    gsSP2Triangles(13, 10, 11, 0, 24, 25, 13, 0),
    gsSP2Triangles(26, 16, 27, 0, 28, 29, 25, 0),
    gsSP2Triangles(29, 18, 19, 0, 30, 24, 31, 0),
    gsSP2Triangles(12, 31, 24, 0, 18, 29, 27, 0),
    gsSP1Triangle(23, 18, 17, 0),
    gsSPVertex(&object_link_childVtx_007900[851], 17, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 2, 0),
    gsSP2Triangles(5, 1, 0, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 11, 5, 12, 0),
    gsSP2Triangles(4, 7, 13, 0, 9, 14, 15, 0),
    gsSP2Triangles(10, 6, 8, 0, 4, 3, 16, 0),
    gsSP2Triangles(0, 12, 5, 0, 2, 13, 0, 0),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(object_link_child_Tex_005500, G_IM_FMT_CI, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPVertex(&object_link_childVtx_007900[868], 14, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(3, 5, 6, 0, 7, 1, 8, 0),
    gsSP2Triangles(2, 1, 7, 0, 2, 9, 0, 0),
    gsSP2Triangles(10, 11, 12, 0, 12, 13, 10, 0),
    gsSP2Triangles(7, 13, 12, 0, 8, 13, 7, 0),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(gLinkHumanBeltClaspTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 16, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 5, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[882], 5, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 2, 0),
    gsSP1Triangle(1, 4, 3, 0),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_006B00, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0, G_TX_NOMIRROR |
                         G_TX_WRAP, G_TX_MIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[887], 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 7, 5, 4, 0),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(gLinkHumanBeltTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 16, 0, G_TX_MIRROR | G_TX_CLAMP, G_TX_MIRROR
                         | G_TX_CLAMP, 3, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[895], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(0, 3, 4, 0, 0, 4, 5, 0),
    gsSP2Triangles(0, 6, 1, 0, 0, 7, 6, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 11, 9, 0),
    gsSP2Triangles(8, 12, 13, 0, 8, 13, 14, 0),
    gsSP2Triangles(8, 14, 15, 0, 8, 10, 12, 0),
    gsSP2Triangles(16, 17, 18, 0, 19, 16, 18, 0),
    gsSP2Triangles(17, 20, 18, 0, 21, 19, 18, 0),
    gsSP2Triangles(22, 23, 24, 0, 22, 24, 25, 0),
    gsSP2Triangles(26, 22, 25, 0, 27, 28, 29, 0),
    gsSP1Triangle(27, 30, 28, 0),
    gsSPVertex(&object_link_childVtx_007900[926], 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 7, 5, 0),
    gsSPEndDisplayList(),
};

Gfx gLinkHumanHeadDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[338], 7, 0),
    gsSPMatrix(0x0D0001C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(object_link_child_Tex_005500, G_IM_FMT_CI, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPVertex(&object_link_childVtx_007900[345], 15, 7),
    gsSP2Triangles(7, 3, 5, 0, 5, 8, 9, 0),
    gsSP2Triangles(3, 10, 11, 0, 12, 0, 1, 0),
    gsSP2Triangles(1, 13, 14, 0, 15, 16, 5, 0),
    gsSP2Triangles(6, 0, 17, 0, 18, 2, 4, 0),
    gsSP2Triangles(19, 20, 1, 0, 1, 2, 21, 0),
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0xFF, 255, 255, 255, 255),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(0x08000000, G_IM_FMT_CI, G_IM_SIZ_8b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR |
                         G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[360], 19, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 3, 0, 0),
    gsSP2Triangles(4, 0, 5, 0, 5, 0, 6, 0),
    gsSP2Triangles(6, 0, 7, 0, 4, 1, 0, 0),
    gsSP2Triangles(8, 1, 4, 0, 6, 9, 5, 0),
    gsSP2Triangles(7, 10, 6, 0, 7, 0, 11, 0),
    gsSP2Triangles(0, 3, 11, 0, 4, 5, 12, 0),
    gsSP2Triangles(13, 12, 5, 0, 13, 5, 14, 0),
    gsSP2Triangles(5, 9, 14, 0, 15, 10, 7, 0),
    gsSP2Triangles(15, 7, 16, 0, 7, 17, 16, 0),
    gsSP2Triangles(17, 7, 11, 0, 11, 3, 18, 0),
    gsSP2Triangles(17, 11, 18, 0, 8, 4, 12, 0),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanMouthTLUT),
    gsDPLoadTextureBlock(0x09000000, G_IM_FMT_CI, G_IM_SIZ_8b, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR |
                         G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[379], 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(2, 3, 4, 0, 1, 5, 3, 0),
    gsSP2Triangles(3, 5, 6, 0, 3, 6, 4, 0),
    gsSP1Triangle(4, 6, 7, 0),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005D80, G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[387], 22, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 1, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 5, 7, 6, 0),
    gsSP2Triangles(3, 1, 7, 0, 7, 8, 3, 0),
    gsSP2Triangles(8, 7, 5, 0, 9, 8, 5, 0),
    gsSP2Triangles(9, 5, 4, 0, 4, 10, 9, 0),
    gsSP2Triangles(4, 11, 10, 0, 11, 12, 13, 0),
    gsSP2Triangles(10, 11, 13, 0, 13, 14, 10, 0),
    gsSP2Triangles(14, 9, 10, 0, 14, 8, 9, 0),
    gsSP2Triangles(15, 16, 12, 0, 11, 15, 12, 0),
    gsSP2Triangles(15, 11, 4, 0, 4, 17, 15, 0),
    gsSP2Triangles(17, 16, 15, 0, 17, 18, 16, 0),
    gsSP2Triangles(19, 16, 18, 0, 18, 20, 19, 0),
    gsSP2Triangles(6, 18, 17, 0, 6, 17, 4, 0),
    gsSP2Triangles(19, 20, 21, 0, 21, 20, 0, 0),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(object_link_child_Tex_005500, G_IM_FMT_CI, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPVertex(&object_link_childVtx_007900[409], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 8, 15, 6, 0),
    gsSP2Triangles(8, 16, 17, 0, 18, 16, 8, 0),
    gsSP2Triangles(16, 18, 19, 0, 8, 17, 20, 0),
    gsSP2Triangles(18, 8, 7, 0, 8, 20, 15, 0),
    gsSP2Triangles(15, 20, 21, 0, 22, 21, 23, 0),
    gsSP2Triangles(20, 24, 21, 0, 25, 23, 26, 0),
    gsSP2Triangles(21, 26, 23, 0, 24, 26, 21, 0),
    gsSP2Triangles(27, 15, 21, 0, 21, 22, 27, 0),
    gsSP2Triangles(28, 29, 30, 0, 2, 31, 0, 0),
    gsSPVertex(&object_link_childVtx_007900[441], 6, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005400, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[447], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(4, 3, 6, 0, 7, 8, 9, 0),
    gsSP2Triangles(7, 10, 8, 0, 11, 10, 7, 0),
    gsSP2Triangles(10, 11, 12, 0, 13, 14, 15, 0),
    gsSP2Triangles(15, 14, 16, 0, 16, 17, 15, 0),
    gsSP2Triangles(17, 16, 18, 0, 19, 20, 21, 0),
    gsSP2Triangles(20, 22, 21, 0, 23, 24, 25, 0),
    gsSP2Triangles(25, 24, 17, 0, 24, 26, 17, 0),
    gsSP2Triangles(17, 26, 15, 0, 26, 27, 15, 0),
    gsSP2Triangles(27, 13, 15, 0, 13, 27, 28, 0),
    gsSP2Triangles(27, 26, 29, 0, 29, 28, 27, 0),
    gsSP2Triangles(30, 28, 31, 0, 29, 31, 28, 0),
    gsSPVertex(&object_link_childVtx_007900[479], 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 9, 8, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 11, 12, 13, 0),
    gsSP2Triangles(10, 14, 15, 0, 16, 15, 14, 0),
    gsSP2Triangles(15, 16, 17, 0, 18, 19, 20, 0),
    gsSP2Triangles(21, 22, 23, 0, 22, 21, 24, 0),
    gsSP2Triangles(25, 21, 23, 0, 23, 26, 25, 0),
    gsSP2Triangles(21, 25, 27, 0, 28, 27, 25, 0),
    gsSP2Triangles(28, 25, 26, 0, 28, 26, 29, 0),
    gsSP2Triangles(29, 26, 30, 0, 29, 30, 31, 0),
    gsSPVertex(&object_link_childVtx_007900[511], 27, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 4, 0),
    gsSP2Triangles(5, 4, 3, 0, 3, 6, 5, 0),
    gsSP2Triangles(5, 6, 7, 0, 8, 9, 10, 0),
    gsSP2Triangles(11, 10, 9, 0, 12, 10, 11, 0),
    gsSP2Triangles(13, 12, 11, 0, 14, 15, 16, 0),
    gsSP2Triangles(17, 16, 15, 0, 18, 19, 20, 0),
    gsSP2Triangles(21, 22, 23, 0, 21, 23, 24, 0),
    gsSP1Triangle(25, 26, 18, 0),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(object_link_child_Tex_005540, G_IM_FMT_CI, G_IM_SIZ_8b, 16, 16, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[538], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(gLinkHumanEarTex, G_IM_FMT_CI, G_IM_SIZ_8b, 16, 16, 0, G_TX_MIRROR | G_TX_CLAMP, G_TX_MIRROR |
                         G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[541], 10, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 0, 2, 0),
    gsSP2Triangles(2, 4, 3, 0, 5, 6, 7, 0),
    gsSP2Triangles(7, 8, 5, 0, 7, 9, 8, 0),
    gsSPEndDisplayList(),
};

Gfx gLinkHumanHatDL[] = {
    gsSPMatrix(0x0D0001C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[300], 6, 0),
    gsSPMatrix(0x0D000200, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005D80, G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[306], 20, 6),
    gsSP2Triangles(6, 7, 0, 0, 8, 3, 1, 0),
    gsSP2Triangles(9, 1, 0, 0, 10, 11, 2, 0),
    gsSP2Triangles(2, 3, 12, 0, 1, 13, 14, 0),
    gsSP2Triangles(15, 16, 3, 0, 4, 17, 18, 0),
    gsSP2Triangles(19, 20, 5, 0, 21, 4, 2, 0),
    gsSP2Triangles(2, 22, 23, 0, 0, 5, 24, 0),
    gsSP1Triangle(5, 4, 25, 0),
    gsSPVertex(&object_link_childVtx_007900[326], 12, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 4, 0),
    gsSP2Triangles(4, 5, 6, 0, 7, 8, 2, 0),
    gsSP2Triangles(4, 6, 0, 0, 8, 3, 0, 0),
    gsSP2Triangles(6, 1, 0, 0, 2, 8, 0, 0),
    gsSP2Triangles(9, 8, 7, 0, 9, 10, 11, 0),
    gsSP2Triangles(9, 3, 8, 0, 9, 11, 4, 0),
    gsSP2Triangles(11, 5, 4, 0, 4, 3, 9, 0),
    gsSP1Triangle(7, 10, 9, 0),
    gsSPEndDisplayList(),
};

Gfx gLinkHumanRightShoulderDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[745], 6, 0),
    gsSPMatrix(0x0D000340, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005C40, G_IM_FMT_I, G_IM_SIZ_8b, 16, 16, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[751], 18, 6),
    gsSP2Triangles(0, 1, 6, 0, 7, 2, 8, 0),
    gsSP2Triangles(9, 2, 0, 0, 3, 10, 11, 0),
    gsSP2Triangles(3, 12, 13, 0, 4, 14, 1, 0),
    gsSP2Triangles(15, 4, 3, 0, 16, 0, 17, 0),
    gsSP2Triangles(5, 2, 18, 0, 0, 19, 20, 0),
    gsSP2Triangles(21, 3, 5, 0, 22, 1, 23, 0),
    gsSPVertex(&object_link_childVtx_007900[769], 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 1, 4, 0),
    gsSP2Triangles(2, 5, 6, 0, 1, 3, 7, 0),
    gsSP2Triangles(7, 5, 2, 0, 7, 2, 1, 0),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(object_link_child_Tex_005500, G_IM_FMT_CI, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPVertex(&object_link_childVtx_007900[777], 11, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 2, 7, 0, 8, 9, 1, 0),
    gsSP2Triangles(9, 8, 4, 0, 3, 9, 4, 0),
    gsSP2Triangles(7, 5, 6, 0, 1, 0, 8, 0),
    gsSP2Triangles(5, 7, 3, 0, 2, 6, 0, 0),
    gsSP2Triangles(6, 5, 10, 0, 10, 5, 4, 0),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005D40, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[788], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPEndDisplayList(),
};

Gfx gLinkHumanLeftShoulderDL[] = {
    gsSPMatrix(0x0D000440, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPVertex(&object_link_childVtx_007900[635], 6, 0),
    gsSPMatrix(0x0D000280, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005C40, G_IM_FMT_I, G_IM_SIZ_8b, 16, 16, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[641], 18, 6),
    gsSP2Triangles(6, 4, 3, 0, 7, 5, 8, 0),
    gsSP2Triangles(3, 5, 9, 0, 10, 11, 0, 0),
    gsSP2Triangles(12, 13, 0, 0, 4, 14, 2, 0),
    gsSP2Triangles(0, 2, 15, 0, 16, 3, 17, 0),
    gsSP2Triangles(18, 5, 1, 0, 19, 20, 3, 0),
    gsSP2Triangles(1, 0, 21, 0, 22, 4, 23, 0),
    gsSPVertex(&object_link_childVtx_007900[659], 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 1, 4, 0),
    gsSP2Triangles(5, 6, 0, 0, 7, 4, 1, 0),
    gsSP2Triangles(0, 6, 7, 0, 1, 0, 7, 0),
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsDPLoadTLUT_pal256(gLinkHumanSkinTLUT),
    gsDPLoadTextureBlock(object_link_child_Tex_005500, G_IM_FMT_CI, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsSPVertex(&object_link_childVtx_007900[667], 11, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 0, 7, 0, 1, 8, 9, 0),
    gsSP2Triangles(4, 9, 8, 0, 4, 8, 5, 0),
    gsSP2Triangles(7, 3, 6, 0, 9, 2, 1, 0),
    gsSP2Triangles(5, 6, 3, 0, 2, 7, 0, 0),
    gsSP2Triangles(10, 3, 7, 0, 4, 3, 10, 0),
    gsDPPipeSync(),
    // gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, TEXEL0, ENVIRONMENT, 0, COMBINED, 0, 0, 0, 0, COMBINED),
    gsDPSetPrimColor(0, 0xFF, 30, 105, 27, 255),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPLoadTextureBlock(object_link_child_Tex_005D40, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_MIRROR | G_TX_CLAMP,
                         G_TX_MIRROR | G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(&object_link_childVtx_007900[678], 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPEndDisplayList(),
};

void updateHumanLinkColor(PlayState* play, s32 limbIndex, Gfx** dList)
{
	if(limbIndex == PLAYER_LIMB_RIGHT_THIGH) {
		*dList = gLinkHumanRightThighDL;
	}
	if(limbIndex == PLAYER_LIMB_LEFT_THIGH) {
		*dList = gLinkHumanLeftThighDL;
	}
	if(limbIndex == PLAYER_LIMB_WAIST) {
		*dList = gLinkHumanWaistDL;
	}
	if(limbIndex == PLAYER_LIMB_COLLAR) {
		*dList = gLinkHumanCollarDL;
	}
	if(limbIndex == PLAYER_LIMB_TORSO) {
		*dList = gLinkHumanTorsoDL;
	}
	if(limbIndex == PLAYER_LIMB_HEAD) {
		*dList = gLinkHumanHeadDL;
	}
	if(limbIndex == PLAYER_LIMB_HAT) {
		*dList = gLinkHumanHatDL;
	}
	if(limbIndex == PLAYER_LIMB_RIGHT_SHOULDER) {
		*dList = gLinkHumanRightShoulderDL;
	}
	if(limbIndex == PLAYER_LIMB_LEFT_SHOULDER) {
		*dList = gLinkHumanLeftShoulderDL;
	}

	OPEN_DISPS(play->state.gfxCtx);
	gDPSetEnvColor(POLY_OPA_DISP++, humanTunicColor->r, humanTunicColor->g, humanTunicColor->b, 0);
	CLOSE_DISPS(play->state.gfxCtx);
}

extern PlayerModelType sPlayerLeftHandType;
extern PlayerModelType sPlayerRightHandType;
extern s32 D_801F59E0;
extern Gfx* gPlayerLeftHandOpenDLs[2 * PLAYER_FORM_MAX];
extern Gfx* gPlayerLeftHandClosedDLs[2 * PLAYER_FORM_MAX];
extern Gfx* D_801C018C[];
extern Gfx** D_801C095C[];
extern s32 sPlayerLod;
extern LinkAnimationHeader gPlayerAnim_pg_punchA;
extern struct_80124618 D_801C0750[];
extern Gfx gLinkZoraLeftHandOpenDL[];
extern LinkAnimationHeader gPlayerAnim_pz_gakkistart;
extern LinkAnimationHeader gPlayerAnim_pz_gakkiplay;
extern Gfx object_link_zora_DL_00E2A0[];
extern Gfx** leftHandDLists;
extern struct_80124618 D_801C0538[];
extern struct_80124618 D_801C0560[];
extern Gfx gLinkZoraRightHandOpenDL[];
extern Gfx* gPlayerHandHoldingShields[2 * (PLAYER_SHIELD_MAX - 1)];
extern Gfx* gPlayerRightHandClosedDLs[2 * PLAYER_FORM_MAX];
extern Gfx** D_801C0964[];
extern LinkAnimationHeader gPlayerAnim_pg_punchB;
extern struct_80124618 D_801C0784[];
extern Gfx* gPlayerSheathedSwords[];
extern Gfx* gPlayerSwordSheaths[];

s32 Player_OverrideLimbDrawGameplayCommon(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx);
void func_80125CE0(Player* player, struct_80124618* arg1, Vec3f* pos, Vec3s* rot);

RECOMP_PATCH s32 Player_OverrideLimbDrawGameplayDefault(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                           Actor* actor) {
    Player* player = (Player*)actor;

    if (!Player_OverrideLimbDrawGameplayCommon(play, limbIndex, dList, pos, rot, &player->actor)) {
        // change human link's colors
		if(player->transformation == PLAYER_FORM_HUMAN)
		{
			updateHumanLinkColor(play, limbIndex, dList);
		}
        
        if (limbIndex == PLAYER_LIMB_LEFT_HAND) {
            Gfx** leftHandDLists = player->leftHandDLists;
            EquipValueSword swordEquipValue;

            if (player->stateFlags3 & PLAYER_STATE3_2000) {
                rot->z -= player->unk_B8C;
            } else if ((sPlayerLeftHandType == PLAYER_MODELTYPE_LH_4) &&
                       (player->stateFlags1 & PLAYER_STATE1_2000000)) {
                leftHandDLists = &gPlayerLeftHandOpenDLs[D_801F59E0];
                sPlayerLeftHandType = PLAYER_MODELTYPE_LH_OPEN;
            } else if ((player->leftHandType == PLAYER_MODELTYPE_LH_OPEN) && (player->actor.speed > 2.0f) &&
                       !(player->stateFlags1 & PLAYER_STATE1_8000000)) {
                leftHandDLists = &gPlayerLeftHandClosedDLs[D_801F59E0];
                sPlayerLeftHandType = PLAYER_MODELTYPE_LH_CLOSED;
            } else if ((player->leftHandType == PLAYER_MODELTYPE_LH_ONE_HAND_SWORD) &&
                       (player->transformation == PLAYER_FORM_HUMAN) &&
                       ((swordEquipValue = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD),
                         swordEquipValue != EQUIP_VALUE_SWORD_NONE))) {
                leftHandDLists = &D_801C018C[2 * ((swordEquipValue - 1) ^ 0)];
            } else {
                s32 handIndex = GET_LEFT_HAND_INDEX_FROM_JOINT_TABLE(player->skelAnime.jointTable);

                if (handIndex != 0) {
                    handIndex = (handIndex >> 12) - 1;
                    if (handIndex >= 2) {
                        handIndex = 0;
                    }
                    leftHandDLists = &D_801C095C[handIndex][D_801F59E0];
                }
            }

            *dList = leftHandDLists[sPlayerLod];

            if (player->transformation == PLAYER_FORM_GORON) {
                if (player->skelAnime.animation == &gPlayerAnim_pg_punchA) {
                    func_80125CE0(player, D_801C0750, pos, rot);
                }
            } else if (player->transformation == PLAYER_FORM_ZORA) {
                if ((player->stateFlags1 & PLAYER_STATE1_2) || (player->stateFlags1 & PLAYER_STATE1_400) ||
                    func_801242B4(player)) {
                    *dList = gLinkZoraLeftHandOpenDL;
                } else {
                    s32 phi_a1 = (player->skelAnime.animation == &gPlayerAnim_pz_gakkistart) &&
                                 (player->skelAnime.curFrame >= 6.0f);

                    if (phi_a1 || (player->skelAnime.animation == &gPlayerAnim_pz_gakkiplay)) {
                        *dList = object_link_zora_DL_00E2A0;
                        func_80125CE0(player, phi_a1 ? D_801C0538 : D_801C0560, pos, rot);
                    }
                }
            }
        } else if (limbIndex == PLAYER_LIMB_RIGHT_HAND) {
            if ((player->transformation == PLAYER_FORM_ZORA) &&
                (((player->stateFlags1 & PLAYER_STATE1_2)) || (player->stateFlags1 & PLAYER_STATE1_400) ||
                 func_801242B4(player))) {
                *dList = gLinkZoraRightHandOpenDL;
            } else {
                Gfx** rightHandDLists = player->rightHandDLists;

                if (player->stateFlags3 & PLAYER_STATE3_2000) {
                    rot->z -= player->unk_B8C;
                }

                if (sPlayerRightHandType == PLAYER_MODELTYPE_RH_SHIELD) {
                    if (player->transformation == PLAYER_FORM_HUMAN) {
                        if (player->currentShield != PLAYER_SHIELD_NONE) {
                            //! FAKE
                            rightHandDLists = &gPlayerHandHoldingShields[2 * ((player->currentShield - 1) ^ 0)];
                        }
                    }
                } else if ((player->rightHandType == PLAYER_MODELTYPE_RH_OPEN) && (player->actor.speed > 2.0f) &&
                           (!(player->stateFlags1 & PLAYER_STATE1_8000000))) {
                    rightHandDLists = &gPlayerRightHandClosedDLs[D_801F59E0];
                    sPlayerRightHandType = PLAYER_MODELTYPE_RH_CLOSED;
                } else {
                    s32 handIndex = GET_RIGHT_HAND_INDEX_FROM_JOINT_TABLE(player->skelAnime.jointTable);

                    if (handIndex != 0) {
                        handIndex = (handIndex >> 8) - 1;
                        rightHandDLists = &D_801C0964[handIndex][D_801F59E0];
                    }
                }

                *dList = rightHandDLists[sPlayerLod];
                if (player->skelAnime.animation == &gPlayerAnim_pg_punchB) {
                    func_80125CE0(player, D_801C0784, pos, rot);
                }
            }
        } else if (limbIndex == PLAYER_LIMB_SHEATH) {
            Gfx** sheathDLists = player->sheathDLists;

            if (player->transformation == PLAYER_FORM_HUMAN) {
                EquipValueSword swordEquipValue = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);

                if (swordEquipValue != EQUIP_VALUE_SWORD_NONE) {
                    if ((player->sheathType == PLAYER_MODELTYPE_SHEATH_14) ||
                        (player->sheathType == PLAYER_MODELTYPE_SHEATH_12)) {
                        sheathDLists = &gPlayerSheathedSwords[2 * ((swordEquipValue - 1) ^ 0)];
                    } else {
                        sheathDLists = &gPlayerSwordSheaths[2 * ((swordEquipValue - 1) ^ 0)];
                    }
                }
            }

            *dList = sheathDLists[sPlayerLod];
        } else if (limbIndex == PLAYER_LIMB_WAIST) {
            *dList = player->waistDLists[sPlayerLod];
			// not sure why the waist is reupdated here
			if(player->transformation == PLAYER_FORM_HUMAN)
			{
				updateHumanLinkColor(play, limbIndex, dList);
			}
        } else if (limbIndex == PLAYER_LIMB_HAT) {
            if (player->transformation == PLAYER_FORM_ZORA) {
                Matrix_Scale((player->unk_B10[0] * 1) + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
            }
        }
    }

    return false;
}