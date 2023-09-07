EXECUTABLE = bwc

PACKAGES = \
	glib-2.0 \
	luajit \
	lua \
	pixman-1 \
	xkbcommon \
	wlroots \
	wayland-server

CC = clang
FLAGS = -DWLR_USE_UNSTABLE -DG_LOG_DOMAIN=\"$(EXECUTABLE)\"
CFLAGS := -I./build $(shell pkg-config --cflags $(PACKAGES))
LIBS := $(shell pkg-config --libs $(PACKAGES))

SOURCES := $(shell find src -name '*.c')
OBJECTS := $(SOURCES:src/%.c=build/%.o)

WAYLAND_SCANNER := $(shell pkg-config --variable=wayland_scanner wayland-scanner)
WAYLAND_PROTOCOLS := $(shell pkg-config --variable=pkgdatadir wayland-protocols)

$(shell mkdir -p build)

build/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<

build/xdg-shell-protocol.h:
	$(WAYLAND_SCANNER) server-header $(WAYLAND_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@

build/$(EXECUTABLE): build/xdg-shell-protocol.h $(OBJECTS)
	$(CC) $(FLAGS) $(LIBS) $(OBJECTS) -o build/$(EXECUTABLE)

clean:
	if [[ -d build ]]; then rm -r build; fi

dev: FLAGS += -DDEV_MODE
dev: build/$(EXECUTABLE)
	@build/$(EXECUTABLE)

.PHONY = clean dev
.DEFAULT_GOAL = build/$(EXECUTABLE)
