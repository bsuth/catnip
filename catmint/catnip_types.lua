--- @class CatnipCanvasPath
--- @field fill_color number?
--- @field fill_opacity number?
--- @field stroke_color number?
--- @field stroke_opacity number?
--- @field stroke_size number?
-- src/canvas/lua_canvas_path.c
-- TODO: Support typing path commands?

--- @class CatnipCanvasTextOptions
--- @field x number?
--- @field y number?
--- @field width number?
--- @field height number?
--- @field font string?
--- @field size number?
--- @field weight number?
--- @field italic boolean?
--- @field color number?
--- @field opacity number?
--- @field align ('left' | 'center' | 'right')?
--- @field ellipsis (boolean | 'start' | 'middle' | 'end')?
--- @field wrap (boolean | 'char' | 'word' | 'auto')?
-- src/canvas/lua_canvas_text.c

--- @class CatnipCanvasPngOptions
--- @field x number?
--- @field y number?
--- @field width number?
--- @field height number?
-- src/canvas/lua_canvas_png.c

--- @class CatnipCanvasSvgOptions
--- @field x number?
--- @field y number?
--- @field width number?
--- @field height number?
--- @field stylesheet string?
-- src/canvas/lua_canvas_svg.c

--- @class CatnipCanvas
--- @field x number
--- @field y number
--- @field z number
--- @field width number
--- @field height number
--- @field visible boolean
--- @field clear fun(canvas: CatnipCursor)
--- @field path fun(canvas: CatnipCursor, path: CatnipCanvasPath)
--- @field text fun(canvas: CatnipCursor, text: string, options: CatnipCanvasTextOptions?)
--- @field svg fun(canvas: CatnipCursor, svg: string, options: CatnipCanvasSvgOptions?)
--- @field png fun(canvas: CatnipCursor, png: string, options: CatnipCanvasPngOptions?)
-- src/canvas/lua_canvas.c

--- @class CatnipCursor
--- @field x number
--- @field y number
--- @field name string
--- @field size number
--- @field theme string
--- @field subscribe fun(event: string, callback: fun(...)): fun(...)
--- @field unsubscribe fun(event: string, callback: fun(...))
--- @field publish fun(event: string, ...)
--  src/cursor/lua_cursor.c

--- @class CatnipKeyboard
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
-- src/keyboard/lua_keyboard.c

--- @class CatnipOutputMode
--- @field width number
--- @field height number
--- @field refresh number
-- src/output/lua_output_mode.c

--- @class CatnipOutput
--- @field id number
--- @field x number
--- @field y number
--- @field width number
--- @field height number
--- @field refresh number
--- @field mode CatnipOutputMode
--- @field modes CatnipOutputMode[]
--- @field scale number
--- @field subscribe fun(output: CatnipOutput, event: string, callback: fun(...)): fun(...)
--- @field unsubscribe fun(output: CatnipOutput, event: string, callback: fun(...))
--- @field publish fun(output: CatnipOutput, event: string, ...)
-- src/output/lua_output.c

--- @class CatnipWindow
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
-- src/window/lua_window.c
