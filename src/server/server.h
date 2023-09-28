#ifndef CATNIP_SERVER_H
#define CATNIP_SERVER_H

extern struct wlr_compositor* server_compositor;

void
init_server();

void
start_server();

void
stop_server();

#endif
