#include "server.h"
#include "config/events.h"
#include "server/display.h"

void
server_stop()
{
  config_events_publish("quit");
  wl_display_terminate(server_display);
}
