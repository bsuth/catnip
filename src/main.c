#include "config/config.h"
#include "meta.h"
#include "server/server.h"
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  // TODO CLI (getopt)

  wlr_log_init(WLR_LOG_LEVEL, NULL);

  server_init();
  config_init();

  server_start();

  return 0;
}
