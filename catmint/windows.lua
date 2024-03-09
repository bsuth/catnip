local catnip = require('catmint.catnip')
local geometry = require('catmint.geometry')

local M = {}

--- @return CatnipWindow | nil
function M.get_focused_window()
  for _, window in pairs(catnip.windows) do
    if window.focused then
      return window
    end
  end
end

--- @return CatnipWindow[]
function M.get_visible_windows()
  local visible_windows = {}

  for _, window in pairs(catnip.windows) do
    if window.visible then
      table.insert(visible_windows, window)
    end
  end

  return visible_windows
end

--- @param direction Direction
--- @param window? CatnipWindow
--- @return CatnipWindow | nil
function M.get_window_in_direction(direction, window)
  window = window or M.get_focused_window()

  if window == nil then
    return nil
  end

  -- NOTE: `geometry.get_box_in_direction` returns a reference to the original
  -- box, but currently LuaCATS does not support generics very well so for now
  -- we have to cast.
  --
  -- see https://github.com/LuaLS/lua-language-server/issues/1861
  return geometry.get_box_in_direction(
    window,
    M.get_visible_windows(),
    direction
  ) --- @type CatnipWindow
end

--- @param direction Direction
--- @param window? CatnipWindow
function M.focus_window_in_direction(direction, window)
  window = window or M.get_focused_window()

  if window == nil then
    return
  end

  local new_focused_window = M.get_window_in_direction(direction, window)

  if new_focused_window == nil then
    return
  end

  new_focused_window.focused = true
end

--- @param direction Direction
--- @param window? CatnipWindow
function M.swap_window_in_direction(direction, window)
  window = window or M.get_focused_window()

  if window == nil then
    return
  end

  local swap_window = M.get_window_in_direction(direction, window)

  if swap_window == nil then
    return
  end

  geometry.swap_boxes(window, swap_window)
end

return M
