require('_setup')

register('test1')
register('test2')

local timer = uv.new_timer()

os.execute('foot & disown')

catnip.subscribe('window::create', function()
  pass('test1')
end)

timer:start(1000, 0, function()
  fail('test2', 'mymessage')
end)
