local catnip = require('catnip')

catnip.bind('Control', 'w', function()
  local fields = {
    'lx',
    'ly',
    'gx',
    'gy',
    'width',
    'height',
    'focused',
    'maximized',
    'fullscreen',
  }

  for i = 1, #catnip.windows do
    local window = catnip.windows[i]

    print('window ' .. i);
    print('--------');

    for _, field in ipairs(fields) do
      print(field .. ': ' .. tostring(window[field]))
    end
  end
end)

catnip.bind('Control', 'k', function()
  for i = 1, #catnip.windows do
    local window = catnip.windows[i]
    window.focused = true
    window.lx = window.lx + 10
    window.ly = window.ly + 10
  end
end)

catnip.bind('Control', 'a', function()
  catnip.cursor.lx = 20
  catnip.cursor.ly = 20
  os.execute("foot & disown")
end)
