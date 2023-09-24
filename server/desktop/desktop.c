#include "desktop.h"
#include "server/desktop/monitor.h"
#include "server/desktop/windows.h"

void
init_desktop()
{
  init_desktop_windows();
  init_desktop_monitors();
}
