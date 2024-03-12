-- README
--
-- This module simply re-exports the core `catnip` module with LuaCATS
-- annotations. This is used over definition files to save the user from having
-- to deal with workspace settings, i.e. annotations should "just work".
--
-- Note that since the C core populates `path.loaded.catnip` when initializing
-- the lua_State, `require('catnip')` will always return the core C module and
-- _not_ this module. Other modules should prefer using
-- `require('catmint.catnip')` in order to get proper annnotations.

local catnip = require('catnip') --- @type Catnip
return catnip

--- @alias CatnipResourceList<T> { [number]: T } | fun(): T

--- @class (exact) Catnip
--- @field cursor CatnipCursor
--- @field keyboards CatnipResourceList<CatnipKeyboard>
--- @field outputs CatnipResourceList<CatnipOutput>
--- @field windows CatnipResourceList<CatnipWindow>
--- @field canvas fun(): CatnipCanvas
--- @field subscribe fun(event: string, callback: fun(...)): fun(...)
--- @field unsubscribe fun(event: string, callback: fun(...))
--- @field publish fun(event: string, ...)
--- @field reload fun()
--- @field quit fun()

--- @class (exact) CatnipCanvas
--- @field x number
--- @field y number
--- @field z number
--- @field width number
--- @field height number
--- @field visible boolean
--- @field clear fun(canvas: CatnipCanvas)
--- @field path fun(canvas: CatnipCanvas, path: CatnipCanvasPath)
--- @field png fun(canvas: CatnipCanvas, png: string, options: CatnipCanvasPngOptions?)
--- @field svg fun(canvas: CatnipCanvas, svg: string, options: CatnipCanvasSvgOptions?)
--- @field text fun(canvas: CatnipCanvas, text: string, options: CatnipCanvasTextOptions?)

--- @class (exact) CatnipCanvasPath
--- @field fill_color number?
--- @field fill_opacity number?
--- @field stroke_color number?
--- @field stroke_opacity number?
--- @field stroke_size number?
-- TODO: Support typing path commands?

--- @class (exact) CatnipCanvasPngOptions
--- @field x number?
--- @field y number?
--- @field width number?
--- @field height number?

--- @class (exact) CatnipCanvasSvgOptions
--- @field x number?
--- @field y number?
--- @field width number?
--- @field height number?
--- @field stylesheet string?

--- @class (exact) CatnipCanvasTextOptions
--- @field x number?
--- @field y number?
--- @field width number?
--- @field height number?
--- @field align ('left' | 'center' | 'right')?
--- @field color number?
--- @field ellipsis (boolean | 'start' | 'middle' | 'end')?
--- @field font string?
--- @field italic boolean?
--- @field opacity number?
--- @field size number?
--- @field weight number?
--- @field wrap (boolean | 'char' | 'word' | 'auto')?

--- @class (exact) CatnipCursor
--- @field x number
--- @field y number
--- @field name string
--- @field size number
--- @field theme string
--- @field subscribe fun(event: string, callback: fun(...)): fun(...)
--- @field unsubscribe fun(event: string, callback: fun(...))
--- @field publish fun(event: string, ...)

--- @class (exact) CatnipKeyboard
--- @field id number
--- @field name string
--- @field xkb_rules string?
--- @field xkb_model string?
--- @field xkb_layout string?
--- @field xkb_variant string?
--- @field xkb_options string?
--- @field subscribe fun(keyboard: CatnipKeyboard, event: string, callback: fun(...)): fun(...)
--- @field unsubscribe fun(keyboard: CatnipKeyboard, event: string, callback: fun(...))
--- @field publish fun(keyboard: CatnipKeyboard, event: string, ...)

--- @class (exact) CatnipOutput
--- @field id number
--- @field x number
--- @field y number
--- @field width number
--- @field height number
--- @field refresh number
--- @field mode CatnipOutputMode
--- @field modes CatnipResourceList<CatnipOutputMode>
--- @field scale number
--- @field subscribe fun(output: CatnipOutput, event: string, callback: fun(...)): fun(...)
--- @field unsubscribe fun(output: CatnipOutput, event: string, callback: fun(...))
--- @field publish fun(output: CatnipOutput, event: string, ...)

--- @class (exact) CatnipOutputMode
--- @field width number
--- @field height number
--- @field refresh number

--- @class (exact) CatnipWindow
--- @field id number
--- @field x number
--- @field y number
--- @field z number
--- @field width number
--- @field height number
--- @field visible boolean
--- @field focused boolean
--- @field fullscreen boolean
--- @field maximized boolean
--- @field subscribe fun(window: CatnipWindow, event: string, callback: fun(...)): fun(...)
--- @field unsubscribe fun(window: CatnipWindow, event: string, callback: fun(...))
--- @field publish fun(window: CatnipWindow, event: string, ...)
--- @field destroy fun(window: CatnipWindow)
