#include "widget_block.h"
#include "config.h"
#include "extensions/cairo.h"
#include "widget/widget_base.h"
#include <lauxlib.h>
#include <stdbool.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct catnip_widget_block*
catnip_widget_block_create()
{
  struct catnip_widget_block* block =
    calloc(1, sizeof(struct catnip_widget_block));

  lua_newtable(catnip_L);
  block->children = luaL_ref(catnip_L, LUA_REGISTRYINDEX);

  block->styles.bg_color = -1;
  block->styles.bg_opacity = 1;

  block->styles.border_color = -1;
  block->styles.border_opacity = 1;
  block->styles.border_width = 1;

  block->styles.radius_top_left = -1;
  block->styles.radius_top_right = -1;
  block->styles.radius_bottom_left = -1;
  block->styles.radius_bottom_right = -1;

  return block;
}

void
catnip_widget_block_destroy(struct catnip_widget_block* block)
{
  // TODO
  // lua_pushnil(catnip_L);
  // while (lua_next(catnip_L, 2) != 0) {
  //   struct catnip_widget_base* child = lua_touserdata(catnip_L, -1);
  //   child->root = NULL;
  //   child->parent = NULL;
  //   lua_pop(catnip_L, 1);
  // }
  luaL_unref(catnip_L, LUA_REGISTRYINDEX, block->children);

  free(block);
}

void
catnip_widget_block_insert(struct catnip_widget_block* block)
{
}

void
catnip_widget_block_remove(struct catnip_widget_block* block)
{
}

// -----------------------------------------------------------------------------
// Layout
// -----------------------------------------------------------------------------

static void
catnip_widget_block_pre_layout(struct catnip_widget_block* block)
{
}

static void
catnip_widget_block_post_layout(struct catnip_widget_block* block)
{
}

void
catnip_widget_block_layout(struct catnip_widget_block* block)
{
  catnip_widget_block_pre_layout(block);

  // TODO
  block->computed.x = block->styles.x;
  block->computed.y = block->styles.y;
  block->computed.width = block->styles.width;
  block->computed.height = block->styles.height;

  catnip_widget_block_post_layout(block);
}

// -----------------------------------------------------------------------------
// Draw
// -----------------------------------------------------------------------------

void
catnip_widget_block_draw(struct catnip_widget_block* block, cairo_t* cr)
{
  cairo_save(cr);

  int radius_top_left = block->styles.radius_top_left == -1
    ? block->styles.radius
    : block->styles.radius_top_left;
  int radius_top_right = block->styles.radius_top_right == -1
    ? block->styles.radius
    : block->styles.radius_top_right;
  int radius_bottom_right = block->styles.radius_bottom_right == -1
    ? block->styles.radius
    : block->styles.radius_bottom_right;
  int radius_bottom_left = block->styles.radius_bottom_left == -1
    ? block->styles.radius
    : block->styles.radius_bottom_left;

  bool is_rounded = radius_top_left != 0 || radius_top_right != 0
    || radius_bottom_right != 0 || radius_bottom_left != 0;

  if (block->styles.bg_color != -1 && block->styles.bg_opacity > 0) {
    cairo_set_source_hexa(cr, block->styles.bg_color, block->styles.bg_opacity);

    if (is_rounded) {
      cairo_rounded_rectangle(
        cr,
        block->computed.x,
        block->computed.y,
        block->computed.width,
        block->computed.height,
        radius_top_left,
        radius_top_right,
        radius_bottom_right,
        radius_bottom_left
      );
    } else {
      cairo_rectangle(
        cr,
        block->computed.x,
        block->computed.y,
        block->computed.width,
        block->computed.height
      );
    }

    cairo_fill(cr);
  }

  if (block->styles.border_color != -1 && block->styles.border_opacity > 0
      && block->styles.border_width > 0) {
    double border_offset = (double) block->styles.border_width / 2;

    if (is_rounded) {
      cairo_rounded_rectangle(
        cr,
        block->computed.x + border_offset,
        block->computed.y + border_offset,
        block->computed.width - block->styles.border_width,
        block->computed.height - block->styles.border_width,
        radius_top_left,
        radius_top_right,
        radius_bottom_right,
        radius_bottom_left
      );
    } else {
      cairo_rectangle(
        cr,
        block->computed.x + border_offset,
        block->computed.y + border_offset,
        block->computed.width - block->styles.border_width,
        block->computed.height - block->styles.border_width
      );
    }

    cairo_set_line_width(cr, block->styles.border_width);
    cairo_set_source_hexa(
      cr,
      block->styles.border_color,
      block->styles.border_opacity
    );

    cairo_stroke(cr);
  }

  cairo_restore(cr);

  // TODO: inheritable properties
  // TODO: draw children
}
