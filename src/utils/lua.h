#ifndef CATNIP_UTILS_LUA_H
#define CATNIP_UTILS_LUA_H

#include <lua.h>
#include <stdbool.h>

typedef int lua_Ref;

// -----------------------------------------------------------------------------
// Stack Utils
// -----------------------------------------------------------------------------

int
lua_popboolean(lua_State* L);

lua_CFunction
lua_popcfunction(lua_State* L);

lua_Integer
lua_popinteger(lua_State* L);

const char*
lua_poplstring(lua_State* L, size_t* len);

lua_Number
lua_popnumber(lua_State* L);

const void*
lua_poppointer(lua_State* L);

const char*
lua_popstring(lua_State* L);

lua_State*
lua_popthread(lua_State* L);

void*
lua_popuserdata(lua_State* L);

int
lua_pullboolean(lua_State* L, int index);

lua_CFunction
lua_pullcfunction(lua_State* L, int index);

lua_Integer
lua_pullinteger(lua_State* L, int index);

const char*
lua_pulllstring(lua_State* L, int index, size_t* len);

lua_Number
lua_pullnumber(lua_State* L, int index);

const void*
lua_pullpointer(lua_State* L, int index);

const char*
lua_pullstring(lua_State* L, int index);

lua_State*
lua_pullthread(lua_State* L, int index);

void*
lua_pulluserdata(lua_State* L, int index);

bool
lua_hasfield(lua_State* L, int index, const char* field);

// -----------------------------------------------------------------------------
// Error Messages
// -----------------------------------------------------------------------------

const char*
lua_get_arg_error_msg(lua_State* L, const int arg_index, const char* details);

const char*
lua_get_arg_type_error_msg(
  lua_State* L,
  int arg_index,
  const int expected_type
);

#endif
