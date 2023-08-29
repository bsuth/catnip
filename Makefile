EXECUTABLE = bwc

PACKAGES = \
	luajit \
	lua \
	pixman-1 \
	xkbcommon \
	wlroots \
	wayland-server

CC = clang
FLAGS = -DWLR_USE_UNSTABLE
CFLAGS = -I./build $(shell pkg-config --cflags $(PACKAGES))
LIBS = $(shell pkg-config --libs $(PACKAGES))

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=build/%.o)

WAYLAND_SCANNER = $(shell pkg-config --variable=wayland_scanner wayland-scanner)
WAYLAND_PROTOCOLS = $(shell pkg-config --variable=pkgdatadir wayland-protocols)

$(shell [[ ! -d build ]] && mkdir -p build)

build/%.o: src/%.c
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<

build/xdg-shell-protocol.h:
	$(WAYLAND_SCANNER) server-header $(WAYLAND_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@

build/$(EXECUTABLE): build/xdg-shell-protocol.h $(OBJECTS)
	if [[ ! -d build ]]; then mkdir build; fi
	$(CC) $(FLAGS) $(LIBS) $(OBJECTS) -o build/$(EXECUTABLE)

clean:
	if [[ -d build ]]; then rm -r build; fi

.PHONY = clean
.DEFAULT_GOAL = build/$(EXECUTABLE)
