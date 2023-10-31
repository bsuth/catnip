#ifndef CATNIP_SERVER_SEAT_H
#define CATNIP_SERVER_SEAT_H

#include <wlr/types/wlr_seat.h>

extern struct wlr_seat* server_seat;

void
server_seat_init();

#endif
