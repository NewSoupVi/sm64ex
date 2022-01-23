#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
    #include "types.h"
}
#else
#include "types.h"
#endif

#define SM64AP_ID_OFFSET 3626000
#define SM64AP_ITEMID_STAR SM64AP_ID_OFFSET
#define SM64AP_ITEMID_KEY1 (SM64AP_ID_OFFSET+178)
#define SM64AP_ITEMID_KEY2 (SM64AP_ITEMID_KEY1+1)
#define SM64AP_ITEMID_WINGCAP (SM64AP_ITEMID_KEY2+1)
#define SM64AP_ITEMID_METALCAP (SM64AP_ITEMID_WINGCAP+1)
#define SM64AP_ITEMID_VANISHCAP (SM64AP_ITEMID_METALCAP+1)

#define SM64AP_NUM_LOCS 183


#ifdef __cplusplus
//Init
extern "C" void SM64AP_Init(const char*, const char*, const char*);

// Local Stars, Keys and Caps
extern "C" int SM64AP_GetStars();
extern "C" int SM64AP_StarsToFinish();
extern "C" u32 SM64AP_CourseStarFlags(s32);
extern "C" bool SM64AP_CheckedKey1();
extern "C" bool SM64AP_HaveKey1();
extern "C" bool SM64AP_CheckedKey2();
extern "C" bool SM64AP_HaveKey2();
extern "C" bool SM64AP_HaveCap(int);

// Send Item
extern "C" void SM64AP_SendItem(int);

// Print Next Message to Screen
extern "C" void SM64AP_PrintNext();

// Called when Story completed, sends StatusUpdate
extern "C" void SM64AP_StoryComplete();
#else
void SM64AP_Init(const char*, const char*, const char*);
int SM64AP_GetStars();
int SM64AP_StarsToFinish();
u32 SM64AP_CourseStarFlags(s32);
bool SM64AP_CheckedKey1();
bool SM64AP_HaveKey1();
bool SM64AP_CheckedKey2();
bool SM64AP_HaveKey2();
bool SM64AP_HaveCap(int);
void SM64AP_SendItem(int);
void SM64AP_PrintNext();
void SM64AP_StoryComplete();
#endif
