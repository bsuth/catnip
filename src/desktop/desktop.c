#include "desktop.h"
#include "desktop/monitor.h"
#include "desktop/window.h"

void
init_desktop()
{
  init_desktop_windows();
  init_desktop_monitors();
}
