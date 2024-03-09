local catnip = require('catmint.catnip')
local geometry = require('catmint.geometry')

local M = {}

--- @return CatnipOutput | nil
function M.get_focused_output()
  for _, output in pairs(catnip.outputs) do
    if geometry.is_point_in_box(catnip.cursor, output) then
      return output
    end
  end
end

--- @param direction Direction
--- @param output? CatnipOutput
--- @return CatnipOutput | nil
function M.get_output_in_direction(direction, output)
  output = output or M.get_focused_output()

  if output == nil then
    return nil
  end

  -- NOTE: `geometry.get_box_in_direction` returns a reference to the original
  -- box, but currently LuaCATS does not support generics very well so for now
  -- we have to cast.
  --
  -- see https://github.com/LuaLS/lua-language-server/issues/1861
  return geometry.get_box_in_direction(
    output,
    catnip.outputs,
    direction
  ) --- @type CatnipWindow
end

--- @param direction Direction
--- @param output? CatnipOutput
function M.focus_output_in_direction(direction, output)
  output = output or M.get_focused_output()

  if output == nil then
    return
  end

  local new_focused_output = M.get_output_in_direction(direction, output)

  if new_focused_output == nil then
    return
  end

  local new_focused_output_center = geometry.get_box_center(new_focused_output)
  catnip.cursor.x = new_focused_output_center.x
  catnip.cursor.y = new_focused_output_center.y
end

--- @param direction Direction
--- @param output? CatnipOutput
function M.swap_output_in_direction(direction, output)
  output = output or M.get_focused_output()

  if output == nil then
    return
  end

  local swap_output = M.get_output_in_direction(direction, output)

  if swap_output == nil then
    return
  end

  geometry.swap_boxes(output, swap_output)
end

return M
