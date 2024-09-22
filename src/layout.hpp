#ifndef _ca_layout_h
#define _ca_layout_h

#include <vector>
#include <raylib.h>

typedef struct
{
    float position_x;
    float position_y;
} PianoKey;

class PianoLayout
{
public:
    PianoLayout(int first_key, int last_key);

    std::vector<PianoKey> keys;
    std::vector<bool> black_key;
    std::vector<bool> pressed_keys;
    std::vector<Color> key_colors;

    int num_keys;
    float keyboard_height;
    float key_width;

    Texture white_key_tex;
    Texture black_key_tex;

    Texture white_key_pressed_tex;
    Texture black_key_pressed_tex;

    void update(float delta);
    void draw();
};

#endif
