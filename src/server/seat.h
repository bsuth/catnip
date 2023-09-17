#ifndef CATNIP_SERVER_SEAT_H
#define CATNIP_SERVER_SEAT_H

#include <wlr/types/wlr_seat.h>

extern struct wlr_seat* server_seat;

void
init_server_seat();

#endif
