#include "modding.h"
#include "z64save.h"
#include "z64horse.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"

#include "apcommon.h"

bool saveOpened = false;

RECOMP_CALLBACK("*", recomp_on_load_save)
void rando_on_load_save(FileSelectState* fileSelect, SramContext* sramCtx) {
    saveOpened = true;
}

extern SavePlayerData sSaveDefaultPlayerData;
extern ItemEquips sSaveDefaultItemEquips;
extern Inventory sSaveDefaultInventory;

extern Inventory sSaveDebugInventory;

void Sram_ClearHighscores(void);
void Sram_GenerateRandomSaveFields(void);
void Sram_ResetSave(void);

bool drankChateau = false;

void Sram_SetInitialWeekEvents(void) {
    SET_WEEKEVENTREG(WEEKEVENTREG_15_20);
    SET_WEEKEVENTREG(WEEKEVENTREG_59_04);
    SET_WEEKEVENTREG(WEEKEVENTREG_31_04);

    // Guards let you out of Clock Town
    SET_WEEKEVENTREG(WEEKEVENTREG_12_20);

    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_EAST_CLOCK_TOWN);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WEST_CLOCK_TOWN);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_NORTH_CLOCK_TOWN);

    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_TERMINA_FIELD);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_IKANA_GRAVEYARD);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_ROMANI_RANCH);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_GORMAN_TRACK);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_MOUNTAIN_VILLAGE_WINTER);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_GORON_SHRINE);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_SNOWHEAD);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_SOUTHERN_SWAMP_POISONED);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_DEKU_PALACE);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_GREAT_BAY_COAST);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_PIRATES_FORTRESS);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_ZORA_HALL);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WATERFALL_RAPIDS);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_IKANA_CANYON);
    // Attached to the scene but unused. Entrance cutscene is instead triggered by `ACTOR_OBJ_DEMO`
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_IKANA_CASTLE);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_STONE_TOWER);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_STONE_TOWER_INVERTED);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_EAST_CLOCK_TOWN);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WEST_CLOCK_TOWN);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_NORTH_CLOCK_TOWN);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_SNOWHEAD_TEMPLE);
    // Attached to the scene but unused. Entrance cutscene is instead triggered by `ACTOR_OBJ_DEMO`
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_PIRATES_FORTRESS_EXTERIOR);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_STONE_TOWER_TEMPLE);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_STONE_TOWER_TEMPLE_INVERTED);
    // Unused as no cutscene is attached to this script
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_THE_MOON);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_MOON_DEKU_TRIAL);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_MOON_GORON_TRIAL);
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_MOON_ZORA_TRIAL);

    // skippable mask transformations
    SET_WEEKEVENTREG(PACK_WEEKEVENTREG_FLAG(30, 0x10));
    SET_WEEKEVENTREG(PACK_WEEKEVENTREG_FLAG(30, 0x20));
    SET_WEEKEVENTREG(PACK_WEEKEVENTREG_FLAG(30, 0x40));
    SET_WEEKEVENTREG(PACK_WEEKEVENTREG_FLAG(30, 0x80));

    // moon's tear deku scrub starts out in flower
    SET_WEEKEVENTREG(PACK_WEEKEVENTREG_FLAG(73, 0x04));

    // skip the princess prison cutscene
    SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE_PRISON);

    // restore chateau romani state after cycle reset
    if(drankChateau) {
        SET_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI);
        drankChateau = false;
    }
}

RECOMP_PATCH void Sram_InitDebugSave(void) {
    PlayState* play = gPlay;
    Sram_ResetSave();

    Lib_MemCpy(&gSaveContext.save.saveInfo.playerData, &sSaveDefaultPlayerData, sizeof(SavePlayerData));
    Lib_MemCpy(&gSaveContext.save.saveInfo.equips, &sSaveDefaultItemEquips, sizeof(ItemEquips));
    Lib_MemCpy(&gSaveContext.save.saveInfo.inventory, &sSaveDefaultInventory, sizeof(Inventory));
    gSaveContext.save.saveInfo.checksum = 0;
    
    gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;

    gSaveContext.save.hasTatl = true;

    // TODO: move below to YAML options
    // start with basic consumables
    gSaveContext.save.saveInfo.playerData.rupees = 99;
    gSaveContext.save.saveInfo.inventory.items[SLOT_DEKU_STICK] = ITEM_DEKU_STICK;
    gSaveContext.save.saveInfo.inventory.ammo[SLOT_DEKU_STICK] = 10;
    gSaveContext.save.saveInfo.inventory.items[SLOT_DEKU_NUT] = ITEM_DEKU_NUT;
    gSaveContext.save.saveInfo.inventory.ammo[SLOT_DEKU_NUT] = 20;

    gSaveContext.save.timeSpeedOffset = -2;

    // TODO: hide song on quest screen + add playback text
    SET_QUEST_ITEM(ITEM_SONG_SUN + QUEST_SONG_SUN);

    gSaveContext.save.saveInfo.horseData.sceneId = SCENE_F01;
    gSaveContext.save.saveInfo.horseData.pos.x = -1420;
    gSaveContext.save.saveInfo.horseData.pos.y = 257;
    gSaveContext.save.saveInfo.horseData.pos.z = -1285;
    gSaveContext.save.saveInfo.horseData.yaw = -0x7554;

    gSaveContext.save.isFirstCycle = true;

    Sram_SetInitialWeekEvents();

    // skip the *insanely long* skull kid tatl/tael backstory cutscene
    u8* save_ptr = (u8*) &gSaveContext;
    save_ptr[0x5EB] |= 0x10;
    save_ptr[0x42F3] |= 0x10;

    gSaveContext.cycleSceneFlags[SCENE_INSIDETOWER].switch0 = 1;
    gSaveContext.save.saveInfo.permanentSceneFlags[SCENE_INSIDETOWER].switch0 = 1;

    gSaveContext.save.saveInfo.playerData.healthCapacity = 0x30;
    gSaveContext.save.saveInfo.playerData.health = 0x30;

    Sram_GenerateRandomSaveFields();

    gSaveContext.save.saveInfo.playerData.threeDayResetCount = 1;
}

RECOMP_PATCH void Sram_InitSave(FileSelectState* fileSelect2, SramContext* sramCtx) {
    s32 phi_v0;
    u16 i;
    FileSelectState* fileSelect = fileSelect2;
    s16 maskCount;

    if (gSaveContext.flashSaveAvailable) {
        Sram_InitDebugSave();
        if (fileSelect->buttonIndex == 0) {
            //gSaveContext.save.cutsceneIndex = 0xFFF0;
        }

        for (phi_v0 = 0; phi_v0 < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); phi_v0++) {
            gSaveContext.save.saveInfo.playerData.playerName[phi_v0] =
                fileSelect->fileNames[fileSelect->buttonIndex][phi_v0];
        }

        gSaveContext.save.saveInfo.playerData.newf[0] = 'Z';
        gSaveContext.save.saveInfo.playerData.newf[1] = 'E';
        gSaveContext.save.saveInfo.playerData.newf[2] = 'L';
        gSaveContext.save.saveInfo.playerData.newf[3] = 'D';
        gSaveContext.save.saveInfo.playerData.newf[4] = 'A';
        gSaveContext.save.saveInfo.playerData.newf[5] = '3';

        gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext.save, sizeof(Save));

        Lib_MemCpy(sramCtx->saveBuf, &gSaveContext.save, sizeof(Save));
        Lib_MemCpy(&sramCtx->saveBuf[0x2000], &gSaveContext.save, sizeof(Save));

        for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.newf); i++) {
            fileSelect->newf[fileSelect->buttonIndex][i] = gSaveContext.save.saveInfo.playerData.newf[i];
        }

        fileSelect->threeDayResetCount[fileSelect->buttonIndex] =
            gSaveContext.save.saveInfo.playerData.threeDayResetCount;

        for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); i++) {
            fileSelect->fileNames[fileSelect->buttonIndex][i] = gSaveContext.save.saveInfo.playerData.playerName[i];
        }

        fileSelect->healthCapacity[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.playerData.healthCapacity;
        fileSelect->health[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.playerData.health;
        fileSelect->defenseHearts[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.inventory.defenseHearts;
        fileSelect->questItems[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.inventory.questItems;
        fileSelect->time[fileSelect->buttonIndex] = gSaveContext.save.time;
        fileSelect->day[fileSelect->buttonIndex] = gSaveContext.save.day;
        fileSelect->isOwlSave[fileSelect->buttonIndex] = gSaveContext.save.isOwlSave;
        fileSelect->rupees[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.playerData.rupees;
        fileSelect->walletUpgrades[fileSelect->buttonIndex] = CUR_UPG_VALUE(UPG_WALLET);

        for (i = 0, maskCount = 0; i < 24; i++) {
            if (gSaveContext.save.saveInfo.inventory.items[i + 24] != ITEM_NONE) {
                maskCount++;
            }
        }

        fileSelect->maskCount[fileSelect->buttonIndex] = maskCount;
        fileSelect->heartPieceCount[fileSelect->buttonIndex] = GET_QUEST_HEART_PIECE_COUNT;
    }

    gSaveContext.save.time = *((uintptr_t*) 0x801F6AF0);
    gSaveContext.flashSaveAvailable = *((uintptr_t*) 0x801F6AF2);
}

extern u16 sPersistentCycleWeekEventRegs[ARRAY_COUNT(gSaveContext.save.saveInfo.weekEventReg)];
extern u8 sBitFlags8[8];
extern u8 gAmmoItems[24];

void Sram_ClearHighscores(void);
void Sram_SetInitialWeekEvents(void);

typedef struct PersistentCycleSceneFlags {
    /* 0x0 */ u32 switch0;
    /* 0x4 */ u32 switch1;
    /* 0x8 */ u32 chest;
    /* 0xC */ u32 collectible;
} PersistentCycleSceneFlags; // size = 0x10

#define PERSISTENT_CYCLE_FLAGS_SET(switch0, switch1, chest, collectible) { switch0, switch1, chest, collectible },
#define PERSISTENT_CYCLE_FLAGS_NONE PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, 0)

#define DEFINE_SCENE(_name, _enumValue, _textId, _drawConfig, _restrictionFlags, persistentCycleFlags) \
    persistentCycleFlags
#define DEFINE_SCENE_UNSET(_enumValue) PERSISTENT_CYCLE_FLAGS_NONE

PersistentCycleSceneFlags sPersistentCycleSceneFlags[SCENE_MAX] = {
#include "tables/scene_table.h"
};

void Sram_ResetSaveCycle(PlayState* play) {
    u8 slot;
    s32 j;
    s32 i;

    if (gSaveContext.save.saveInfo.playerData.health < 0x30) {
        gSaveContext.save.saveInfo.playerData.health = 0x30;
    }

    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) <= EQUIP_VALUE_SWORD_RAZOR) {
        //SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);

        u8 sword_level = rando_has_item(GI_SWORD_KOKIRI);
        if (gSaveContext.save.playerForm == PLAYER_FORM_HUMAN) {
            if ((STOLEN_ITEM_1 <= ITEM_SWORD_GILDED && STOLEN_ITEM_1 >= ITEM_SWORD_KOKIRI) || (STOLEN_ITEM_2 <= ITEM_SWORD_GILDED && STOLEN_ITEM_2 >= ITEM_SWORD_KOKIRI)) {
                CUR_FORM_EQUIP(EQUIP_SLOT_B) = ITEM_SWORD_KOKIRI + sword_level - 1;
            }
        } else {
            if ((STOLEN_ITEM_1 <= ITEM_SWORD_GILDED && STOLEN_ITEM_1 >= ITEM_SWORD_KOKIRI) || (STOLEN_ITEM_2 <= ITEM_SWORD_GILDED && STOLEN_ITEM_2 >= ITEM_SWORD_KOKIRI)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_KOKIRI + sword_level - 1;
            }
        }
        SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI + sword_level - 1);
    }

    if ((STOLEN_ITEM_1 == ITEM_SWORD_GREAT_FAIRY) || (STOLEN_ITEM_2 == ITEM_SWORD_GREAT_FAIRY)) {
        INV_CONTENT(ITEM_SWORD_GREAT_FAIRY) = ITEM_SWORD_GREAT_FAIRY;
    }

    if (STOLEN_ITEM_1 == ITEM_BOTTLE) {
        slot = SLOT(ITEM_BOTTLE);
        for (i = BOTTLE_FIRST; i < BOTTLE_MAX; i++) {
            if (gSaveContext.save.saveInfo.inventory.items[slot + i] == ITEM_NONE) {
                gSaveContext.save.saveInfo.inventory.items[slot + i] = ITEM_BOTTLE;
                break;
            }
        }
    }

    if (STOLEN_ITEM_2 == ITEM_BOTTLE) {
        slot = SLOT(ITEM_BOTTLE);
        for (i = BOTTLE_FIRST; i < BOTTLE_MAX; i++) {
            if (gSaveContext.save.saveInfo.inventory.items[slot + i] == ITEM_NONE) {
                gSaveContext.save.saveInfo.inventory.items[slot + i] = ITEM_BOTTLE;
                break;
            }
        }
    }

    SET_STOLEN_ITEM_1(STOLEN_ITEM_NONE);
    SET_STOLEN_ITEM_2(STOLEN_ITEM_NONE);

    for (i = SLOT_BOTTLE_1; i <= SLOT_BOTTLE_6; i++) {
        // Check for Deku Princess
        if (gSaveContext.save.saveInfo.inventory.items[i] == ITEM_DEKU_PRINCESS) {
            for (j = EQUIP_SLOT_C_LEFT; j <= EQUIP_SLOT_C_RIGHT; j++) {
                if (GET_CUR_FORM_BTN_ITEM(j) == gSaveContext.save.saveInfo.inventory.items[i]) {
                    SET_CUR_FORM_BTN_ITEM(j, ITEM_BOTTLE);
                    Interface_LoadItemIconImpl(play, j);
                }
            }
            gSaveContext.save.saveInfo.inventory.items[i] = ITEM_BOTTLE;
        }
    }

    Horse_ResetHorseData(play);

    if (rando_has_item(0x01007F)) {
        randoItemGive(0x01007F);
    }

    //recomp_save_items_index(gSaveContext.fileNum, old_items_size);

    DUNGEON_KEY_COUNT(0) = rando_has_item(0x090078);
}

/**
 * Used by Song of Time (when clicking "Yes") and (indirectly) by the "Dawn of the New Day" cutscene
 */
RECOMP_PATCH void Sram_SaveEndOfCycle(PlayState* play) {
    s16 sceneId;
    s32 j;
    s32 i;
    u8 item;

    // gSaveContext.save.timeSpeedOffset = 0;
    gSaveContext.save.eventDayCount = 0;
    gSaveContext.save.day = 0;
    gSaveContext.save.time = CLOCK_TIME(6, 0) - 1;

    gSaveContext.save.saveInfo.playerData.threeDayResetCount++;
    if (gSaveContext.save.saveInfo.playerData.threeDayResetCount > 999) {
        gSaveContext.save.saveInfo.playerData.threeDayResetCount = 999;
    }

    sceneId = Play_GetOriginalSceneId(play->sceneId);
    Play_SaveCycleSceneFlags(&play->state);

    play->actorCtx.sceneFlags.chest &= sPersistentCycleSceneFlags[sceneId].chest;
    play->actorCtx.sceneFlags.switches[0] &= sPersistentCycleSceneFlags[sceneId].switch0;
    play->actorCtx.sceneFlags.switches[1] &= sPersistentCycleSceneFlags[sceneId].switch1;
    play->actorCtx.sceneFlags.collectible[0] &= sPersistentCycleSceneFlags[sceneId].collectible;
    play->actorCtx.sceneFlags.clearedRoom = 0;

    for (i = 0; i < SCENE_MAX; i++) {
        gSaveContext.cycleSceneFlags[i].switch0 =
            ((void)0, gSaveContext.cycleSceneFlags[i].switch0) & sPersistentCycleSceneFlags[i].switch0;
        gSaveContext.cycleSceneFlags[i].switch1 =
            ((void)0, gSaveContext.cycleSceneFlags[i].switch1) & sPersistentCycleSceneFlags[i].switch1;
        gSaveContext.cycleSceneFlags[i].chest =
            ((void)0, gSaveContext.cycleSceneFlags[i].chest) & sPersistentCycleSceneFlags[i].chest;
        gSaveContext.cycleSceneFlags[i].collectible =
            ((void)0, gSaveContext.cycleSceneFlags[i].collectible) & sPersistentCycleSceneFlags[i].collectible;
        gSaveContext.cycleSceneFlags[i].clearedRoom = 0;
        gSaveContext.save.saveInfo.permanentSceneFlags[i].unk_14 = 0;
        gSaveContext.save.saveInfo.permanentSceneFlags[i].rooms = 0;
    }

    for (; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
        gSaveContext.cycleSceneFlags[i].chest = 0;
        gSaveContext.cycleSceneFlags[i].switch0 = 0;
        gSaveContext.cycleSceneFlags[i].switch1 = 0;
        gSaveContext.cycleSceneFlags[i].clearedRoom = 0;
        gSaveContext.cycleSceneFlags[i].collectible = 0;
    }

    for (i = 0; i < ARRAY_COUNT(gSaveContext.masksGivenOnMoon); i++) {
        gSaveContext.masksGivenOnMoon[i] = 0;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_20)) {
        //Inventory_DeleteItem(ITEM_MASK_FIERCE_DEITY, SLOT(ITEM_MASK_FIERCE_DEITY));
    }

    // persistent chateau romani state
    drankChateau = CHECK_WEEKEVENTREG(WEEKEVENTREG_DRANK_CHATEAU_ROMANI);

    for (i = 0; i < ARRAY_COUNT(sPersistentCycleWeekEventRegs); i++) {
        u16 isPersistentBits = sPersistentCycleWeekEventRegs[i];

        for (j = 0; j < ARRAY_COUNT(sBitFlags8); j++) {
            if (!(isPersistentBits & 3)) {
                gSaveContext.save.saveInfo.weekEventReg[i] =
                    ((void)0, gSaveContext.save.saveInfo.weekEventReg[i]) & (0xFF ^ sBitFlags8[j]);
            }
            isPersistentBits >>= 2;
        }
    }

    Sram_SetInitialWeekEvents();

    for (i = 0; i < ARRAY_COUNT(gSaveContext.eventInf); i++) {
        gSaveContext.eventInf[i] = 0;
    }

    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_RUPEES);
    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_BOMB_AMMO);
    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_NUT_AMMO);
    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_STICK_AMMO);
    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_ARROW_AMMO);

    if (gSaveContext.save.saveInfo.playerData.rupees != 0) {
        //SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_RUPEES);
    }

    /*if (INV_CONTENT(ITEM_BOMB) == ITEM_BOMB) {
        item = INV_CONTENT(ITEM_BOMB);
        if (AMMO(item) != 0) {
            SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_BOMB_AMMO);
        }
    }
    if (INV_CONTENT(ITEM_DEKU_NUT) == ITEM_DEKU_NUT) {
        item = INV_CONTENT(ITEM_DEKU_NUT);
        if (AMMO(item) != 0) {
            SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_NUT_AMMO);
        }
    }
    if (INV_CONTENT(ITEM_DEKU_STICK) == ITEM_DEKU_STICK) {
        item = INV_CONTENT(ITEM_DEKU_STICK);
        if (AMMO(item) != 0) {
            SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_STICK_AMMO);
        }
    }
    if (INV_CONTENT(ITEM_BOW) == ITEM_BOW) {
        item = INV_CONTENT(ITEM_BOW);
        if (AMMO(item) != 0) {
            SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_ARROW_AMMO);
        }
    }
    for (i = 0; i < ARRAY_COUNT(gAmmoItems); i++) {
        if (gAmmoItems[i] != ITEM_NONE) {
            if ((gSaveContext.save.saveInfo.inventory.items[i] != ITEM_NONE) && (i != SLOT_PICTOGRAPH_BOX)) {
                item = gSaveContext.save.saveInfo.inventory.items[i];
                AMMO(item) = 0;
            }
        }
    }*/

    //Inventory_DeleteItem(ITEM_OCARINA_FAIRY, SLOT_TRADE_DEED);
    //Inventory_DeleteItem(ITEM_SLINGSHOT, SLOT_TRADE_KEY_MAMA);
    //Inventory_DeleteItem(ITEM_LONGSHOT, SLOT_TRADE_COUPLE);

    /*for (j = EQUIP_SLOT_C_LEFT; j <= EQUIP_SLOT_C_RIGHT; j++) {
        if ((GET_CUR_FORM_BTN_ITEM(j) >= ITEM_MOONS_TEAR) && (GET_CUR_FORM_BTN_ITEM(j) <= ITEM_PENDANT_OF_MEMORIES)) {
            SET_CUR_FORM_BTN_ITEM(j, ITEM_NONE);
            Interface_LoadItemIconImpl(play, j);
        }
    }*/

    //gSaveContext.save.saveInfo.skullTokenCount &= ~0xFFFF0000;
    //gSaveContext.save.saveInfo.skullTokenCount &= ~0x0000FFFF;
    gSaveContext.save.saveInfo.unk_EA0 = 0;

    gSaveContext.save.saveInfo.unk_E64[0] = 0;
    gSaveContext.save.saveInfo.unk_E64[1] = 0;
    gSaveContext.save.saveInfo.unk_E64[2] = 0;
    gSaveContext.save.saveInfo.unk_E64[3] = 0;
    gSaveContext.save.saveInfo.unk_E64[4] = 0;
    gSaveContext.save.saveInfo.unk_E64[5] = 0;
    gSaveContext.save.saveInfo.unk_E64[6] = 0;

    Sram_ClearHighscores();

    /*for (i = 0; i < 8; i++) {
        gSaveContext.save.saveInfo.inventory.dungeonItems[i] &= (u8)~1; // remove boss key
        DUNGEON_KEY_COUNT(i) = 0;
        gSaveContext.save.saveInfo.inventory.strayFairies[i] = 0;
    }*/

    //~ gSaveContext.save.saveInfo.playerData.rupees = 0;
    //gSaveContext.save.saveInfo.scarecrowSpawnSongSet = false;
    gSaveContext.powderKegTimer = 0;
    gSaveContext.unk_1014 = 0;
    gSaveContext.jinxTimer = 0;
    gSaveContext.rupeeAccumulator = 0;

    Sram_ResetSaveCycle(play);
}