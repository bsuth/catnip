#include "widget_base.h"
#include "widget/widget_root.h"
#include <stdio.h>

void
catnip_widget_base_request_render(struct catnip_widget_base* base)
{
  if (base->root != NULL) {
    catnip_widget_root_request_render(base->root);
  }
}
