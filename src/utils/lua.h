#ifndef CATNIP_UTILS_LUA_H
#define CATNIP_UTILS_LUA_H

#include <lua.h>
#include <stdbool.h>

typedef int lua_Ref;

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
// Stack Pulling
// -----------------------------------------------------------------------------

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
// Error Messages
// -----------------------------------------------------------------------------

char*
lua_error_msg(lua_State* L, const char* details);

char*
lua_error_msg_bad_type(lua_State* L, int index);

char*
lua_error_msg_expected_type(lua_State* L, int index, int expected_type);

char*
lua_arg_error_msg(lua_State* L, int arg_index, const char* details);

char*
lua_arg_error_msg_bad_type(lua_State* L, int arg_index);

char*
lua_arg_error_msg_expected_type(lua_State* L, int arg_index, int expected_type);

char*
lua_field_error_msg(lua_State* L, const char* field, const char* details);

char*
lua_field_error_msg_bad_type(lua_State* L, const char* field, int value_index);

char*
lua_field_error_msg_expected_type(
  lua_State* L,
  const char* field,
  int value_index,
  int expected_type
);

#endif
