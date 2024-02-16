local catnip = require('catnip')
local uv = require('luv')

-- -----------------------------------------------------------------------------
-- LibUV
-- -----------------------------------------------------------------------------

catnip.subscribe('tick', function()
  uv.run('nowait')
end)

-- -----------------------------------------------------------------------------
-- Tests
-- -----------------------------------------------------------------------------

local test_result_file = assert(io.open(os.getenv('CATNIP_TEST_RESULT_PATH'), 'w'))
local num_tests = 0
local num_pass_tests = 0

local function format(success, id, message)
  local parts = { success and 'PASS' or 'FAIL' }

  if id ~= nil then
    table.insert(parts, '[' .. id .. ']')
  end

  if message ~= nil then
    table.insert(parts, '(' .. message .. ')')
  end

  return table.concat(parts, ' ')
end

local function commit(success, id, message)
  test_result_file:write(format(success, id, message))
  test_result_file:close()
  os.exit() -- abort immediately
end

-- -----------------------------------------------------------------------------
-- Timeout
-- -----------------------------------------------------------------------------

local timer = uv.new_timer()

timer:start(10000, 0, function()
  commit(false, nil, 'timeout')
end)

-- -----------------------------------------------------------------------------
-- Return
-- -----------------------------------------------------------------------------

return {
  fail = function(message)
    commit(false, nil, message)
  end,

  pass = function()
    commit(true)
  end,

  new = function()
    local test = {}

    num_tests = num_tests + 1
    test.id = num_tests

    function test:fail(message)
      commit(false, self.id, message)
    end

    function test:pass(message)
      num_pass_tests = num_pass_tests + 1
      if num_pass_tests == num_tests then
        commit(true)
      end
    end

    return test
  end,
}
