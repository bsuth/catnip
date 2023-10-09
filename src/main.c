#include "config/config.h"
#include "meta.h"
#include "server/display.h"
#include "server/server.h"
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  wlr_log_init(WLR_LOG_LEVEL, NULL);

  // TODO CLI (getopt)

  init_server();
  init_config();

  start_server();

  return 0;
}
