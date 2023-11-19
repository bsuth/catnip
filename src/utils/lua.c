#include "lua.h"
#include "utils/glib.h"
#include "utils/log.h"
#include <glib.h>
#include <lauxlib.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// Stack Popping
// -----------------------------------------------------------------------------

int
lua_popboolean(lua_State* L)
{
  int b = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return b;
}

lua_CFunction
lua_popcfunction(lua_State* L)
{
  lua_CFunction f = lua_tocfunction(L, -1);
  lua_pop(L, 1);
  return f;
}

lua_Integer
lua_popinteger(lua_State* L)
{
  lua_Integer n = lua_tointeger(L, -1);
  lua_pop(L, 1);
  return n;
}

const char*
lua_poplstring(lua_State* L, size_t* len)
{
  const char* s = lua_tolstring(L, -1, len);
  lua_pop(L, 1);
  return s;
}

lua_Number
lua_popnumber(lua_State* L)
{
  lua_Number n = lua_tonumber(L, -1);
  lua_pop(L, 1);
  return n;
}

const void*
lua_poppointer(lua_State* L)
{
  const void* p = lua_topointer(L, -1);
  lua_pop(L, 1);
  return p;
}

const char*
lua_popstring(lua_State* L)
{
  const char* s = lua_tostring(L, -1);
  lua_pop(L, 1);
  return s;
}

lua_State*
lua_popthread(lua_State* L)
{
  lua_State* t = lua_tothread(L, -1);
  lua_pop(L, 1);
  return t;
}

void*
lua_popuserdata(lua_State* L)
{
  void* p = lua_touserdata(L, -1);
  lua_pop(L, 1);
  return p;
}

// -----------------------------------------------------------------------------
// Stack Pulling
// -----------------------------------------------------------------------------

int
lua_pullboolean(lua_State* L, int index)
{
  int b = lua_toboolean(L, index);
  lua_remove(L, index);
  return b;
}

lua_CFunction
lua_pullcfunction(lua_State* L, int index)
{
  lua_CFunction f = lua_tocfunction(L, index);
  lua_remove(L, index);
  return f;
}

lua_Integer
lua_pullinteger(lua_State* L, int index)
{
  lua_Integer n = lua_tointeger(L, index);
  lua_remove(L, index);
  return n;
}

const char*
lua_pulllstring(lua_State* L, int index, size_t* len)
{
  const char* s = lua_tolstring(L, index, len);
  lua_remove(L, index);
  return s;
}

lua_Number
lua_pullnumber(lua_State* L, int index)
{
  lua_Number n = lua_tonumber(L, index);
  lua_remove(L, index);
  return n;
}

const void*
lua_pullpointer(lua_State* L, int index)
{
  const void* p = lua_topointer(L, index);
  lua_remove(L, index);
  return p;
}

const char*
lua_pullstring(lua_State* L, int index)
{
  const char* s = lua_tostring(L, index);
  lua_remove(L, index);
  return s;
}

lua_State*
lua_pullthread(lua_State* L, int index)
{
  lua_State* t = lua_tothread(L, index);
  lua_remove(L, index);
  return t;
}

void*
lua_pulluserdata(lua_State* L, int index)
{
  void* p = lua_touserdata(L, index);
  lua_remove(L, index);
  return p;
}

// -----------------------------------------------------------------------------
// Table Fields
// -----------------------------------------------------------------------------

bool
lua_hasfield(lua_State* L, int index, const char* field)
{
  lua_getfield(L, index, field);

  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);
    return false;
  } else {
    return true;
  }
}

bool
lua_hasfieldtype(lua_State* L, int index, const char* field, int type)
{
  if (!lua_hasfield(L, index, field)) {
    return false;
  } else if (lua_type(L, -1) != type) {
    log_warning("%s", lua_field_error_msg_expected_type(L, field, -1, type));
    lua_pop(L, 1);
    return false;
  } else {
    return true;
  }
}

// -----------------------------------------------------------------------------
// Error Messages
// -----------------------------------------------------------------------------

void
lua_log(lua_State* L, const char* format, ...)
{
  char timestamp[9];
  time_t timer = time(NULL);
  struct tm* tm = localtime(&timer);
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm);

  GString* message = g_string_new(NULL);
  va_list varargs;
  va_start(varargs, format);
  g_string_vprintf(message, format, varargs);
  va_end(varargs);

  char* lua_message = lua_error_msg(L, message->str);
  printf("[%s] %s\n", timestamp, lua_message);

  g_string_free(message, true);
  free(lua_message);
}

char*
lua_error_msg(lua_State* L, const char* details)
{
  GString* error_msg = g_string_new(NULL);
  lua_Debug caller_info;

  if (!lua_getstack(L, 1, &caller_info)) {
    g_string_printf(error_msg, "%s", details);
  } else {
    lua_getinfo(L, "Sl", &caller_info);

    g_string_printf(
      error_msg,
      "%s:%d: %s",
      caller_info.short_src,
      caller_info.currentline,
      details
    );
  }

  return g_string_free_and_steal(error_msg);
}

char*
lua_error_msg_bad_type(lua_State* L, int index)
{
  return g_string_free_and_steal(
    g_string_new_printf("bad type '%s'", luaL_typename(L, index))
  );
}

char*
lua_error_msg_expected_type(lua_State* L, int index, int expected_type)
{
  return g_string_free_and_steal(g_string_new_printf(
    "%s expected, got %s",
    lua_typename(L, expected_type),
    luaL_typename(L, index)
  ));
}

char*
lua_arg_error_msg(lua_State* L, int arg_index, const char* details)
{
  lua_Debug callee_info;

  if (!lua_getstack(L, 0, &callee_info)) {
    GString* error_msg = g_string_new(NULL);
    g_string_printf(error_msg, "bad argument #%d (%s)", arg_index, details);
    return g_string_free_and_steal(error_msg);
  }

  lua_getinfo(L, "n", &callee_info);

  GString* full_details = g_string_new_printf(
    "bad argument #%d to '%s' (%s)",
    arg_index,
    callee_info.name ? callee_info.name : "?",
    details
  );

  char* error_msg = lua_error_msg(L, full_details->str);
  g_string_free(full_details, true);

  return error_msg;
}

char*
lua_arg_error_msg_bad_type(lua_State* L, int arg_index)
{
  char* details = lua_error_msg_bad_type(L, arg_index);
  char* error_msg = lua_arg_error_msg(L, arg_index, details);
  free(details);
  return error_msg;
}

char*
lua_arg_error_msg_expected_type(lua_State* L, int arg_index, int expected_type)
{
  char* details = lua_error_msg_expected_type(L, arg_index, expected_type);
  char* error_msg = lua_arg_error_msg(L, arg_index, details);
  free(details);
  return error_msg;
}

char*
lua_field_error_msg(lua_State* L, const char* field, const char* details)
{
  GString* full_details =
    g_string_new_printf("bad field '%s' (%s)", field, details);
  char* error_msg = lua_error_msg(L, full_details->str);
  g_string_free(full_details, true);
  return error_msg;
}

char*
lua_field_error_msg_bad_type(lua_State* L, const char* field, int value_index)
{
  char* details = lua_error_msg_bad_type(L, value_index);
  char* error_msg = lua_field_error_msg(L, field, details);
  free(details);
  return error_msg;
}

char*
lua_field_error_msg_expected_type(
  lua_State* L,
  const char* field,
  int value_index,
  int expected_type
)
{
  char* details = lua_error_msg_expected_type(L, value_index, expected_type);
  char* error_msg = lua_field_error_msg(L, field, details);
  free(details);
  return error_msg;
}
