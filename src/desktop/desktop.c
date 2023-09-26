#include "desktop.h"
#include "desktop/monitor.h"
#include "desktop/windows.h"

void
init_desktop()
{
  init_desktop_windows();
  init_desktop_monitors();
}
