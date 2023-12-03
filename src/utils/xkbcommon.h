#ifndef CATNIP_UTILS_XKBCOMMON_H
#define CATNIP_UTILS_XKBCOMMON_H

#include <stdbool.h>
#include <xkbcommon/xkbcommon.h>

bool
xkb_is_modifier(xkb_keysym_t keysym);

#endif
