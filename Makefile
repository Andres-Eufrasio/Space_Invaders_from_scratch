# Space Invaders from scratch

CC ?= cc
TARGET    := space_invaders
SRC_DIR   := src
BUILD_DIR := build

SRCS := main.c aliens.c shields.c hashmap.c
OBJS := $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))
DEPS := $(OBJS:.o=.d)

# SDL2 flags: pkg-config first, sdl2-config as fallback.
SDL_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null || sdl2-config --cflags 2>/dev/null)
SDL_LIBS   := $(shell pkg-config --libs   sdl2 2>/dev/null || sdl2-config --libs   2>/dev/null)

CPPFLAGS += -I$(SRC_DIR) $(SDL_CFLAGS)
CFLAGS   += -std=c11 -Wall -Wextra -O2
LDLIBS   += $(SDL_LIBS)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)
