#include "modding.h"
#include "global.h"
#include "ultra64.h"

#include "apcommon.h"
#include "box_ap.h"
#include "box_ootmm.h"

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));

struct EnBox;
struct struct_80867BDC_a0;

typedef void (*EnBoxActionFunc)(struct EnBox*, PlayState*);
typedef void (*EnBoxUnkFunc)(struct struct_80867BDC_a0* arg0, PlayState* play);

typedef struct struct_80867BDC_a0 {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ EnBoxUnkFunc unk_0C;
    /* 0x10 */ EnBoxUnkFunc unk_10;
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ s32 unk_20;
} struct_80867BDC_a0; // size 0x24

typedef enum {
    /*  0 */ ENBOX_TYPE_BIG,                    // big
    /*  1 */ ENBOX_TYPE_BIG_ROOM_CLEAR,         // appear on room clear, store temp clear as permanent clear
    /*  2 */ ENBOX_TYPE_BIG_ORNATE,             // boss key chest
    /*  3 */ ENBOX_TYPE_BIG_SWITCH_FLAG_FALL,   // falling, appear on switch flag set
    /*  4 */ ENBOX_TYPE_BIG_INVISIBLE,          // big, revealed with lens of truth or when opened
    /*  5 */ ENBOX_TYPE_SMALL,                  // small
    /*  6 */ ENBOX_TYPE_SMALL_INVISIBLE,        // small, revealed with lens of truth or when opened
    /*  7 */ ENBOX_TYPE_SMALL_ROOM_CLEAR,       // use room clear, store temp clear as perm clear
    /*  8 */ ENBOX_TYPE_SMALL_SWITCH_FLAG_FALL, // falling, appear on switch flag set
    /*  9 */ ENBOX_TYPE_BIG_SONG_ZELDAS_LULLABY,// NOT IMPLEMENTED, behaves like big chest
    /* 10 */ ENBOX_TYPE_BIG_SONG_SUNS,          // NOT IMPLEMENTED, behaves like big chest
    /* 11 */ ENBOX_TYPE_BIG_SWITCH_FLAG,        // big, appear on switch flag set
    /* 12 */ ENBOX_TYPE_SMALL_SWITCH_FLAG       // small, appear on switch flag set
} EnBoxType;

#define ENBOX_GET_TYPE(thisx) (((thisx)->params >> 12) & 0xF)
#define ENBOX_GET_ITEM(thisx) (((thisx)->params >> 5) & 0x7F)
#define ENBOX_GET_CHEST_FLAG(thisx) ((thisx)->params & 0x1F)
#define ENBOX_GET_SWITCH_FLAG(thisx) ((thisx)->world.rot.z)
// Codegen in EnTorch_Init() requires leaving out the & 0x7F for `item`
#define ENBOX_PARAMS(type, item, chestFlag) ((((type) & 0xF) << 12) | ((item) << 5) | ((chestFlag) & 0x1F))

#define OBJECT_BOX_CHEST_LIMB_01 0x1
#define OBJECT_BOX_CHEST_LIMB_03 0x3
#define OBJECT_BOX_CHEST_LIMB_MAX 0x5

extern AnimationHeader gBoxChestOpenAnim;
extern CollisionHeader gBoxChestCol;
extern SkeletonHeader gBoxChestSkel;

typedef struct EnBox {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ s32 unk_1A0;
    /* 0x1A4 */ UNK_TYPE1 unk_1A4[0x04];
    /* 0x1A8 */ f32 unk_1A8;
    /* 0x1AC */ EnBoxActionFunc actionFunc;
    /* 0x1B0 */ Vec3s jointTable[OBJECT_BOX_CHEST_LIMB_MAX];
    /* 0x1CE */ Vec3s morphTable[OBJECT_BOX_CHEST_LIMB_MAX];
    /* 0x1EC */ s16 unk_1EC;
    /* 0x1EE */ u8 movementFlags;
    /* 0x1EF */ u8 alpha;
    /* 0x1F0 */ u8 switchFlag;
    /* 0x1F1 */ u8 type;
    /* 0x1F2 */ u8 iceSmokeTimer;
    /* 0x1F3 */ s8 unk_1F3;
    /* 0x1F4 */ struct_80867BDC_a0 unk_1F4;
    /* 0x218 */ s16 csId1;
    /* 0x21A */ s16 csId2;
    /* 0x21C */ s32 getItemId;
    /* 0x220 */ s32 collectableFlag;
} EnBox; // size = 0x224

#define FLAGS 0x00000000

/*
set on init unless treasure flag is set
if clear, chest moves (Actor_MoveForward) (falls, likely)
ends up cleared from SWITCH_FLAG_FALL types when switch flag is set
*/
#define ENBOX_MOVE_IMMOBILE (1 << 0)
/*
set in the logic for SWITCH_FLAG_FALL types
otherwise unused
*/
#define ENBOX_MOVE_UNUSED (1 << 1)
/*
set with 50% chance on init for SWITCH_FLAG_FALL types
only used for SWITCH_FLAG_FALL types
ends up "blinking" (set/clear every frame) once switch flag is set,
if some collision-related condition (?) is met
only used for signum of z rotation
*/
#define ENBOX_MOVE_FALL_ANGLE_SIDE (1 << 2)
/*
when set, gets cleared next EnBox_Update call and clip to the floor
*/
#define ENBOX_MOVE_STICK_TO_GROUND (1 << 4)
#define ENBOX_MOVE_0x20 (1 << 5)
#define ENBOX_MOVE_0x40 (1 << 6)
#define ENBOX_MOVE_0x80 (1 << 7)

#define LOCATION_ENBOX ((0x060000) | (play->sceneId << 8) | ENBOX_GET_CHEST_FLAG(&this->dyna.actor))

#define THIS ((EnBox*)thisx)

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, TARGET_MODE_0, ICHAIN_STOP),
};

// Custom rando function based off of Actor_IsSmallChest
s32 EnBox_IsSmallChest(u8 type)
{
    if (type == ENBOX_TYPE_SMALL || type == ENBOX_TYPE_SMALL_INVISIBLE ||
        type == ENBOX_TYPE_SMALL_ROOM_CLEAR || type == ENBOX_TYPE_SMALL_SWITCH_FLAG_FALL ||
        type == ENBOX_TYPE_SMALL_SWITCH_FLAG) {
        return true;
    }
    return false;
}

void EnBox_SetupAction(EnBox* this, EnBoxActionFunc func);
void EnBox_Open(EnBox* this, PlayState* play);
void EnBox_WaitOpen(EnBox* this, PlayState* play);
void EnBox_FallOnSwitchFlag(EnBox* this, PlayState* play);
void EnBox_AppearSwitchFlag(EnBox* this, PlayState* play);
void EnBox_AppearOnRoomClear(EnBox* this, PlayState* play);
void func_80867BDC(struct_80867BDC_a0* arg0, PlayState* play, Vec3f* pos);

RECOMP_IMPORT(".", bool rando_get_camc_enabled());

RECOMP_PATCH void EnBox_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBox* this = THIS;
    s16 csId;
    CollisionHeader* colHeader;
    f32 startFrame;
    f32 endFrame;
    u8 vanillaType = ENBOX_GET_TYPE(&this->dyna.actor);

    colHeader = NULL;
    startFrame = 0.0f;
    endFrame = Animation_GetLastFrame(&gBoxChestOpenAnim);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gBoxChestCol, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->movementFlags = 0;

    if (rando_get_camc_enabled()) {
        switch (rando_get_location_type(LOCATION_ENBOX)) {
            case 0:
            case 2:
                this->type = ENBOX_TYPE_SMALL;
                break;
            default:
                this->type = ENBOX_TYPE_BIG;
                break;
        }
    } else {
        this->type = vanillaType;
    }

    this->iceSmokeTimer = 0;
    this->unk_1F3 = 0;
    this->dyna.actor.gravity = -5.5f;
    this->dyna.actor.terminalVelocity = -50.0f;
    this->switchFlag = ENBOX_GET_SWITCH_FLAG(&this->dyna.actor);
    this->dyna.actor.floorHeight = this->dyna.actor.world.pos.y;
    if (this->dyna.actor.world.rot.x == 180) {
        this->movementFlags |= ENBOX_MOVE_0x80;
        this->dyna.actor.world.rot.x = 0x7FFF;
        this->collectableFlag = 0;
    } else {
        DynaPoly_DisableCeilingCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        this->collectableFlag = (this->dyna.actor.world.rot.x & 0x7F);
        this->dyna.actor.world.rot.x = 0;
    }
    thisx->shape.rot.x = this->dyna.actor.world.rot.x;

    if (LOCATION_ENBOX == 0x061700) {
        if (ENBOX_GET_ITEM(&this->dyna.actor) == 0x0C) {
            if (rando_location_is_checked(LOCATION_ENBOX)) {
                this->getItemId = GI_RUPEE_HUGE;
            } else {
                this->getItemId = rando_get_item_id(LOCATION_ENBOX);
            }
        } else {
            this->getItemId = ENBOX_GET_ITEM(thisx);
        }
    } else if (rando_location_is_checked(LOCATION_ENBOX)) {
        Flags_SetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor));
    } else {
        this->getItemId = rando_get_item_id(LOCATION_ENBOX);
    }

    if (Flags_GetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor)) || this->getItemId == GI_NONE) {
        if (LOCATION_ENBOX == 0x060700) {
            Actor_Kill(&this->dyna.actor);
            return;
        }
        this->alpha = 255;
        this->iceSmokeTimer = 100;
        EnBox_SetupAction(this, EnBox_Open);
        this->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
        startFrame = endFrame;
    } else if (((vanillaType == ENBOX_TYPE_BIG_SWITCH_FLAG_FALL) || (vanillaType == ENBOX_TYPE_SMALL_SWITCH_FLAG_FALL)) &&
               !Flags_GetSwitch(play, this->switchFlag)) {
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        if (Rand_ZeroOne() < 0.5f) {
            this->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        this->unk_1A0 = -12;
        EnBox_SetupAction(this, EnBox_FallOnSwitchFlag);
        this->alpha = 0;
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.flags |= ACTOR_FLAG_10;
    } else if (((vanillaType == ENBOX_TYPE_BIG_ROOM_CLEAR) || (vanillaType == ENBOX_TYPE_SMALL_ROOM_CLEAR)) &&
               !Flags_GetClear(play, this->dyna.actor.room)) {
        EnBox_SetupAction(this, EnBox_AppearOnRoomClear);
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        if (this->movementFlags & ENBOX_MOVE_0x80) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 50.0f;
        } else {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        }
        this->alpha = 0;
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.flags |= ACTOR_FLAG_10;
    } else if ((vanillaType == ENBOX_TYPE_BIG_SONG_ZELDAS_LULLABY) || (vanillaType == ENBOX_TYPE_BIG_SONG_SUNS)) {

    } else if (((vanillaType == ENBOX_TYPE_BIG_SWITCH_FLAG) || (vanillaType == ENBOX_TYPE_SMALL_SWITCH_FLAG)) &&
               !Flags_GetSwitch(play, this->switchFlag)) {
        EnBox_SetupAction(this, EnBox_AppearSwitchFlag);
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        if (this->movementFlags & ENBOX_MOVE_0x80) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 50.0f;
        } else {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        }
        this->alpha = 0;
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.flags |= ACTOR_FLAG_10;
    } else {
        if ((vanillaType == ENBOX_TYPE_BIG_INVISIBLE) || (vanillaType == ENBOX_TYPE_SMALL_INVISIBLE)) {
            this->dyna.actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        }
        EnBox_SetupAction(this, EnBox_WaitOpen);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
    }

    if ((this->getItemId == GI_STRAY_FAIRY) && !Flags_GetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor))) {
        this->dyna.actor.flags |= ACTOR_FLAG_10;
    }

    this->dyna.actor.shape.rot.y += 0x8000;
    this->dyna.actor.home.rot.z = this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z = 0;

    SkelAnime_Init(play, &this->skelAnime, &gBoxChestSkel, &gBoxChestOpenAnim, this->jointTable, this->morphTable,
                   OBJECT_BOX_CHEST_LIMB_MAX);
    Animation_Change(&this->skelAnime, &gBoxChestOpenAnim, 1.5f, startFrame, endFrame, ANIMMODE_ONCE, 0.0f);
    if (EnBox_IsSmallChest(vanillaType)) {
        Actor_SetScale(&this->dyna.actor, 0.0075f);
        Actor_SetFocus(&this->dyna.actor, 20.0f);
    } else {
        Actor_SetScale(&this->dyna.actor, 0.01f);
        Actor_SetFocus(&this->dyna.actor, 40.0f);
    }

    this->csId1 = CS_ID_NONE;
    this->csId2 = CS_ID_NONE;
    csId = this->dyna.actor.csId;

    while (csId != CS_ID_NONE) {
        if (CutsceneManager_GetCutsceneCustomValue(csId) == 1) {
            this->csId2 = csId;
        } else {
            this->csId1 = csId;
        }
        csId = CutsceneManager_GetAdditionalCsId(csId);
    }
    func_80867BDC(&this->unk_1F4, play, &this->dyna.actor.home.pos);
}

extern AnimationHeader gBoxBigChestOpenAdultAnim;
extern AnimationHeader gBoxBigChestOpenGoronAnim;
extern AnimationHeader gBoxBigChestOpenDekuAnim;
extern AnimationHeader gBoxBigChestOpenChildAnim;

RECOMP_PATCH void EnBox_WaitOpen(EnBox* this, PlayState* play) {
    static AnimationHeader* sBigChestAnimations[PLAYER_FORM_MAX] = {
        &gBoxBigChestOpenAdultAnim, // PLAYER_FORM_FIERCE_DEITY
        &gBoxBigChestOpenGoronAnim, // PLAYER_FORM_GORON
        &gBoxBigChestOpenAdultAnim, // PLAYER_FORM_ZORA
        &gBoxBigChestOpenDekuAnim,  // PLAYER_FORM_DEKU
        &gBoxBigChestOpenChildAnim, // PLAYER_FORM_HUMAN
    };
    s32 pad;
    AnimationHeader* anim;
    f32 endFrame;
    f32 playSpeed;

    this->alpha = 255;
    this->movementFlags |= ENBOX_MOVE_IMMOBILE;
    if ((this->unk_1EC != 0) && ((this->csId2 < 0) || (CutsceneManager_GetCurrentCsId() == this->csId2) ||
                                 (CutsceneManager_GetCurrentCsId() == CS_ID_NONE))) {
        if (this->unk_1EC < 0) {
            anim = &gBoxChestOpenAnim;
            playSpeed = 1.5f;
        } else {
            f32 sPlaybackSpeeds[PLAYER_FORM_MAX] = {
                1.5f, // PLAYER_FORM_FIERCE_DEITY
                1.0f, // PLAYER_FORM_GORON
                1.5f, // PLAYER_FORM_ZORA
                1.0f, // PLAYER_FORM_DEKU
                1.5f, // PLAYER_FORM_HUMAN
            };

            anim = sBigChestAnimations[GET_PLAYER_FORM];
            playSpeed = sPlaybackSpeeds[GET_PLAYER_FORM];
        }

        endFrame = Animation_GetLastFrame(anim);
        Animation_Change(&this->skelAnime, anim, playSpeed, 0.0f, endFrame, ANIMMODE_ONCE, 0.0f);

        // Account for Treasure Game chest (always same location)
        if (LOCATION_ENBOX != 0x061700 || ENBOX_GET_ITEM(&this->dyna.actor) == 0x0C) {
            rando_send_location(LOCATION_ENBOX);
            recomp_printf("box location: 0x%06X, box item: 0x%06X\n", LOCATION_ENBOX, ENBOX_GET_ITEM((Actor*) this));
        }
        EnBox_SetupAction(this, EnBox_Open);
        if (this->unk_1EC > 0) {
            Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_DEMO_TRE_LGT,
                               this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                               this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z,
                               -1);
            Audio_PlayFanfare(NA_BGM_OPEN_CHEST | 0x900);
        }

        if (this->getItemId == GI_STRAY_FAIRY) {
            this->movementFlags |= ENBOX_MOVE_0x20;
        } else {
            if ((this->getItemId == GI_HEART_PIECE) || (this->getItemId == GI_BOTTLE)) {
                Flags_SetCollectible(play, this->collectableFlag);
            }
            Flags_SetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor));
        }
    } else {
        Player* player = GET_PLAYER(play);
        Vec3f offset;

        Actor_OffsetOfPointInActorCoords(&this->dyna.actor, &offset, &player->actor.world.pos);
        if ((offset.z > -50.0f) && (offset.z < 0.0f) && (fabsf(offset.y) < 10.0f) && (fabsf(offset.x) < 20.0f) &&
            Player_IsFacingActor(&this->dyna.actor, 0x3000, play)) {
            if (((this->getItemId == GI_HEART_PIECE) || (this->getItemId == GI_BOTTLE)) &&
                Flags_GetCollectible(play, this->collectableFlag)) {
                this->getItemId = GI_RECOVERY_HEART;
            }
            if ((this->getItemId == GI_MASK_CAPTAIN) && (INV_CONTENT(ITEM_MASK_CAPTAIN) == ITEM_MASK_CAPTAIN)) {
                this->getItemId = GI_RECOVERY_HEART;
            }
            if ((this->getItemId == GI_MASK_GIANT) && (INV_CONTENT(ITEM_MASK_GIANT) == ITEM_MASK_GIANT)) {
                this->getItemId = GI_RECOVERY_HEART;
            }

            if (LOCATION_ENBOX == 0x061700) {
                if (ENBOX_GET_ITEM(&this->dyna.actor) == 0x0C && !rando_location_is_checked(LOCATION_ENBOX)) {
                    Actor_OfferGetItemHook(&this->dyna.actor, play, -this->getItemId, 0, 50.0f, 10.0f, false, true);
                } else {
                    Actor_OfferGetItemHook(&this->dyna.actor, play, -this->getItemId, 0, 50.0f, 10.0f, false, false);
                }
            } else {
                Actor_OfferGetItemHook(&this->dyna.actor, play, -this->getItemId, LOCATION_ENBOX, 50.0f, 10.0f, false, true);
            }
        }
        if (Flags_GetTreasure(play, ENBOX_GET_CHEST_FLAG(&this->dyna.actor))) {
            EnBox_SetupAction(this, EnBox_Open);
        }
    }
}

extern Gfx gBoxChestBaseOrnateDL[];
extern Gfx gBoxChestBaseGildedDL[];
extern Gfx gBoxChestBaseDL[];

extern Gfx gBoxChestLidOrnateDL[];
extern Gfx gBoxChestLidGildedDL[];
extern Gfx gBoxChestLidDL[];

RECOMP_PATCH void EnBox_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    s32 pad;
    EnBox* this = THIS;

    if (limbIndex == OBJECT_BOX_CHEST_LIMB_01) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (rando_get_camc_enabled()) {
            switch (rando_get_item_id(LOCATION_ENBOX)) {
                case GI_AP_FILLER:
                    gSPDisplayList((*gfx)++, &apJunkChestBaseDL);
                    return;
                case GI_AP_USEFUL:
                    gSPDisplayList((*gfx)++, &apUsefulChestBaseDL);
                    return;
                case GI_AP_PROG:
                    gSPDisplayList((*gfx)++, &apProgChestBaseDL);
                    return;
                case GI_KEY_SMALL:
                    gSPDisplayList((*gfx)++, &keyChestBaseDL);
                    return;
                case GI_B2:
                    gSPDisplayList((*gfx)++, &fairyChestBaseDL);
                    return;
                case GI_HEART_PIECE:
                case GI_HEART_CONTAINER:
                    gSPDisplayList((*gfx)++, &heartChestBaseDL);
                    return;
                case GI_TRUE_SKULL_TOKEN:
                case GI_OCEAN_SKULL_TOKEN:
                    gSPDisplayList((*gfx)++, &spiderChestBaseDL);
                    return;
                case GI_KEY_BOSS:
                    gSPDisplayList((*gfx)++, &gBoxChestBaseOrnateDL);
                    return;
            }
        }
        if (this->type == ENBOX_TYPE_SMALL) {
            gSPDisplayList((*gfx)++, &gBoxChestBaseDL);
        } else {
            gSPDisplayList((*gfx)++, &gBoxChestBaseGildedDL);
        }
    } else if (limbIndex == OBJECT_BOX_CHEST_LIMB_03) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (rando_get_camc_enabled()) {
            switch (rando_get_item_id(LOCATION_ENBOX)) {
                case GI_AP_FILLER:
                    gSPDisplayList((*gfx)++, &apJunkChestLidDL);
                    return;
                case GI_AP_USEFUL:
                    gSPDisplayList((*gfx)++, &apUsefulChestLidDL);
                    return;
                case GI_AP_PROG:
                    gSPDisplayList((*gfx)++, &apProgChestLidDL);
                    return;
                case GI_KEY_SMALL:
                    gSPDisplayList((*gfx)++, &keyChestLidDL);
                    return;
                case GI_B2:
                    gSPDisplayList((*gfx)++, &fairyChestLidDL);
                    return;
                case GI_HEART_PIECE:
                case GI_HEART_CONTAINER:
                    gSPDisplayList((*gfx)++, &heartChestLidDL);
                    return;
                case GI_TRUE_SKULL_TOKEN:
                case GI_OCEAN_SKULL_TOKEN:
                    gSPDisplayList((*gfx)++, &spiderChestLidDL);
                    return;
                case GI_KEY_BOSS:
                    gSPDisplayList((*gfx)++, &gBoxChestLidOrnateDL);
                    return;
            }
        }
        if (this->type == ENBOX_TYPE_SMALL) {
            gSPDisplayList((*gfx)++, &gBoxChestLidDL);
        } else {
            gSPDisplayList((*gfx)++, &gBoxChestLidGildedDL);
        }
    }
}