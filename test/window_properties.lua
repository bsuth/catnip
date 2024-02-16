local catnip = require('catnip')
local uv = require('luv')
local test = require('_setup')

local timer = uv.new_timer()

os.execute('foot & disown')

catnip.subscribe('window::create', function()
  test.pass()
end)

timer:start(1000, 0, function()
  test.fail('no window')
end)
