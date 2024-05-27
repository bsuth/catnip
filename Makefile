EXECUTABLE = catnip

CC = clang

DEPENDENCIES = \
	librsvg-2.0 \
	cairo \
	pango \
	pangocairo \
	luajit \
	xkbcommon \
	wlroots \
	wayland-server

SOURCE_DIR := src
SOURCES := $(shell find $(SOURCE_DIR) -name '*.c')

BUILD_DIR := build
$(shell mkdir -p $(BUILD_DIR))

.PHONY = default dev clean format

# ------------------------------------------------------------------------------
# Builds
# ------------------------------------------------------------------------------

default: $(BUILD_DIR)/$(EXECUTABLE)

dev: CFLAGS += -g
dev: $(BUILD_DIR)/$(EXECUTABLE)
	@echo "$(LUA_CONFIG)"
	@$(BUILD_DIR)/$(EXECUTABLE)

# ------------------------------------------------------------------------------
# Wayland Protocols
# ------------------------------------------------------------------------------

WAYLAND_SCANNER := $(shell pkg-config --variable=wayland_scanner wayland-scanner)
WAYLAND_PROTOCOLS := $(shell pkg-config --variable=pkgdatadir wayland-protocols)

PROTOCOLS_DIR := protocols

PROTOCOLS_BUILD_DIR := $(BUILD_DIR)/protocols
$(shell mkdir -p $(PROTOCOLS_BUILD_DIR))

$(PROTOCOLS_BUILD_DIR)/xdg-shell-protocol.h:
	$(WAYLAND_SCANNER) server-header $(WAYLAND_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@

$(PROTOCOLS_BUILD_DIR)/wlr-layer-shell-unstable-v1-protocol.h:
	$(WAYLAND_SCANNER) server-header $(PROTOCOLS_DIR)/wlr-layer-shell-unstable-v1.xml $@

# ------------------------------------------------------------------------------
# Objects
# ------------------------------------------------------------------------------

CFLAGS := \
	-DG_LOG_DOMAIN=\"$(EXECUTABLE)\" \
	-DWLR_USE_UNSTABLE \
	$(shell pkg-config --cflags $(DEPENDENCIES)) \
	-I$(PROTOCOLS_BUILD_DIR) \
	-I$(SOURCE_DIR)

OBJECT_BUILD_DIR := $(BUILD_DIR)/objects
$(shell mkdir -p $(OBJECT_BUILD_DIR))

$(OBJECT_BUILD_DIR)/default_config.o: $(SOURCE_DIR)/default_config.lua
	@cat $< | \
	sed 's/"/\\"/g' | \
	awk '{ printf "%s%s", separator, $$0; separator = "\\n" }' - | \
	xargs -0  printf '#include "default_config.h"\nconst char* catnip_default_config = "%s";' | \
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ -xc -

$(OBJECT_BUILD_DIR)/luacats.o: $(SOURCE_DIR)/luacats.lua
	@cat $< | \
	sed 's/"/\\"/g' | \
	awk '{ printf "%s%s", separator, $$0; separator = "\\n" }' - | \
	xargs -0  printf '#include "luacats.h"\nconst char* catnip_luacats = "%s";' | \
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ -xc -

$(OBJECT_BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@-mkdir -p $(@D)
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<

# ------------------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------------------

LIBS := \
	-lrt \
	$(shell pkg-config --libs $(DEPENDENCIES))

OBJECTS := $(SOURCES:$(SOURCE_DIR)/%.c=$(OBJECT_BUILD_DIR)/%.o) $(OBJECT_BUILD_DIR)/default_config.o $(OBJECT_BUILD_DIR)/luacats.o

PROTOCOL_HEADERS = \
	$(PROTOCOLS_BUILD_DIR)/wlr-layer-shell-unstable-v1-protocol.h \
	$(PROTOCOLS_BUILD_DIR)/xdg-shell-protocol.h

$(BUILD_DIR)/$(EXECUTABLE): $(PROTOCOL_HEADERS) $(OBJECTS)
	$(CC) $(FLAGS) $(LIBS) $(OBJECTS) -o $@

# ------------------------------------------------------------------------------
# Clean
# ------------------------------------------------------------------------------

clean:
	if [[ -d $(BUILD_DIR) ]]; then rm -r $(BUILD_DIR); fi

# ------------------------------------------------------------------------------
# Format
# ------------------------------------------------------------------------------

format:
	clang-format -i $(shell find $(SOURCE_DIR) -type f -regex .*\.[ch]$)
