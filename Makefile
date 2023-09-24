EXECUTABLE = catnip

CC = clang
CFLAGS := -I.
LIBS := -lrt

BUILD_DIR := build
$(shell mkdir -p $(BUILD_DIR))

.PHONY = clean dev

# ------------------------------------------------------------------------------
# Default
# ------------------------------------------------------------------------------

# TODO: add client
default: $(BUILD_DIR)/$(EXECUTABLE)

# ------------------------------------------------------------------------------
# Wayland Protocols
# ------------------------------------------------------------------------------

WAYLAND_SCANNER := $(shell pkg-config --variable=wayland_scanner wayland-scanner)
WAYLAND_PROTOCOLS := $(shell pkg-config --variable=pkgdatadir wayland-protocols)

PROTOCOLS_BUILD_DIR := $(BUILD_DIR)/protocols
$(shell mkdir -p $(PROTOCOLS_BUILD_DIR))

$(PROTOCOLS_BUILD_DIR)/xdg-shell-protocol.h:
	$(WAYLAND_SCANNER) server-header $(WAYLAND_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@

# ------------------------------------------------------------------------------
# Catnip Common
# ------------------------------------------------------------------------------

COMMON_CFLAGS := $(CFLAGS)
COMMON_LIBS := $(LIBS)

COMMON_SOURCE_DIR := common
COMMON_BUILD_DIR := $(BUILD_DIR)/$(COMMON_SOURCE_DIR)
$(shell mkdir -p $(COMMON_BUILD_DIR))

COMMON_SOURCES := $(shell find $(COMMON_SOURCE_DIR) -name '*.c')
COMMON_OBJECTS := $(COMMON_SOURCES:$(COMMON_SOURCE_DIR)/%.c=$(COMMON_BUILD_DIR)/%.o)

$(COMMON_BUILD_DIR)/%.o: $(COMMON_SOURCE_DIR)/%.c
	@-mkdir -p $(@D)
	$(CC) $(COMMON_FLAGS) $(COMMON_CFLAGS) -c -o $@ $<

# ------------------------------------------------------------------------------
# Catnip Server
# ------------------------------------------------------------------------------

# TODO: remove glib and lua dependencies
SERVER_DEPENDENCIES = \
	glib-2.0 \
	luajit \
	lua \
	pixman-1 \
	xkbcommon \
	wlroots \
	wayland-server

# TODO: remove G_LOG_DOMAIN
SERVER_FLAGS = -DWLR_USE_UNSTABLE -DG_LOG_DOMAIN=\"$(EXECUTABLE)\"
SERVER_CFLAGS := $(CFLAGS) -I$(PROTOCOLS_BUILD_DIR) $(shell pkg-config --cflags $(SERVER_DEPENDENCIES))
SERVER_LIBS := $(LIBS) $(shell pkg-config --libs $(SERVER_DEPENDENCIES))

SERVER_SOURCE_DIR := server
SERVER_BUILD_DIR := $(BUILD_DIR)/$(SERVER_SOURCE_DIR)
$(shell mkdir -p $(SERVER_BUILD_DIR))

SERVER_SOURCES := $(shell find $(SERVER_SOURCE_DIR) -name '*.c')
SERVER_OBJECTS := $(SERVER_SOURCES:$(SERVER_SOURCE_DIR)/%.c=$(SERVER_BUILD_DIR)/%.o)

SERVER_PROTOCOLS = \
	$(PROTOCOLS_BUILD_DIR)/xdg-shell-protocol.h

$(BUILD_DIR)/$(EXECUTABLE): $(SERVER_PROTOCOLS) $(COMMON_OBJECTS) $(SERVER_OBJECTS)
	$(CC) $(SERVER_FLAGS) $(SERVER_LIBS) $(COMMON_OBJECTS) $(SERVER_OBJECTS) -o $@

$(SERVER_BUILD_DIR)/%.o: $(SERVER_SOURCE_DIR)/%.c
	@-mkdir -p $(@D)
	$(CC) $(SERVER_FLAGS) $(SERVER_CFLAGS) -c -o $@ $<

dev: SERVER_FLAGS += -g -DDEV_MODE
dev: $(BUILD_DIR)/$(EXECUTABLE)
	@$(BUILD_DIR)/$(EXECUTABLE)

# ------------------------------------------------------------------------------
# Catnip Client
# ------------------------------------------------------------------------------

# TODO: make shared library, allow install
$(BUILD_DIR)/client: client/main.c
	$(CC) $(FLAGS) $(CFLAGS) -o $@ $<

# ------------------------------------------------------------------------------
# Clean
# ------------------------------------------------------------------------------

clean:
	if [[ -d $(BUILD_DIR) ]]; then rm -r $(BUILD_DIR); fi
