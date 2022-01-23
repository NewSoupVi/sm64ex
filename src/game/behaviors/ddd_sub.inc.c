// ddd_sub.c.inc
#include "../../sm64ap.h"
void bhv_bowsers_sub_loop(void) {
    if (SM64AP_CheckedKey2())
        obj_mark_for_deletion(o);
}
