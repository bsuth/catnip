#include "server/desktop/desktop.h"
#include "server/ipc/ipc.h"
#include "server/server/display.h"
#include "server/server/server.h"
#include "server/user_config/user_config.h"
#include <wlr/util/log.h>

int
main(int argc, char* argv[])
{
  wlr_log_init(WLR_DEBUG, NULL);

  // TODO CLI (getopt)

  init_server();
  init_desktop();
  init_user_config();
  init_ipc();

  start_server();

  return 0;
}
