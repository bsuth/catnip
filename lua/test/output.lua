local catnip = require('catnip')

catnip.bind('Control', 'o', function()
  local fields = {
    'x',
    'y',
    'width',
    'height',
    'refresh',
    'scale',
    'mode',
  }

  for i = 1, #catnip.outputs do
    local output = catnip.outputs[i]

    print('output ' .. i);
    print('--------');

    for _, field in ipairs(fields) do
      print(field .. ': ' .. tostring(output[field]))
    end
  end
end)
