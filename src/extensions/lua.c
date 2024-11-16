#include "lua.h"
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
    lua_pop(L, 1);
    return false;
  } else {
    return true;
  }
}

// -----------------------------------------------------------------------------
// Error Handling
// -----------------------------------------------------------------------------

int
luaL_pcall(lua_State* L, int nargs, int nresults)
{
  lua_getglobal(L, "debug");
  lua_getfield(L, -1, "traceback");
  lua_remove(L, -2);
  int errfunc = lua_gettop(L) - (nargs + 1);
  lua_insert(L, errfunc);
  int result = lua_pcall(L, nargs, nresults, errfunc);
  lua_remove(L, errfunc);
  return result;
}
