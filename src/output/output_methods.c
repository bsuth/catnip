#include "output_methods.h"

void
catnip_output_configure(
  struct catnip_output* output,
  int width,
  int height,
  int refresh
)
{
  wlr_output_set_custom_mode(
    output->wlr_output,
    width != -1 ? width
      : output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? output->wlr_output->pending.custom_mode.width
      : output->wlr_output->pending.mode != NULL
      ? output->wlr_output->pending.mode->width
      : output->wlr_output->width,
    height != -1 ? height
      : output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? output->wlr_output->pending.custom_mode.height
      : output->wlr_output->pending.mode != NULL
      ? output->wlr_output->pending.mode->height
      : output->wlr_output->height,
    refresh != -1 ? refresh
      : output->wlr_output->pending.mode_type == WLR_OUTPUT_STATE_MODE_CUSTOM
      ? output->wlr_output->pending.custom_mode.refresh
      : output->wlr_output->pending.mode != NULL
      ? output->wlr_output->pending.mode->refresh
      : output->wlr_output->refresh
  );
}
