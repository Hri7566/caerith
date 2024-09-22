#ifndef _ca_screen_h
#define _ca_screen_h

#include "midi.hpp"

#include <raylib.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "Aerith (C++)"

void start_screen(MIDI *smf);
Color get_track_color(int num);

#endif
