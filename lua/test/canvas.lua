local catnip = require('catnip')

local function read_file(file)
  local f = io.open(file, 'r')
  if f == nil then return '' end
  local data = f:read('*a')
  f:close()
  return data
end

local red_svg = catnip.svg(read_file('test.svg'), [[ path { stroke: red }  ]])
local blue_svg = catnip.svg('test.svg', [[ path { stroke: blue } ]])
local test_png = catnip.png('test.png')

local test_canvas = catnip.canvas({
  width = 500,
  height = 700,
})

local counter = 1

catnip.bind('Control', 't', function()
  test_canvas:clear()
  counter = counter + 1

  if counter % 2 == 0 then
    test_canvas:rectangle(40, 10, 200, 100, {
      bg = 0xff0000,
      border = 0x0000ff,
      border_width = 4,
      radius = 4,
      opacity = 0.5,
    })
  else
    test_canvas:rectangle(20, 90, 50, 40, {
      bg = 0x00ff00,
    })
  end

  if counter % 2 == 0 then
    test_canvas:rectangle(200, 200, 50, 50, { bg = 0xff0000 })
  else
    test_canvas:rectangle(200, 200, 50, 50, {
      bg = 0xff0000,
      border = 0x0000ff,
      border_width = 20,
    })
  end

  if counter % 2 == 0 then
    test_canvas:text("<span color='#ff0000'>hello world</span>", {
      width = 50,
      ellipsis = "middle",
    })
  else
    test_canvas:text("<span color='#00ff00'>hello world</span>")
  end

  local svg_rect = { x = 20, y = 500, width = 50 }
  test_canvas:rectangle(svg_rect.x, svg_rect.y, svg_rect.width, 50, { bg = 0xFFFFFF })

  if counter % 2 == 0 then
    test_canvas:svg(red_svg, svg_rect)
  else
    test_canvas:svg(blue_svg, svg_rect)
  end
end)