#include "screen.hpp"
#include "layout.hpp"

#include <iostream>
#include <string>

Color note_colors[] = {BLUE, GREEN, RED, YELLOW, GRAY, LIME, PINK, ORANGE, PURPLE, BROWN, MAGENTA, LIGHTGRAY, BLUE, GREEN, BLUE, GREEN};

void start_screen(MIDI *smf)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);
    SetWindowMonitor(0);

    float delta = 0.0f;
    float midi_time = 0.0f;

    float speed = 500.0f;
    float sprint = 100.0f;

    float aspect_ratio = WINDOW_WIDTH / WINDOW_HEIGHT;
    float note_screen_time = 10;

    int first_key = 0;
    int last_key = 127;

    float scale = 0.5;

    while (!WindowShouldClose())
    {
        delta = GetFrameTime();
        uint32_t note_thickness = (WINDOW_WIDTH / 88);

        uint32_t offset_y = 0;

        if (IsKeyDown(KEY_UP))
        {
            if (IsKeyDown(KEY_SPACE))
            {
                midi_time -= speed * sprint * delta;
            }
            else
            {
                midi_time -= speed * delta;
            }
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            if (IsKeyDown(KEY_SPACE))
            {
                midi_time += speed * sprint * delta;
            }
            else
            {
                midi_time += speed * delta;
            }
        }

        if (IsKeyDown(KEY_LEFT))
        {
            scale -= 0.01;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            scale += 0.01;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        /*

        int i = 0;

        for (auto &track : smf->tracks)
        {
            if (!track.notes.empty())
            {
                uint32_t note_range = track.max_note - track.min_note;
                Color color = note_colors[i];

                // DrawRectangle(0, 0, WINDOW_WIDTH, (note_range + 1) * note_height, DARKGRAY);
                // DrawText(track.name.c_str(), 1, 1, 14, GRAY);

                for (auto &note : track.notes)
                {
                    // DrawRectangle(
                    //     (note.start_time - track_offset) / scale,
                    //     (note_range - (note.key - track.min_note)) * note_height,
                    //     (note.duration / scale),
                    //     (note_height),
                    //     color);

                    // DrawRectangle(
                    //     (note_range - (note.key - track.min_note)) * note_thickness,
                    //     (note.start_time - track_offset) / scale,
                    //     (note_thickness),
                    //     (note.duration / scale),
                    //     color);

                    DrawRectangle(
                        ((note_range - (note.key - track.min_note)) * note_thickness) + 100,
                        (note.start_time - track_offset) / scale,
                        (note_thickness),
                        (note.duration / scale),
                        color);
                }

                // offset_y += (note_range + 1) * note_height + 4;
            }

            i++;
        }

        DrawFPS(0, 0);
        */

        double keyboard_height = 0.20 * WINDOW_HEIGHT;
        keyboard_height = keyboard_height / (smf->max_note - smf->min_note) * 128;
        keyboard_height = keyboard_height / (1920.0 / 1080.0) * aspect_ratio;

        double note_pos_factor = 1 / note_screen_time * (1 - keyboard_height);
        double render_cutoff = midi_time + note_screen_time;

        Layout layout(first_key, last_key);

        Color key_colors[514];
        bool key_pressed[257];
        int key_notes[257];

        uint32_t note_range = smf->max_note - smf->min_note;

        int i = 0;
        for (auto &track : smf->tracks)
        {
            Color color = note_colors[i];

            for (auto &note : track.notes)
            {
                // off screen?
                if ((note.start_time + note.duration < midi_time - WINDOW_HEIGHT) || (note.start_time - midi_time > render_cutoff))
                    continue;
                Key *key = &layout.keys[note.key];

                double top = 1 - (render_cutoff - (note.start_time + note.duration)) * note_pos_factor;
                double bottom = 1 - (render_cutoff - note.start_time) * note_pos_factor;
                double left = layout.keys[note.key].left;   // TODO layout
                double right = layout.keys[note.key].right; // TODO layout

                // TODO note.hasEnded?

                // TODO load textures
                double top_cap_height = (right - left);
                double bottom_cap_height = (right - left);
                double cap_top = top - top_cap_height;
                double cap_bottom = bottom + bottom_cap_height;

                if (cap_top < cap_bottom)
                {
                    cap_top = (cap_top + cap_bottom) / 2;
                    cap_bottom = cap_top;
                    top = cap_top + top_cap_height;
                    bottom = cap_bottom - bottom_cap_height;
                }

                Color left_col;
                Color right_col;

                if (note.start_time < midi_time)
                {
                    // key_pressed[note.key] = true;

                    // TODO blend colors

                    key_notes[note.key]++;
                }
                // DrawRectangle(left, cap_top, right - left, cap_bottom - cap_top, RAYWHITE);
                // DrawRectangle(left, top, right - left, bottom - top, RAYWHITE);
                // DrawRectangle(left, cap_top, right - left, cap_bottom - top, RAYWHITE);

                double y = note.start_time - midi_time;
                double height = note.duration;

                if (!layout.black_key[note.key])
                {
                    DrawRectangle(
                        key->left,
                        (-y - height) / scale,
                        key->right - key->left,
                        height / scale,
                        color);
                }
                else
                {
                    DrawRectangle(
                        key->left - (KEY_WIDTH / 4),
                        (-y - height) / scale,
                        key->right - key->left - (KEY_WIDTH / 2),
                        height / scale,
                        color);
                }
            }

            i++;
        }

        for (int i = first_key; i < last_key; i++)
        {
            if (layout.black_key[i])
                continue;

            DrawRectangle(layout.keys[i].left, WINDOW_HEIGHT - keyboard_height, layout.keys[i].right - layout.keys[i].left, keyboard_height, RAYWHITE);
        }

        for (int i = first_key; i < last_key; i++)
        {
            if (!layout.black_key[i])
                continue;

            DrawRectangle(
                layout.keys[i].left - (KEY_WIDTH / 4),
                WINDOW_HEIGHT - keyboard_height,
                layout.keys[i].right - layout.keys[i].left - (KEY_WIDTH / 2),
                (keyboard_height / 3) * 2, BLACK);
        }

        char midi_text[256];
        sprintf(midi_text, "Position: %f", midi_time);
        DrawText(midi_text, 0, 50, 20, GRAY);

        sprintf(midi_text, "Scale: %f", scale);
        DrawText(midi_text, 0, 80, 20, GRAY);

        DrawFPS(0, 0);

        EndDrawing();
    }

    CloseWindow();
}
