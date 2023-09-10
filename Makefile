EXECUTABLE = catnip

PACKAGES = \
	glib-2.0 \
	luajit \
	lua \
	pixman-1 \
	xkbcommon \
	wlroots \
	wayland-server

SOURCE_DIR = src
BUILD_DIR = build
BUILD_INCLUDE_DIR = $(BUILD_DIR)/include

CC = clang
FLAGS = -DWLR_USE_UNSTABLE -DG_LOG_DOMAIN=\"$(EXECUTABLE)\"
CFLAGS := -I$(SOURCE_DIR) -I$(BUILD_INCLUDE_DIR) $(shell pkg-config --cflags $(PACKAGES))
LIBS := $(shell pkg-config --libs $(PACKAGES))

SOURCES := $(shell find $(SOURCE_DIR) -name '*.c')
OBJECTS := $(SOURCES:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)

WAYLAND_SCANNER := $(shell pkg-config --variable=wayland_scanner wayland-scanner)
WAYLAND_PROTOCOLS := $(shell pkg-config --variable=pkgdatadir wayland-protocols)

$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BUILD_INCLUDE_DIR))

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_INCLUDE_DIR)/xdg-shell-protocol.h:
	$(WAYLAND_SCANNER) server-header $(WAYLAND_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@

$(BUILD_DIR)/$(EXECUTABLE): $(BUILD_INCLUDE_DIR)/xdg-shell-protocol.h $(OBJECTS)
	$(CC) $(FLAGS) $(LIBS) $(OBJECTS) -o $(BUILD_DIR)/$(EXECUTABLE)

clean:
	if [[ -d $(BUILD_DIR) ]]; then rm -r $(BUILD_DIR); fi
	if [[ -d $(BUILD_INCLUDE_DIR) ]]; then rm -r $(BUILD_INCLUDE_DIR); fi

dev: FLAGS += -g -DDEV_MODE
dev: $(BUILD_DIR)/$(EXECUTABLE)
	@$(BUILD_DIR)/$(EXECUTABLE)

.PHONY = clean dev
.DEFAULT_GOAL = $(BUILD_DIR)/$(EXECUTABLE)
