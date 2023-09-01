#ifndef BWC_SERVER_SEAT_H
#define BWC_SERVER_SEAT_H

#include <wlr/types/wlr_seat.h>

extern struct wlr_seat* bwc_seat;

void
bwc_seat_init();

#endif
