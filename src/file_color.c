#include "modding.h"
#include "global.h"

// set to default values for now
Color_RGB16 windowColor = {100, 150, 255};

extern Color_RGB8 heartColor;
extern Color_RGB8 heartColorBG;

// s16 sWindowContentColors[] = { 100, 150, 255 };
extern s16 sWindowContentColors[];

#include "overlays/gamestates/ovl_file_choose/z_file_select.h"

extern TexturePtr sTitleLabels[];
extern TexturePtr sFileInfoBoxTextures[];
extern s16 sFileInfoBoxPartWidths[];
extern TexturePtr sFileButtonTextures[];
extern u64 gFileSelFileNameBoxTex[];
extern u64 gFileSelConnectorTex[];
extern u64 gFileSelBlankButtonTex[];
extern TexturePtr sActionButtonTextures[];
extern u64 gFileSelOptionsButtonENGTex[];
extern u64 gFileSelBigButtonHighlightTex[];
extern TexturePtr sWarningLabels[];

void FileSelect_DrawFileInfo(GameState* thisx, s16 fileIndex);

/**
 * Draw most window contents including buttons, labels, and icons.
 * Does not include anything from the keyboard and settings windows.
 */
RECOMP_PATCH void FileSelect_DrawWindowContents(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 fileIndex;
    s16 temp;
    s16 i;
    s16 quadVtxIndex;

	// this part could be done better, but hey it works
	sWindowContentColors[0] = windowColor.r;
	sWindowContentColors[1] = windowColor.g;
	sWindowContentColors[2] = windowColor.b;
	this->windowColor[0] = sWindowContentColors[0];
	this->windowColor[1] = sWindowContentColors[1];
	this->windowColor[2] = sWindowContentColors[2];

    // TODO: add seperate + default color
	Skybox_SetColors(&this->skyboxCtx, windowColor.r, windowColor.g, windowColor.b, 0, 0, 0);

    if (1) {}

    OPEN_DISPS(this->state.gfxCtx);

    // draw title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_CUR]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[0], 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[this->titleLabel], G_IM_FMT_IA, G_IM_SIZ_8b, 128, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    // draw next title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[FS_TITLE_NEXT]);
    gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[this->nextTitleLabel], G_IM_FMT_IA, G_IM_SIZ_8b, 128, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    temp = 4;

    gDPPipeSync(POLY_OPA_DISP++);

    // draw file info box (large box when a file is selected)
    for (fileIndex = 0; fileIndex < 3; fileIndex++, temp += 28) {
        if (fileIndex < 2) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->fileInfoAlpha[fileIndex]);
            gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[temp], 28, 0);

            for (quadVtxIndex = 0, i = 0; i < 7; i++, quadVtxIndex += 4) {
                if ((i < 5) || (this->isOwlSave[fileIndex + 2] && (i >= 5))) {
                    gDPLoadTextureBlock(POLY_OPA_DISP++, sFileInfoBoxTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b,
                                        sFileInfoBoxPartWidths[i], 56, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                    gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1,
                                   0);
                }
            }
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);

    for (i = 0; i < 3; i++, temp += 16) {
        if (i < 2) {
            // draw file button
            gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[temp], 16, 0);

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                            sWindowContentColors[2], this->fileButtonAlpha[i]);
            gDPLoadTextureBlock(POLY_OPA_DISP++, sFileButtonTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b, 64, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

            // draw file name box
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                            sWindowContentColors[2], this->nameBoxAlpha[i]);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelFileNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                            sWindowContentColors[2], this->connectorAlpha[i]);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelConnectorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);

            if (this->isOwlSave[i + 2]) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[0], sWindowContentColors[1],
                                sWindowContentColors[2], this->nameBoxAlpha[i]);
                gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelBlankButtonTex, G_IM_FMT_IA, G_IM_SIZ_16b, 52, 16, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);
            }
        }
    }

    // draw file info
    for (fileIndex = 0; fileIndex < 2; fileIndex++) {
        FileSelect_DrawFileInfo(&this->state, fileIndex);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[0x3AC], 20, 0);

    // draw primary action buttons (copy/erase)
    for (quadVtxIndex = 0, i = 0; i < 2; i++, quadVtxIndex += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->actionButtonAlpha[i]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b, 64, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    // draw confirm buttons (yes/quit)
    for (quadVtxIndex = 0, i = FS_BTN_CONFIRM_YES; i <= FS_BTN_CONFIRM_QUIT; i++, quadVtxIndex += 4) {
        temp = this->confirmButtonTexIndices[i];
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->confirmButtonAlpha[i]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[temp], G_IM_FMT_IA, G_IM_SIZ_16b, 64, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
    }

    // draw options button
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                    this->optionButtonAlpha);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelOptionsButtonENGTex, G_IM_FMT_IA, G_IM_SIZ_16b, 64, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);

    // draw highlight over currently selected button
    if (((this->menuMode == FS_MENU_MODE_CONFIG) &&
         ((this->configMode == CM_MAIN_MENU) || (this->configMode == CM_SELECT_COPY_SOURCE) ||
          (this->configMode == CM_SELECT_COPY_DEST) || (this->configMode == CM_COPY_CONFIRM) ||
          (this->configMode == CM_ERASE_SELECT) || (this->configMode == CM_ERASE_CONFIRM))) ||
        ((this->menuMode == FS_MENU_MODE_SELECT) && (this->selectMode == SM_CONFIRM_FILE))) {
        gDPPipeSync(POLY_OPA_DISP++);

        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->highlightColor[0], this->highlightColor[1],
                        this->highlightColor[2], this->highlightColor[3]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelBigButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 72, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);
    }

    // draw warning labels
    if (this->warningLabel > FS_WARNING_NONE) {
        gDPPipeSync(POLY_OPA_DISP++);

        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->emptyFileTextAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sWarningLabels[this->warningLabel], G_IM_FMT_IA, G_IM_SIZ_8b, 128, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(this->state.gfxCtx);
}

s16 sFileSelRupeePrimColors[3][3] = {
    { 200, 255, 100 }, // Default Wallet
    { 170, 170, 255 }, // Adult Wallet
    { 255, 105, 105 }, // Giant Wallet
};
s16 sFileSelRupeeEnvColors[3][3] = {
    { 0, 80, 0 },   // Default Wallet
    { 10, 10, 80 }, // Adult Wallet
    { 40, 10, 0 },  // Giant Wallet
};
// static s16 sHeartPrimColors[2][3] = {
//     { 255, 70, 50 },
//     { 200, 0, 0 },
// };
// static s16 sHeartEnvColors[2][3] = {
//     { 50, 40, 60 },
//     { 255, 255, 255 },
// };
extern Color_RGB8 heartColors[];
extern Color_RGB8 heartColorsBG[];

extern u16 D_80814654[];
extern u64 gFileSelRupeeTex[];
extern s16 sWalletFirstDigit[];
extern TexturePtr sFileSelHeartPieceTextures[];
extern u8 sHealthToQuarterHeartCount[];
extern TexturePtr sFileSelRemainsTextures[];
extern u64 gFileSelMASKSENGTex[];
extern u64 gFileSelOwlSaveIconTex[];
extern TexturePtr sFileSelDayENGTextures[];

extern u64 gHeartEmptyTex[];
extern u64 gHeartQuarterTex[];
extern u64 gHeartHalfTex[];
extern u64 gHeartThreeQuarterTex[];
extern u64 gHeartFullTex[];
extern u64 gDefenseHeartEmptyTex[];
extern u64 gDefenseHeartQuarterTex[];
extern u64 gDefenseHeartHalfTex[];
extern u64 gDefenseHeartThreeQuarterTex[];
extern u64 gDefenseHeartFullTex[];
static TexturePtr sHeartTextures[2][5] = {
    {
        gHeartEmptyTex,
        gHeartQuarterTex,
        gHeartHalfTex,
        gHeartThreeQuarterTex,
        gHeartFullTex,
    },
    {
        gDefenseHeartEmptyTex,
        gDefenseHeartQuarterTex,
        gDefenseHeartHalfTex,
        gDefenseHeartThreeQuarterTex,
        gDefenseHeartFullTex,
    },
};

void FileSelect_SplitNumber(u16 value, u16* hundreds, u16* tens, u16* ones);
Gfx* FileSelect_DrawTexQuadIA8(Gfx* gfx, TexturePtr texture, s16 width, s16 height, s16 point);

RECOMP_PATCH void FileSelect_DrawFileInfo(GameState* thisx, s16 fileIndex) {
    FileSelectState* this = (FileSelectState*)thisx;
    Font* font = &this->font;
    s16 j;
    s16 vtxOffset;
    s32 heartType;
    s16 i;
    s16 sp20C;
    s16 health;
    s16 timeDigits[5];
    u16 digits[3]; // rupees and mask count
    u8 quarterHeartCount;

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    sp20C = fileIndex;

    // draw file name
    if (this->nameAlpha[fileIndex] != 0) {
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + (4 * 8)], 32, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->nameAlpha[fileIndex]);

        for (vtxOffset = 0, i = 0; vtxOffset < (4 * 8); i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx,
                                     font->fontBuf + this->fileNames[fileIndex][i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex]], 32, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->nameAlpha[fileIndex]);

        for (vtxOffset = 0, i = 0; vtxOffset < (4 * 8); i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx,
                                     font->fontBuf + this->fileNames[fileIndex][i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }
    }

    if ((fileIndex == this->selectedFileIndex) || (fileIndex == this->copyDestFileIndex)) {
        if (this->isOwlSave[fileIndex + 2]) {
            sp20C = fileIndex + 2;
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sFileSelRupeePrimColors[this->walletUpgrades[sp20C]][0],
                        sFileSelRupeePrimColors[this->walletUpgrades[sp20C]][1],
                        sFileSelRupeePrimColors[this->walletUpgrades[sp20C]][2], this->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, sFileSelRupeeEnvColors[this->walletUpgrades[sp20C]][0],
                       sFileSelRupeeEnvColors[this->walletUpgrades[sp20C]][1],
                       sFileSelRupeeEnvColors[this->walletUpgrades[sp20C]][2], 255);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xC8], 4, 0);

        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelRupeeTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x4C], 12, 0);

        FileSelect_SplitNumber((u16)this->rupees[sp20C], &digits[0], &digits[1], &digits[2]);

        for (vtxOffset = 0, i = sWalletFirstDigit[this->walletUpgrades[sp20C]]; i < 3; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + digits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        if (this->rupees[sp20C] == gUpgradeCapacities[4][this->walletUpgrades[sp20C]]) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 120, 255, 0, this->fileInfoAlpha[fileIndex]);
        } else if (this->rupees[sp20C] != 0) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 100, 100, 100, this->fileInfoAlpha[fileIndex]);
        }

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x40], 12, 0);

        for (vtxOffset = 0, i = sWalletFirstDigit[this->walletUpgrades[sp20C]]; i < 3; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + digits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 0, 0, this->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xCC], 4, 0);

        POLY_OPA_DISP = FileSelect_DrawTexQuadIA8(
            POLY_OPA_DISP, sFileSelHeartPieceTextures[this->heartPieceCount[sp20C]], 0x18, 0x10, (s16)0);

        if (this->defenseHearts[sp20C] == 0) {
            heartType = 0;
        } else {
            heartType = 1;
        }

        // gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sHeartPrimColors[heartType][0], sHeartPrimColors[heartType][1],
        //                 sHeartPrimColors[heartType][2], this->fileInfoAlpha[fileIndex]);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, heartColor.r, heartColor.g,
                        heartColor.b, this->fileInfoAlpha[fileIndex]);
        // gDPSetEnvColor(POLY_OPA_DISP++, sHeartEnvColors[heartType][0], sHeartEnvColors[heartType][1],
        //                sHeartEnvColors[heartType][2], 255);
        gDPSetEnvColor(POLY_OPA_DISP++, heartColorBG.r, heartColorBG.g,
                       heartColorBG.b, 255);

        i = this->healthCapacity[sp20C] / 0x10;

        health = this->health[sp20C];
        if (health <= 0x30) {
            health = 0x30;
        }

        quarterHeartCount = 4;
        for (vtxOffset = 0, j = 0; j < i; j++, vtxOffset += 4) {
            if (health < 0x10) {
                if (health != 0) {
                    quarterHeartCount = sHealthToQuarterHeartCount[health];
                    health = 0;
                } else {
                    quarterHeartCount = 0;
                }
            } else {
                health -= 0x10;
            }

            gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x68 + vtxOffset], 4, 0);
            POLY_OPA_DISP =
                FileSelect_DrawTexQuadIA8(POLY_OPA_DISP, sHeartTextures[heartType][quarterHeartCount], 0x10, 0x10, 0);
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

        for (vtxOffset = 0, j = 0; j < 4; j++, vtxOffset += 4) {
            if (this->questItems[sp20C] & gBitFlags[j]) {
                gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xB8 + vtxOffset], 4, 0);
                gDPLoadTextureBlock(POLY_OPA_DISP++, sFileSelRemainsTextures[j], G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            }
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xD0], 8, 0);
        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelMASKSENGTex, G_IM_FMT_I, 64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        gDPPipeSync(POLY_OPA_DISP++);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x60], 8, 0);

        FileSelect_SplitNumber(this->maskCount[sp20C], &digits[0], &digits[1], &digits[2]);

        for (vtxOffset = 0, i = 1; i < 3; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + digits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0x58], 8, 0);

        for (vtxOffset = 0, i = 1; i < 3; i++, vtxOffset += 4) {

            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + digits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }
    }

    if (this->isOwlSave[fileIndex + 2]) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->nameAlpha[fileIndex]);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xD8], 4, 0);

        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelOwlSaveIconTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 12, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xDC], 8, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, sFileSelDayENGTextures[this->day[sp20C]], G_IM_FMT_I, 48, 24, 0,
                               G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        timeDigits[0] = 0;
        timeDigits[1] = TIME_TO_MINUTES_F(this->time[sp20C]) / 60.0f;

        while (timeDigits[1] >= 10) {
            timeDigits[0]++;
            timeDigits[1] -= 10;
        }

        timeDigits[3] = 0;
        timeDigits[4] = (s32)TIME_TO_MINUTES_F(this->time[sp20C]) % 60;

        while (timeDigits[4] >= 10) {
            timeDigits[3]++;
            timeDigits[4] -= 10;
        }
        timeDigits[2] = 0x41;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 0, 0, 0, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xF8], 20, 0);

        for (i = 0, vtxOffset = 0; i < 5; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + timeDigits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_80814654[fileIndex] + 0xE4], 20, 0);

        for (i = 0, vtxOffset = 0; i < 5; i++, vtxOffset += 4) {
            FileSelect_DrawTexQuadI4(this->state.gfxCtx, font->fontBuf + timeDigits[i] * FONT_CHAR_TEX_SIZE, vtxOffset);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);

    CLOSE_DISPS(this->state.gfxCtx);
}