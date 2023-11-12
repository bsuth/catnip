local catnip = require('catnip')

require('canvas')
require('cursor')
require('output')
require('window')

catnip.subscribe('reload', function()
  print('about to reload')
end)

catnip.bind('Control', 'r', function()
  catnip.publish('reload') -- fake reload
end)

catnip.bind('Alt', 'r', function()
  catnip.reload()
end)

catnip.bind('Alt', 'q', function()
  catnip.quit()
end)
