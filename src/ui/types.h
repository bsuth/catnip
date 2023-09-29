#ifndef CATNIP_UI_TYPES_H
#define CATNIP_UI_TYPES_H

enum UI_SIZE_TYPE {
  UI_SIZE_FIT,
  UI_SIZE_PERCENT,
  UI_SIZE_PX,
  // TODO: allow function to calculate height?
  // UI_SIZE_CALLBACK,
};

struct ui_color {
  int r;
  int g;
  int b;
  int a;
};

#endif
