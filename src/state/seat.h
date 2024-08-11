#ifndef CATNIP_STATE_SEAT_H
#define CATNIP_STATE_SEAT_H

#include <wlr/types/wlr_seat.h>

extern struct wlr_seat* catnip_seat;

void
catnip_seat_init();

#endif
