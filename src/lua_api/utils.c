#include "utils.h"
#include <glib.h>
#include <lauxlib.h>
#include <stdbool.h>
#include <string.h>

const char*
get_arg_error_msg(lua_State* L, const int arg_index, const char* details)
{
  GString* error_msg = g_string_new(NULL);

  lua_Debug callee_info;
  lua_Debug caller_info;

  if (!lua_getstack(L, 0, &callee_info) || !lua_getstack(L, 1, &caller_info)) {
    g_string_printf(error_msg, "bad argument #%d (%s)", arg_index, details);
    return g_string_free_and_steal(error_msg);
  }

  lua_getinfo(L, "n", &callee_info);
  lua_getinfo(L, "Sl", &caller_info);

  g_string_printf(error_msg,
                  "%s:%d: bad argument #%d to '%s' (%s)",
                  caller_info.short_src,
                  caller_info.currentline,
                  arg_index,
                  callee_info.name ? callee_info.name : "?",
                  details);

  return g_string_free_and_steal(error_msg);
}

const char*
get_arg_type_error_msg(lua_State* L, int arg_index, const int expected_type)
{
  GString* details = g_string_new(NULL);

  g_string_printf(details,
                  "%s expected, got %s",
                  lua_typename(L, expected_type),
                  luaL_typename(L, arg_index));

  const char* error_msg = get_arg_error_msg(L, arg_index, details->str);

  g_string_free(details, true);

  return error_msg;
}
