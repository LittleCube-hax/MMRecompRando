#include "modding.h"
#include "global.h"

// set to default values for now
Color_RGB8 heartColor = {255, 70, 50};
Color_RGB8 heartColorBG = {50, 40, 60};

// unsure what some of the below do
s16 sHeartsPrimColors[3][3] = { { 255, 70, 50 }, { 255, 190, 0 }, { 100, 100, 255 } };
s16 sHeartsEnvColors[3][3] = { { 50, 40, 60 }, { 255, 0, 0 }, { 0, 0, 255 } };
s16 sHeartsPrimFactors[3][3] = { { 0, 0, 0 }, { 0, 120, -50 }, { -155, 30, 205 } };
s16 sHeartsEnvFactors[3][3] = { { 0, 0, 0 }, { 205, -40, -60 }, { -50, -40, 195 } };
s16 sHeartsDDPrimColors[3][3] = { { 255, 255, 255 }, { 255, 190, 0 }, { 100, 100, 255 } };
s16 sHeartsDDEnvColors[3][3] = { { 200, 0, 0 }, { 255, 0, 0 }, { 0, 0, 255 } };
s16 sHeartsDDPrimFactors[3][3] = { { 0, 0, 0 }, { 0, -65, -255 }, { -155, -155, 0 } };
s16 sHeartsDDEnvFactors[3][3] = { { 0, 0, 0 }, { 55, 0, 0 }, { -200, 0, 255 } };

s16 sBeatingHeartsDDPrim[3];
s16 sBeatingHeartsDDEnv[3];
s16 sHeartsDDPrim[2][3];
s16 sHeartsDDEnv[2][3];

RECOMP_PATCH void LifeMeter_UpdateColors(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    f32 factorBeating = interfaceCtx->lifeColorChange * 0.1f;
    f32 ddFactor;
    s32 type = 0;
    s32 ddType;
    s16 rFactor;
    s16 gFactor;
    s16 bFactor;

    if (interfaceCtx) {}

    if (interfaceCtx->lifeColorChangeDirection != 0) {
        interfaceCtx->lifeColorChange--;
        if (interfaceCtx->lifeColorChange <= 0) {
            interfaceCtx->lifeColorChange = 0;
            interfaceCtx->lifeColorChangeDirection = 0;
        }
    } else {
        interfaceCtx->lifeColorChange++;
        if (interfaceCtx->lifeColorChange >= 10) {
            interfaceCtx->lifeColorChange = 10;
            interfaceCtx->lifeColorChangeDirection = 1;
        }
    }

    ddFactor = factorBeating;

    // interfaceCtx->heartsPrimR[0] = 255;
    // interfaceCtx->heartsPrimG[0] = 70;
    // interfaceCtx->heartsPrimB[0] = 50;
    interfaceCtx->heartsPrimR[0] = heartColor.r;
    interfaceCtx->heartsPrimG[0] = heartColor.g;
    interfaceCtx->heartsPrimB[0] = heartColor.b;

    // interfaceCtx->heartsEnvR[0] = 50;
    // interfaceCtx->heartsEnvG[0] = 40;
    // interfaceCtx->heartsEnvB[0] = 60;
    interfaceCtx->heartsEnvR[0] = heartColorBG.r;
    interfaceCtx->heartsEnvG[0] = heartColorBG.g;
    interfaceCtx->heartsEnvB[0] = heartColorBG.b;

    interfaceCtx->heartsPrimR[1] = sHeartsPrimColors[type][0];
    interfaceCtx->heartsPrimG[1] = sHeartsPrimColors[type][1];
    interfaceCtx->heartsPrimB[1] = sHeartsPrimColors[type][2];

    interfaceCtx->heartsEnvR[1] = sHeartsEnvColors[type][0];
    interfaceCtx->heartsEnvG[1] = sHeartsEnvColors[type][1];
    interfaceCtx->heartsEnvB[1] = sHeartsEnvColors[type][2];

    rFactor = sHeartsPrimFactors[0][0] * factorBeating;
    gFactor = sHeartsPrimFactors[0][1] * factorBeating;
    bFactor = sHeartsPrimFactors[0][2] * factorBeating;

    interfaceCtx->beatingHeartPrim[0] = (u8)(rFactor + heartColor.r) & 0xFF;
    interfaceCtx->beatingHeartPrim[1] = (u8)(gFactor + heartColor.g) & 0xFF;
    interfaceCtx->beatingHeartPrim[2] = (u8)(bFactor + heartColor.b) & 0xFF;

    rFactor = sHeartsEnvFactors[0][0] * factorBeating;
    gFactor = sHeartsEnvFactors[0][1] * factorBeating;
    bFactor = sHeartsEnvFactors[0][2] * factorBeating;

    if (1) {}
    ddType = type;

    interfaceCtx->beatingHeartEnv[0] = (u8)(rFactor + heartColorBG.r) & 0xFF;
    interfaceCtx->beatingHeartEnv[1] = (u8)(gFactor + heartColorBG.g) & 0xFF;
    interfaceCtx->beatingHeartEnv[2] = (u8)(bFactor + heartColorBG.b) & 0xFF;

    sHeartsDDPrim[0][0] = 255;
    sHeartsDDPrim[0][1] = 255;
    sHeartsDDPrim[0][2] = 255;

    sHeartsDDEnv[0][0] = 200;
    sHeartsDDEnv[0][1] = 0;
    sHeartsDDEnv[0][2] = 0;

    sHeartsDDPrim[1][0] = sHeartsDDPrimColors[ddType][0];
    sHeartsDDPrim[1][1] = sHeartsDDPrimColors[ddType][1];
    sHeartsDDPrim[1][2] = sHeartsDDPrimColors[ddType][2];

    sHeartsDDEnv[1][0] = sHeartsDDEnvColors[ddType][0];
    sHeartsDDEnv[1][1] = sHeartsDDEnvColors[ddType][1];
    sHeartsDDEnv[1][2] = sHeartsDDEnvColors[ddType][2];

    rFactor = sHeartsDDPrimFactors[ddType][0] * ddFactor;
    gFactor = sHeartsDDPrimFactors[ddType][1] * ddFactor;
    bFactor = sHeartsDDPrimFactors[ddType][2] * ddFactor;

    sBeatingHeartsDDPrim[0] = (u8)(rFactor + 255) & 0xFF;
    sBeatingHeartsDDPrim[1] = (u8)(gFactor + 255) & 0xFF;
    sBeatingHeartsDDPrim[2] = (u8)(bFactor + 255) & 0xFF;

    rFactor = sHeartsDDEnvFactors[ddType][0] * ddFactor;
    gFactor = sHeartsDDEnvFactors[ddType][1] * ddFactor;
    bFactor = sHeartsDDEnvFactors[ddType][2] * ddFactor;

    sBeatingHeartsDDEnv[0] = (u8)(rFactor + 200) & 0xFF;
    sBeatingHeartsDDEnv[1] = (u8)(gFactor + 0) & 0xFF;
    sBeatingHeartsDDEnv[2] = (u8)(bFactor + 0) & 0xFF;
}