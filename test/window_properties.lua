require('_setup')

local timer = uv.new_timer()

os.execute('foot & disown')

catnip.subscribe('window::create', function()
  pass()
end)

timer:start(1000, 0, function()
  fail('no window')
end)
