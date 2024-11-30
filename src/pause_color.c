#include "modding.h"
#include "global.h"

// set to default values for now
Color_RGB8 pauseMainColor = {180, 180, 120};
Color_RGB8 pauseExtraColor = {150, 140, 90};

extern Color_RGB8 buttonAColor;
extern Color_RGB8 buttonCColor;

#define IS_PAUSE_STATE_GAMEOVER \
    ((pauseCtx->state >= PAUSE_STATE_GAMEOVER_0) && (pauseCtx->state <= PAUSE_STATE_GAMEOVER_10))

typedef enum {
    /* 0x00 */ PAUSE_STATE_OFF,
    /* 0x01 */ PAUSE_STATE_OPENING_0,
    /* 0x02 */ PAUSE_STATE_OPENING_1,
    /* 0x03 */ PAUSE_STATE_OPENING_2,
    /* 0x04 */ PAUSE_STATE_OPENING_3,
    /* 0x05 */ PAUSE_STATE_OPENING_4,
    /* 0x06 */ PAUSE_STATE_MAIN, // Pause menu ready for player inputs.
    /* 0x07 */ PAUSE_STATE_SAVEPROMPT,
    /* 0x08 */ PAUSE_STATE_GAMEOVER_0,
    /* 0x09 */ PAUSE_STATE_GAMEOVER_1,
    /* 0x0A */ PAUSE_STATE_GAMEOVER_2,
    /* 0x0B */ PAUSE_STATE_GAMEOVER_3,
    /* 0x0C */ PAUSE_STATE_GAMEOVER_4,
    /* 0x0D */ PAUSE_STATE_GAMEOVER_5,
    /* 0x0E */ PAUSE_STATE_GAMEOVER_SAVE_PROMPT,
    /* 0x0F */ PAUSE_STATE_GAMEOVER_7,
    /* 0x10 */ PAUSE_STATE_GAMEOVER_8,
    /* 0x11 */ PAUSE_STATE_GAMEOVER_CONTINUE_PROMPT,
    /* 0x12 */ PAUSE_STATE_GAMEOVER_10,
    /* 0x13 */ PAUSE_STATE_OWLWARP_0,
    /* 0x14 */ PAUSE_STATE_OWLWARP_1,
    /* 0x15 */ PAUSE_STATE_OWLWARP_2,
    /* 0x16 */ PAUSE_STATE_OWLWARP_3,
    /* 0x17 */ PAUSE_STATE_OWLWARP_SELECT, // Selecting the destination
    /* 0x18 */ PAUSE_STATE_OWLWARP_CONFIRM, // Confirming the choice given
    /* 0x19 */ PAUSE_STATE_OWLWARP_6,
    /* 0x1A */ PAUSE_STATE_UNPAUSE_SETUP, // Unpause
    /* 0x1B */ PAUSE_STATE_UNPAUSE_CLOSE
} PauseState;

s16 sGameOverPrimR = 0;
s16 sGameOverPrimG = 0;
s16 sGameOverPrimB = 0;
s16 sGameOverPrimAlpha = 255;
// s16 sCursorPrimR = 0;
// s16 sCursorPrimG = 0;
// s16 sCursorPrimB = 0;
// s16 sCursorEnvR = 0;
// s16 sCursorEnvG = 0;
// s16 sCursorEnvB = 0;
extern s16 sCursorPrimR;
extern s16 sCursorPrimG;
extern s16 sCursorPrimB;
extern s16 sCursorEnvR;
extern s16 sCursorEnvG;
extern s16 sCursorEnvB;
s16 sGameOverEnvR = 255;
s16 sGameOverEnvG = 0;
s16 sGameOverEnvB = 0;
// TODO: add cursor colors
s16 sCursorPrimColorTarget[][3] = {
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 0 }, { 255, 255, 0 }, { 100, 150, 255 }, { 100, 255, 255 },
};
s16 sCursorEnvColorTarget[][3] = {
    { 0, 0, 0 }, { 170, 170, 170 }, { 0, 0, 0 }, { 255, 160, 0 }, { 0, 0, 100 }, { 0, 150, 255 },
};

typedef enum {
    /* 0 */ PAUSE_ITEM,
    /* 1 */ PAUSE_MAP,
    /* 2 */ PAUSE_QUEST,
    /* 3 */ PAUSE_MASK,
    /* 4 */ PAUSE_WORLD_MAP
} PauseMenuPage;

f32 sPauseMenuVerticalOffset = 0.0f;
f32 D_8082B90C = 0.0f;
f32 sPauseCursorLeftMoveOffsetX = 40.0f;
f32 sPauseCursorRightMoveOffsetX = -40.0f;

extern s16 sPauseCursorLeftX;
extern s16 sPauseCursorRightX;

s16 D_8082B920 = 10;

s16 sPauseZRCursorColorTimerInits[] = { 20, 4, 20, 10 };

extern TexturePtr sItemPageBgTextures[];
extern TexturePtr sMapPageBgTextures[];
extern TexturePtr sQuestPageBgTextures[];
extern TexturePtr sMaskPageBgTextures[];
extern s16 sInDungeonScene;

typedef enum {
    /* 0x00 */ PAUSE_MAIN_STATE_IDLE, // Await input for the next action
    /* 0x01 */ PAUSE_MAIN_STATE_SWITCHING_PAGE,
    /* 0x02 */ PAUSE_MAIN_STATE_SONG_PLAYBACK,
    /* 0x03 */ PAUSE_MAIN_STATE_EQUIP_ITEM,
    /* 0x04 */ PAUSE_MAIN_STATE_SONG_PROMPT_INIT,
    /* 0x05 */ PAUSE_MAIN_STATE_SONG_PROMPT,
    /* 0x06 */ PAUSE_MAIN_STATE_SONG_PROMPT_DONE,
    /* 0x07 */ PAUSE_MAIN_STATE_SONG_PROMPT_UNUSED,
    /* 0x08 */ PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG, // Await input but the cursor is on a song
    /* 0x09 */ PAUSE_MAIN_STATE_SONG_PLAYBACK_INIT,
    /* 0x0F */ PAUSE_MAIN_STATE_EQUIP_MASK = 0xF,
    /* 0x10 */ PAUSE_MAIN_STATE_BOMBERS_NOTEBOOK_OPEN,
    /* 0x11 */ PAUSE_MAIN_STATE_UNK
} PauseMainState;

typedef enum {
    /* 0x00 */ PAUSE_SAVEPROMPT_STATE_APPEARING,
    /* 0x01 */ PAUSE_SAVEPROMPT_STATE_1,
    /* 0x02 */ PAUSE_SAVEPROMPT_STATE_RETURN_TO_MENU,
    /* 0x03 */ PAUSE_SAVEPROMPT_STATE_3,
    /* 0x04 */ PAUSE_SAVEPROMPT_STATE_4,
    /* 0x05 */ PAUSE_SAVEPROMPT_STATE_5,
    /* 0x06 */ PAUSE_SAVEPROMPT_STATE_6,
    /* 0x07 */ PAUSE_SAVEPROMPT_STATE_7
} PauseSavePromptState;

Gfx* KaleidoScope_DrawPageSections(Gfx* gfx, Vtx* vertices, TexturePtr* textures);
void KaleidoScope_DrawItemSelect(PlayState* play);
void KaleidoScope_DrawDungeonMap(PlayState* play);
void KaleidoScope_DrawWorldMap(PlayState* play);
void KaleidoScope_DrawQuestStatus(PlayState* play);
void KaleidoScope_DrawMaskSelect(PlayState* play);

RECOMP_PATCH void KaleidoScope_DrawPages(PlayState* play, GraphicsContext* gfxCtx) {
    static s16 sCursorColorTimer = 10;
    static s16 sCursorColorTargetIndex = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 stepR;
    s16 stepG;
    s16 stepB;

    OPEN_DISPS(gfxCtx);

    if (!IS_PAUSE_STATE_GAMEOVER) {
        if (pauseCtx->state != PAUSE_STATE_SAVEPROMPT) {

            stepR =
                ABS_ALT(sCursorPrimR - sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0]) /
                sCursorColorTimer;
            stepG =
                ABS_ALT(sCursorPrimG - sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1]) /
                sCursorColorTimer;
            stepB =
                ABS_ALT(sCursorPrimB - sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2]) /
                sCursorColorTimer;

            if (sCursorPrimR >= sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0]) {
                sCursorPrimR -= stepR;
            } else {
                sCursorPrimR += stepR;
            }

            if (sCursorPrimG >= sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1]) {
                sCursorPrimG -= stepG;
            } else {
                sCursorPrimG += stepG;
            }

            if (sCursorPrimB >= sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2]) {
                sCursorPrimB -= stepB;
            } else {
                sCursorPrimB += stepB;
            }

            stepR =
                ABS_ALT(sCursorEnvR - sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0]) /
                sCursorColorTimer;
            stepG =
                ABS_ALT(sCursorEnvG - sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1]) /
                sCursorColorTimer;
            stepB =
                ABS_ALT(sCursorEnvB - sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2]) /
                sCursorColorTimer;

            if (sCursorEnvR >= sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0]) {
                sCursorEnvR -= stepR;
            } else {
                sCursorEnvR += stepR;
            }

            if (sCursorEnvG >= sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1]) {
                sCursorEnvG -= stepG;
            } else {
                sCursorEnvG += stepG;
            }

            if (sCursorEnvB >= sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2]) {
                sCursorEnvB -= stepB;
            } else {
                sCursorEnvB += stepB;
            }

            sCursorColorTimer--;
            if (sCursorColorTimer == 0) {
                sCursorPrimR = sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0];
                sCursorPrimG = sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1];
                sCursorPrimB = sCursorPrimColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2];
                sCursorEnvR = sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][0];
                sCursorEnvG = sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][1];
                sCursorEnvB = sCursorEnvColorTarget[pauseCtx->cursorColorSet + sCursorColorTargetIndex][2];
                sCursorColorTargetIndex ^= 1;
                sCursorColorTimer = 10;
            }
        }

        if ((pauseCtx->pageIndex != PAUSE_ITEM) && (pauseCtx->pageIndex != PAUSE_QUEST)) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);

            // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255);
			gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseMainColor.r, pauseMainColor.g, pauseMainColor.b, 255);


            Matrix_RotateYF(0.0f, MTXMODE_NEW);
            Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-pauseCtx->itemPageRoll / 100.0f);

            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->itemPageVtx, sItemPageBgTextures);

            KaleidoScope_DrawItemSelect(play);
        }

        if ((pauseCtx->pageIndex != PAUSE_MAP) && (pauseCtx->pageIndex != PAUSE_MASK)) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);

            // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255);
			gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseMainColor.r, pauseMainColor.g, pauseMainColor.b, 255);


            Matrix_RotateYF(-1.57f, MTXMODE_NEW);
            Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-pauseCtx->mapPageRoll / 100.0f);

            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->mapPageVtx, sMapPageBgTextures);

            if (sInDungeonScene) {
                KaleidoScope_DrawDungeonMap(play);
                Gfx_SetupDL42_Opa(gfxCtx);
                gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                func_801091F0(play);
            } else {
                KaleidoScope_DrawWorldMap(play);
            }
        }

        if ((pauseCtx->pageIndex != PAUSE_QUEST) && (pauseCtx->pageIndex != PAUSE_ITEM)) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);

            // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255);
			gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseMainColor.r, pauseMainColor.g, pauseMainColor.b, 255);


            Matrix_RotateYF(-3.14f, MTXMODE_NEW);
            Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-pauseCtx->questPageRoll / 100.0f);

            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->questPageVtx, sQuestPageBgTextures);

            KaleidoScope_DrawQuestStatus(play);
        }

        if ((pauseCtx->pageIndex != PAUSE_MASK) && (pauseCtx->pageIndex != PAUSE_MAP)) {
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE, 0,
                              TEXEL0, 0, SHADE, 0);

            // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255);
			gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseMainColor.r, pauseMainColor.g, pauseMainColor.b, 255);


            Matrix_RotateYF(1.57f, MTXMODE_NEW);
            Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
            Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
            Matrix_RotateXFApply(-pauseCtx->maskPageRoll / 100.0f);

            gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->maskPageVtx, sMaskPageBgTextures);

            KaleidoScope_DrawMaskSelect(play);
        }

        switch (pauseCtx->pageIndex) {
            case PAUSE_ITEM:
                if (pauseCtx->mainState <= PAUSE_MAIN_STATE_EQUIP_MASK) {
                    gDPPipeSync(POLY_OPA_DISP++);

                    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0,
                                      PRIMITIVE, 0, TEXEL0, 0, SHADE, 0);

                    // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255);
					gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseMainColor.r, pauseMainColor.g, pauseMainColor.b, 255);


                    Matrix_RotateYF(0.0f, MTXMODE_NEW);
                    Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                    Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                    Matrix_RotateXFApply(-pauseCtx->itemPageRoll / 100.0f);

                    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                    POLY_OPA_DISP =
                        KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->itemPageVtx, sItemPageBgTextures);

                    KaleidoScope_DrawItemSelect(play);
                }
                break;

            case PAUSE_MAP:
                gDPPipeSync(POLY_OPA_DISP++);

                gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE,
                                  0, TEXEL0, 0, SHADE, 0);

                // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255);
				gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseMainColor.r, pauseMainColor.g, pauseMainColor.b, 255);


                Matrix_RotateYF(-1.57f, MTXMODE_NEW);
                Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateXFApply(-pauseCtx->mapPageRoll / 100.0f);

                gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                POLY_OPA_DISP = KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->mapPageVtx, sMapPageBgTextures);

                if (sInDungeonScene) {
                    KaleidoScope_DrawDungeonMap(play);
                    Gfx_SetupDL42_Opa(gfxCtx);

                    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

                    func_801091F0(play);
                } else {
                    Matrix_RotateYF(R_PAUSE_WORLD_MAP_YAW / 1000.0f, MTXMODE_NEW);

                    if ((pauseCtx->state == PAUSE_STATE_OPENING_3) || (pauseCtx->state == PAUSE_STATE_OWLWARP_3) ||
                        (pauseCtx->state >= PAUSE_STATE_OWLWARP_6) ||
                        ((pauseCtx->state == PAUSE_STATE_SAVEPROMPT) &&
                         ((pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_3) ||
                          (pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_7)))) {
                        Matrix_Translate(0.0f, (R_PAUSE_WORLD_MAP_Y_OFFSET - 8000) / 100.0f,
                                         R_PAUSE_WORLD_MAP_DEPTH / 100.0f, MTXMODE_APPLY);
                    } else {
                        Matrix_Translate(0.0f, R_PAUSE_WORLD_MAP_Y_OFFSET / 100.0f, R_PAUSE_WORLD_MAP_DEPTH / 100.0f,
                                         MTXMODE_APPLY);
                    }

                    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
                    Matrix_RotateXFApply(-pauseCtx->mapPageRoll / 100.0f);

                    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                    KaleidoScope_DrawWorldMap(play);
                }
                break;

            case PAUSE_QUEST:
                gDPPipeSync(POLY_OPA_DISP++);

                gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE,
                                  0, TEXEL0, 0, SHADE, 0);

                // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255);
				gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseMainColor.r, pauseMainColor.g, pauseMainColor.b, 255);


                gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

                Matrix_RotateYF(-3.14f, MTXMODE_NEW);
                Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateXFApply(-pauseCtx->questPageRoll / 100.0f);

                gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                POLY_OPA_DISP =
                    KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->questPageVtx, sQuestPageBgTextures);

                KaleidoScope_DrawQuestStatus(play);
                break;

            case PAUSE_MASK:
                gDPPipeSync(POLY_OPA_DISP++);

                gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, SHADE, 0, TEXEL0, 0, PRIMITIVE,
                                  0, TEXEL0, 0, SHADE, 0);

                // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 180, 180, 120, 255);
				gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseMainColor.r, pauseMainColor.g, pauseMainColor.b, 255);


                Matrix_RotateYF(1.57f, MTXMODE_NEW);
                Matrix_Translate(0.0f, sPauseMenuVerticalOffset / 100.0f, -93.0f, MTXMODE_APPLY);
                Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
                Matrix_RotateXFApply(-pauseCtx->maskPageRoll / 100.0f);

                gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                POLY_OPA_DISP =
                    KaleidoScope_DrawPageSections(POLY_OPA_DISP, pauseCtx->maskPageVtx, sMaskPageBgTextures);

                KaleidoScope_DrawMaskSelect(play);
                break;
        }
    }

    CLOSE_DISPS(gfxCtx);
}

extern TexturePtr D_8082B998[];
extern TexturePtr D_8082B9A8[];

#define PAUSE_CURSOR_PAGE_LEFT 10
#define PAUSE_CURSOR_PAGE_RIGHT 11

extern Gfx gItemNamePanelDL[];
extern Gfx gZButtonIconDL[];
extern Gfx gRButtonIconDL[];
extern Gfx gAButtonIconDL[];
extern Gfx gCButtonIconsDL[];

extern u64 gPauseToDecideENGTex[];
extern u64 gPauseToEquipENGTex[];
extern u64 gPauseToViewNotebookENGTex[];
extern u64 gPauseToPlayMelodyENGTex[];

#define PAUSE_NAME_COLOR_SET_WHITE 0
#define PAUSE_NAME_COLOR_SET_GREY 1
#define PAUSE_ITEM_NONE 999

RECOMP_PATCH void KaleidoScope_DrawInfoPanel(PlayState* play) {
    static s16 sPauseZRCursorColorTargets[][4] = {
        { 180, 210, 255, 220 },
        { 100, 100, 150, 220 },
    };
    static s16 sPauseZRCursorColorTimer = 20;
    static s16 sPauseZRCursorColorIndex = 0;
    static s16 sPauseZRCursorRed;
    static s16 sPauseZRCursorGreen;
    static s16 sPauseZRCursorBlue;
    static s16 sPauseZRCursorAlpha;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    s16 stepA;
    s16 y;
    s16 i;
    s16 j;

    OPEN_DISPS(play->state.gfxCtx);

    stepR =
        ABS_ALT(sPauseZRCursorRed - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0]) / sPauseZRCursorColorTimer;
    stepG = ABS_ALT(sPauseZRCursorGreen - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1]) /
            sPauseZRCursorColorTimer;
    stepB = ABS_ALT(sPauseZRCursorBlue - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2]) /
            sPauseZRCursorColorTimer;
    stepA = ABS_ALT(sPauseZRCursorAlpha - sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3]) /
            sPauseZRCursorColorTimer;

    if (sPauseZRCursorRed >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0]) {
        sPauseZRCursorRed -= stepR;
    } else {
        sPauseZRCursorRed += stepR;
    }

    if (sPauseZRCursorGreen >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1]) {
        sPauseZRCursorGreen -= stepG;
    } else {
        sPauseZRCursorGreen += stepG;
    }

    if (sPauseZRCursorBlue >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2]) {
        sPauseZRCursorBlue -= stepB;
    } else {
        sPauseZRCursorBlue += stepB;
    }

    if (sPauseZRCursorAlpha >= sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3]) {
        sPauseZRCursorAlpha -= stepA;
    } else {
        sPauseZRCursorAlpha += stepA;
    }

    sPauseZRCursorColorTimer--;
    if (sPauseZRCursorColorTimer == 0) {
        sPauseZRCursorRed = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][0];
        sPauseZRCursorGreen = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][1];
        sPauseZRCursorBlue = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][2];
        sPauseZRCursorAlpha = sPauseZRCursorColorTargets[sPauseZRCursorColorIndex][3];
        sPauseZRCursorColorTimer = sPauseZRCursorColorTimerInits[0];
        sPauseZRCursorColorIndex ^= 1;
    }

    y = pauseCtx->infoPanelOffsetY - 76;
    for (j = 0, i = 0; i < 7; i++, j += 4) {
        pauseCtx->infoPanelVtx[j + 0].v.ob[0] = pauseCtx->infoPanelVtx[j + 2].v.ob[0] = -72;

        pauseCtx->infoPanelVtx[j + 1].v.ob[0] = pauseCtx->infoPanelVtx[j + 3].v.ob[0] = 0;

        pauseCtx->infoPanelVtx[j + 0].v.ob[1] = pauseCtx->infoPanelVtx[j + 1].v.ob[1] = y;

        pauseCtx->infoPanelVtx[j + 2].v.ob[1] = pauseCtx->infoPanelVtx[j + 3].v.ob[1] = y - 24;

        pauseCtx->infoPanelVtx[j + 0].v.ob[2] = pauseCtx->infoPanelVtx[j + 1].v.ob[2] =
            pauseCtx->infoPanelVtx[j + 2].v.ob[2] = pauseCtx->infoPanelVtx[j + 3].v.ob[2] = 0;

        pauseCtx->infoPanelVtx[j + 0].v.flag = pauseCtx->infoPanelVtx[j + 1].v.flag =
            pauseCtx->infoPanelVtx[j + 2].v.flag = pauseCtx->infoPanelVtx[j + 3].v.flag = 0;

        pauseCtx->infoPanelVtx[j + 0].v.tc[0] = pauseCtx->infoPanelVtx[j + 0].v.tc[1] =
            pauseCtx->infoPanelVtx[j + 1].v.tc[1] = pauseCtx->infoPanelVtx[j + 2].v.tc[0] = 0;

        pauseCtx->infoPanelVtx[j + 1].v.tc[0] = pauseCtx->infoPanelVtx[j + 3].v.tc[0] = 72 * (1 << 5);

        pauseCtx->infoPanelVtx[j + 2].v.tc[1] = pauseCtx->infoPanelVtx[j + 3].v.tc[1] = 24 * (1 << 5);

        pauseCtx->infoPanelVtx[j + 0].v.cn[0] = pauseCtx->infoPanelVtx[j + 2].v.cn[0] =
            pauseCtx->infoPanelVtx[j + 0].v.cn[1] = pauseCtx->infoPanelVtx[j + 2].v.cn[1] =
                pauseCtx->infoPanelVtx[j + 0].v.cn[2] = pauseCtx->infoPanelVtx[j + 2].v.cn[2] =
                    pauseCtx->infoPanelVtx[j + 1].v.cn[0] = pauseCtx->infoPanelVtx[j + 3].v.cn[0] =
                        pauseCtx->infoPanelVtx[j + 1].v.cn[1] = pauseCtx->infoPanelVtx[j + 3].v.cn[1] =
                            pauseCtx->infoPanelVtx[j + 1].v.cn[2] = pauseCtx->infoPanelVtx[j + 3].v.cn[2] = 200;

        pauseCtx->infoPanelVtx[j + 0].v.cn[3] = pauseCtx->infoPanelVtx[j + 2].v.cn[3] =
            pauseCtx->infoPanelVtx[j + 1].v.cn[3] = pauseCtx->infoPanelVtx[j + 3].v.cn[3] = pauseCtx->alpha;
    }

    pauseCtx->infoPanelVtx[4].v.ob[0] = pauseCtx->infoPanelVtx[6].v.ob[0] = pauseCtx->infoPanelVtx[0].v.ob[0] + 72;

    pauseCtx->infoPanelVtx[5].v.ob[0] = pauseCtx->infoPanelVtx[7].v.ob[0] = pauseCtx->infoPanelVtx[4].v.ob[0] + 72;

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
        pauseCtx->infoPanelVtx[8].v.ob[0] = pauseCtx->infoPanelVtx[10].v.ob[0] = sPauseCursorLeftX;

        pauseCtx->infoPanelVtx[9].v.ob[0] = pauseCtx->infoPanelVtx[11].v.ob[0] = pauseCtx->infoPanelVtx[8].v.ob[0] + 24;

        pauseCtx->infoPanelVtx[8].v.ob[1] = pauseCtx->infoPanelVtx[9].v.ob[1] = D_8082B920;

        pauseCtx->infoPanelVtx[10].v.ob[1] = pauseCtx->infoPanelVtx[11].v.ob[1] =
            pauseCtx->infoPanelVtx[8].v.ob[1] - 32;
    } else {
        pauseCtx->infoPanelVtx[8].v.ob[0] = pauseCtx->infoPanelVtx[10].v.ob[0] = sPauseCursorLeftX + 3;

        pauseCtx->infoPanelVtx[9].v.ob[0] = pauseCtx->infoPanelVtx[11].v.ob[0] = pauseCtx->infoPanelVtx[8].v.ob[0] + 18;

        pauseCtx->infoPanelVtx[8].v.ob[1] = pauseCtx->infoPanelVtx[9].v.ob[1] = D_8082B920 - 3;

        pauseCtx->infoPanelVtx[10].v.ob[1] = pauseCtx->infoPanelVtx[11].v.ob[1] =
            pauseCtx->infoPanelVtx[8].v.ob[1] - 26;
    }

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
        pauseCtx->infoPanelVtx[12].v.ob[0] = pauseCtx->infoPanelVtx[14].v.ob[0] = sPauseCursorRightX;

        pauseCtx->infoPanelVtx[13].v.ob[0] = pauseCtx->infoPanelVtx[15].v.ob[0] =
            pauseCtx->infoPanelVtx[12].v.ob[0] + 24;

        pauseCtx->infoPanelVtx[12].v.ob[1] = pauseCtx->infoPanelVtx[13].v.ob[1] = D_8082B920;

        pauseCtx->infoPanelVtx[14].v.ob[1] = pauseCtx->infoPanelVtx[15].v.ob[1] =
            pauseCtx->infoPanelVtx[12].v.ob[1] - 32;
    } else {
        pauseCtx->infoPanelVtx[12].v.ob[0] = pauseCtx->infoPanelVtx[14].v.ob[0] = sPauseCursorRightX + 3;

        pauseCtx->infoPanelVtx[13].v.ob[0] = pauseCtx->infoPanelVtx[15].v.ob[0] =
            pauseCtx->infoPanelVtx[12].v.ob[0] + 18;

        pauseCtx->infoPanelVtx[12].v.ob[1] = pauseCtx->infoPanelVtx[13].v.ob[1] = D_8082B920 - 3;

        pauseCtx->infoPanelVtx[14].v.ob[1] = pauseCtx->infoPanelVtx[15].v.ob[1] =
            pauseCtx->infoPanelVtx[12].v.ob[1] - 26;
    }

    pauseCtx->infoPanelVtx[9].v.tc[0] = pauseCtx->infoPanelVtx[11].v.tc[0] = pauseCtx->infoPanelVtx[13].v.tc[0] =
        pauseCtx->infoPanelVtx[15].v.tc[0] = 24 * (1 << 5);

    pauseCtx->infoPanelVtx[10].v.tc[1] = pauseCtx->infoPanelVtx[11].v.tc[1] = pauseCtx->infoPanelVtx[14].v.tc[1] =
        pauseCtx->infoPanelVtx[15].v.tc[1] = 32 * (1 << 5);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    Matrix_Translate(0.0f, 0.0f, -144.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 140, 90, 255);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseExtraColor.r, pauseExtraColor.g, pauseExtraColor.b, 255);
    gSPVertex(POLY_OPA_DISP++, &pauseCtx->infoPanelVtx[0], 16, 0);

    gSPDisplayList(POLY_OPA_DISP++, gItemNamePanelDL);

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) &&
        (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK))) {
        // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 140, 90, sPauseZRCursorAlpha);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseExtraColor.r, pauseExtraColor.g, pauseExtraColor.b, sPauseZRCursorAlpha);
    }

    gSPDisplayList(POLY_OPA_DISP++, gZButtonIconDL);

    if ((pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) &&
        (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK))) {
        // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 140, 90, sPauseZRCursorAlpha);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, pauseExtraColor.r, pauseExtraColor.g, pauseExtraColor.b, sPauseZRCursorAlpha);
    }

    gSPDisplayList(POLY_OPA_DISP++, gRButtonIconDL);

    if (pauseCtx->cursorSpecialPos != 0) {
        j = (pauseCtx->cursorSpecialPos * 4) - 32;
        pauseCtx->cursorVtx[0].v.ob[0] = pauseCtx->infoPanelVtx[j].v.ob[0];
        pauseCtx->cursorVtx[0].v.ob[1] = pauseCtx->infoPanelVtx[j].v.ob[1];
    }

    y = pauseCtx->infoPanelOffsetY - 80;
    pauseCtx->infoPanelVtx[16].v.ob[1] = pauseCtx->infoPanelVtx[17].v.ob[1] = y;

    pauseCtx->infoPanelVtx[18].v.ob[1] = pauseCtx->infoPanelVtx[19].v.ob[1] = pauseCtx->infoPanelVtx[16].v.ob[1] - 16;

    pauseCtx->infoPanelVtx[18].v.tc[1] = pauseCtx->infoPanelVtx[19].v.tc[1] = 16 * (1 << 5);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetEnvColor(POLY_OPA_DISP++, 20, 30, 40, 0);

    if (pauseCtx->itemDescriptionOn ||
        ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->namedItem != PAUSE_ITEM_NONE) &&
         (pauseCtx->nameDisplayTimer < 40) &&
         (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK) ||
          (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK) ||
          ((pauseCtx->mainState >= PAUSE_MAIN_STATE_SONG_PROMPT_INIT) &&
           (pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PROMPT_UNUSED)) ||
          (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) &&
         (pauseCtx->cursorSpecialPos == 0))) {
        if (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK) ||
            (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK) ||
            ((pauseCtx->mainState >= PAUSE_MAIN_STATE_SONG_PROMPT_INIT) &&
             (pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PROMPT_UNUSED)) ||
            (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) {

            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -63;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 128;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 128 * (1 << 5);

            gSPVertex(POLY_OPA_DISP++, &pauseCtx->infoPanelVtx[16], 4, 0);

            if (pauseCtx->nameColorSet == PAUSE_NAME_COLOR_SET_GREY) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 70, 70, 70, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
            }

            POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, pauseCtx->nameSegment, G_IM_FMT_IA, 128, 16, 0);
        }
    } else if ((pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PLAYBACK) ||
               (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT_UNUSED) ||
               (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) ||
               (pauseCtx->mainState == PAUSE_MAIN_STATE_UNK)) {
        pauseCtx->infoPanelVtx[20].v.ob[1] = pauseCtx->infoPanelVtx[21].v.ob[1] = y;

        pauseCtx->infoPanelVtx[22].v.ob[1] = pauseCtx->infoPanelVtx[23].v.ob[1] =
            pauseCtx->infoPanelVtx[20].v.ob[1] - 16;

        pauseCtx->infoPanelVtx[22].v.tc[1] = pauseCtx->infoPanelVtx[23].v.tc[1] = 16 * (1 << 5);

        gSPVertex(POLY_OPA_DISP++, &pauseCtx->infoPanelVtx[16], 8, 0);

        if (pauseCtx->state == PAUSE_STATE_SAVEPROMPT) {
            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -33;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

            pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 0x10;

            pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                pauseCtx->infoPanelVtx[20].v.ob[0] + 0x30;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 24 * (1 << 5);

            pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = 48 * (1 << 5);

            gSPDisplayList(POLY_OPA_DISP++, gAButtonIconDL);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

            //! @bug: Incorrect dimensions. Should be 64x16
            POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, gPauseToDecideENGTex, G_IM_FMT_IA, 48, 16, 4);

        } else if (pauseCtx->cursorSpecialPos != 0) {
            if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {

                pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -63;

                pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 128;

                pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 128 * (1 << 5);

                gDPPipeSync(POLY_OPA_DISP++);
				// may change this later
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 200, 0, 255);

                if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
                    POLY_OPA_DISP =
                        Gfx_DrawTexQuad4b(POLY_OPA_DISP, D_8082B998[pauseCtx->pageIndex], G_IM_FMT_IA, 128, 16, 0);
                } else {
                    POLY_OPA_DISP =
                        Gfx_DrawTexQuad4b(POLY_OPA_DISP, D_8082B9A8[pauseCtx->pageIndex], G_IM_FMT_IA, 128, 16, 0);
                }
            }
        } else if ((!pauseCtx->pageIndex || (pauseCtx->pageIndex == PAUSE_MASK)) &&
                   (pauseCtx->namedItem != PAUSE_ITEM_NONE)) {
            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -49;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 48;

            pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 47;

            pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                pauseCtx->infoPanelVtx[20].v.ob[0] + 64;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 48 * (1 << 5);

            pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = 64 * (1 << 5);

            gSPDisplayList(POLY_OPA_DISP++, gCButtonIconsDL);

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

            POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, gPauseToEquipENGTex, G_IM_FMT_IA, 64, 16, 4);
        } else if ((pauseCtx->pageIndex == PAUSE_MAP) && sInDungeonScene) {
            // No code in this case
        } else if ((pauseCtx->pageIndex == PAUSE_QUEST) &&
                   (pauseCtx->cursorSlot[PAUSE_QUEST] == QUEST_BOMBERS_NOTEBOOK)) {
            if (pauseCtx->namedItem != PAUSE_ITEM_NONE) {
                // The cursor is on the bombers notebook
                pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -58;

                pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

                pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                    pauseCtx->infoPanelVtx[16].v.ob[0] + 0x14;

                pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                    pauseCtx->infoPanelVtx[20].v.ob[0] + 0x60;

                pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 24 * (1 << 5);

                pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = 96 * (1 << 5);

                gSPDisplayList(POLY_OPA_DISP++, gAButtonIconDL);

                gDPPipeSync(POLY_OPA_DISP++);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

                POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, gPauseToViewNotebookENGTex, G_IM_FMT_IA, 96, 16, 4);
            }
        } else if ((pauseCtx->pageIndex == PAUSE_QUEST) && (pauseCtx->cursorSlot[PAUSE_QUEST] >= QUEST_SONG_SONATA) &&
                   (pauseCtx->cursorSlot[PAUSE_QUEST] <= QUEST_SONG_SUN) && (pauseCtx->namedItem != PAUSE_ITEM_NONE)) {
            // The cursor is on a learned song
            pauseCtx->infoPanelVtx[16].v.ob[0] = pauseCtx->infoPanelVtx[18].v.ob[0] = -55;

            pauseCtx->infoPanelVtx[17].v.ob[0] = pauseCtx->infoPanelVtx[19].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 24;

            pauseCtx->infoPanelVtx[20].v.ob[0] = pauseCtx->infoPanelVtx[22].v.ob[0] =
                pauseCtx->infoPanelVtx[16].v.ob[0] + 20;

            pauseCtx->infoPanelVtx[21].v.ob[0] = pauseCtx->infoPanelVtx[23].v.ob[0] =
                pauseCtx->infoPanelVtx[20].v.ob[0] + 96;

            pauseCtx->infoPanelVtx[17].v.tc[0] = pauseCtx->infoPanelVtx[19].v.tc[0] = 24 * (1 << 5);

            pauseCtx->infoPanelVtx[21].v.tc[0] = pauseCtx->infoPanelVtx[23].v.tc[0] = 96 * (1 << 5);

            gSPDisplayList(POLY_OPA_DISP++, gAButtonIconDL);

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

            POLY_OPA_DISP = Gfx_DrawTexQuad4b(POLY_OPA_DISP, gPauseToPlayMelodyENGTex, G_IM_FMT_IA, 96, 16, 4);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// pause menu ocarina buttons
extern Color_RGB8 buttonCColors[];
extern Color_RGB8 buttonAColors[];

extern u64 gOcarinaATex[];
extern u64 gOcarinaCDownTex[];
extern u64 gOcarinaCRightTex[];
extern u64 gOcarinaCLeftTex[];
extern u64 gOcarinaCUpTex[];
extern u64 gItemIconQuiver30Tex[];
extern u64 gItemIconQuiver40Tex[];
extern u64 gItemIconQuiver50Tex[];
extern u64 gItemIconBombBag20Tex[];
extern u64 gItemIconBombBag30Tex[];
extern u64 gItemIconBombBag40Tex[];

extern s16 sQuestRemainsEnvRed[];
extern s16 sQuestRemainsEnvGreen[];
extern s16 sQuestRemainsEnvBlue[];
extern s16 sQuestRemainsEnvTargets[][3];
extern s16 sQuestRemainsColorTimerInit[];

extern u64 gItemIconSongNoteTex[];
extern u64 gItemIconBombersNotebookTex[];
extern s16 sQuestHpPrimColorTargets[][4];
extern s16 sQuestHpColorTimerInits[];

extern s16 sQuestSongPlayedOcarinaButtonsNum;
extern u8 sQuestSongPlayedOcarinaButtons[];
extern s16 sQuestSongPlayedOcarinaButtonsAlpha[];

#define IS_PAUSE_MAIN_STATE_SONG_PROMPT                            \
    ((pauseCtx->mainState >= PAUSE_MAIN_STATE_SONG_PROMPT_INIT) && \
     (pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PROMPT_DONE))

extern u64 gCounterDigit0Tex[];

void KaleidoScope_SetCursorVtxPos(PauseContext* pauseCtx, u16 vtxIndex, Vtx* vtx);
void KaleidoScope_DrawTexQuadRGBA32(GraphicsContext* gfxCtx, TexturePtr texture, u16 width, u16 height, u16 point);

RECOMP_PATCH void KaleidoScope_DrawQuestStatus(PlayState* play) {
    static s16 sQuestRemainsColorTimer = 20;
    static s16 sQuestRemainsColorTimerIndex = 0;
    static s16 sQuestHpPrimRed = 0;
    static s16 sQuestHpPrimGreen = 0;
    static s16 sQuestHpPrimBlue = 0;
    static s16 sQuestHpPrimAlpha = 0;
    static s16 sQuestHpColorTimer = 20;
    static s16 sQuestHpPrimColorTargetIndex = 0;
    static TexturePtr sOcarinaButtonTextures[] = {
        gOcarinaATex, gOcarinaCDownTex, gOcarinaCRightTex, gOcarinaCLeftTex, gOcarinaCUpTex,
    };
    static s16 sQuestSongsPrimRed[] = {
        150, // QUEST_SONG_SONATA
        255, // QUEST_SONG_LULLABY
        100, // QUEST_SONG_BOSSA_NOVA
        255, // QUEST_SONG_ELEGY
        255, // QUEST_SONG_OATH
        255, // QUEST_SONG_SARIA
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_HEALING
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SOARING
        255, // QUEST_SONG_STORMS
        255, // QUEST_SONG_SUN
    };
    static s16 sQuestSongsPrimGreen[] = {
        255, // QUEST_SONG_SONATA
        80,  // QUEST_SONG_LULLABY
        150, // QUEST_SONG_BOSSA_NOVA
        160, // QUEST_SONG_ELEGY
        100, // QUEST_SONG_OATH
        240, // QUEST_SONG_SARIA
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_HEALING
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SOARING
        255, // QUEST_SONG_STORMS
        255, // QUEST_SONG_SUN
    };
    static s16 sQuestSongsPrimBlue[] = {
        100, // QUEST_SONG_SONATA
        40,  // QUEST_SONG_LULLABY
        255, // QUEST_SONG_BOSSA_NOVA
        0,   // QUEST_SONG_ELEGY
        255, // QUEST_SONG_OATH
        100, // QUEST_SONG_SARIA
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_HEALING
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SOARING
        255, // QUEST_SONG_STORMS
        255, // QUEST_SONG_SUN
    };
    static TexturePtr sQuestUpgradeTextures[][3] = {
        { gItemIconQuiver30Tex, gItemIconQuiver40Tex, gItemIconQuiver50Tex },    // UPG_QUIVER
        { gItemIconBombBag20Tex, gItemIconBombBag30Tex, gItemIconBombBag40Tex }, // UPG_BOMB_BAG
    };
    static u8 sQuestUpgrades[] = { UPG_QUIVER, UPG_BOMB_BAG };
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 sp1CA; // colorSetR and numOcarinaButtons
    s16 sp1C8; // colorSetG and ocarinaButtonIndex
    s16 sp1C6;
    s16 sp1C4;
    s16 var_v1;
    s16 i;
    s16 j;
    s16 k;
    s16 skullTokenDigits[3];
    u16 isDigitDrawn;
    u32* questItemsPtr;

    OPEN_DISPS(play->state.gfxCtx);

    KaleidoScope_SetCursorVtxPos(pauseCtx, pauseCtx->cursorSlot[PAUSE_QUEST] * 4, pauseCtx->questVtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    // Draw the Boss Remains icons
    sQuestRemainsColorTimer--;
    for (i = 0, j = 0; i < 4; i++, j += 4) {
        if ((sQuestRemainsColorTimerIndex != 1) && (sQuestRemainsColorTimerIndex != 3)) {
            var_v1 = (sQuestRemainsColorTimerIndex != 0) ? (i + 6) : i;

            if (sQuestRemainsColorTimer != 0) {
                sp1CA = ABS_ALT(sQuestRemainsEnvRed[i] - sQuestRemainsEnvTargets[var_v1][0]) / sQuestRemainsColorTimer;
                sp1C8 =
                    ABS_ALT(sQuestRemainsEnvGreen[i] - sQuestRemainsEnvTargets[var_v1][1]) / sQuestRemainsColorTimer;
                sp1C6 = ABS_ALT(sQuestRemainsEnvBlue[i] - sQuestRemainsEnvTargets[var_v1][2]) / sQuestRemainsColorTimer;

                if (sQuestRemainsEnvRed[i] >= sQuestRemainsEnvTargets[var_v1][0]) {
                    sQuestRemainsEnvRed[i] -= sp1CA;
                } else {
                    sQuestRemainsEnvRed[i] += sp1CA;
                }
                if (sQuestRemainsEnvGreen[i] >= sQuestRemainsEnvTargets[var_v1][1]) {
                    sQuestRemainsEnvGreen[i] -= sp1C8;
                } else {
                    sQuestRemainsEnvGreen[i] += sp1C8;
                }
                if (sQuestRemainsEnvBlue[i] >= sQuestRemainsEnvTargets[var_v1][2]) {
                    sQuestRemainsEnvBlue[i] -= sp1C6;
                } else {
                    sQuestRemainsEnvBlue[i] += sp1C6;
                }
            } else {
                sQuestRemainsEnvRed[i] = sQuestRemainsEnvTargets[var_v1][0];
                sQuestRemainsEnvGreen[i] = sQuestRemainsEnvTargets[var_v1][1];
                sQuestRemainsEnvBlue[i] = sQuestRemainsEnvTargets[var_v1][2];
            }
        }

        if (CHECK_QUEST_ITEM(i)) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
            gDPSetEnvColor(POLY_OPA_DISP++, sQuestRemainsEnvRed[i], sQuestRemainsEnvGreen[i], sQuestRemainsEnvBlue[i],
                           0);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
            KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[ITEM_REMAINS_ODOLWA + i], 32, 32, 0);
        }
    }

    if (sQuestRemainsColorTimer == 0) {
        sQuestRemainsColorTimer = sQuestRemainsColorTimerInit[sQuestRemainsColorTimerIndex];
        if (++sQuestRemainsColorTimerIndex > 3) {
            sQuestRemainsColorTimerIndex = 0;
        }
    }

    // Draw shield
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) != EQUIP_VALUE_SHIELD_NONE) {
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
        KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[(ITEM_SHIELD_HERO - 1) + GET_CUR_EQUIP_VALUE(1)],
                                       32, 32, 0);
    }

    j += 4;

    // Draw Sword
    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) != EQUIP_VALUE_SWORD_NONE) {
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
        KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[(ITEM_SWORD_KOKIRI - 1) + GET_CUR_EQUIP_VALUE(0)],
                                       32, 32, 0);
    }

    j += 4;

    // Draw Songs
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gItemIconSongNoteTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    // Removes Sun's Song from displaying
    for (i = 0; i < 12; i++, j += 4) {
        // if (CHECK_QUEST_ITEM(i + QUEST_SONG_SONATA) ||
        //     ((i == (QUEST_SONG_LULLABY - QUEST_SONG_SONATA)) && !CHECK_QUEST_ITEM(i + QUEST_SONG_SONATA) &&
        //      CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO))) {
        if ((CHECK_QUEST_ITEM(i + QUEST_SONG_SONATA) ||
            ((i == (QUEST_SONG_LULLABY - QUEST_SONG_SONATA)) && !CHECK_QUEST_ITEM(i + QUEST_SONG_SONATA) &&
             CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO))) && (i + QUEST_SONG_SONATA != QUEST_SONG_SUN)) {
            if ((i + QUEST_SONG_SONATA) == pauseCtx->cursorSlot[PAUSE_QUEST]) {
                pauseCtx->questVtx[j + 0].v.ob[0] = pauseCtx->questVtx[j + 2].v.ob[0] =
                    pauseCtx->questVtx[j + 0].v.ob[0] - 2;

                pauseCtx->questVtx[j + 1].v.ob[0] = pauseCtx->questVtx[j + 3].v.ob[0] =
                    pauseCtx->questVtx[j + 1].v.ob[0] + 4;

                pauseCtx->questVtx[j + 0].v.ob[1] = pauseCtx->questVtx[j + 1].v.ob[1] =
                    pauseCtx->questVtx[j + 0].v.ob[1] + 2;

                pauseCtx->questVtx[j + 2].v.ob[1] = pauseCtx->questVtx[j + 3].v.ob[1] =
                    pauseCtx->questVtx[j + 2].v.ob[1] - 4;
            }

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sQuestSongsPrimRed[i], sQuestSongsPrimGreen[i],
                            sQuestSongsPrimBlue[i], pauseCtx->alpha);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
        }
    }

    // Draw Bombers Notebook
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
        KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIconBombersNotebookTex, 32, 32, 0);
    }

    j += 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    // Loop over quest item upgrades
    for (i = 0; i < 2; i++, j += 4) {
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (GET_CUR_UPG_VALUE(sQuestUpgrades[i]) != 0) {
            KaleidoScope_DrawTexQuadRGBA32(
                play->state.gfxCtx, sQuestUpgradeTextures[i][GET_CUR_UPG_VALUE(sQuestUpgrades[i]) - 1], 32, 32, 0);
        }
    }

    // Skip over &pauseCtx->questVtx[84], which should be `QUEST_SKULL_TOKEN`
    j += 4;

    sp1CA = ABS_ALT(sQuestHpPrimRed - sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][0]) / sQuestHpColorTimer;
    sp1C8 = ABS_ALT(sQuestHpPrimGreen - sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][1]) / sQuestHpColorTimer;
    sp1C6 = ABS_ALT(sQuestHpPrimBlue - sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][2]) / sQuestHpColorTimer;
    sp1C4 = ABS_ALT(sQuestHpPrimAlpha - sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][3]) / sQuestHpColorTimer;

    if (sQuestHpPrimRed >= sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][0]) {
        sQuestHpPrimRed -= sp1CA;
    } else {
        sQuestHpPrimRed += sp1CA;
    }

    if (sQuestHpPrimGreen >= sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][1]) {
        sQuestHpPrimGreen -= sp1C8;
    } else {
        sQuestHpPrimGreen += sp1C8;
    }

    if (sQuestHpPrimBlue >= sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][2]) {
        sQuestHpPrimBlue -= sp1C6;
    } else {
        sQuestHpPrimBlue += sp1C6;
    }

    if (sQuestHpPrimAlpha >= sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][3]) {
        sQuestHpPrimAlpha -= sp1C4;
    } else {
        sQuestHpPrimAlpha += sp1C4;
    }

    sQuestHpColorTimer--;
    if (sQuestHpColorTimer == 0) {
        sQuestHpPrimRed = sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][0];
        sQuestHpPrimGreen = sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][1];
        sQuestHpPrimBlue = sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][2];
        sQuestHpPrimAlpha = sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][3];
        sQuestHpColorTimer = sQuestHpColorTimerInits[sQuestHpPrimColorTargetIndex];
        if (++sQuestHpPrimColorTargetIndex > 3) {
            sQuestHpPrimColorTargetIndex = 0;
        }
    }

    //! FAKE: Used to load `0xF0000000` early
    if ((GET_SAVE_INVENTORY_QUEST_ITEMS & 0xF0000000) != 0) {}
    questItemsPtr = &gSaveContext.save.saveInfo.inventory.questItems;
    if (1) {}

    if ((*questItemsPtr >> QUEST_HEART_PIECE_COUNT) != 0) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        if ((pauseCtx->state == PAUSE_STATE_OPENING_3) || (pauseCtx->state == PAUSE_STATE_UNPAUSE_SETUP)) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sQuestHpPrimColorTargets[0][0], sQuestHpPrimColorTargets[0][1],
                            sQuestHpPrimColorTargets[0][2], pauseCtx->alpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sQuestHpPrimRed, sQuestHpPrimGreen, sQuestHpPrimBlue,
                            sQuestHpPrimAlpha);
        }

        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);

        POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
            POLY_OPA_DISP,
            gItemIcons[(0x7A + ((GET_SAVE_INVENTORY_QUEST_ITEMS & 0xF0000000) >> QUEST_HEART_PIECE_COUNT))], 48, 48, 0);
    }

    j += 4;

    if (pauseCtx->state == PAUSE_STATE_MAIN) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK) {
            // Draw ocarina buttons as they are played back
            pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();

            if (pauseCtx->ocarinaStaff->pos != 0) {
                if (sQuestSongPlayedOcarinaButtonsNum == (pauseCtx->ocarinaStaff->pos - 1)) {
                    sQuestSongPlayedOcarinaButtonsNum++;
                    sQuestSongPlayedOcarinaButtons[pauseCtx->ocarinaStaff->pos - 1] =
                        pauseCtx->ocarinaStaff->buttonIndex;
                }

                for (i = 0, k = 0; i < 8; i++, k += 4, j += 4) {
                    if (sQuestSongPlayedOcarinaButtons[i] == OCARINA_BTN_INVALID) {
                        break;
                    }

                    if (sQuestSongPlayedOcarinaButtonsAlpha[i] != 255) {
                        sQuestSongPlayedOcarinaButtonsAlpha[i] += 50;
                        if (sQuestSongPlayedOcarinaButtonsAlpha[i] >= 255) {
                            sQuestSongPlayedOcarinaButtonsAlpha[i] = 255;
                        }
                    }

                    pauseCtx->questVtx[j + 0].v.ob[1] = pauseCtx->questVtx[j + 1].v.ob[1] =
                        pauseCtx->ocarinaButtonsY[sQuestSongPlayedOcarinaButtons[i]];

                    pauseCtx->questVtx[j + 2].v.ob[1] = pauseCtx->questVtx[j + 3].v.ob[1] =
                        pauseCtx->questVtx[j + 0].v.ob[1] - 12;

                    gDPPipeSync(POLY_OPA_DISP++);

                    if (sQuestSongPlayedOcarinaButtons[i] == OCARINA_BTN_A) {
                        // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 150, 255, sQuestSongPlayedOcarinaButtonsAlpha[i]);
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, buttonAColor.r, buttonAColor.g, buttonAColor.b, sQuestSongPlayedOcarinaButtonsAlpha[i]);
                    } else {
                        // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, sQuestSongPlayedOcarinaButtonsAlpha[i]);
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, buttonCColor.r, buttonCColor.g, buttonCColor.b, sQuestSongPlayedOcarinaButtonsAlpha[i]);
                    }

                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);

                    POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
                        POLY_OPA_DISP, sOcarinaButtonTextures[sQuestSongPlayedOcarinaButtons[i]], 16, 16, 0);
                }
            }
        } else if (IS_PAUSE_MAIN_STATE_SONG_PROMPT || (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) {
            // Draw the buttons for playing a song
            sp1C8 = pauseCtx->ocarinaSongIndex;
            sp1CA = gOcarinaSongButtons[sp1C8].numButtons;

            i = j;

            for (k = 0; k < sp1CA; k++, j += 4) {
                pauseCtx->questVtx[j + 0].v.ob[1] = pauseCtx->questVtx[j + 1].v.ob[1] =
                    pauseCtx->ocarinaButtonsY[gOcarinaSongButtons[sp1C8].buttonIndex[k]];

                pauseCtx->questVtx[j + 2].v.ob[1] = pauseCtx->questVtx[j + 3].v.ob[1] =
                    pauseCtx->questVtx[j + 0].v.ob[1] - 12;

                gDPPipeSync(POLY_OPA_DISP++);

                if (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) {
                    // Draw ocarina buttons colored
                    if (gOcarinaSongButtons[sp1C8].buttonIndex[k] == OCARINA_BTN_A) {
                        // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 150, 255, 200);
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, buttonAColor.r, buttonAColor.g, buttonAColor.b, 200);
                    } else {
                        // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, 200);
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, buttonCColor.r, buttonCColor.g, buttonCColor.b, 200);
                    }
                } else {
                    // Gray out buttons while reading ocarina song inputs
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 150, 150, 150);
                }

                gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);

                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
                    POLY_OPA_DISP, sOcarinaButtonTextures[gOcarinaSongButtons[sp1C8].buttonIndex[k]], 16, 16, 0);
            }

            if (pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) {
                pauseCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();

                // Update ocarina song inputs
                if (pauseCtx->ocarinaStaff->pos != 0) {
                    if (sQuestSongPlayedOcarinaButtonsNum == (pauseCtx->ocarinaStaff->pos - 1)) {
                        if (pauseCtx->ocarinaStaff->buttonIndex <= OCARINA_BTN_C_UP) {
                            sQuestSongPlayedOcarinaButtons[pauseCtx->ocarinaStaff->pos - 1] =
                                pauseCtx->ocarinaStaff->buttonIndex;
                            sQuestSongPlayedOcarinaButtons[pauseCtx->ocarinaStaff->pos] = OCARINA_BTN_INVALID;
                            sQuestSongPlayedOcarinaButtonsNum++;
                        }
                    }
                }

                // Draw the buttons colored as the ocarina song inputs are read from
                j = i + 32;
                k = 0;
                for (; k < 8; k++, j += 4) {
                    if (sQuestSongPlayedOcarinaButtons[k] == OCARINA_BTN_INVALID) {
                        continue;
                    }

                    if (sQuestSongPlayedOcarinaButtonsAlpha[k] != 255) {
                        sQuestSongPlayedOcarinaButtonsAlpha[k] += 50;
                        if (sQuestSongPlayedOcarinaButtonsAlpha[k] >= 255) {
                            sQuestSongPlayedOcarinaButtonsAlpha[k] = 255;
                        }
                    }

                    pauseCtx->questVtx[j + 0].v.ob[1] = pauseCtx->questVtx[j + 1].v.ob[1] =
                        pauseCtx->ocarinaButtonsY[sQuestSongPlayedOcarinaButtons[k]];

                    pauseCtx->questVtx[j + 2].v.ob[1] = pauseCtx->questVtx[j + 3].v.ob[1] =
                        pauseCtx->questVtx[j + 0].v.ob[1] - 12;

                    gDPPipeSync(POLY_OPA_DISP++);

                    if (sQuestSongPlayedOcarinaButtons[k] == OCARINA_BTN_A) {
                        // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 150, 255, sQuestSongPlayedOcarinaButtonsAlpha[k]);
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, buttonAColor.r, buttonAColor.g, buttonAColor.b, sQuestSongPlayedOcarinaButtonsAlpha[k]);
                    } else {
                        // gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, sQuestSongPlayedOcarinaButtonsAlpha[k]);
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, buttonCColor.r, buttonCColor.g, buttonCColor.b, sQuestSongPlayedOcarinaButtonsAlpha[k]);
                    }

                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);

                    POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
                        POLY_OPA_DISP, sOcarinaButtonTextures[sQuestSongPlayedOcarinaButtons[k]], 16, 16, 0);
                }

                if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT_INIT) {
                    for (k = 0; k < 8; k++) {
                        sQuestSongPlayedOcarinaButtons[k] = OCARINA_BTN_INVALID;
                        sQuestSongPlayedOcarinaButtonsAlpha[k] = 0;
                    }

                    sQuestSongPlayedOcarinaButtonsNum = 0;
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                    AudioOcarina_StartDefault((1 << pauseCtx->ocarinaSongIndex) | 0x80000000);
                    pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
                    pauseCtx->ocarinaStaff->pos = 0;
                    pauseCtx->ocarinaStaff->state = 0xFE;
                    pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PROMPT;
                }
            }
        }
    }

    // Draw Skull Token Count
    // QUEST_SKULL_TOKEN never properly set, see Item_Give(),
    // Vertices not well placed, digits are not aligned and placed in unintended positions
    if (CHECK_QUEST_ITEM(QUEST_SKULL_TOKEN) && ((play->sceneId == SCENE_KINSTA1) || (play->sceneId == SCENE_KINDAN2))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        skullTokenDigits[0] = skullTokenDigits[1] = 0;
        skullTokenDigits[2] = Inventory_GetSkullTokenCount(play->sceneId);

        while (skullTokenDigits[2] >= 100) {
            skullTokenDigits[0]++;
            skullTokenDigits[2] -= 100;
        }

        while (skullTokenDigits[2] >= 10) {
            skullTokenDigits[1]++;
            skullTokenDigits[2] -= 10;
        }

        //! @bug: &pauseCtx->questVtx[84] is the questVtx for skull tokens
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[152], 24, 0);

        // Loop over two sets of digits, the first is shadowed, the second is colored
        for (k = 0, i = 0; k < 2; k++) {
            if (k == 0) {
                // shadow
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, pauseCtx->alpha);
            } else {
                if (Inventory_GetSkullTokenCount(play->sceneId) == 100) {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 200, 50, 50, pauseCtx->alpha);
                } else {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
                }
            }

            isDigitDrawn = false;
            for (j = 0; j < 3; j++, i += 4) {
                if ((j >= 2) || (skullTokenDigits[j] != 0) || isDigitDrawn) {
                    gDPLoadTextureBlock(POLY_OPA_DISP++, ((u8*)gCounterDigit0Tex + (8 * 16 * skullTokenDigits[j])),
                                        G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, i, i + 2, i + 3, i + 1, 0);

                    isDigitDrawn = true;
                }
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}