#include "id.h"

// DO NOT USE 0 AS AN ID.
//
// Lua's `__index` metamethod always converts the key to a string, forcing us
// to use `lua_tonumber` to convert any ids back to numbers. However, since
// `lua_tonumber` returns 0 to indicate that the conversion failed, we cannot
// use 0 as an id itself in order to avoid ambiguity.
static int catnip_id_counter = 1;

int
generate_catnip_id()
{
  return catnip_id_counter++;
}
