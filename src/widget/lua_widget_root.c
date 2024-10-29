#include "lua_widget_root.h"
#include "compositor/event_loop.h"
#include "compositor/scene.h"
#include "config.h"
#include "extensions/string.h"
#include "widget/lua_widget_block.h"
#include <drm_fourcc.h>
#include <lauxlib.h>
#include <wlr/interfaces/wlr_buffer.h>

// -----------------------------------------------------------------------------
// Buffer
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_root_buffer_destroy(struct wlr_buffer* wlr_buffer)
{
}

static bool
catnip_lua_widget_root_buffer_begin_data_ptr_access(
  struct wlr_buffer* wlr_buffer,
  uint32_t flags,
  void** data,
  uint32_t* format,
  size_t* stride
)
{
  struct catnip_lua_widget_root* root =
    wl_container_of(wlr_buffer, root, wlr.buffer);

  *data = (void*) cairo_image_surface_get_data(root->cairo.surface);
  *format = DRM_FORMAT_ARGB8888;
  *stride = cairo_image_surface_get_stride(root->cairo.surface);

  return true;
}

static void
catnip_lua_widget_root_buffer_end_data_ptr_access(struct wlr_buffer* wlr_buffer)
{
}

const struct wlr_buffer_impl catnip_lua_widget_root_buffer = {
  .destroy = catnip_lua_widget_root_buffer_destroy,
  .begin_data_ptr_access = catnip_lua_widget_root_buffer_begin_data_ptr_access,
  .end_data_ptr_access = catnip_lua_widget_root_buffer_end_data_ptr_access,
};

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_widget_root__index(lua_State* L)
{
  struct catnip_lua_widget_root* root = lua_touserdata(L, 1);

  if (lua_type(L, 2) == LUA_TNUMBER) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, root->block_ref);
    lua_pushvalue(L, 2);
    lua_gettable(L, -2);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    lua_pushnil(L);
  } else if (streq(key, "type")) {
    lua_pushliteral(L, "root");
  } else if (streq(key, "x")) {
    lua_pushnumber(L, root->wlr.scene_buffer->node.x);
  } else if (streq(key, "y")) {
    lua_pushnumber(L, root->wlr.scene_buffer->node.y);
  } else if (streq(key, "width")) {
    lua_pushnumber(L, root->wlr.buffer.width);
  } else if (streq(key, "height")) {
    lua_pushnumber(L, root->wlr.buffer.width);
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, root->wlr.scene_buffer->node.enabled);
  } else {
    lua_rawgeti(L, LUA_REGISTRYINDEX, root->block_ref);
    lua_pushvalue(L, 2);
    lua_gettable(L, -2);
  }

  return 1;
}

static int
catnip_lua_widget_root__newindex(lua_State* L)
{
  struct catnip_lua_widget_root* root = lua_touserdata(L, 1);

  if (lua_type(L, 2) == LUA_TNUMBER) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, root->block_ref);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_settable(L, -3);
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (key == NULL) {
    return 0;
  }

  if (streq(key, "x")) {
    wlr_scene_node_set_position(
      &root->wlr.scene_buffer->node,
      luaL_checkinteger(L, 3),
      root->wlr.scene_buffer->node.y
    );
  } else if (streq(key, "y")) {
    wlr_scene_node_set_position(
      &root->wlr.scene_buffer->node,
      root->wlr.scene_buffer->node.x,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "width")) {
    catnip_lua_widget_root_request_resize(
      root,
      luaL_checkinteger(L, 3),
      root->request.height
    );
  } else if (streq(key, "height")) {
    catnip_lua_widget_root_request_resize(
      root,
      root->request.width,
      luaL_checkinteger(L, 3)
    );
  } else if (streq(key, "visible")) {
    wlr_scene_node_set_enabled(
      &root->wlr.scene_buffer->node,
      lua_toboolean(L, 3)
    );
  } else {
    lua_rawgeti(L, LUA_REGISTRYINDEX, root->block_ref);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_settable(L, -3);
  }

  return 0;
}

static int
catnip_lua_widget_root__gc(lua_State* L)
{
  struct catnip_lua_widget_root* root = lua_touserdata(L, 1);

  luaL_unref(L, LUA_REGISTRYINDEX, root->block_ref);

  cairo_surface_destroy(root->cairo.surface);
  cairo_destroy(root->cairo.cr);

  wlr_buffer_drop(&root->wlr.buffer);
  wlr_scene_node_destroy(&root->wlr.scene_buffer->node);

  if (root->request.event_source != NULL) {
    wl_event_source_remove(root->request.event_source);
  }

  catnip_lua_widget_base_cleanup(L, &root->base);

  return 0;
}

static const struct luaL_Reg catnip_lua_widget_root_mt[] = {
  {"__index", catnip_lua_widget_root__index},
  {"__newindex", catnip_lua_widget_root__newindex},
  {"__gc", catnip_lua_widget_root__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_widget_root_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.widget.root");
  luaL_setfuncs(L, catnip_lua_widget_root_mt, 0);
  lua_pop(L, 1);
}

int
catnip_lua_widget_lua_root(lua_State* L)
{
  struct catnip_lua_widget_root* root =
    lua_newuserdata(L, sizeof(struct catnip_lua_widget_root));
  luaL_setmetatable(L, "catnip.widget.root");

  catnip_lua_widget_base_setup(L, &root->base);
  root->base.type = CATNIP_LUA_WIDGET_ROOT;

  bool has_init_table = lua_type(L, 1) == LUA_TTABLE;

  int initial_width =
    has_init_table && lua_hasnumberfield(L, 1, "width") ? lua_popnumber(L) : 0;
  int initial_height =
    has_init_table && lua_hasnumberfield(L, 1, "height") ? lua_popnumber(L) : 0;

  root->cairo.surface = cairo_image_surface_create(
    CAIRO_FORMAT_ARGB32,
    initial_width,
    initial_height
  );

  root->cairo.cr = cairo_create(root->cairo.surface);

  wlr_buffer_init(
    &root->wlr.buffer,
    &catnip_lua_widget_root_buffer,
    initial_width,
    initial_height
  );

  root->wlr.scene_buffer =
    wlr_scene_buffer_create(&catnip_scene->tree, &root->wlr.buffer);

  root->request.event_source = NULL;
  root->request.width = initial_width;
  root->request.height = initial_height;
  root->request.layout = false;
  root->request.draw = false;

  if (has_init_table) {
    wlr_scene_node_set_position(
      &root->wlr.scene_buffer->node,
      lua_hasnumberfield(L, 1, "x") ? lua_popnumber(L) : 0,
      lua_hasnumberfield(L, 1, "y") ? lua_popnumber(L) : 0
    );

    wlr_scene_node_set_enabled(
      &root->wlr.scene_buffer->node,
      lua_hasbooleanfield(L, 1, "visible") ? lua_popboolean(L) : true
    );
  }

  catnip_lua_widget_lua_block(L);
  root->block = lua_touserdata(L, -1);
  root->block->base.parent = &root->base;
  root->block_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  root->block->base.bounding_box.x = 0;
  root->block->base.bounding_box.y = 0;
  root->block->base.bounding_box.width = initial_width;
  root->block->base.bounding_box.height = initial_height;

  catnip_lua_widget_root_request_layout(root);

  return 1;
}

// -----------------------------------------------------------------------------
// Refresh
// -----------------------------------------------------------------------------

static void
catnip_lua_widget_root_resize(
  struct catnip_lua_widget_root* root,
  int new_width,
  int new_height
)
{
  cairo_surface_destroy(root->cairo.surface);
  cairo_destroy(root->cairo.cr);

  // Cairo does not allow an image surface to be resized after creation. Thus,
  // we have to destroy any previous surfaces / contexts and create new ones.
  root->cairo.surface =
    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, new_width, new_height);
  root->cairo.cr = cairo_create(root->cairo.surface);

  root->wlr.buffer.width = new_width;
  root->wlr.buffer.height = new_height;

  root->block->base.bounding_box.width = new_width;
  root->block->base.bounding_box.height = new_height;

  // Unset buffer to force full update on refresh. Required to make sure that
  // widget->scene_buffer->node.visible is updated properly.
  wlr_scene_buffer_set_buffer_with_damage(root->wlr.scene_buffer, NULL, NULL);
}

static void
catnip_lua_widget_root_draw(lua_State* L, struct catnip_lua_widget_root* root)
{
  cairo_save(root->cairo.cr);
  cairo_set_operator(root->cairo.cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint(root->cairo.cr);
  cairo_restore(root->cairo.cr);

  catnip_lua_widget_block_draw(L, root->block, root->cairo.cr);

  wlr_scene_buffer_set_buffer_with_damage(
    root->wlr.scene_buffer,
    &root->wlr.buffer,
    NULL
  );
}

static void
catnip_lua_widget_root_refresh(void* data)
{
  struct catnip_lua_widget_root* root = data;

  bool needs_resize = root->request.width != root->wlr.buffer.width
    || root->request.height != root->wlr.buffer.height;

  if (needs_resize) {
    catnip_lua_widget_root_resize(
      root,
      root->request.width,
      root->request.height
    );
  }

  if (needs_resize || root->request.layout) {
    catnip_lua_widget_block_layout(catnip_L, root->block);
  }

  if (needs_resize || root->request.layout || root->request.draw) {
    catnip_lua_widget_root_draw(catnip_L, root);
  }

  root->request.layout = false;
  root->request.draw = false;
  root->request.event_source = NULL;
}

static void
catnip_widget_root_queue_refresh(struct catnip_lua_widget_root* root)
{
  if (root->request.event_source != NULL) {
    return; // already queued
  }

  root->request.event_source = wl_event_loop_add_idle(
    catnip_event_loop,
    catnip_lua_widget_root_refresh,
    root
  );
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void
catnip_lua_widget_root_request_resize(
  struct catnip_lua_widget_root* root,
  int new_width,
  int new_height
)
{
  root->request.width = new_width;
  root->request.height = new_height;
  catnip_widget_root_queue_refresh(root);
}

void
catnip_lua_widget_root_request_layout(struct catnip_lua_widget_root* root)
{
  root->request.layout = true;
  catnip_widget_root_queue_refresh(root);
}

void
catnip_lua_widget_root_request_draw(struct catnip_lua_widget_root* root)
{
  root->request.draw = true;
  catnip_widget_root_queue_refresh(root);
}
