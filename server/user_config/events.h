#ifndef CATNIP_USER_CONFIG_EVENTS_H
#define CATNIP_USER_CONFIG_EVENTS_H

void
init_event_listeners();

void
add_event_listener(const char* event, int lua_callback_ref);

void
remove_event_listener(const char* event, int lua_callback_ref);

void
clear_event_listeners(const char* event);

void
call_event_listeners(const char* event);

#endif
