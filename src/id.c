#include "id.h"

static int catnip_id_counter = 0;

int
generate_catnip_id()
{
  return catnip_id_counter++;
}
