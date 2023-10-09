#ifndef CATNIP_UI_PROPERTIES_H
#define CATNIP_UI_PROPERTIES_H

enum UI_DIMENSION_TYPE {
  UI_DIMENSION_TYPE_AUTO,
  UI_DIMENSION_TYPE_FIT,
  UI_DIMENSION_TYPE_PERCENT,
  UI_DIMENSION_TYPE_PX,
  // TODO: allow function to calculate width?
  // UI_DIM_TYPE_CALLBACK,
};

struct ui_dimension {
  int value;
  enum UI_DIMENSION_TYPE type;
};

#endif
