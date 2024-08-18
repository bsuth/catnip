#ifndef CATNIP_DESKTOP_WINDOWS_H
#define CATNIP_DESKTOP_WINDOWS_H

#include "desktop/window.h"

extern struct wl_list catnip_windows;

void
catnip_windows_init();

struct catnip_window*
catnip_windows_get_focused();

#endif
