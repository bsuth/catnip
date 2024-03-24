#ifndef CATNIP_CURSOR_METHODS_H
#define CATNIP_CURSOR_METHODS_H

#include <stdint.h>

void
catnip_cursor_notify(uint32_t time_msec);

void
catnip_cursor_xcursor(const char* name, int size);

void
catnip_cursor_move(double x, double y);

#endif
