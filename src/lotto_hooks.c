#include "modding.h"
#include "global.h"

#include "apcommon.h"

#include "overlays\actors\ovl_En_Kujiya\z_en_kujiya.h"

void EnKujiya_SetupFinishGivePrize(EnKujiya* this);

RECOMP_PATCH void EnKujiya_GivePrize(EnKujiya* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        EnKujiya_SetupFinishGivePrize(this);
    } else {
        // Actor_OfferGetItem(&this->actor, play, GI_RUPEE_PURPLE, 500.0f, 100.0f);
        Actor_OfferGetItemHook(&this->actor, play, rando_get_item_id(LOCATION_LOTTERY_SHOP), LOCATION_LOTTERY_SHOP, 500.0f, 100.0f, true, true);
    }
}