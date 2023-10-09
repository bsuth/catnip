#include "properties.h"

int
compute_ui_dimension(struct ui_dimension* dimension)
{
  if (dimension->type == UI_DIMENSION_TYPE_PX) {
    return dimension->value;
  } else {
    return 0;
  }
}
