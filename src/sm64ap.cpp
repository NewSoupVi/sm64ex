#include "sm64ap.h"
#include "Archipelago.h"

extern "C" {
    #include "game/print.h"
}

#include <deque>
#include <string>
#include <chrono>
#include <cstdlib>

int starsCollected = 0;
bool sm64_have_key1 = false;
bool sm64_have_key2 = false;
bool sm64_have_wingcap = false;
bool sm64_have_metalcap = false;
bool sm64_have_vanishcap = false;
int msg_frame_duration = 90; // 4 Secounds at 30F/s
int cur_msg_frame_duration = msg_frame_duration; 

void SM64AP_RecvItem(int idx);

void SM64AP_ResetItems() {
    sm64_have_key1 = false;
    sm64_have_key2 = false;
    sm64_have_wingcap = false;
    sm64_have_metalcap = false;
    sm64_have_vanishcap = false;
    starsCollected = 0;
}

void SM64AP_Init(const char* ip, const char* player_name, const char* passwd) {
    if (AP_IsInit()) {
        return;
    }

    AP_Init(ip, "Super Mario 64", player_name, passwd);
    AP_SetItemClearCallback(&SM64AP_ResetItems);
    AP_SetItemRecvCallback(&SM64AP_RecvItem);
    AP_Start();
}

void SM64AP_SendItem(int idxNoOffset) {
    AP_SendItem(idxNoOffset + SM64AP_ID_OFFSET);
}

void SM64AP_RecvItem(int idx) {
    switch (idx) {
        case SM64AP_ITEMID_STAR:
            starsCollected++;
            break;
        case SM64AP_ITEMID_KEY1:
            sm64_have_key1 = true;
            break;
        case SM64AP_ITEMID_KEY2:
            sm64_have_key2 = true;
            break;
        case SM64AP_ITEMID_WINGCAP:
            sm64_have_wingcap = true;
            break;
        case SM64AP_ITEMID_METALCAP:
            sm64_have_metalcap = true;
            break;
        case SM64AP_ITEMID_VANISHCAP:
            sm64_have_vanishcap = true;
            break;
    }
}

void SM64AP_StoryComplete() {
    AP_StoryComplete();
}

int SM64AP_GetStars() {
    return starsCollected;
}

bool SM64AP_HaveKey1() {
    return sm64_have_key1;
}

bool SM64AP_HaveKey2() {
    return sm64_have_key2;
}

bool SM64AP_HaveCap(int flag) {
    switch (flag) {
        case 2:
            return sm64_have_wingcap;
            break;
        case 4:
            return sm64_have_metalcap;
            break;
        case 8:
            return sm64_have_vanishcap;
            break;
        default:
            //Probably coin/1up or something
            return true;
    }
}

bool SM64AP_DeathLinkRecv() {
    return false;
}

void SM64AP_DeathLinkClear() {

}

void SM64AP_DeathLinkSend() {

}

void SM64AP_PrintNext() {
    if (AP_GetMsgQueue().empty()) return;
    int amount = AP_GetMsgQueue().front().second;
    for (int i = 0; i <= amount; i++) {
        print_text(-60, (amount-i)*20, AP_GetMsgQueue().at(i).first.c_str());
    }
    if (cur_msg_frame_duration > 0) {
        cur_msg_frame_duration--;
    } else {
        for (int i = 0; i <= amount; i++) {
            AP_GetMsgQueue().pop_front();
        }
        AP_GetMsgQueue().shrink_to_fit();
        cur_msg_frame_duration = msg_frame_duration;
    }
}