#include "modding.h"
#include "global.h"

// sets both inner and outer to be the same for now
Color_RGB8 tatlSolidColor = {255, 255, 230};

typedef struct {
    /* 0x0 */ Color_RGBA8 inner;
    /* 0x4 */ Color_RGBA8 outer;
} TatlColor; // size = 0x8

// unsure if these will be used
TatlColor sTatlColorList[] = {
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_SWITCH
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_BG
    { { 255, 255, 230, 255 }, { 220, 160, 80, 0 } },  // ACTORCAT_PLAYER
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_EXPLOSIVES
    { { 150, 150, 255, 255 }, { 150, 150, 255, 0 } }, // ACTORCAT_NPC
    { { 255, 255, 0, 255 }, { 200, 155, 0, 0 } },     // ACTORCAT_ENEMY
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_PROP
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_ITEMACTION
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_MISC
    { { 255, 255, 0, 255 }, { 200, 155, 0, 0 } },     // ACTORCAT_BOSS
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_DOOR
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_CHEST
    { { 0, 255, 0, 255 }, { 0, 255, 0, 0 } },         // ACTORCAT_MAX
};

RECOMP_PATCH void Target_SetFairyState(TargetContext* targetCtx, Actor* actor, ActorType type, PlayState* play) {
    targetCtx->fairyPos.x = actor->focus.pos.x;
    targetCtx->fairyPos.y = actor->focus.pos.y + (actor->targetArrowOffset * actor->scale.y);
    targetCtx->fairyPos.z = actor->focus.pos.z;

    // targetCtx->fairyInnerColor.r = sTatlColorList[type].inner.r;
    // targetCtx->fairyInnerColor.g = sTatlColorList[type].inner.g;
    // targetCtx->fairyInnerColor.b = sTatlColorList[type].inner.b;
    // targetCtx->fairyInnerColor.a = sTatlColorList[type].inner.a;
    // targetCtx->fairyOuterColor.r = sTatlColorList[type].outer.r;
    // targetCtx->fairyOuterColor.g = sTatlColorList[type].outer.g;
    // targetCtx->fairyOuterColor.b = sTatlColorList[type].outer.b;
    // targetCtx->fairyOuterColor.a = sTatlColorList[type].outer.a;
    targetCtx->fairyInnerColor.r = tatlSolidColor.r;
    targetCtx->fairyInnerColor.g = tatlSolidColor.g;
    targetCtx->fairyInnerColor.b = tatlSolidColor.b;
    targetCtx->fairyInnerColor.a = sTatlColorList[type].inner.a;
    targetCtx->fairyOuterColor.r = tatlSolidColor.r;
    targetCtx->fairyOuterColor.g = tatlSolidColor.g;
    targetCtx->fairyOuterColor.b = tatlSolidColor.b;
    targetCtx->fairyOuterColor.a = sTatlColorList[type].outer.a;
}

void Target_SetLockOnPos(TargetContext* targetCtx, s32 index, f32 x, f32 y, f32 z);

RECOMP_PATCH void Target_InitLockOn(TargetContext* targetCtx, ActorType type, PlayState* play) {
    TatlColor* tatlColorEntry;
    s32 i;
    LockOnTriangleSet* triangleSet;

    Math_Vec3f_Copy(&targetCtx->lockOnPos, &play->view.eye);
    targetCtx->lockOnAlpha = 256;
    // tatlColorEntry = &sTatlColorList[type];
    targetCtx->lockOnRadius = 500.0f;

    triangleSet = targetCtx->lockOnTriangleSets;
    for (i = 0; i < ARRAY_COUNT(targetCtx->lockOnTriangleSets); i++, triangleSet++) {
        Target_SetLockOnPos(targetCtx, i, 0.0f, 0.0f, 0.0f);

        // triangleSet->color.r = tatlColorEntry->inner.r;
        // triangleSet->color.g = tatlColorEntry->inner.g;
        // triangleSet->color.b = tatlColorEntry->inner.b;
        triangleSet->color.r = tatlSolidColor.r;
        triangleSet->color.g = tatlSolidColor.g;
        triangleSet->color.b = tatlSolidColor.b;
    }
}

extern Gfx gZTargetLockOnTriangleDL[];
extern Gfx gZTargetArrowDL[];

RECOMP_PATCH void Target_Draw(TargetContext* targetCtx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* actor;

    if (player->stateFlags1 & (PLAYER_STATE1_2 | PLAYER_STATE1_40 | PLAYER_STATE1_80 | PLAYER_STATE1_200 |
                               PLAYER_STATE1_400 | PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000)) {
        return;
    }

    actor = targetCtx->lockOnActor;

    OPEN_DISPS(play->state.gfxCtx);

    if (targetCtx->lockOnAlpha != 0) {
        LockOnTriangleSet* entry;
        s16 alpha = 255;
        f32 projectdPosScale = 1.0f;
        Vec3f projectedPos;
        s32 totalEntries;
        f32 invW;
        s32 i;
        s32 index;
        f32 lockOnScaleX;

        if (targetCtx->rotZTick != 0) {
            totalEntries = 1;
        } else {
            // Use multiple entries for the movement effect when the triangles are getting closer to the actor from the
            // margin of the screen
            totalEntries = ARRAY_COUNT(targetCtx->lockOnTriangleSets);
        }

        if (actor != NULL) {
            Math_Vec3f_Copy(&targetCtx->lockOnPos, &actor->focus.pos);
            projectdPosScale = (500.0f - targetCtx->lockOnRadius) / 420.0f;
        } else {
            targetCtx->lockOnAlpha -= 120;
            if (targetCtx->lockOnAlpha < 0) {
                targetCtx->lockOnAlpha = 0;
            }
            alpha = targetCtx->lockOnAlpha;
        }

        Actor_GetProjectedPos(play, &targetCtx->lockOnPos, &projectedPos, &invW);

        projectedPos.x = ((SCREEN_WIDTH / 2) * (projectedPos.x * invW)) * projectdPosScale;
        projectedPos.x = CLAMP(projectedPos.x, -SCREEN_WIDTH, SCREEN_WIDTH);

        projectedPos.y = ((SCREEN_HEIGHT / 2) * (projectedPos.y * invW)) * projectdPosScale;
        projectedPos.y = CLAMP(projectedPos.y, -SCREEN_HEIGHT, SCREEN_HEIGHT);

        projectedPos.z *= projectdPosScale;

        targetCtx->lockOnIndex--;
        if (targetCtx->lockOnIndex < 0) {
            targetCtx->lockOnIndex = ARRAY_COUNT(targetCtx->lockOnTriangleSets) - 1;
        }

        Target_SetLockOnPos(targetCtx, targetCtx->lockOnIndex, projectedPos.x, projectedPos.y, projectedPos.z);

        if (!(player->stateFlags1 & PLAYER_STATE1_40) || (actor != player->lockOnActor)) {
            OVERLAY_DISP = Gfx_SetupDL(OVERLAY_DISP, SETUPDL_57);

            for (i = 0, index = targetCtx->lockOnIndex; i < totalEntries;
                 i++, index = (index + 1) % ARRAY_COUNT(targetCtx->lockOnTriangleSets)) {
                entry = &targetCtx->lockOnTriangleSets[index];

                if (entry->radius < 500.0f) {
                    s32 triangleIndex;

                    if (entry->radius <= 120.0f) {
                        lockOnScaleX = 0.15f;
                    } else {
                        lockOnScaleX = ((entry->radius - 120.0f) * 0.001f) + 0.15f;
                    }

                    Matrix_Translate(entry->pos.x, entry->pos.y, 0.0f, MTXMODE_NEW);
                    Matrix_Scale(lockOnScaleX, 0.15f, 1.0f, MTXMODE_APPLY);

                    // gDPSetPrimColor(OVERLAY_DISP++, 0, 0, entry->color.r, entry->color.g, entry->color.b, (u8)alpha);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, tatlSolidColor.r, tatlSolidColor.g, tatlSolidColor.b, (u8)alpha);

                    Matrix_RotateZS(targetCtx->rotZTick * 0x200, MTXMODE_APPLY);

                    // Draw the 4 lock-on triangles
                    for (triangleIndex = 0; triangleIndex < 4; triangleIndex++) {
                        Matrix_RotateZS(0x10000 / 4, MTXMODE_APPLY);
                        Matrix_Push();
                        Matrix_Translate(entry->radius, entry->radius, 0.0f, MTXMODE_APPLY);
                        gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
                        gSPDisplayList(OVERLAY_DISP++, gZTargetLockOnTriangleDL);
                        Matrix_Pop();
                    }
                }

                alpha -= 255 / ARRAY_COUNT(targetCtx->lockOnTriangleSets);
                if (alpha < 0) {
                    alpha = 0;
                }
            }
        }
    }

    actor = targetCtx->arrowPointedActor;
    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_CANT_LOCK_ON)) {
        TatlColor* color = &sTatlColorList[actor->category];

        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_7);

        Matrix_Translate(actor->focus.pos.x, actor->focus.pos.y + (actor->targetArrowOffset * actor->scale.y) + 17.0f,
                         actor->focus.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(play->gameplayFrames * 0xBB8, MTXMODE_APPLY);
        Matrix_Scale((iREG(27) + 35) / 1000.0f, (iREG(28) + 60) / 1000.0f, (iREG(29) + 50) / 1000.0f, MTXMODE_APPLY);

        // gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, color->inner.r, color->inner.g, color->inner.b, 255);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, tatlSolidColor.r, tatlSolidColor.g, tatlSolidColor.b, 255);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_XLU_DISP++, gZTargetArrowDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}