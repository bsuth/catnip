#include "server/server.h"
#include "user_config/user_config.h"
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  wlr_log_init(WLR_DEBUG, NULL);

  // TODO CLI (getopt)

  init_server();
  init_user_config();

  start_server();

  return 0;
}
