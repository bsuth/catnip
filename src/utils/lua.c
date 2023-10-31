#include "lua.h"
#include <glib.h>
#include <lauxlib.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// Stack Utils
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

// -----------------------------------------------------------------------------
// Error Messages
// -----------------------------------------------------------------------------

const char*
lua_get_arg_error_msg(lua_State* L, const int arg_index, const char* details)
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

  g_string_printf(
    error_msg,
    "%s:%d: bad argument #%d to '%s' (%s)",
    caller_info.short_src,
    caller_info.currentline,
    arg_index,
    callee_info.name ? callee_info.name : "?",
    details
  );

  return g_string_free_and_steal(error_msg);
}

const char*
lua_get_arg_type_error_msg(lua_State* L, int arg_index, const int expected_type)
{
  GString* details = g_string_new(NULL);

  g_string_printf(
    details,
    "%s expected, got %s",
    lua_typename(L, expected_type),
    luaL_typename(L, arg_index)
  );

  const char* error_msg = lua_get_arg_error_msg(L, arg_index, details->str);

  g_string_free(details, true);

  return error_msg;
}
