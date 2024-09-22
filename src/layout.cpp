#include "layout.hpp"
#include "screen.hpp"
#include <cmath>

PianoLayout::PianoLayout(int first_key, int last_key)
{
    num_keys = last_key - first_key + 1;
    keyboard_height = WINDOW_HEIGHT / 6;

    int num_white = 0;

    // Load keyboard with keys
    for (int i = first_key; i <= last_key; i++)
    {
        PianoKey key = PianoKey();

        if (
            i % 12 == 1 ||
            i % 12 == 3 ||
            i % 12 == 6 ||
            i % 12 == 8 ||
            i % 12 == 10)
        {
            black_key.push_back(true);
        }
        else
        {
            num_white++;
            black_key.push_back(false);
        }

        keys.push_back(key);
        pressed_keys.push_back(false);
        key_colors.push_back(WHITE);
    }

    key_width = (float)WINDOW_WIDTH / (float)num_white;

    // Set key positions
    int white_pos = 0;
    for (int i = 0; i < num_keys; i++)
    {
        auto &key = keys.at(i);

        key.position_x = white_pos * key_width;
        key.position_y = WINDOW_HEIGHT - keyboard_height;

        if (black_key[i])
        {
            key.position_x -= key_width / 2;
        }
        else
        {
            white_pos++;
        }
    }

    // Load key images
    white_key_tex = LoadTexture("textures/keyWhite.png");
    black_key_tex = LoadTexture("textures/keyBlack.png");

    white_key_pressed_tex = LoadTexture("textures/keyWhitePressed.png");
    black_key_pressed_tex = LoadTexture("textures/keyBlackPressed.png");
}

void PianoLayout::update(float delta)
{
    int i = 0;
    for (PianoKey key : keys)
    {
        pressed_keys[i] = false;
        key_colors[i] = WHITE;
        i++;
    }
}

void PianoLayout::draw()
{
    // Draw white keys
    int i = 0;
    for (PianoKey key : keys)
    {
        if (!black_key[i])
        {
            Rectangle source = {
                0,
                0,
                29,
                177};

            Rectangle dest = {
                floor(key.position_x),
                floor(key.position_y),
                key_width,
                keyboard_height};

            if (pressed_keys[i])
            {
                DrawTexturePro(white_key_pressed_tex, source, dest, {0, 0}, 0, key_colors[i]);
            }
            else
            {
                DrawTexturePro(white_key_tex, source, dest, {0, 0}, 0, key_colors[i]);
            }
        }

        i++;
    }

    // Draw black keys
    i = 0;
    for (PianoKey key : keys)
    {
        if (black_key[i])
        {
            // DrawRectangle(
            //     key.position_x,
            //     key.position_y,
            //     key_width / 2,
            //     (keyboard_height / 3) * 2,
            //     BLACK);

            Rectangle source = {
                0,
                0,
                40,
                117};

            Rectangle dest = {
                key.position_x,
                key.position_y,
                key_width,
                (keyboard_height / 3) * 2};

            if (pressed_keys[i])
            {
                DrawTexturePro(black_key_pressed_tex, source, dest, {0, 0}, 0, key_colors[i]);
            }
            else
            {
                DrawTexturePro(black_key_tex, source, dest, {0, 0}, 0, key_colors[i]);
            }
        }

        i++;

        // Draw red line
        float line_size = (keyboard_height / 20);
        DrawRectangleGradientV(
            0,
            WINDOW_HEIGHT - keyboard_height - line_size,
            WINDOW_WIDTH,
            (int)line_size,
            Color{0x88, 0x1a, 0x1a, 0xff}, Color{0x48, 0x05, 0x05, 0xff});
    }
}