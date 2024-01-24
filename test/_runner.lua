local lfs = require('lfs')
local uv = require('luv')

local UV_ENV_LUA_PATH = 'LUA_PATH=' .. os.getenv('LUA_PATH')
local UV_ENV_LUA_CPATH = 'LUA_CPATH=' .. os.getenv('LUA_CPATH')
local UV_ENV_XDG_RUNTIME_DIR = 'XDG_RUNTIME_DIR=' .. os.getenv('XDG_RUNTIME_DIR')
local UV_ENV_WLR_BACKENDS = 'WLR_BACKENDS=headless'

for filename in lfs.dir('test') do
  if filename:match('^[^_].*%.lua$') then
    local test_result_path = os.tmpname()

    -- https://github.com/luvit/luv/blob/master/docs.md#uvspawnpath-options-on_exit
    uv.spawn(
      'build/catnip',
      {
        args = { '-c', 'test/' .. filename },
        env = {
          UV_ENV_LUA_PATH,
          UV_ENV_LUA_CPATH,
          UV_ENV_XDG_RUNTIME_DIR,
          UV_ENV_WLR_BACKENDS,
          'CATNIP_TEST_RESULT_PATH=' .. test_result_path,
        },
      },
      function()
        local test_result_file = assert(io.open(test_result_path, 'r'))
        print(('%s: %s'):format(filename, assert(test_result_file:read('*a'))))
        test_result_file:close()
        os.remove(test_result_path)
      end
    )
  end
end

uv.run()
