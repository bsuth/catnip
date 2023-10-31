#include "events.h"
#include "config/config.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>
#include <stdbool.h>
#include <string.h>

static GHashTable* event_registry;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static void
free_lua_callback_ref(void* data)
{
  const int* lua_callback_ref = data;
  luaL_unref(L, LUA_REGISTRYINDEX, *lua_callback_ref);
}

// -----------------------------------------------------------------------------
// Events
// -----------------------------------------------------------------------------

void
config_events_subscribe(const char* event, const int lua_callback_ref)
{
  GArray* event_listeners = g_hash_table_lookup(event_registry, event);

  if (event_listeners == NULL) {
    event_listeners = g_array_new(false, true, sizeof(int));
    g_array_set_clear_func(event_listeners, free_lua_callback_ref);
    g_hash_table_insert(event_registry, strdup(event), event_listeners);
  }

  g_array_append_val(event_listeners, lua_callback_ref);
}

void
config_events_unsubscribe(const char* event, const int lua_callback_ref)
{
  GArray* event_listeners = g_hash_table_lookup(event_registry, event);

  if (event_listeners != NULL) {
    for (int i = 0; i < event_listeners->len; ++i) {
      if (g_array_index(event_listeners, int, i) == lua_callback_ref) {
        g_array_remove_index_fast(event_listeners, i);
        return;
      }
    }
  }
}

void
config_events_clear_subscriptions(const char* event)
{
  if (event == NULL) {
    g_hash_table_remove_all(event_registry);
  } else {
    g_hash_table_remove(event_registry, event);
  }
}

void
config_events_publish(const char* event)
{
  GArray* event_listeners = g_hash_table_lookup(event_registry, event);

  if (event_listeners != NULL) {
    for (int i = 0; i < event_listeners->len; ++i) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, g_array_index(event_listeners, int, i));

      // TODO: event parameters?
      if (lua_pcall(L, 0, 0, 0) != 0) {
        log_error("%s", lua_tostring(L, -1));
      }
    }
  }
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
free_event_listeners(void* data)
{
  GArray* event_listeners = data;
  g_array_free(event_listeners, true);
}

void
config_events_init()
{
  event_registry =
    g_hash_table_new_full(g_str_hash, g_str_equal, free, free_event_listeners);
}
