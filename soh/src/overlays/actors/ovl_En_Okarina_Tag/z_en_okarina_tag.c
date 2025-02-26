/*
 * File: z_en_okarina_tag.c
 * Overlay: ovl_En_Okarina_Tag
 * Description: Music Staff (Ocarina) spot
 */

#include "z_en_okarina_tag.h"
#include "scenes/misc/hakaana_ouke/hakaana_ouke_scene.h"
#include "scenes/overworld/spot02/spot02_scene.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnOkarinaTag_Init(Actor* thisx, GlobalContext* globalCtx);
void EnOkarinaTag_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnOkarinaTag_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80ABEF2C(EnOkarinaTag* this, GlobalContext* globalCtx);
void func_80ABF708(EnOkarinaTag* this, GlobalContext* globalCtx);
void func_80ABF28C(EnOkarinaTag* this, GlobalContext* globalCtx);
void func_80ABF0CC(EnOkarinaTag* this, GlobalContext* globalCtx);
void func_80ABF4C8(EnOkarinaTag* this, GlobalContext* globalCtx);
void func_80ABF7CC(EnOkarinaTag* this, GlobalContext* globalCtx);

const ActorInit En_Okarina_Tag_InitVars = {
    ACTOR_EN_OKARINA_TAG,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnOkarinaTag),
    (ActorFunc)EnOkarinaTag_Init,
    (ActorFunc)EnOkarinaTag_Destroy,
    (ActorFunc)EnOkarinaTag_Update,
    NULL,
    NULL,
};

extern CutsceneData D_80ABF9D0[];
extern CutsceneData D_80ABFB40[];

void EnOkarinaTag_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnOkarinaTag_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnOkarinaTag* this = (EnOkarinaTag*)thisx;

    osSyncPrintf("\n\n");
    // "Ocarina tag outbreak"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ オカリナタグ発生 ☆☆☆☆☆ %x\n" VT_RST, this->actor.params);
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->type = (this->actor.params >> 0xA) & 0x3F;
    this->ocarinaSong = (this->actor.params >> 6) & 0xF;
    this->switchFlag = this->actor.params & 0x3F;
    if (this->switchFlag == 0x3F) {
        this->switchFlag = -1;
    }
    if (this->ocarinaSong == 0xF) {
        this->ocarinaSong = 0;
        this->unk_158 = 1;
    }
    this->actor.targetMode = 1;
    if (this->actor.world.rot.z > 0) {
        this->interactRange = this->actor.world.rot.z * 40.0f;
    }

    // "Save information"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ セーブ情報\t ☆☆☆☆☆ %d\n" VT_RST, this->switchFlag);
    // "Type index"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 種類インデックス ☆☆☆☆☆ %d\n" VT_RST, this->type);
    // "Correct answer information"
    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 正解情報\t ☆☆☆☆☆ %d\n" VT_RST, this->ocarinaSong);
    // "Range information"
    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 範囲情報\t ☆☆☆☆☆ %d\n" VT_RST, this->actor.world.rot.z);
    // "Processing range information"
    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 処理範囲情報\t ☆☆☆☆☆ %f\n" VT_RST, this->interactRange);
    // "Hit?"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 当り？\t\t ☆☆☆☆☆ %d\n" VT_RST, this->unk_158);
    osSyncPrintf("\n\n");

    if ((this->switchFlag >= 0) && (Flags_GetSwitch(globalCtx, this->switchFlag))) {
        Actor_Kill(&this->actor);
    } else {
        switch (this->type) {
            case 7:
                this->actionFunc = func_80ABEF2C;
                break;
            case 2:
                if (LINK_IS_ADULT) {
                    Actor_Kill(&this->actor);
                    break;
                }
            case 1:
            case 4:
            case 6:
                this->actionFunc = func_80ABF28C;
                break;
            case 5:
                this->actor.textId = 0x5021;
                this->actionFunc = func_80ABF708;
                break;
            default:
                Actor_Kill(&this->actor);
                break;
        }
    }
}

void func_80ABEF2C(EnOkarinaTag* this, GlobalContext* globalCtx) {
    Player* player;
    u16 ocarinaSong;

    player = GET_PLAYER(globalCtx);
    this->unk_15A++;
    if ((this->switchFlag >= 0) && (Flags_GetSwitch(globalCtx, this->switchFlag))) {
        this->actor.flags &= ~ACTOR_FLAG_0;
    } else {
        if ((this->ocarinaSong != 6) || (gSaveContext.scarecrowSpawnSongSet)) {
            if (player->stateFlags2 & 0x1000000) {
                // "North! ! ! ! !"
                osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ 北！！！！！ ☆☆☆☆☆ %f\n" VT_RST, this->actor.xzDistToPlayer);
            }
            if ((this->actor.xzDistToPlayer < (90.0f + this->interactRange)) &&
                (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 80.0f)) {
                if (player->stateFlags2 & 0x2000000) {
                    ocarinaSong = this->ocarinaSong;
                    if (ocarinaSong == 6) {
                        ocarinaSong = 0xA;
                    }
                    player->stateFlags2 |= 0x800000;
                    //Logic for the enhancement to skip playing the ocarina
                    bool hasSong = false;
                    //Confirms that the necessary song is in the player's inventory.
                    if (ocarinaSong == 0 && CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
                        hasSong = true;
                    } else if (ocarinaSong == 1 && CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
                        hasSong = true;
                    } else if (ocarinaSong == 2 && CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                        hasSong = true;
                    } else if (ocarinaSong == 3 && CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
                        hasSong = true;
                    } else if (ocarinaSong == 4 && CHECK_QUEST_ITEM(QUEST_SONG_TIME)) {
                        hasSong = true;
                    } else if (ocarinaSong == 5 && CHECK_QUEST_ITEM(QUEST_SONG_STORMS)) {
                        hasSong = true;
                    } else {
                        hasSong = false;
                    }
                    //Enables skipping song if quick ocarina is on AND the song is in their inventory.
                    if (hasSong && (CVar_GetS32("gQuickOcarina",0)!=0)) {
                        hasSong = false;
                        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_03;
                        this->actionFunc = func_80ABF0CC;
                    } else {
                        gSaveContext.dayTime = 0x0000;
                        func_8010BD58(globalCtx, ocarinaSong + OCARINA_ACTION_CHECK_SARIA);
                        this->actionFunc = func_80ABF0CC;
                    }
                } else if ((this->actor.xzDistToPlayer < (50.0f + this->interactRange) &&
                            ((fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 40.0f)))) {
                    this->unk_15A = 0;
                    player->unk_6A8 = &this->actor;
                }
            }
        }
    }
}

void func_80ABF0CC(EnOkarinaTag* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        this->actionFunc = func_80ABEF2C;
    } else {
        if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
            if (this->switchFlag >= 0) {
                Flags_SetSwitch(globalCtx, this->switchFlag);
            }
            if (globalCtx->sceneNum == SCENE_MIZUSIN) {
                globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
            }
            if ((globalCtx->sceneNum != SCENE_DAIYOUSEI_IZUMI) && (globalCtx->sceneNum != SCENE_YOUSEI_IZUMI_YOKO)) {
                globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
            }
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            this->actionFunc = func_80ABEF2C;
            return;
        }
        if (this->unk_158 != 0) {
            if ((globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_05) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_06) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_07) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_08) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_09) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_0A) ||
                (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_0D)) {
                if (this->switchFlag >= 0) {
                    Flags_SetSwitch(globalCtx, this->switchFlag);
                }
                globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                this->actionFunc = func_80ABEF2C;
                return;
            }
        }
        if ((globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_05) && (globalCtx->msgCtx.ocarinaMode < OCARINA_MODE_0E)) {
            globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
            this->actionFunc = func_80ABEF2C;
        } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= 0x800000;
        }
    }
}

void func_80ABF28C(EnOkarinaTag* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    this->unk_15A++;
    if ((this->ocarinaSong != 6) || (gSaveContext.scarecrowSpawnSongSet)) {
        if ((this->switchFlag >= 0) && Flags_GetSwitch(globalCtx, this->switchFlag)) {
            this->actor.flags &= ~ACTOR_FLAG_0;
        } else if (((this->type != 4) || !(gSaveContext.eventChkInf[4] & 0x800)) &&
                   ((this->type != 6) || !(gSaveContext.eventChkInf[1] & 0x2000)) &&
                   (this->actor.xzDistToPlayer < (90.0f + this->interactRange)) &&
                   (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 80.0f)) {
            if (player->stateFlags2 & 0x1000000) {
                switch (this->type) {
                    case 1:
                        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_LULLABY);
                        break;
                    case 2:
                        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_STORMS);
                        break;
                    case 4:
                        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_TIME);
                        break;
                    case 6:
                        func_8010BD58(globalCtx, OCARINA_ACTION_CHECK_LULLABY);
                        break;
                    default:
                        // "Ocarina Invisible-kun demo start check error source"
                        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ オカリナ透明君デモ開始チェックエラー原 ☆☆☆☆☆ %d\n" VT_RST,
                                     this->type);
                        Actor_Kill(&this->actor);
                        break;
                }
                player->stateFlags2 |= 0x800000;
                this->actionFunc = func_80ABF4C8;
            } else if ((this->actor.xzDistToPlayer < (50.0f + this->interactRange)) &&
                       (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 40.0f)) {
                this->unk_15A = 0;
                player->stateFlags2 |= 0x800000;
            }
        }
    }
}

void func_80ABF4C8(EnOkarinaTag* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        this->actionFunc = func_80ABF28C;
    } else if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        if (!gSaveContext.n64ddFlag || (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_DOOR_OF_TIME) != 2)) {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        }
        if (this->switchFlag >= 0) {
            Flags_SetSwitch(globalCtx, this->switchFlag);
        }
        switch (this->type) {
            case 1:
                Flags_SetSwitch(globalCtx, this->switchFlag);
                gSaveContext.eventChkInf[3] |= 0x200;
                break;
            case 2:
                if (!gSaveContext.n64ddFlag) {
                    globalCtx->csCtx.segment = D_80ABF9D0;
                    gSaveContext.cutsceneTrigger = 1;
                } else {
                    gSaveContext.eventChkInf[6] |= 0x80;
                    gSaveContext.eventChkInf[6] |= 0x20;
                }
                func_800F574C(1.18921f, 0x5A);
                break;
            case 4:
                if (gSaveContext.n64ddFlag) {
                    int doorOfTime = Randomizer_GetSettingValue(RSK_DOOR_OF_TIME);
                    if (doorOfTime == 2 &&
                        (INV_CONTENT(ITEM_OCARINA_FAIRY) != ITEM_OCARINA_TIME ||
                         !CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) || !CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ||
                         !CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE))) {
                        func_80078884(NA_SE_SY_OCARINA_ERROR);
                        break;
                    } else {
                        func_80078884(NA_SE_SY_CORRECT_CHIME);
                    }
                }
                globalCtx->csCtx.segment = D_80ABFB40;
                gSaveContext.cutsceneTrigger = 1;
                break;
            case 6:
                // Don't start the cutscene in a rando save.
                if (!(gSaveContext.n64ddFlag)) {
                    globalCtx->csCtx.segment = LINK_IS_ADULT ? SEGMENTED_TO_VIRTUAL(&spot02_scene_Cs_003C80)
                                                             : SEGMENTED_TO_VIRTUAL(&spot02_scene_Cs_005020);
                    gSaveContext.cutsceneTrigger = 1;
                }
                gSaveContext.eventChkInf[1] |= 0x2000;
                func_80078884(NA_SE_SY_CORRECT_CHIME);
                break;
            default:
                break;
        }
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
        this->actionFunc = func_80ABF28C;
    } else {
        if (globalCtx->msgCtx.ocarinaMode >= OCARINA_MODE_05) {
            if (globalCtx->msgCtx.ocarinaMode < OCARINA_MODE_0E) {
                globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
                this->actionFunc = func_80ABF28C;
                return;
            }
        }
        if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_01) {
            player->stateFlags2 |= 0x800000;
        }
    }
}

void func_80ABF708(EnOkarinaTag* this, GlobalContext* globalCtx) {
    s16 yawDiff;
    s16 yawDiffNew;

    if (Actor_ProcessTalkRequest(&this->actor, globalCtx)) {
        this->actionFunc = func_80ABF7CC;
    } else {
        yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        this->unk_15A++;
        if (!(this->actor.xzDistToPlayer > 120.0f)) {
            if (CHECK_QUEST_ITEM(QUEST_SONG_SUN) || gSaveContext.n64ddFlag) {
                this->actor.textId = 0x5021;
            }
            yawDiffNew = ABS(yawDiff);
            if (yawDiffNew < 0x4300) {
                this->unk_15A = 0;
                func_8002F2CC(&this->actor, globalCtx, 70.0f);
            }
        }
    }
}

void GivePlayerRandoRewardSunSong(EnOkarinaTag* song, GlobalContext* globalCtx, RandomizerCheck check) {
    Flags_SetTreasure(globalCtx, 0x1F);
    GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(check, GI_LETTER_ZELDA);
    GiveItemEntryFromActor(&song->actor, globalCtx, getItemEntry, 10000.0f, 100.0f);
}

void func_80ABF7CC(EnOkarinaTag* this, GlobalContext* globalCtx) {
    // "Open sesame sesame!"
    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 開けゴマゴマゴマ！ ☆☆☆☆☆ %d\n" VT_RST, Message_GetState(&globalCtx->msgCtx));

    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(globalCtx)) {
        Message_CloseTextbox(globalCtx);
        if (!gSaveContext.n64ddFlag && !CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(&gSunSongGraveSunSongTeachCs);
            gSaveContext.cutsceneTrigger = 1;
        } else if (!Flags_GetTreasure(globalCtx, 0x1F)) {
            GivePlayerRandoRewardSunSong(this, globalCtx, RC_SONG_FROM_ROYAL_FAMILYS_TOMB);
        }
        this->actionFunc = func_80ABF708;
    }
}

void EnOkarinaTag_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnOkarinaTag* this = (EnOkarinaTag*)thisx;

    this->actionFunc(this, globalCtx);
    if (BREG(0) != 0) {
        if (this->unk_15A != 0) {
            if (!(this->unk_15A & 1)) {
                DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                       this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                       1.0f, 1.0f, 120, 120, 120, 255, 4, globalCtx->state.gfxCtx);
            }
        } else {
            DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                   this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f,
                                   1.0f, 1.0f, 255, 0, 0, 255, 4, globalCtx->state.gfxCtx);
        }
    }
}
