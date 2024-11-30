#include "modding.h"
#include "global.h"

// set to default values for now
Color_RGB8 magicColor = {0, 200, 0};
Color_RGB8 magicColorChateau = {0, 0, 200};

extern u64 gMagicMeterEndTex[];
extern u64 gMagicMeterMidTex[];
extern u64 gMagicMeterFillTex[];

s16 sMagicMeterOutlinePrimRed = 255;
s16 sMagicMeterOutlinePrimGreen = 255;
s16 sMagicMeterOutlinePrimBlue = 255;

Gfx* Gfx_DrawTexRectIA8_DropShadow(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft,
                                   s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy, s16 r, s16 g, s16 b,
                                   s16 a);
Gfx* Gfx_DrawTexRectIA8_DropShadowOffset(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight,
                                         s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy,
                                         s16 r, s16 g, s16 b, s16 a, s32 masks, s32 rects);

RECOMP_PATCH void Magic_DrawMeter(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 magicBarY;

    OPEN_DISPS(play->state.gfxCtx);

    if (gSaveContext.save.saveInfo.playerData.magicLevel != 0) {
        if (gSaveContext.save.saveInfo.playerData.healthCapacity > 0xA0) {
            magicBarY = 42; // two rows of hearts
        } else {
            magicBarY = 34; // one row of hearts
        }

        Gfx_SetupDL39_Overlay(play->state.gfxCtx);

        gDPSetEnvColor(OVERLAY_DISP++, 100, 50, 50, 255);

        OVERLAY_DISP = Gfx_DrawTexRectIA8_DropShadow(
            OVERLAY_DISP, gMagicMeterEndTex, 8, 16, 18, magicBarY, 8, 16, 1 << 10, 1 << 10, sMagicMeterOutlinePrimRed,
            sMagicMeterOutlinePrimGreen, sMagicMeterOutlinePrimBlue, interfaceCtx->magicAlpha);
        OVERLAY_DISP = Gfx_DrawTexRectIA8_DropShadow(OVERLAY_DISP, gMagicMeterMidTex, 24, 16, 26, magicBarY,
                                                     ((void)0, gSaveContext.magicCapacity), 16, 1 << 10, 1 << 10,
                                                     sMagicMeterOutlinePrimRed, sMagicMeterOutlinePrimGreen,
                                                     sMagicMeterOutlinePrimBlue, interfaceCtx->magicAlpha);
        OVERLAY_DISP = Gfx_DrawTexRectIA8_DropShadowOffset(
            OVERLAY_DISP, gMagicMeterEndTex, 8, 16, ((void)0, gSaveContext.magicCapacity) + 26, magicBarY, 8, 16,
            1 << 10, 1 << 10, sMagicMeterOutlinePrimRed, sMagicMeterOutlinePrimGreen, sMagicMeterOutlinePrimBlue,
            interfaceCtx->magicAlpha, 3, 0x100);

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);

        if (gSaveContext.magicState == MAGIC_STATE_METER_FLASH_2) {
            // Yellow part of the meter indicating the amount of magic to be subtracted
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 250, 250, 0, interfaceCtx->magicAlpha);
            gDPLoadTextureBlock_4b(OVERLAY_DISP++, gMagicMeterFillTex, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(OVERLAY_DISP++, 104, (magicBarY + 3) << 2,
                                (((void)0, gSaveContext.save.saveInfo.playerData.magic) + 26) << 2,
                                (magicBarY + 10) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

            // Fill the rest of the meter with the normal magic color
            gDPPipeSync(OVERLAY_DISP++);
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI)) {
                // Blue magic
				// gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 200, interfaceCtx->magicAlpha);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, magicColorChateau.r, magicColorChateau.g, magicColorChateau.b, interfaceCtx->magicAlpha);
            } else {
                // Green magic (default)
				// gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 200, 0, interfaceCtx->magicAlpha);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, magicColor.r, magicColor.g, magicColor.b, interfaceCtx->magicAlpha);
            }

            gSPTextureRectangle(
                OVERLAY_DISP++, 104, (magicBarY + 3) << 2,
                ((((void)0, gSaveContext.save.saveInfo.playerData.magic) - ((void)0, gSaveContext.magicToConsume)) + 26)
                    << 2,
                (magicBarY + 10) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        } else {
            // Fill the whole meter with the normal magic color
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI)) {
                // Blue magic
				// gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 200, interfaceCtx->magicAlpha);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, magicColorChateau.r, magicColorChateau.g, magicColorChateau.b, interfaceCtx->magicAlpha);
            } else {
                // Green magic (default)
				// gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 200, 0, interfaceCtx->magicAlpha);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, magicColor.r, magicColor.g, magicColor.b, interfaceCtx->magicAlpha);
            }

            gDPLoadTextureBlock_4b(OVERLAY_DISP++, gMagicMeterFillTex, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(OVERLAY_DISP++, 104, (magicBarY + 3) << 2,
                                (((void)0, gSaveContext.save.saveInfo.playerData.magic) + 26) << 2,
                                (magicBarY + 10) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}