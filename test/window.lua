local catnip = require('catnip')
local uv = require('luv')
local test = require('_setup')

local window_created = test.new()
local timed_out = test.new()

local timer = uv.new_timer()

os.execute('foot & disown')

catnip.subscribe('window::create', function()
  window_created:pass()
end)

timer:start(1000, 0, function()
  timed_out:fail('mymessage')
end)
