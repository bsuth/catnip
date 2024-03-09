--- @alias Point { x: number, y: number }
--- @alias Box { x: number, y: number, width: number, height: number }
--- @alias Direction 'left' | 'right' | 'up' | 'down'

local M = {}

--- @param box Box
--- @return Point
function M.get_box_center(box)
  return {
    x = box.x + box.width / 2,
    y = box.y + box.height / 2,
  }
end

--- @param point Point
--- @param box Box
--- @return boolean
function M.is_point_in_box(point, box)
  return not (
    point.x < box.x or
    point.x > box.x + box.width or
    point.y < box.y or
    point.y > box.y + box.height
  )
end

--- @param inner_box Box
--- @param outer_box Box
--- @return boolean
function M.is_box_in_box(inner_box, outer_box)
  return not (
    inner_box.x < outer_box.x or
    inner_box.x + inner_box.width > outer_box.x + outer_box.width or
    inner_box.y < outer_box.y or
    inner_box.y + inner_box.height > outer_box.y + outer_box.height
  )
end

--- @param box1 Box
--- @param box2 Box
function M.swap_boxes(box1, box2)
  local tmp = {
    x = box1.x,
    y = box1.y,
    width = box1.width,
    height = box1.height,
  }

  box1.x = box2.x
  box1.y = box2.y
  box1.width = box2.width
  box1.height = box2.height

  box2.x = tmp.x
  box2.y = tmp.y
  box2.width = tmp.width
  box2.height = tmp.height
end

--- @param box Box
--- @param other_boxes { [any]: Box }
--- @param direction Direction
--- @return Box | nil
function M.get_box_in_direction(box, other_boxes, direction)
  local closest_box --- @type Box | nil
  local closest_box_distance = math.huge

  for _, other_box in pairs(other_boxes) do
    local distance --- @type false | number

    if direction == 'left' then
      distance = (
        other_box.x < box.x and
        other_box.y < box.y + box.height and
        other_box.y + other_box.height > box.y and
        box.x - other_box.x
      )
    elseif direction == 'right' then
      distance = (
        other_box.x + other_box.width > box.x + box.width and
        other_box.y < box.y + box.height and
        other_box.y + other_box.height > box.y and
        (other_box.x + other_box.width) - (box.x + box.width)
      )
    elseif direction == 'up' then
      distance = (
        other_box.y < box.y and
        other_box.x < box.x + box.width and
        other_box.x + other_box.width > box.x and
        box.y - other_box.y
      )
    elseif direction == 'down' then
      distance = (
        other_box.y + other_box.height > box.y + box.height and
        other_box.x < box.x + box.width and
        other_box.x + other_box.width > box.x and
        (other_box.y + other_box.height) - (box.y + box.height)
      )
    end

    if distance and distance < closest_box_distance then
      closest_box = other_box
      closest_box_distance = distance
    end
  end

  return closest_box
end

return M
