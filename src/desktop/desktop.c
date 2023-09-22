#include "desktop.h"
#include "monitor.h"
#include "windows.h"

void
init_desktop()
{
  init_desktop_windows();
  init_desktop_monitors();
}
