#ifndef CATNIP_UTILS_LUA_H
#define CATNIP_UTILS_LUA_H

#include "utils/log.h"
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
// Messages
// -----------------------------------------------------------------------------

char*
lua_msg_bad_type(lua_State* L, int index);

char*
lua_msg_expected_type(lua_State* L, int index, int type);

char*
lua_msg_bad_arg(lua_State* L, int index, const char* details);

char*
lua_msg_bad_field(lua_State* L, const char* field, const char* details);

// -----------------------------------------------------------------------------
// Logging
// -----------------------------------------------------------------------------

void
lua_log(lua_State* L, enum LOG_LEVEL log_level, const char* format, ...);

#define lua_log_error(L, ...) lua_log(L, LOG_LEVEL_ERROR, __VA_ARGS__)
#define lua_log_warning(L, ...) lua_log(L, LOG_LEVEL_WARNING, __VA_ARGS__)
#define lua_log_info(L, ...) lua_log(L, LOG_LEVEL_INFO, __VA_ARGS__)
#define lua_log_debug(L, ...) lua_log(L, LOG_LEVEL_DEBUG, __VA_ARGS__)

void
lua_log_bad_arg_type(lua_State* L, int index);

void
lua_log_expected_arg_type(lua_State* L, int index, int type);

void
lua_log_bad_field_type(lua_State* L, int index, const char* field);

void
lua_log_expected_field_type(
  lua_State* L,
  int index,
  const char* field,
  int type
);

#endif
