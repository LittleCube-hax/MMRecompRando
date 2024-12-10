#include "modding.h"
#include "global.h"

struct EnRacedog;
#define DOG_LIMB_MAX 0x0D
typedef void (*EnRacedogActionFunc)(struct EnRacedog*, PlayState*);
typedef struct EnRacedog {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnRacedogActionFunc actionFunc;
    /* 0x148 */ UNK_TYPE1 unk_148[0x4];
    /* 0x14C */ SkelAnime skelAnime;
    /* 0x190 */ ColliderCylinder collider;
    /* 0x1DC */ UNK_TYPE1 unk_1DC[0x4];
    /* 0x1E0 */ Path* path;
    /* 0x1E4 */ UNK_TYPE1 unk_1E4[0x4];
    /* 0x1E8 */ s32 currentPoint;
    /* 0x1EC */ Vec3s jointTable[DOG_LIMB_MAX];
    /* 0x23A */ Vec3s morphTable[DOG_LIMB_MAX];
    /* 0x288 */ s16 selectionArrowTimer;
    /* 0x28A */ s16 raceStartTimer;
    /* 0x28C */ s16 extraTimeBeforeRaceStart;
    /* 0x28A */ UNK_TYPE1 unk_28E[0x2];
    /* 0x290 */ s16 index;
    /* 0x292 */ s16 selectedDogIndex;
    /* 0x294 */ UNK_TYPE1 unk_294[0x4];
    /* 0x298 */ f32 targetSpeed;
    /* 0x29C */ s16 raceStatus;
    /* 0x2A0 */ Vec3f prevRot;
    /* 0x2AC */ Vec3f curRot;
    /* 0x2B8 */ s32 pointForCurrentTargetSpeed;
    /* 0x2BC */ s32 selectionArrowGreenPrimColor;
    /* 0x2C0 */ s32 selectionArrowGreenEnvColor;
    /* 0x2C4 */ f32 selectionArrowScale;
} EnRacedog; // size = 0x2C8

s16 EnRacedog_GetYRotation(Path* path, s32 pointIndex, Vec3f* pos, f32* distSQ);
void EnRacedog_UpdateSpeed(EnRacedog* this);
void EnRacedog_UpdateRaceVariables(EnRacedog* this);
void EnRacedog_CheckForFinish(EnRacedog* this);
void EnRacedog_UpdateRunAnimationPlaySpeed(EnRacedog* this);
void EnRacedog_PlaySfxWalk(EnRacedog* this);
void EnRacedog_SpawnFloorDustRing(EnRacedog* this, PlayState* play);

static s16 sFirstPlaceIndex = -1;
static s16 sSprintTimer = 0;

RECOMP_PATCH void EnRacedog_Race(EnRacedog* this, PlayState* play) {
    s16 yRotation;
    f32 distSq;

    this->collider.dim.radius = 15;
    if (this->path != NULL) {
        yRotation = EnRacedog_GetYRotation(this->path, this->currentPoint, &this->actor.world.pos, &distSq);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            yRotation = this->actor.wallYaw;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, yRotation, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;

        if (distSq <= SQ(50.0f)) {
            this->currentPoint++;
            if (this->currentPoint >= (this->path->count - 1)) {
                this->currentPoint = 0;
            }
        }

        EnRacedog_UpdateSpeed(this);

        // Turns selected dog into giga dog
        if (this->index == this->selectedDogIndex && Player_GetMask(play) == PLAYER_MASK_TRUTH) {
            this->actor.speed = 10.0f;
        }

        // Putting this after EnRacedog_UpdateSpeed will ensure that when EnRacedog_CalculateFinalStretchTargetSpeed
        // is called for the first-place dog, the sprint timer will be 0, so the first-place dog will be guaranteed
        // to have a sprint speed multiplier of 1.
        if ((this->currentPoint >= ((this->path->count / 4) * 3)) && (this->index == sFirstPlaceIndex)) {
            sSprintTimer++;
        }

        EnRacedog_UpdateRaceVariables(this);
        EnRacedog_CheckForFinish(this);
        Actor_MoveWithGravity(&this->actor);
    }

    EnRacedog_UpdateRunAnimationPlaySpeed(this);
    EnRacedog_PlaySfxWalk(this);
    EnRacedog_SpawnFloorDustRing(this, play);
}