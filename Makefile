CC := cc
CFLAGS := -std=gnu11 -O2 -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-parentheses -Wno-deprecated-declarations -DGL_SILENCE_DEPRECATION $(shell pkg-config --cflags sdl2 libjpeg)
LDLIBS := $(shell pkg-config --libs sdl2 libjpeg) -framework OpenGL
WTK_NG_DIR ?= ../wtk-ng
GAME_SOURCES := rocks_gui.c misc.c gui.c mouse.c sensor.c network.c readini.c
GAME_OBJECTS := $(GAME_SOURCES:.c=.o)
WTK_NG_LIBRARY := $(WTK_NG_DIR)/libwtk-ng.a
.PHONY: all clean run

all: rocks-ng

rocks-ng: $(GAME_OBJECTS) $(WTK_NG_LIBRARY)
	$(CC) $(GAME_OBJECTS) $(WTK_NG_LIBRARY) $(LDLIBS) -lm -o $@

$(WTK_NG_LIBRARY):
	$(MAKE) -C $(WTK_NG_DIR)

%.o: %.c $(WTK_NG_DIR)/wt.h rocks.h
	$(CC) $(CFLAGS) -I. -I$(WTK_NG_DIR) -c $< -o $@

run: rocks-ng
	./rocks-ng -w

clean:
	rm -f $(GAME_OBJECTS) rocks-ng
