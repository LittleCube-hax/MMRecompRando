#include "modding.h"
#include "global.h"

#define FAIRY_PARAMS(type, boolParam, collectibleFlag) (((type) /* & 0xF */) | (((boolParam) & 0x1) << 8) | ((((collectibleFlag) & 0x7F) << 9) & 0xFE00))

typedef enum {
    /* 0x0 */ FAIRY_TYPE_0,
    /* 0x1 */ FAIRY_TYPE_1,
    /* 0x2 */ FAIRY_TYPE_2,
    /* 0x3 */ FAIRY_TYPE_3,
    /* 0x4 */ FAIRY_TYPE_4,
    /* 0x5 */ FAIRY_TYPE_5,
    /* 0x6 */ FAIRY_TYPE_6,
    /* 0x7 */ FAIRY_TYPE_7,
    /* 0x8 */ FAIRY_TYPE_8,
    /* 0x9 */ FAIRY_TYPE_9,
    /* 0xA */ FAIRY_TYPE_10
} FairyType;

void func_800A6780(EnItem00* this, PlayState* play);
s16 func_800A7650(s16 dropId);

#define ITEM00_BOMBCHU 0x1E

u8 sDropTable[DROP_TABLE_SIZE * DROP_TABLE_NUMBER] = {
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_BLUE,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_BOMBS_A,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,

    ITEM00_RUPEE_GREEN,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,

    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_GREEN,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_BOMBS_A,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,

    ITEM00_RUPEE_GREEN,
    ITEM00_NO_DROP,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_RED,
    ITEM00_NO_DROP,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_BOMBS_A,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,

    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,

    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,

    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,
    
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,

    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_30,
    ITEM00_ARROWS_30,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,

    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,

    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_DEKU_NUTS_1,
    ITEM00_DEKU_NUTS_1,
    ITEM00_NO_DROP,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_DEKU_STICK,
    ITEM00_DEKU_STICK,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,

    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_RED,
    ITEM00_NO_DROP,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_30,
    ITEM00_BOMBS_A,
    ITEM00_BOMBCHU, // ITEM00_NO_DROP,
    ITEM00_DEKU_STICK,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,

    ITEM00_RUPEE_GREEN,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
};

u8 sDropTableAmounts[DROP_TABLE_SIZE * DROP_TABLE_NUMBER] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x01, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
};

RECOMP_PATCH void Item_DropCollectibleRandom(PlayState* play, Actor* fromActor, Vec3f* spawnPos, s16 params) {
    EnItem00* spawnedActor;
    u8 dropId;
    s32 dropQuantity;
    s16 dropTableIndex = Rand_ZeroOne() * 16.0f;
    s16 param8000 = params & 0x8000;
    u8 dropFlag;

    params &= 0x1F0;

    if (params < 0x101) {
        dropId = sDropTable[params + dropTableIndex];
        dropQuantity = sDropTableAmounts[params + dropTableIndex];

        if (dropId == ITEM00_MASK) {
            switch (GET_PLAYER_FORM) {
                case PLAYER_FORM_HUMAN:
                    dropId = ITEM00_ARROWS_10;
                    break;

                case PLAYER_FORM_ZORA:
                    dropId = ITEM00_RECOVERY_HEART;
                    break;

                case PLAYER_FORM_GORON:
                    dropId = ITEM00_MAGIC_JAR_SMALL;
                    break;

                default:
                    dropId = ITEM00_RUPEE_GREEN;
                    break;
            }
            dropQuantity = 1;
        }

        if (fromActor != NULL) {
            dropFlag = fromActor->dropFlag;

            if (dropFlag != DROPFLAG_NONE) {
                if (fromActor->dropFlag & DROPFLAG_1) {
                    params = 0x10;
                    dropId = ITEM00_ARROWS_30;
                    dropQuantity = 1;
                } else if (fromActor->dropFlag & DROPFLAG_2) {
                    params = 0x10;
                    dropId = ITEM00_RECOVERY_HEART;
                    dropQuantity = 1;
                } else if (fromActor->dropFlag & DROPFLAG_20) {
                    dropId = ITEM00_RUPEE_PURPLE;
                    dropQuantity = 1;
                }
            }
        }

        if (dropId == ITEM00_FLEXIBLE) {
            if (gSaveContext.save.saveInfo.playerData.health <= 0x10) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, spawnPos->x, spawnPos->y + 40.0f, spawnPos->z, 0, 0, 0,
                            FAIRY_PARAMS(FAIRY_TYPE_2, false, 0));
                SoundSource_PlaySfxAtFixedWorldPos(play, spawnPos, 40, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                return;
            }

            if (gSaveContext.save.saveInfo.playerData.health <= 0x30) {
                params = 0x10;
                dropId = ITEM00_RECOVERY_HEART;
                dropQuantity = 3;
            } else if (gSaveContext.save.saveInfo.playerData.health <= 0x50) {
                params = 0x10;
                dropId = ITEM00_RECOVERY_HEART;
                dropQuantity = 1;
            } else if ((gSaveContext.save.saveInfo.playerData.magicLevel != 0) &&
                       (gSaveContext.save.saveInfo.playerData.magic == 0)) {
                params = 0xD0;
                dropId = ITEM00_MAGIC_JAR_BIG;
                dropQuantity = 1;
            } else if ((gSaveContext.save.saveInfo.playerData.magicLevel != 0) &&
                       ((gSaveContext.save.saveInfo.playerData.magicLevel >> 1) >=
                        gSaveContext.save.saveInfo.playerData.magic)) {
                params = 0xD0;
                dropId = ITEM00_MAGIC_JAR_BIG;
                dropQuantity = 1;
            } else if (AMMO(ITEM_BOW) < 6) {
                params = 0xA0;
                dropId = ITEM00_ARROWS_30;
                dropQuantity = 1;
            } else if (AMMO(ITEM_BOMB) < 6) {
                params = 0xB0;
                dropId = ITEM00_BOMBS_A;
                dropQuantity = 1;
            } else if (AMMO(ITEM_BOMBCHU) < 6) {
                params = 0xB0;
                dropId = ITEM00_BOMBS_A;
                dropQuantity = 1;
            } else if (gSaveContext.save.saveInfo.playerData.rupees < 11) {
                params = 0xA0;
                dropId = ITEM00_RUPEE_RED;
                dropQuantity = 1;
            } else {
                return;
            }
        }

        if (dropId != (u8)ITEM00_NO_DROP) {
            while (dropQuantity > 0) {
                if (param8000 == 0) {
                    dropId = func_800A7650(dropId);
                    if (dropId != (u8)ITEM00_NO_DROP) {
                        spawnedActor = (EnItem00*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, spawnPos->x,
                                                              spawnPos->y, spawnPos->z, 0, 0, 0, dropId);
                        if ((spawnedActor != 0) && (dropId != (u8)ITEM00_NO_DROP)) {
                            spawnedActor->actor.velocity.y = 8.0f;
                            spawnedActor->actor.speed = 2.0f;
                            spawnedActor->actor.gravity = -0.9f;
                            spawnedActor->actor.world.rot.y = Rand_ZeroOne() * 40000.0f;
                            Actor_SetScale(&spawnedActor->actor, 0.0f);
                            spawnedActor->actionFunc = func_800A6780;
                            spawnedActor->actor.flags = spawnedActor->actor.flags | ACTOR_FLAG_10;
                            if ((spawnedActor->actor.params != ITEM00_SMALL_KEY) &&
                                (spawnedActor->actor.params != ITEM00_HEART_PIECE) &&
                                (spawnedActor->actor.params != ITEM00_HEART_CONTAINER)) {
                                spawnedActor->actor.room = -1;
                            }
                            spawnedActor->unk152 = 220;
                        }
                    }
                } else {
                    Item_DropCollectible(play, spawnPos, params | 0x8000);
                }

                dropQuantity--;
            }
        }
    }
}

extern Gfx gItemDropDL[];
extern u64 gDropRecoveryHeartTex[];
extern u64 gDropBombTex[];
extern u64 gDropArrows1Tex[];
extern u64 gDropArrows2Tex[];
extern u64 gDropArrows3Tex[];
extern u64 gDropDekuNutTex[];
extern u64 gDropDekuStickTex[];
extern u64 gDropMagicLargeTex[];
extern u64 gDropMagicSmallTex[];
extern u64 gDropKeySmallTex[];
extern u64 gDropBombchuTex[];

TexturePtr sItemDropTextures[] = {
    gDropRecoveryHeartTex, // Heart (Not used)
    gDropBombTex,          // Bombs (A), Bombs (0)
    gDropArrows1Tex,       // Arrows (10)
    gDropArrows2Tex,       // Arrows (30)
    gDropArrows3Tex,       // Arrows (40), Arrows (50)
    gDropBombTex,          // Bombs (B)
    gDropDekuNutTex,       // Nuts (1), Nuts (10)
    gDropDekuStickTex,     // Sticks (1)
    gDropMagicLargeTex,    // Magic (Large)
    gDropMagicSmallTex,    // Magic (Small)
    NULL,
    gDropKeySmallTex, // Small Key
    gDropBombchuTex,
};

RECOMP_PATCH void EnItem00_DrawSprite(EnItem00* this, PlayState* play) {
    s32 texIndex = this->actor.params - 3;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    if (this->actor.params == ITEM00_DEKU_NUTS_10) {
        texIndex = 6;
    } else if (this->actor.params == ITEM00_BOMBS_0) {
        texIndex = 1;
    } else if (this->actor.params >= ITEM00_ARROWS_30) {
        texIndex -= 3;
        if (this->actor.params < ITEM00_ARROWS_50) {
            texIndex++;
        } else if (this->actor.params == ITEM00_BOMBCHU) {
            texIndex = 12;
        }
    }

    POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sItemDropTextures[texIndex]));

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

RECOMP_PATCH s16 func_800A7650(s16 dropId) {
    if ((((dropId == ITEM00_BOMBS_A) || (dropId == ITEM00_BOMBS_0) || (dropId == ITEM00_BOMBS_B)) &&
         (INV_CONTENT(ITEM_BOMB) == ITEM_NONE)) ||
        (dropId == ITEM00_BOMBCHU && (INV_CONTENT(ITEM_BOMBCHU) == ITEM_NONE)) ||
        (((dropId == ITEM00_ARROWS_10) || (dropId == ITEM00_ARROWS_30) || (dropId == ITEM00_ARROWS_40) ||
          (dropId == ITEM00_ARROWS_50)) &&
         (INV_CONTENT(ITEM_BOW) == ITEM_NONE)) ||
        (((dropId == ITEM00_MAGIC_JAR_BIG) || (dropId == ITEM00_MAGIC_JAR_SMALL)) &&
         (gSaveContext.save.saveInfo.playerData.magicLevel == 0))) {
        return ITEM00_NO_DROP;
    }

    if (dropId == ITEM00_RECOVERY_HEART) {
        if (((void)0, gSaveContext.save.saveInfo.playerData.healthCapacity) ==
            ((void)0, gSaveContext.save.saveInfo.playerData.health)) {
            return ITEM00_RUPEE_GREEN;
        }
    }

    return dropId;
}