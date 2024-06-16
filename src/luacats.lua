---@meta catnip

---@class CatnipResource
---A table for users to attach custom data to this resource.
---@field data table
---Registers a callback to be run whenever `event` is published from `self`.
---Returns the callback itself, which may be used to unsubscribe.
---Varargs passed to `self:publish(event, ...)` are available as arguments in `callback`.
---@field subscribe fun(self: self, event: string, callback: fun(...)): fun(...)
---Unregisters a previously subscribed callback.
---@field unsubscribe fun(self: self, event: string, callback: fun(...))
---Runs all callbacks registered via `self:subscribe(event, callback)`.
---The provided varargs are passed as arguments to each callback.
---@field publish fun(self: self, event: string, ...)

---@class (exact) Catnip
---The global cursor.
---@field cursor CatnipCursor
---An iterator to traverse through all currently available keyboards.
---@field keyboards fun(): CatnipKeyboard
---An iterator to traverse through all currently available outputs.
---@field outputs fun(): CatnipOutput
---An iterator to traverse through all currently available windows.
---@field windows fun(): CatnipWindow
---The currently focused window, or `nil` if no window is focused.
---@field focused CatnipWindow | nil
---Creates a new canvas.
---@field canvas fun(options: CatnipCanvasOptions?): CatnipCanvas
---Registers a callback to be run whenever `event` is published.
---Returns the callback itself, which may be used to unsubscribe.
---Varargs passed to `catnip.publish(event, ...)` are available as arguments in `callback`.
---@field subscribe fun(event: string, callback: fun(...)): fun(...)
---Unregisters a previously subscribed callback.
---@field unsubscribe fun(event: string, callback: fun(...))
---Runs all callbacks registered via `catnip.subscribe(event, callback)`.
---The provided varargs are passed as arguments to each callback.
---@field publish fun(event: string, ...)
---Queues the compositor to reload the user config after the current event loop
---tick has finished executing (i.e. after the next tick event).
---@field reload fun()
---Queues the compositor to terminate after the current event loop tick has finished executing (i.e. after the next `tick` event).
---@field quit fun()

---@class (exact) CatnipCursor: CatnipResource
---The x-coordinate of the cursor (in pixels).
---@field x number
---The y-coordinate of the cursor (in pixels).
---@field y number
---The [XCursor](https://man.archlinux.org/man/Xcursor.3) name to use for the cursor.
---This value should match a filename in the `cursors/` subdirectory of the current cursor theme.
---@field name string
---The size of the cursor (in pixels).
---@field size number
---The [XCursor theme](https://man.archlinux.org/man/Xcursor.3#THEMES) to use for the cursor.
---This value should match the directory name of the cursor theme.
---@field theme string

---@class (exact) CatnipKeyboard: CatnipResource
---(readonly) The keyboard ID. This is a unique number assigned to each keyboard that persists across reloads.
---@field id number
---The name of the keyboard.
---@field name string
---The [xkbcommon rules](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a0968f4602001f2306febd32c34bd2280) for the current keymap.
---@field xkb_rules string?
---The [xkbcommon model](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a1c897b49b49c7cd495db4a424bd27265) for the current keymap.
---@field xkb_model string?
---The [xkbcommon layout](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a243bb3aa36b4ea9ca402ed330a757746) for the current keymap.
---@field xkb_layout string?
---The [xkbcommon variant](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a758e1865c002d8f4fb59b2e3dda83b66) for the current keymap.
---@field xkb_variant string?
---The [xkbcommon options](https://xkbcommon.org/doc/current/structxkb__rule__names.html#a556899eafb333c440be64ede408644df) for the current keymap.
---@field xkb_options string?

---@class (exact) CatnipKeyEvent
---(readonly) The [xkbcommon keysym](https://xkbcommon.org/doc/current/group__keysyms.html#ga79e604a22703391bdfe212cfc10ea007).
---@field code number
---(readonly) An [xkbcommon key name](https://xkbcommon.org/doc/current/xkbcommon-keysyms_8h.html) with the `XKB_KEY_` prefix stripped.
---@field name string
---(readonly) Whether the shift modifier is active.
---@field shift boolean
---(readonly) Whether the control modifier is active.
---@field ctrl boolean
---(readonly) Whether the mod1 modifier is active.
---@field mod1 boolean
---(readonly) Whether the mod2 modifier is active.
---@field mod2 boolean
---(readonly) Whether the mod3 modifier is active.
---@field mod3 boolean
---(readonly) Whether the mod4 modifier is active.
---@field mod4 boolean
---(readonly) Whether the mod5 modifier is active.
---@field mod5 boolean
---Controls whether to notify the currently focused window about this key event.
---@field prevent_notify boolean

---@class (exact) CatnipOutput: CatnipResource
---(readonly) The output ID. This is a unique number assigned to each output that persists across reloads.
---@field id number
---The x-coordinate of the output (in pixels).
---@field x number
---The y-coordinate of the output (in pixels).
---@field y number
---The width of the output (in pixels).
---More specifically, this is the width of the window space viewport that is displayed on the output.
---@field width number
---The height of the output (in pixels).
---More specifically, this is the height of the window space viewport that is displayed on the output.
---@field height number
---The refresh rate of the output (in mHz).
---This value may be set to 0 in order for a default refresh rate to be determined automatically.
---@field refresh number
---The current output mode, or `nil` if a custom mode is being used.
---@field mode CatnipOutputMode
---An iterator to traverse through all available modes for this output.
---@field modes fun(): CatnipOutputMode
---The scale factor of the output.
---@field scale number

---@class (exact) CatnipOutputMode: CatnipResource
---The width of the output mode (in pixels).
---More specifically, this is the width of the window space viewport that will be displayed on the output.
---@field width number
---The height of the output mode (in pixels).
---More specifically, this is the height of the window space viewport that will be displayed on the output.
---@field height number
---The refresh rate of the output mode (in mHz).
---@field refresh number

---@class (exact) CatnipWindow: CatnipResource
---(readonly) The window ID. This is a unique number assigned to each window that persists across reloads.
---@field id number
---The x-coordinate of the window (in pixels).
---@field x number
---The y-coordinate of the window (in pixels).
---@field y number
---The z-index of the window (1-indexed).
---@field z number
---The width of the window (in pixels).
---@field width number
---The height of the window (in pixels).
---@field height number
---Controls whether the window should be rendered or not.
---@field visible boolean
---The title of the window.
---This is set by the client itself and is often the name of the application.
---@field title string
---Requests the window to be closed.
---@field destroy fun(self: CatnipWindow)

---@class CatnipCanvasOptions
---The x-coordinate of the canvas (in pixels).
---@field x number?
---The y-coordinate of the canvas (in pixels).
---@field y number?
---The z-index of the canvas (1-indexed).
---@field z number?
---The width of the canvas (in pixels).
---@field width number?
---The height of the canvas (in pixels).
---@field height number?
---Controls whether the canvas should be rendered or not.
---@field visible boolean?

---@class (exact) CatnipCanvas: CatnipResource
---The x-coordinate of the canvas (in pixels).
---@field x number
---The y-coordinate of the canvas (in pixels).
---@field y number
---The z-index of the canvas (1-indexed).
---@field z number
---The width of the canvas (in pixels).
---@field width number
---The height of the canvas (in pixels).
---@field height number
---Controls whether the canvas should be rendered or not.
---@field visible boolean
---Renders a path onto the canvas.
---@field path fun(self: CatnipCanvas, path: CatnipCanvasPath)
---Renders a rectangle onto the canvas.
---@field rectangle fun(self: CatnipCanvas, path: CatnipCanvasRectangle)
---Renders text onto the canvas.
---@field text fun(self: CatnipCanvas, text: string, options: CatnipCanvasTextOptions?)
---Renders a PNG image onto the canvas.
---@field png fun(self: CatnipCanvas, path: string, options: CatnipCanvasPngOptions?)
---Renders an SVG onto the canvas.
---@field svg fun(self: CatnipCanvas, document: string, options: CatnipCanvasSvgOptions?)
---Clears the canvas, removing anything previously drawn.
---@field clear fun(self: CatnipCanvas)
---Destroys the canvas immediately. After this is called, the canvas will no longer be visible nor useable.
---This is automatically called when the canvas is garbage collected.
---@field destroy fun(self: CatnipCanvas)

---@class CatnipCanvasPathFields
---The starting x-coordinate of the path (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The starting y-coordinate of the path (in pixels) relative to the top left corner of the canvas.
---@field y number?
---Whether to close the path.
---This is not the same as simply adding a line back to the starting point, since such a line will still have line caps.
---@field close boolean?
---The color of the path fill as a hexadecimal number.
---@field fill_color number?
---The opacity of the path fill as a number between 0-1 (inclusive).
---@field fill_opacity number?
---The color of the path stroke as a hexadecimal number.
---@field stroke_color number?
---The opacity of the path stroke as a number between 0-1 (inclusive).
---@field stroke_opacity number?
---The thickness of the path stroke (in pixels).
---@field stroke_size number?
---The caps to use for the start / end of the path stroke.
---Only applies to open (not closed) paths.
---butt = no cap applied (default)
---round = circle cap applied, whose center lies at the start / end points
---square = square cap applied, whose center lies at the start / end points
---@field stroke_cap 'butt' | 'round' | 'square'?

---[2] = the relative x-coordinate of the endpoint
---[3] = the relative y-coordinate of the endpoint
---@alias CatnipCanvasPathLine { [1]: 'line', [2]: number, [3]: number }
---[2] = the relative x-coordinate of the center of the circle
---[3] = the relative y-coordinate of the center of the circle
---[4] = the rotation amount (in radians), may be either positive or negative to indicate direction
---@alias CatnipCanvasPathArc { [1]: 'arc', [2]: number, [3]: number, [4]: number }
---[2] = the relative x-coordinate of the first control point
---[3] = the relative y-coordinate of the first control point
---[4] = the relative x-coordinate of the second control point
---[5] = the relative y-coordinate of the second control point
---[6] = the relative x-coordinate of the endpoint
---[7] = the relative y-coordinate of the endpoint
---@alias CatnipCanvasPathBezier { [1]: 'bezier', [2]: number, [3]: number, [4]: number, [5]: number, [6]: number, [7]: number }
---@alias CatnipCanvasPathCommand CatnipCanvasPathLine | CatnipCanvasPathArc | CatnipCanvasPathBezier

---@alias CatnipCanvasPath CatnipCanvasPathFields | CatnipCanvasPathCommand[]

---@class CatnipCanvasRectangle
---The x-coordinate of the rectangle (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The y-coordinate of the rectangle (in pixels) relative to the top left corner of the canvas.
---@field y number?
---The width of the rectangle (in pixels).
---@field width number?
---The height of the rectangle (in pixels).
---@field height number?
---The default radius for all corners (in pixels).
---This is used to control how "rounded" each corner is.
---@field radius number?
---The radius of the top left corner (in pixels).
---@field radius_top_left number?
---The radius of the top right corner (in pixels).
---@field radius_top_right number?
---The radius of the bottom right corner (in pixels).
---@field radius_bottom_right number?
---The radius of the bottom left corner (in pixels).
---@field radius_bottom_left number?
---The color of the rectangle fill as a hexadecimal number.
---@field fill_color number?
---The opacity of the rectangle fill as a number between 0-1 (inclusive).
---@field fill_opacity number?
---The color of the rectangle stroke as a hexadecimal number.
---@field stroke_color number?
---The opacity of the rectangle stroke as a number between 0-1 (inclusive).
---@field stroke_opacity number?
---The thickness of the rectangle stroke (in pixels).
---@field stroke_size number?

---@class CatnipCanvasTextOptions
---The x-coordinate of the text (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The y-coordinate of the text (in pixels) relative to the top left corner of the canvas.
---@field y number?
---The width of the bounding box (in pixels).
---@field width number?
---The height of the bounding box (in pixels).
---@field height number?
---The horizontal alignment of the text, relative to the bounding box.
---@field align ('left' | 'center' | 'right')?
---The vertical alignment of the text, relative to the bounding box.
---@field valign ('left' | 'center' | 'right')?
---Controls whether and where to truncate text and apply ellipsis in the case of overflow of the bounding box.
---@field ellipsis (boolean | 'start' | 'middle' | 'end')?
---Controls whether and how to wrap text in the case of overflow on the main axis (usually the horizontal axis).
---char = allows text to wrap at any character, including in the middle of words
---word = allows text to wrap only at word boundaries
---auto = wraps text at word boundaries when possible, otherwise breaks words when necessary
---@field wrap (boolean | 'char' | 'word' | 'auto')?
---The font family.
---A comma separated list of font families may be used to specify fallbacks in case font families are missing.
---@field font string?
---The size of the font (in pixels).
---@field size number?
---The weight, i.e. "thickness" of the font as a number between 100-1000 (inclusive).
---The default weight is 400.
---@field weight number?
---Controls whether the font should be rendered in italics.
---@field italic boolean?
---The color of the text as a hexadecimal number.
---@field color number?
---The opacity of the text as a number between 0-1 (inclusive).
---@field opacity number?

---@class CatnipCanvasPngOptions
---The x-coordinate of the PNG (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The y-coordinate of the PNG (in pixels) relative to the top left corner of the canvas.
---@field y number?
---The width to render the PNG (in pixels).
---If left undefined, this is automatically scaled to preserve the aspect ratio.
---@field width number?
---The height to render the PNG (in pixels).
---If left undefined, this is automatically scaled to preserve the aspect ratio.
---@field height number?
---Controls whether the PNG should be cached for the next canvas render cycle.
---@field cache boolean?

---@class CatnipCanvasSvgOptions
---The x-coordinate of the SVG (in pixels) relative to the top left corner of the canvas.
---@field x number?
---The y-coordinate of the SVG (in pixels) relative to the top left corner of the canvas.
---@field y number?
---The width to render the SVG (in pixels).
---If left undefined, this is automatically scaled to preserve the aspect ratio.
---@field width number?
---The height to render the SVG (in pixels).
---If left undefined, this is automatically scaled to preserve the aspect ratio.
---@field height number?
---Controls whether the SVG should be cached for the next canvas render cycle.
---@field cache boolean?
---A [CSS stylesheet](https://gnome.pages.gitlab.gnome.org/librsvg/devel-docs/features.html#css-properties) to apply to the SVG.
---@field styles string?

local catnip ---@type Catnip
return catnip
