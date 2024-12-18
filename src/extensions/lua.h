#ifndef CATNIP_EXTENSIONS_LUA_H
#define CATNIP_EXTENSIONS_LUA_H

#include <lua.h>
#include <stdbool.h>

typedef int lua_Ref;

// -----------------------------------------------------------------------------
// Debugging
// -----------------------------------------------------------------------------

void
lua_dumpstack(lua_State* L);

// -----------------------------------------------------------------------------
// Stack Popping
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

// -----------------------------------------------------------------------------
// Table Fields
// -----------------------------------------------------------------------------

bool
lua_hasfield(lua_State* L, int index, const char* field);

bool
lua_hasfieldtype(lua_State* L, int index, const char* field, int type);

#define lua_hasbooleanfield(L, index, field) \
  lua_hasfieldtype(L, index, field, LUA_TBOOLEAN)
#define lua_haslightdatafield(L, index, field) \
  lua_hasfieldtype(L, index, field, LUA_TLIGHTUSERDATA)
#define lua_hasnumberfield(L, index, field) \
  lua_hasfieldtype(L, index, field, LUA_TNUMBER)
#define lua_hasstringfield(L, index, field) \
  lua_hasfieldtype(L, index, field, LUA_TSTRING)
#define lua_hastablefield(L, index, field) \
  lua_hasfieldtype(L, index, field, LUA_TTABLE)
#define lua_hasfunctionfield(L, index, field) \
  lua_hasfieldtype(L, index, field, LUA_TFUNCTION)
#define lua_hasuserdatafield(L, index, field) \
  lua_hasfieldtype(L, index, field, LUA_TUSERDATA)
#define lua_hasthreadfield(L, index, field) \
  lua_hasfieldtype(L, index, field, LUA_TTHREAD)

// -----------------------------------------------------------------------------
// Error Handling
// -----------------------------------------------------------------------------

int
luaL_pcall(lua_State* L, int nargs, int nresults);

#endif
