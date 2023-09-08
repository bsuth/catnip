#ifndef SERVER_SEAT_H
#define SERVER_SEAT_H

#include <wlr/types/wlr_seat.h>

extern struct wlr_seat* server_seat;

void
init_server_seat();

#endif
