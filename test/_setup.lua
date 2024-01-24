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

local tests = {}
local num_tests = 0
local num_resolved_tests = 0

local function format(status, id, message)
  local parts = { status }

  if id ~= nil then
    table.insert(parts, '[' .. id .. ']')
  end

  if message ~= nil then
    table.insert(parts, '(' .. message .. ')')
  end

  return table.concat(parts, ' ')
end

local function commit(status, id, message)
  assert(test_result_file:write(format(status, id, message)))
  test_result_file:close()
  os.exit() -- abort immediately
end

local function resolve(status, id, message)
  -- allow omitting ids and committing immediately for single tests
  if num_tests == 0 then
    commit(status, nil, id)
  end

  -- cannot resolve a nonexistent test
  if tests[id] == nil then
    commit('FAIL', id, 'INTERNAL ERROR NOEXIST')
  end

  -- cannot resolve a test that has already been resolved
  if tests[id] ~= 'pending' then
    commit('FAIL', id, 'INTERNAL ERROR RERESOLVE')
  end

  -- immediately commit on first failure
  if status == 'FAIL' then
    commit('FAIL', id, message)
  end

  tests[id] = status
  num_resolved_tests = num_resolved_tests + 1

  if num_resolved_tests == num_tests then
    commit('PASS')
  end
end

local function fail(...)
  resolve('FAIL', ...)
end

local function pass(...)
  resolve('PASS', ...)
end

local function register(id)
  tests[id] = 'pending'
  num_tests = num_tests + 1
end

-- -----------------------------------------------------------------------------
-- Globals
-- -----------------------------------------------------------------------------

_G.catnip = catnip
_G.uv = uv
_G.register = register
_G.fail = fail
_G.pass= pass
