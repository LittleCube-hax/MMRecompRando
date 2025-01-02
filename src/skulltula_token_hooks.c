#include "modding.h"
#include "global.h"

#include "apcommon.h"

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));

#define ENSI_GET_CHEST_FLAG(thisx) (((thisx)->params & 0xFC) >> 2)

#define LOCATION_SKULL_TOKEN (0x060000 | (play->sceneId << 8) | ENSI_GET_CHEST_FLAG(&this->actor))
#define GET_GI_TOKEN ((play->sceneId == SCENE_KINSTA1) ? GET_GI_SWAMP_TOKEN : GET_GI_OCEAN_TOKEN)
#define GET_GI_SWAMP_TOKEN ((rando_skulltulas_enabled()) ? rando_get_item_id(LOCATION_SKULL_TOKEN) : GI_TRUE_SKULL_TOKEN)
#define GET_GI_OCEAN_TOKEN ((rando_skulltulas_enabled()) ? rando_get_item_id(LOCATION_SKULL_TOKEN) : GI_OCEAN_SKULL_TOKEN)

#define SPIDER_HOUSE_TOKENS_REQUIRED 30

struct EnSi;

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_200)

#define THIS ((EnSi*)thisx)

typedef void (*EnSiActionFunc)(struct EnSi*, PlayState*);

typedef struct EnSi {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnSiActionFunc actionFunc;
    /* 0x148 */ ColliderSphere collider;
} EnSi; // size = 0x1A0

static ColliderSphereInit sSphereInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEMTYPE_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 10 }, 100 },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static u32 tokenPrevGI[30];
static bool tokenPickedUp[30];
static bool tokenObjectStatic[30];
static bool tokenObjectLoading[30];
static bool tokenObjectLoaded[30];
static OSMesgQueue tokenObjectLoadQueue[30];
static void* tokenObjectSegments[30];

void EnSi_TokenCollected(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        player->actor.freezeTimer = 0;
        player->stateFlags1 &= ~PLAYER_STATE1_20000000;
        Actor_Kill(&this->actor);
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        return;
    } else {
        player->actor.freezeTimer = 10;
        player->stateFlags1 |= PLAYER_STATE1_20000000;
    }

    if (CutsceneManager_GetCurrentCsId() != CS_ID_GLOBAL_TALK) {
        if (CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)) {
            CutsceneManager_Start(CS_ID_GLOBAL_TALK, &this->actor);
        } else {
            CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        }
    }
}

RECOMP_PATCH void EnSi_GiveToken(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 chestFlag = ENSI_GET_CHEST_FLAG(&this->actor);
    u8 swamp_token_count = (rando_skulltulas_enabled() ? rando_has_item(GI_TRUE_SKULL_TOKEN) : Inventory_GetSkullTokenCount(0x27));
    u8 ocean_token_count = (rando_skulltulas_enabled() ? rando_has_item(GI_OCEAN_SKULL_TOKEN) : Inventory_GetSkullTokenCount(0x28));

    if ((chestFlag < 0x20) && (chestFlag >= 0)) {
        Flags_SetTreasure(play, chestFlag);
    }

    tokenPickedUp[chestFlag] = true;

    recomp_printf("token location: 0x%06X\n", LOCATION_SKULL_TOKEN);
    rando_send_location(LOCATION_SKULL_TOKEN);
    if (!rando_skulltulas_enabled()) {
        randoItemGive(GET_GI_TOKEN);
    }

    if (tokenPrevGI[chestFlag] == GI_TRUE_SKULL_TOKEN) {
        while (swamp_token_count == (rando_skulltulas_enabled() ? rando_has_item(GI_TRUE_SKULL_TOKEN) : Inventory_GetSkullTokenCount(0x27)));
    }
    if (tokenPrevGI[chestFlag] == GI_OCEAN_SKULL_TOKEN) {
        while (ocean_token_count == (rando_skulltulas_enabled() ? rando_has_item(GI_OCEAN_SKULL_TOKEN) : Inventory_GetSkullTokenCount(0x28)));
    }

    Message_StartTextbox(play, getTextId(tokenPrevGI[chestFlag]), NULL);
    if ((tokenPrevGI[chestFlag] == GI_TRUE_SKULL_TOKEN && Inventory_GetSkullTokenCount(0x27) >= SPIDER_HOUSE_TOKENS_REQUIRED)
        || (tokenPrevGI[chestFlag] == GI_OCEAN_SKULL_TOKEN && Inventory_GetSkullTokenCount(0x28) >= SPIDER_HOUSE_TOKENS_REQUIRED)) {
        Audio_PlayFanfare(NA_BGM_GET_ITEM | 0x900);
    } else {
        Audio_PlayFanfare(NA_BGM_GET_SMALL_ITEM);
    }

    this->actionFunc = EnSi_TokenCollected;
}

RECOMP_PATCH void EnSi_DraggedByHookshot(EnSi* this, PlayState* play) {
    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_2000)) {
        EnSi_GiveToken(this, play);
        Actor_SetScale(&this->actor, 0.0f);
    }
}

RECOMP_PATCH void EnSi_Wait(EnSi* this, PlayState* play) {
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_2000)) {
        this->actionFunc = EnSi_DraggedByHookshot;
    } else if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        EnSi_GiveToken(this, play);
        Actor_SetScale(&this->actor, 0.0f);
        return;
    }
    this->actor.shape.rot.y += 0x38E;
}

void EnSi_LoadObject(EnSi* this, PlayState* play) {
    s32 chestFlag = ENSI_GET_CHEST_FLAG(&this->actor);
    s16 objectSlot = Object_GetSlot(&play->objectCtx, getObjectId(GET_GI_TOKEN));
    if (isAP(GET_GI_TOKEN)) {
        tokenObjectStatic[chestFlag] = true;
        tokenObjectLoaded[chestFlag] = true;
        tokenPrevGI[chestFlag] = GET_GI_TOKEN;
        this->actionFunc = EnSi_Wait;
    } else if (!tokenObjectLoaded[chestFlag] && !tokenObjectLoading[chestFlag] && Object_IsLoaded(&play->objectCtx, objectSlot)) {
        this->actor.objectSlot = objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        tokenObjectStatic[chestFlag] = true;
        tokenObjectLoaded[chestFlag] = true;
        tokenPrevGI[chestFlag] = GET_GI_TOKEN;
        this->actionFunc = EnSi_Wait;
    } else if (!tokenObjectLoading[chestFlag] && !tokenObjectLoaded[chestFlag]) {
        loadObject(play, &tokenObjectSegments[chestFlag], &tokenObjectLoadQueue[chestFlag], getObjectId(GET_GI_TOKEN));
        tokenObjectLoading[chestFlag] = true;
    } else if (osRecvMesg(&tokenObjectLoadQueue[chestFlag], NULL, OS_MESG_NOBLOCK) == 0) {
        tokenObjectLoading[chestFlag] = false;
        tokenObjectLoaded[chestFlag] = true;
        tokenPrevGI[chestFlag] = GET_GI_TOKEN;
        this->actionFunc = EnSi_Wait;
    }
}

RECOMP_PATCH void EnSi_Init(Actor* thisx, PlayState* play) {
    EnSi* this = THIS;
    s32 chestFlag = ENSI_GET_CHEST_FLAG(&this->actor);

    if (rando_skulltulas_enabled() && rando_location_is_checked(LOCATION_SKULL_TOKEN)) {
        Actor_Kill(thisx);
        return;
    }

    tokenPickedUp[chestFlag] = false;
    tokenObjectStatic[chestFlag] = false;
    tokenObjectLoading[chestFlag] = false;
    tokenObjectLoaded[chestFlag] = false;

    Collider_InitSphere(play, &this->collider);
    Collider_SetSphere(play, &this->collider, &this->actor, &sSphereInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.25f);
    this->actionFunc = EnSi_LoadObject;
}

RECOMP_PATCH void EnSi_Draw(Actor* thisx, PlayState* play) {
    EnSi* this = THIS;
    s32 chestFlag = ENSI_GET_CHEST_FLAG(&this->actor);

    if (tokenPickedUp[chestFlag]) {
        return;
    }

    if (GET_GI_TOKEN != tokenPrevGI[chestFlag] && tokenObjectLoaded[chestFlag]) {
        tokenObjectStatic[chestFlag] = false;
        tokenObjectLoading[chestFlag] = false;
        tokenObjectLoaded[chestFlag] = false;
        this->actionFunc = EnSi_LoadObject;
    }

    func_800B8118(&this->actor, play, 0);
    func_800B8050(&this->actor, play, 0);
    if (tokenObjectLoaded[ENSI_GET_CHEST_FLAG(thisx)]) {
        if (tokenObjectStatic[ENSI_GET_CHEST_FLAG(thisx)]) {
            GetItem_Draw(play, getGid(GET_GI_TOKEN));
        } else {
            GetItem_DrawDynamic(play, tokenObjectSegments[ENSI_GET_CHEST_FLAG(thisx)], getGid(GET_GI_TOKEN));
        }
    }
}