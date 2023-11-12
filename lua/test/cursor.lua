local catnip = require('catnip')

catnip.bind('Control', 'c', function()
  local fields = {
    'lx',
    'ly',
    'gx',
    'gy',
  }

  print('cursor');
  print('------');

  for _, field in ipairs(fields) do
    print(field .. ': ' .. tostring(catnip.cursor[field]))
  end
end)
