#include "modding.h"
#include "global.h"

// TODO: apply colors to song playing + message boxes
// set to default values for now
Color_RGB8 buttonBColor = {100, 255, 120};
Color_RGB8 buttonCColor = {255, 240, 0};
Color_RGB8 buttonAColor = {100, 200, 255};
Color_RGB8 buttonStartColor = {255, 130, 60};

extern u64 gTatlCUpENGTex[];
extern u64 gTatlCUpGERTex[];
extern u64 gTatlCUpFRATex[];
extern u64 gTatlCUpESPTex[];
extern u64 gButtonBackgroundTex[];

s16 D_801BF9D4[] = {
    0xA7,  // EQUIP_SLOT_B
    0xE3,  // EQUIP_SLOT_C_LEFT
    0xF9,  // EQUIP_SLOT_C_DOWN
    0x10F, // EQUIP_SLOT_C_RIGHT
};
s16 D_801BF9DC[] = {
    0x11, // EQUIP_SLOT_B
    0x12, // EQUIP_SLOT_C_LEFT
    0x22, // EQUIP_SLOT_C_DOWN
    0x12, // EQUIP_SLOT_C_RIGHT
};
s16 D_801BF9E4[] = {
    0x23F, // EQUIP_SLOT_B
    0x26C, // EQUIP_SLOT_C_LEFT
    0x26C, // EQUIP_SLOT_C_DOWN
    0x26C, // EQUIP_SLOT_C_RIGHT
};

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

typedef enum {
    /* 0 */ DEBUG_EDITOR_NONE,
    /* 1 */ DEBUG_EDITOR_INVENTORY_INIT,
    /* 2 */ DEBUG_EDITOR_INVENTORY,
    /* 3 */ DEBUG_EDITOR_EVENTS
} DebugEditor;

#define DO_ACTION_TEX_WIDTH 48
#define DO_ACTION_TEX_HEIGHT 16
#define DO_ACTION_TEX_SIZE ((DO_ACTION_TEX_WIDTH * DO_ACTION_TEX_HEIGHT) / 2)

typedef enum {
    /* 0 */ PICTO_BOX_STATE_OFF,         // Not using the pictograph
    /* 1 */ PICTO_BOX_STATE_LENS,        // Looking through the lens of the pictograph
    /* 2 */ PICTO_BOX_STATE_SETUP_PHOTO, // Looking at the photo currently taken
    /* 3 */ PICTO_BOX_STATE_PHOTO
} PictoBoxState;
s16 sPictoState = PICTO_BOX_STATE_OFF;

u16 sCUpInvisible = 0;
u16 sCUpTimer = 0;

Gfx* Gfx_DrawTexRectIA8_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                   s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                   s16 a);
Gfx* Gfx_DrawRect_DropShadow(Gfx* gfx, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                             s16 r, s16 g, s16 b, s16 a);


RECOMP_PATCH void Interface_DrawItemButtons(PlayState* play) {
    static TexturePtr cUpLabelTextures[] = {
        gTatlCUpENGTex, gTatlCUpENGTex, gTatlCUpGERTex, gTatlCUpFRATex, gTatlCUpESPTex,
    };
    static s16 startButtonLeftPos[] = {
        // Remnant of OoT
        130, 136, 136, 136, 136,
    };
    static s16 D_801BFAF4[] = {
        0x1D, // EQUIP_SLOT_B
        0x1B, // EQUIP_SLOT_C_LEFT
        0x1B, // EQUIP_SLOT_C_DOWN
        0x1B, // EQUIP_SLOT_C_RIGHT
    };
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    PauseContext* pauseCtx = &play->pauseCtx;
    MessageContext* msgCtx = &play->msgCtx;
    s16 temp; // Used as both an alpha value and a button index
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    // B Button Color & Texture
    // OVERLAY_DISP = Gfx_DrawTexRectIA8_DropShadow(
    //     OVERLAY_DISP, gButtonBackgroundTex, 0x20, 0x20, D_801BF9D4[EQUIP_SLOT_B], D_801BF9DC[EQUIP_SLOT_B],
    //     D_801BFAF4[EQUIP_SLOT_B], D_801BFAF4[EQUIP_SLOT_B], D_801BF9E4[EQUIP_SLOT_B] * 2, D_801BF9E4[EQUIP_SLOT_B] * 2,
    //     100, 255, 120, interfaceCtx->bAlpha);
    OVERLAY_DISP = Gfx_DrawTexRectIA8_DropShadow(
        OVERLAY_DISP, gButtonBackgroundTex, 0x20, 0x20, D_801BF9D4[EQUIP_SLOT_B], D_801BF9DC[EQUIP_SLOT_B],
        D_801BFAF4[EQUIP_SLOT_B], D_801BFAF4[EQUIP_SLOT_B], D_801BF9E4[EQUIP_SLOT_B] * 2, D_801BF9E4[EQUIP_SLOT_B] * 2,
        buttonBColor.r, buttonBColor.g, buttonBColor.b, interfaceCtx->bAlpha);
    gDPPipeSync(OVERLAY_DISP++);

    // C-Left Button Color & Texture
    OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, D_801BF9D4[EQUIP_SLOT_C_LEFT], D_801BF9DC[EQUIP_SLOT_C_LEFT],
                                           D_801BFAF4[EQUIP_SLOT_C_LEFT], D_801BFAF4[EQUIP_SLOT_C_LEFT],
                                           D_801BF9E4[EQUIP_SLOT_C_LEFT] * 2, D_801BF9E4[EQUIP_SLOT_C_LEFT] * 2, 
										   buttonCColor.r, buttonCColor.g, buttonCColor.b, interfaceCtx->cLeftAlpha);
    // C-Down Button Color & Texture
    OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, D_801BF9D4[EQUIP_SLOT_C_DOWN], D_801BF9DC[EQUIP_SLOT_C_DOWN],
                                           D_801BFAF4[EQUIP_SLOT_C_DOWN], D_801BFAF4[EQUIP_SLOT_C_DOWN],
                                           D_801BF9E4[EQUIP_SLOT_C_DOWN] * 2, D_801BF9E4[EQUIP_SLOT_C_DOWN] * 2,
										   buttonCColor.r, buttonCColor.g, buttonCColor.b, interfaceCtx->cDownAlpha);
    // C-Right Button Color & Texture
    OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, D_801BF9D4[EQUIP_SLOT_C_RIGHT], D_801BF9DC[EQUIP_SLOT_C_RIGHT],
                                           D_801BFAF4[EQUIP_SLOT_C_RIGHT], D_801BFAF4[EQUIP_SLOT_C_RIGHT],
                                           D_801BF9E4[EQUIP_SLOT_C_RIGHT] * 2, D_801BF9E4[EQUIP_SLOT_C_RIGHT] * 2,
										   buttonCColor.r, buttonCColor.g, buttonCColor.b, interfaceCtx->cRightAlpha);

    if (!IS_PAUSE_STATE_GAMEOVER) {
        if ((play->pauseCtx.state != PAUSE_STATE_OFF) || (play->pauseCtx.debugEditor != DEBUG_EDITOR_NONE)) {
            // OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, 0x88, 0x11, 0x16, 0x16, 0x5B6, 0x5B6, 0xFF, 0x82, 0x3C,
            //                                        interfaceCtx->startAlpha);
            OVERLAY_DISP = Gfx_DrawRect_DropShadow(OVERLAY_DISP, 0x88, 0x11, 0x16, 0x16, 0x5B6, 0x5B6,
													buttonStartColor.r, buttonStartColor.g, buttonStartColor.b, interfaceCtx->startAlpha);
            // Start Button Texture, Color & Label
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->startAlpha);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE * 2, G_IM_FMT_IA,
                                   DO_ACTION_TEX_WIDTH, DO_ACTION_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(OVERLAY_DISP++, 0x01F8, 0x0054, 0x02D4, 0x009C, G_TX_RENDERTILE, 0, 0, 0x04A6, 0x04A6);
        }
    }

    if (interfaceCtx->tatlCalling && (play->pauseCtx.state == PAUSE_STATE_OFF) &&
        (play->pauseCtx.debugEditor == DEBUG_EDITOR_NONE) && (play->csCtx.state == CS_STATE_IDLE) &&
        (sPictoState == PICTO_BOX_STATE_OFF)) {
        if (sCUpInvisible == 0) {
            // C-Up Button Texture, Color & Label (Tatl Text)
            gDPPipeSync(OVERLAY_DISP++);

            if ((gSaveContext.hudVisibility == HUD_VISIBILITY_NONE) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_NONE_ALT) ||
                (gSaveContext.hudVisibility == HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE) ||
                (msgCtx->msgMode != MSGMODE_NONE)) {
                temp = 0;
            } else if (player->stateFlags1 & PLAYER_STATE1_200000) {
                temp = 70;
            } else {
                temp = interfaceCtx->aAlpha;
            }

            OVERLAY_DISP =
                // Gfx_DrawRect_DropShadow(OVERLAY_DISP, 0xFE, 0x10, 0x10, 0x10, 0x800, 0x800, 0xFF, 0xF0, 0, temp);
                Gfx_DrawRect_DropShadow(OVERLAY_DISP, 0xFE, 0x10, 0x10, 0x10, 0x800, 0x800, buttonCColor.r, buttonCColor.g, buttonCColor.b, temp);

            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, temp);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPLoadTextureBlock_4b(OVERLAY_DISP++, cUpLabelTextures[gSaveContext.options.language], G_IM_FMT_IA, 32, 12,
                                   0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                   G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(OVERLAY_DISP++, 0x03DC, 0x0048, 0x045C, 0x0078, G_TX_RENDERTILE, 0, 0, 1 << 10,
                                1 << 10);
        }

        sCUpTimer--;
        if (sCUpTimer == 0) {
            sCUpInvisible ^= 1;
            sCUpTimer = 10;
        }
    }

    gDPPipeSync(OVERLAY_DISP++);

    // Empty C Button Arrows
    for (temp = EQUIP_SLOT_C_LEFT; temp <= EQUIP_SLOT_C_RIGHT; temp++) {
        if (GET_CUR_FORM_BTN_ITEM(temp) > 0xF0) {
            if (temp == EQUIP_SLOT_C_LEFT) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, buttonCColor.r, buttonCColor.g, buttonCColor.b, interfaceCtx->cLeftAlpha);
            } else if (temp == EQUIP_SLOT_C_DOWN) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, buttonCColor.r, buttonCColor.g, buttonCColor.b, interfaceCtx->cDownAlpha);
            } else { // EQUIP_SLOT_C_RIGHT
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, buttonCColor.r, buttonCColor.g, buttonCColor.b, interfaceCtx->cRightAlpha);
            }
            OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, ((u8*)gButtonBackgroundTex + ((32 * 32) * (temp + 1))),
                                              0x20, 0x20, D_801BF9D4[temp], D_801BF9DC[temp], D_801BFAF4[temp],
                                              D_801BFAF4[temp], D_801BF9E4[temp] * 2, D_801BF9E4[temp] * 2);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

f32 D_801BF9CC[] = { -380.0f, -350.0f };

void Interface_SetPerspectiveView(PlayState* play, s32 topY, s32 bottomY, s32 leftX, s32 rightX);
void Interface_SetPerfectLetters(PlayState* play, s16 perfectLettersType);

RECOMP_PATCH void Interface_DrawAButton(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 aAlpha;

    OPEN_DISPS(play->state.gfxCtx);

    aAlpha = interfaceCtx->aAlpha;

    if (aAlpha > 100) {
        aAlpha = 100;
    }

    Gfx_SetupDL42_Overlay(play->state.gfxCtx);

    Interface_SetPerspectiveView(play, 25 + R_A_BTN_Y_OFFSET, 70 + R_A_BTN_Y_OFFSET, 192, 237);

    gSPClearGeometryMode(OVERLAY_DISP++, G_CULL_BOTH);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetAlphaCompare(OVERLAY_DISP++, G_AC_THRESHOLD);

    Matrix_Translate(0.0f, 0.0f, -38.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateXFApply(interfaceCtx->aButtonRoll / 10000.0f);

    // Draw A button Shadow
    gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(OVERLAY_DISP++);
    gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[4], 4, 0);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, aAlpha);

    OVERLAY_DISP = Gfx_DrawTexQuadIA8(OVERLAY_DISP, gButtonBackgroundTex, 32, 32, 0);

    // Draw A Button Colored
    gDPPipeSync(OVERLAY_DISP++);
    Interface_SetPerspectiveView(play, 23 + R_A_BTN_Y_OFFSET, 68 + R_A_BTN_Y_OFFSET, 190, 235);
    gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[0], 4, 0);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, buttonAColor.r, buttonAColor.g, buttonAColor.b, interfaceCtx->aAlpha);
    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

    // Draw A Button Do-Action
    gDPPipeSync(OVERLAY_DISP++);
    Interface_SetPerspectiveView(play, 23 + R_A_BTN_Y_OFFSET, 68 + R_A_BTN_Y_OFFSET, 190, 235);
    gSPSetGeometryMode(OVERLAY_DISP++, G_CULL_BACK);
    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);

    Matrix_Translate(0.0f, 0.0f, D_801BF9CC[gSaveContext.options.language] / 10.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateXFApply(interfaceCtx->aButtonRoll / 10000.0f);
    gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[8], 4, 0);

    // Draw Action Label
    if (((interfaceCtx->aButtonState <= A_BTN_STATE_1) || (interfaceCtx->aButtonState == A_BTN_STATE_3))) {
        OVERLAY_DISP = Gfx_DrawTexQuad4b(OVERLAY_DISP, interfaceCtx->doActionSegment, 3, DO_ACTION_TEX_WIDTH,
                                         DO_ACTION_TEX_HEIGHT, 0);
    } else {
        OVERLAY_DISP = Gfx_DrawTexQuad4b(OVERLAY_DISP, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE, 3,
                                         DO_ACTION_TEX_WIDTH, DO_ACTION_TEX_HEIGHT, 0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern s16 sOcarinaButtonAPrimR;
extern s16 sOcarinaButtonAPrimB;
extern s16 sOcarinaButtonAPrimG;
extern s16 sOcarinaButtonAEnvR;
extern s16 sOcarinaButtonAEnvB;
extern s16 sOcarinaButtonAEnvG;
extern s16 sOcarinaButtonCPrimR;
extern s16 sOcarinaButtonCPrimB;
extern s16 sOcarinaButtonCPrimG;
extern s16 sOcarinaButtonCEnvR;
extern s16 sOcarinaButtonCEnvB;
extern s16 sOcarinaButtonCEnvG;

void Message_ResetOcarinaButtonAlphas(void);

RECOMP_PATCH void Message_ResetOcarinaButtonState(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;

    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_A] = 189;
    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_DOWN] = 184;
    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_RIGHT] = 179;
    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_LEFT] = 174;
    msgCtx->ocarinaButtonsPosY[OCARINA_BTN_C_UP] = 169;

    Message_ResetOcarinaButtonAlphas();

    // sOcarinaButtonAPrimR = 80;
    // sOcarinaButtonAPrimG = 150;
    // sOcarinaButtonAPrimB = 255;
    sOcarinaButtonAPrimR = buttonAColor.r;
    sOcarinaButtonAPrimG = buttonAColor.g;
    sOcarinaButtonAPrimB = buttonAColor.b;
    sOcarinaButtonAEnvR = 10;
    sOcarinaButtonAEnvG = 10;
    sOcarinaButtonAEnvB = 10;
    // sOcarinaButtonCPrimR = 255;
    // sOcarinaButtonCPrimG = 255;
    // sOcarinaButtonCPrimB = 50;
    sOcarinaButtonCPrimR = buttonCColor.r;
    sOcarinaButtonCPrimG = buttonCColor.g;
    sOcarinaButtonCPrimB = buttonCColor.b;
    sOcarinaButtonCEnvR = 10;
    sOcarinaButtonCEnvG = 10;
    sOcarinaButtonCEnvB = 10;
}

extern s32 sCharTexSize;
extern s32 sCharTexScale;

RECOMP_PATCH void Message_DrawTextboxIcon(PlayState* play, Gfx** gfxP, s16 x, s16 y) {
    // static Color_RGB16 sIconPrimColors[] = {
    //     { 0, 80, 200 },
    //     { 50, 130, 255 },
    // };
    // static Color_RGB16 sIconEnvColors[] = {
    //     { 0, 0, 0 },
    //     { 0, 130, 255 },
    // };
    // static s16 sIconPrimR = 0;
    // static s16 sIconPrimG = 80;
    // static s16 sIconPrimB = 200;
    
    // TODO: add flashing back?
    Color_RGB16 sIconPrimColors[] = {
        {buttonAColor.r, buttonAColor.g, buttonAColor.b},
        {buttonAColor.r, buttonAColor.g, buttonAColor.b},
    };
    static Color_RGB16 sIconEnvColors[] = {
        { 0, 0, 0 },
        { 255, 255, 255 },
    };
    s16 sIconPrimR = buttonAColor.r;
    s16 sIconPrimG = buttonAColor.g;
    s16 sIconPrimB = buttonAColor.b;
    
    static s16 sIconFlashTimer = 12;
    static s16 sIconFlashColorIndex = 0;
    static s16 sIconEnvR = 0;
    static s16 sIconEnvG = 0;
    static s16 sIconEnvB = 0;
    Gfx* gfx;
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;
    s16 primR;
    s16 primG;
    s16 primB;
    s16 envR;
    s16 envG;
    s16 envB;
    u8* iconTexture = msgCtx->font.iconBuf;

    gfx = *gfxP;

    if (!msgCtx->textIsCredits) {
        primR = ABS_ALT(sIconPrimR - sIconPrimColors[sIconFlashColorIndex].r) / sIconFlashTimer;
        primG = ABS_ALT(sIconPrimG - sIconPrimColors[sIconFlashColorIndex].g) / sIconFlashTimer;
        primB = ABS_ALT(sIconPrimB - sIconPrimColors[sIconFlashColorIndex].b) / sIconFlashTimer;

        if (sIconPrimR >= sIconPrimColors[sIconFlashColorIndex].r) {
            sIconPrimR -= primR;
        } else {
            sIconPrimR += primR;
        }

        if (sIconPrimG >= sIconPrimColors[sIconFlashColorIndex].g) {
            sIconPrimG -= primG;
        } else {
            sIconPrimG += primG;
        }

        if (sIconPrimB >= sIconPrimColors[sIconFlashColorIndex].b) {
            sIconPrimB -= primB;
        } else {
            sIconPrimB += primB;
        }

        envR = ABS_ALT(sIconEnvR - sIconEnvColors[sIconFlashColorIndex].r) / sIconFlashTimer;
        envG = ABS_ALT(sIconEnvG - sIconEnvColors[sIconFlashColorIndex].g) / sIconFlashTimer;
        envB = ABS_ALT(sIconEnvB - sIconEnvColors[sIconFlashColorIndex].b) / sIconFlashTimer;

        if (sIconEnvR >= sIconEnvColors[sIconFlashColorIndex].r) {
            sIconEnvR -= envR;
        } else {
            sIconEnvR += envR;
        }

        if (sIconEnvG >= sIconEnvColors[sIconFlashColorIndex].g) {
            sIconEnvG -= envG;
        } else {
            sIconEnvG += envG;
        }

        if (sIconEnvB >= sIconEnvColors[sIconFlashColorIndex].b) {
            sIconEnvB -= envB;
        } else {
            sIconEnvB += envB;
        }

        sIconFlashTimer--;
        if (sIconFlashTimer == 0) {
            sIconPrimR = sIconPrimColors[sIconFlashColorIndex].r;
            sIconPrimG = sIconPrimColors[sIconFlashColorIndex].g;
            sIconPrimB = sIconPrimColors[sIconFlashColorIndex].b;
            sIconEnvR = sIconEnvColors[sIconFlashColorIndex].r;
            sIconEnvG = sIconEnvColors[sIconFlashColorIndex].g;
            sIconEnvB = sIconEnvColors[sIconFlashColorIndex].b;
            sIconFlashTimer = 12;
            sIconFlashColorIndex ^= 1;
        }

        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(gfx++, 0, 0, sIconPrimR, sIconPrimG, sIconPrimB, 255);
        gDPSetEnvColor(gfx++, sIconEnvR, sIconEnvG, sIconEnvB, 255);

        if (!play->pauseCtx.bombersNotebookOpen) {
            gDPLoadTextureBlock_4b(gfx++, iconTexture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                                   G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gfx++, x << 2, y << 2, (x + sCharTexSize) << 2, (y + sCharTexSize) << 2,
                                G_TX_RENDERTILE, 0, 0, sCharTexScale, sCharTexScale);
        }

        msgCtx->stateTimer++;
        *gfxP = gfx;
    }
}