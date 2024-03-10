#include "lua.h"
#include "utils/string.h"
#include <lauxlib.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// Debugging
// -----------------------------------------------------------------------------

void
lua_dumpstack(lua_State* L)
{
  for (int i = lua_gettop(L); i > 0; --i) {
    printf("%d: %s = ", i, luaL_typename(L, i));

    switch (lua_type(L, i)) {
      case LUA_TNIL:
        printf("%s", "nil");
        break;
      case LUA_TBOOLEAN:
        printf("%s", lua_toboolean(L, i) ? "true" : "false");
        break;
      case LUA_TNUMBER:
        printf("%f", lua_tonumber(L, i));
        break;
      case LUA_TSTRING:
        printf("\"%s\"", lua_tostring(L, i));
        break;
      default:
        printf("%p", lua_topointer(L, i));
        break;
    }

    printf("\n");
  }
}

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
    lua_log_expected_field_type(L, index, field, type);
    lua_pop(L, 1);
    return false;
  } else {
    return true;
  }
}

// -----------------------------------------------------------------------------
// Messages
// -----------------------------------------------------------------------------

char*
lua_msg_bad_type(lua_State* L, int index)
{
  return strfmt("bad type '%s'", luaL_typename(L, index));
}

char*
lua_msg_expected_type(lua_State* L, int index, int type)
{
  return strfmt(
    "%s expected, got %s",
    lua_typename(L, type),
    luaL_typename(L, index)
  );
}

char*
lua_msg_bad_arg(lua_State* L, int index, const char* details)
{
  lua_Debug callee_info;

  if (!lua_getstack(L, 0, &callee_info)) {
    return strfmt("bad argument #%d (%s)", index, details);
  }

  lua_getinfo(L, "n", &callee_info);

  return strfmt(
    "bad argument #%d to '%s' (%s)",
    index,
    callee_info.name ? callee_info.name : "?",
    details
  );
}

char*
lua_msg_bad_field(lua_State* L, const char* field, const char* details)
{
  return strfmt("bad field '%s' (%s)", field, details);
}

// -----------------------------------------------------------------------------
// Logging
// -----------------------------------------------------------------------------

void
lua_log(lua_State* L, enum LOG_LEVEL log_level, const char* format, ...)
{
  va_list varargs;
  va_start(varargs, format);
  char* message = strvfmt(format, varargs);
  va_end(varargs);

  lua_Debug caller_info;

  if (!lua_getstack(L, 1, &caller_info)) {
    log_log(log_level, "%s", message);
  } else {
    lua_getinfo(L, "Sl", &caller_info);

    log_log(
      log_level,
      "%s:%d: %s",
      caller_info.short_src,
      caller_info.currentline,
      message
    );
  }

  free(message);
}

void
lua_log_bad_arg_type(lua_State* L, int index)
{
  char* bad_type_msg = lua_msg_bad_type(L, index);
  char* bad_arg_msg = lua_msg_bad_arg(L, index, bad_type_msg);
  lua_log_error(L, "%s", bad_arg_msg);
  free(bad_arg_msg);
  free(bad_type_msg);
}

void
lua_log_expected_arg_type(lua_State* L, int index, int type)
{
  char* expected_type_msg = lua_msg_expected_type(L, index, type);
  char* bad_arg_msg = lua_msg_bad_arg(L, index, expected_type_msg);
  lua_log_error(L, "%s", bad_arg_msg);
  free(bad_arg_msg);
  free(expected_type_msg);
}

void
lua_log_bad_field_type(lua_State* L, int index, const char* field)
{
  lua_getfield(L, index, field);
  char* bad_type_msg = lua_msg_bad_type(L, -1);
  char* bad_field_msg = lua_msg_bad_field(L, field, bad_type_msg);
  lua_log_error(L, "%s", bad_field_msg);
  free(bad_field_msg);
  free(bad_type_msg);
  lua_pop(L, 1);
}

void
lua_log_expected_field_type(
  lua_State* L,
  int index,
  const char* field,
  int type
)
{
  lua_getfield(L, index, field);
  char* expected_type_msg = lua_msg_expected_type(L, -1, type);
  char* bad_field_msg = lua_msg_bad_field(L, field, expected_type_msg);
  lua_log_error(L, "%s", bad_field_msg);
  free(bad_field_msg);
  free(expected_type_msg);
  lua_pop(L, 1);
}
