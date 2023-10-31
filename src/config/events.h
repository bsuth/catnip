#ifndef CATNIP_CONFIG_EVENTS_H
#define CATNIP_CONFIG_EVENTS_H

void
config_events_subscribe(const char* event, int lua_callback_ref);

void
config_events_unsubscribe(const char* event, int lua_callback_ref);

void
config_events_clear_subscriptions(const char* event);

void
config_events_publish(const char* event);

void
config_events_init();

#endif
