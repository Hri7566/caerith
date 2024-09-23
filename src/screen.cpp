#include "screen.hpp"
#include "layout.hpp"

#include <cmath>
#include <iostream>
#include <rtmidi/RtMidi.h>

Color note_colors[] = {BLUE, GREEN, ORANGE, YELLOW, MAGENTA, RED};

void start_screen(MIDI *smf)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetWindowMonitor(0);
    SetTargetFPS(60);

    // ToggleBorderlessWindowed();

    RtMidiOut midi_out;

    uint32_t scroll_pos = 0;
    float scale = 1.0f;

    uint8_t left_key = smf->min_note;
    uint8_t right_key = smf->max_note;

    PianoLayout layout = PianoLayout(left_key, right_key);
    float delta = 0;
    bool playing = false;

    Texture note_tex = LoadTexture("textures/note.png");
    Texture note_top_tex = LoadTexture("textures/noteTop.png");
    Texture note_bottom_tex = LoadTexture("textures/noteBottom.png");

    Rectangle note_source = {
        0,
        0,
        30,
        162};

    Rectangle note_top_source = {
        0,
        0,
        30,
        5};

    Rectangle note_bottom_source = {
        0,
        0,
        30,
        6};

    while (!WindowShouldClose())
    {
        delta = GetFrameTime();

        if (IsKeyPressed(KEY_SPACE))
            playing = !playing;

        if (playing)
        {
            scroll_pos += 750 * delta;
        }

        layout.update(delta);

        BeginDrawing();
        ClearBackground(Color{0x1a, 0x1a, 0x1a});

        int track_num = 0;
        for (AerithTrack track : smf->tracks)
        {
            int i = 0;
            Color track_color = get_track_color(track_num);
            int note_range_size = track.max_note - track.min_note;

            if (track.notes.size() < 1)
                continue;

            for (AerithNote note : track.notes)
            {
                int key_index = note.key - left_key;
                PianoKey key = layout.keys[key_index];

                float note_position = (-(float)note.start_time - (float)note.duration) * scale;
                float note_height = float(note.duration) * scale;
                // std::cout << std::dec << note.start_time << " (" << floor(note_position) << ") " << note.duration << std::endl;

                int note_bottom_pos = (int)floor(note_position + note_height) + scroll_pos;
                int note_top_pos = (int)floor(note_position) + scroll_pos;

                if (note_bottom_pos > 0 && note_top_pos < WINDOW_HEIGHT - layout.keyboard_height)
                {
                    if (!layout.black_key[key_index])
                    {
                        Rectangle dest = {
                            key.position_x,
                            floor(note_position) + scroll_pos,
                            layout.key_width,
                            floor(note_height)};

                        Rectangle inner = {
                            dest.x,
                            dest.y + 4,
                            dest.width,
                            dest.height - 8};

                        Rectangle top_cap = {
                            dest.x,
                            dest.y,
                            dest.width,
                            4};

                        Rectangle bottom_cap = {
                            dest.x,
                            dest.y + dest.height - 4,
                            dest.width,
                            4};

                        DrawTexturePro(note_tex, note_source, inner, {0, 0}, 0, track_color);
                        DrawTexturePro(note_top_tex, note_top_source, top_cap, {0, 0}, 0, track_color);
                        DrawTexturePro(note_bottom_tex, note_bottom_source, bottom_cap, {0, 0}, 0, track_color);
                    }

                    if (
                        note_position + scroll_pos <= WINDOW_HEIGHT - layout.keyboard_height &&
                        note_position + note_height + scroll_pos > WINDOW_HEIGHT - layout.keyboard_height)
                    {
                        layout.key_colors[key_index] = track_color;
                        layout.pressed_keys[key_index] = true;
                    }
                }

                i++;
            }

            for (AerithNote note : track.notes)
            {
                int key_index = note.key - left_key;
                PianoKey key = layout.keys[key_index];

                float note_position = (-(float)note.start_time - (float)note.duration) * scale;
                float note_height = float(note.duration) * scale;
                // std::cout << std::dec << note.start_time << " (" << floor(note_position) << ") " << note.duration << std::endl;

                if (floor(note_position + note_height) + scroll_pos < 0)
                    continue;

                if (floor(note_position) + scroll_pos > WINDOW_HEIGHT)
                    continue;

                if (layout.black_key[key_index])
                {
                    Rectangle dest = {
                        key.position_x + (layout.key_width / 3),
                        floor(note_position) + scroll_pos,
                        layout.key_width / 3,
                        floor(note_height)};

                    Rectangle inner = {
                        dest.x,
                        dest.y + 4,
                        dest.width,
                        dest.height - 8};

                    Rectangle top_cap = {
                        dest.x,
                        dest.y,
                        dest.width,
                        4};

                    Rectangle bottom_cap = {
                        dest.x,
                        dest.y + dest.height - 4,
                        dest.width,
                        4};

                    DrawTexturePro(note_tex, note_source, inner, {0, 0}, 0, track_color);
                    DrawTexturePro(note_top_tex, note_top_source, top_cap, {0, 0}, 0, track_color);
                    DrawTexturePro(note_bottom_tex, note_bottom_source, bottom_cap, {0, 0}, 0, track_color);
                }

                if (
                    note_position + scroll_pos <= WINDOW_HEIGHT - layout.keyboard_height &&
                    note_position + note_height + scroll_pos > WINDOW_HEIGHT - layout.keyboard_height)
                {
                    layout.key_colors[key_index] = track_color;
                    layout.pressed_keys[key_index] = true;
                }

                i++;
            }
            track_num++;
        }

        layout.draw();

        DrawFPS(0, 0);
        // char scroll_pos_text[256];
        // sprintf(scroll_pos_text, "Scroll position: %d", scroll_pos);
        // DrawText(scroll_pos_text, 0, 20, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
}

Color get_track_color(int num)
{
    return note_colors[num % 6];
}
