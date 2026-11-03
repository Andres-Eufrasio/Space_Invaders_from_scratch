# Space Invaders from scratch

CC ?= cc
TARGET    := space_invaders
SRC_DIR   := src
BUILD_DIR := build

SRCS := main.c aliens.c shields.c
OBJS := $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))
DEPS := $(OBJS:.o=.d)

# SDL2 flags: pkg-config first, sdl2-config as fallback.
# --silence-errors keeps the shell syntax portable (cmd.exe and sh).
SDL_CFLAGS := $(shell pkg-config --silence-errors --cflags sdl2 || sdl2-config --cflags)
SDL_LIBS   := $(shell pkg-config --silence-errors --libs   sdl2 || sdl2-config --libs)

CPPFLAGS += -I$(SRC_DIR) $(SDL_CFLAGS)
CFLAGS   += -std=c11 -Wall -Wextra -O2
LDLIBS   += $(SDL_LIBS)

# --- Windows (MinGW) build -------------------------------------------------
# `make windows` compiles with the Windows toolchain into a .exe;
# `make run` builds and runs the host platform's binary.
WIN_CC        ?= gcc
WIN_TARGET    := $(TARGET).exe
WIN_BUILD_DIR := $(BUILD_DIR)/windows
WIN_OBJS      := $(addprefix $(WIN_BUILD_DIR)/,$(SRCS:.c=.o))
WIN_DEPS      := $(WIN_OBJS:.o=.d)

WIN_CPPFLAGS := -I$(SRC_DIR) $(SDL_CFLAGS)
WIN_CFLAGS   := -std=c11 -Wall -Wextra -O2
WIN_LDFLAGS  ?=

# Windows always needs -lmingw32 -lSDL2main -lSDL2 (in that order).
# Keep whatever else pkg-config/sdl2-config reported (library paths, -mwindows),
# but drop the duplicates so the required order is preserved.
WIN_LDLIBS := $(filter-out -lmingw32 -lSDL2main -lSDL2,$(SDL_LIBS)) -lmingw32 -lSDL2main -lSDL2

# Platform-specific remove helpers so `clean` works on both Windows and Unix.
ifeq ($(OS),Windows_NT)
RM_DIR  = cmd /C "if exist $(subst /,\,$(1)) rmdir /S /Q $(subst /,\,$(1))"
RM_FILE = cmd /C "if exist $(1) del /Q $(1)"
RUN_TARGET := $(WIN_TARGET)
RUN_CMD    := cmd /C $(WIN_TARGET)
else
RM_DIR  = rm -rf $(1)
RM_FILE = rm -f $(1)
RUN_TARGET := $(TARGET)
RUN_CMD    := ./$(TARGET)
endif

.PHONY: all run windows clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(RUN_TARGET)
	$(RUN_CMD)

windows: $(WIN_TARGET)

$(WIN_TARGET): $(WIN_OBJS)
	$(WIN_CC) $(WIN_LDFLAGS) -o $@ $^ $(WIN_LDLIBS)

$(WIN_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(WIN_BUILD_DIR)
	$(WIN_CC) $(WIN_CPPFLAGS) $(WIN_CFLAGS) -MMD -MP -c $< -o $@

$(WIN_BUILD_DIR):
	cmd /C "if not exist $(subst /,\,$@) mkdir $(subst /,\,$@)"

clean:
	$(call RM_DIR,$(BUILD_DIR))
	$(call RM_FILE,$(TARGET))
	$(call RM_FILE,$(WIN_TARGET))

-include $(DEPS) $(WIN_DEPS)


# --- Tiny Windows build ---------------------------------------------
WIN_TINY_TARGET    := space_invaders_tiny.exe
WIN_TINY_BUILD_DIR := $(BUILD_DIR)/tiny
WIN_TINY_OBJS      := $(addprefix $(WIN_TINY_BUILD_DIR)/,$(SRCS:.c=.o))
WIN_TINY_DEPS      := $(WIN_TINY_OBJS:.o=.d)

WIN_TINY_CFLAGS := -std=c11 -Wall -Wextra -Oz -flto \
                   -ffunction-sections -fdata-sections

WIN_TINY_LDFLAGS := $(WIN_LDFLAGS) -flto -Wl,--gc-sections -s

.PHONY: tiny run-tiny

tiny: $(WIN_TINY_TARGET)

run-tiny: $(WIN_TINY_TARGET)
	cmd /C $(WIN_TINY_TARGET)

$(WIN_TINY_TARGET): $(WIN_TINY_OBJS)
	$(WIN_CC) $(WIN_TINY_LDFLAGS) -o $@ $^ $(WIN_LDLIBS)

$(WIN_TINY_BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(WIN_TINY_BUILD_DIR)
	$(WIN_CC) $(WIN_CPPFLAGS) $(WIN_TINY_CFLAGS) -MMD -MP -c $< -o $@

$(WIN_TINY_BUILD_DIR):
	cmd /C "if not exist $(subst /,\,$@) mkdir $(subst /,\,$@)"

-include $(WIN_TINY_DEPS)
