#ifndef CATNIP_CONFIG_EVENTS_H
#define CATNIP_CONFIG_EVENTS_H

void
init_config_events();

void
subscribe_config_event(const char* event, int lua_callback_ref);

void
unsubscribe_config_event(const char* event, int lua_callback_ref);

void
clear_config_subscriptions(const char* event);

void
publish_config_event(const char* event);

#endif
