#ifndef __APCOMMON_H__
#define __APCOMMON_H__

#include "modding.h"
#include "global.h"

#define GI_TRUE_SKULL_TOKEN GI_75

#define AP_ITEM_ID_STRAY_FAIRY_WOODFALL 0x010000

#define GI_AP_PROG GI_77
#define GI_AP_FILLER GI_90
#define GI_AP_USEFUL GI_B3

#define GID_SONG_SONATA (GID_MASK_FIERCE_DEITY + 1)
#define GID_SONG_LULLABY (GID_MASK_FIERCE_DEITY + 2)
#define GID_SONG_NOVA (GID_MASK_FIERCE_DEITY + 3)
#define GID_SONG_ELEGY (GID_MASK_FIERCE_DEITY + 4)
#define GID_SONG_OATH (GID_MASK_FIERCE_DEITY + 5)

#define GID_SONG_TIME (GID_MASK_FIERCE_DEITY + 6)
#define GID_SONG_HEALING (GID_MASK_FIERCE_DEITY + 7)
#define GID_SONG_EPONA (GID_MASK_FIERCE_DEITY + 8)
#define GID_SONG_SOARING (GID_MASK_FIERCE_DEITY + 9)
#define GID_SONG_STORMS (GID_MASK_FIERCE_DEITY + 10)

#define GID_APLOGO_FILLER GID_37
#define GID_APLOGO_PROG GID_46
#define GID_APLOGO_USEFUL GID_4C

#define INV_HAS(x) (INV_CONTENT(x) == x)

#define LOCATION_GRANNY_STORY_1 0x070243
#define LOCATION_GRANNY_STORY_2 0x080243
#define LOCATION_PLAYGROUND_ANY_DAY 0x0801C9
#define LOCATION_PLAYGROUND_ALL_DAYS 0x0701C9
#define LOCATION_HONEY_AND_DARLING_ANY_DAY 0x0800B5
#define LOCATION_HONEY_AND_DARLING_ALL_DAYS 0x0700B5
#define LOCATION_BANK_500_REWARD 0x080177
#define LOCATION_BANK_1000_REWARD 0x070177
#define LOCATION_LOTTERY_SHOP 0x070088
#define LOCATION_TOWN_GALLERY_PERFECT 0x07011D
#define LOCATION_SWAMP_GALLERY_PERFECT 0x08011D
#define LOCATION_BEAN_DADDY 0x0800A5

RECOMP_IMPORT(".", bool rando_location_is_checked(u32 location_id));
RECOMP_IMPORT(".", bool rando_get_death_link_pending());
RECOMP_IMPORT(".", bool rando_reset_death_link_pending());
RECOMP_IMPORT(".", bool rando_get_death_link_enabled());
RECOMP_IMPORT(".", bool rando_send_death_link());
RECOMP_IMPORT(".", bool rando_skulltulas_enabled());
RECOMP_IMPORT(".", bool rando_get_location_type(u32 location_id));
RECOMP_IMPORT(".", bool rando_get_item_id(u32 location_id));
RECOMP_IMPORT(".", void rando_send_location(u32 location_id));
RECOMP_IMPORT(".", void rando_complete_goal());
RECOMP_IMPORT(".", u32 rando_has_item(u32 item_id));
RECOMP_IMPORT(".", u32 rando_get_items_size());
RECOMP_IMPORT(".", u32 rando_get_item(u32 items_i));

// WEEKEVENTREG_17_80: we have given the Moon's Tear to the scrub
// WEEKEVENTREG_74_80: the Moon's Tear has fallen
// WEEKEVENTREG_74_40: we have collected the Moon's Tear

extern GetItemId noShuffleList[];

extern PlayState* gPlay;
extern bool saveOpened;
extern bool playing;

extern bool justDied;

bool loadObject(PlayState* play, void** objectSegment, OSMesgQueue* objectLoadQueue, s16 objectId);
void GetItem_DrawDynamic(PlayState* play, void* objectSegment, s16 objectId);
s32 Actor_OfferGetItemHook(Actor* actor, PlayState* play, GetItemId getItemId, u32 location, f32 xzRange, f32 yRange, bool use_workaround, bool item_is_shuffled);

u8 randoItemGive(u32 gi);

typedef struct GetItemEntry {
    /* 0x0 */ u8 itemId;
    /* 0x1 */ u8 field; // various bit-packed data
    /* 0x2 */ s8 gid;   // defines the draw id and chest opening animation
    /* 0x3 */ u8 textId;
    /* 0x4 */ u16 objectId;
} GetItemEntry; // size = 0x6

bool isAP(s16 gi);

u16 getObjectId(s16 gi);

s8 getGid(s16 gi);

u8 getTextId(s16 gi);

extern s8 giToItemId[];

#endif