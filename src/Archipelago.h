#pragma once

#include <stdbool.h>

#define SM64AP_ID_OFFSET 3626000
#define SM64AP_ITEMID_STAR SM64AP_ID_OFFSET
#define SM64AP_ITEMID_KEY1 (SM64AP_ID_OFFSET+178)
#define SM64AP_ITEMID_KEY2 (SM64AP_ITEMID_KEY1+1)
#define SM64AP_ITEMID_WINGCAP (SM64AP_ITEMID_KEY2+1)
#define SM64AP_ITEMID_METALCAP (SM64AP_ITEMID_WINGCAP+1)
#define SM64AP_ITEMID_VANISHCAP (SM64AP_ITEMID_METALCAP+1)

#ifdef __cplusplus
extern "C" void SM64AP_Init(const char* ip, const char* player_name, const char* passwd);
#else
void SM64AP_Init(const char*, const char*, const char*);
#endif

// Sends LocationCheck for given index
#ifdef __cplusplus
extern "C" void SM64AP_SendItem(int);
#else
void SM64AP_SendItem(int);
#endif

// Local Stars, Keys and Caps
#ifdef __cplusplus
extern "C" int SM64AP_GetStars();
extern "C" bool SM64AP_HaveKey1();
extern "C" bool SM64AP_HaveKey2();
extern "C" bool SM64AP_HaveCap(int);
#else
int SM64AP_GetStars();
bool SM64AP_HaveKey1();
bool SM64AP_HaveKey2();
bool SM64AP_HaveCap(int);
#endif

// Print Next Message to Screen
#ifdef __cplusplus
extern "C" void SM64AP_PrintNext();
#else
void SM64AP_PrintNext();
#endif

// DeathLink functions
void SM64AP_DeathLinkSend();
bool SM64AP_DeathLinkRecv();
void SM64AP_DeathLinkClear();

// Differentiate locations and stars
const bool* SM64AP_Locations();
const bool* SM64AP_Stars();

// Called when Story completed, sends StatusUpdate
#ifdef __cplusplus
extern "C" void SM64AP_StoryComplete();
#else
void SM64AP_StoryComplete();
#endif