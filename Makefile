CC ?= cc
PKG_CONFIG ?= pkg-config
CFLAGS ?= -std=gnu11 -O2 -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-parentheses -Wno-deprecated-declarations
CPPFLAGS += $(filter-out -Dmain=SDL_main,$(shell $(PKG_CONFIG) --cflags sdl2 libjpeg))
LDLIBS += $(shell $(PKG_CONFIG) --libs sdl2 libjpeg)
ifeq ($(OS),Windows_NT)
EXEEXT := .exe
LDLIBS += -lopengl32
else ifeq ($(shell uname -s),Darwin)
CPPFLAGS += -DGL_SILENCE_DEPRECATION
LDLIBS += -framework OpenGL
else
LDLIBS += -lGL
endif
LDLIBS += -lm
TARGET := rocks-ng$(EXEEXT)
WTK_NG_DIR ?= ../wtk-ng
GAME_SOURCES := rocks_gui.c misc.c gui.c mouse.c sensor.c network.c readini.c
GAME_OBJECTS := $(GAME_SOURCES:.c=.o)
WTK_NG_LIBRARY := $(WTK_NG_DIR)/libwtk-ng.a
.PHONY: all clean run dist-macos

all: $(TARGET)

$(TARGET): $(GAME_OBJECTS) $(WTK_NG_LIBRARY)
	$(CC) $(LDFLAGS) $(GAME_OBJECTS) $(WTK_NG_LIBRARY) $(LDLIBS) -o $@

$(WTK_NG_LIBRARY):
	$(MAKE) -C $(WTK_NG_DIR)

%.o: %.c $(WTK_NG_DIR)/wt.h rocks.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I$(WTK_NG_DIR) -c $< -o $@

run: $(TARGET)
	./$(TARGET) -w

dist-macos: rocks-ng
	./scripts/package-macos.sh

clean:
	rm -f $(GAME_OBJECTS) rocks-ng rocks-ng.exe
