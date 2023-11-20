#ifndef CATNIP_SEAT_H
#define CATNIP_SEAT_H

#include <wlr/types/wlr_seat.h>

extern struct wlr_seat* catnip_seat;

void
catnip_seat_init();

#endif
