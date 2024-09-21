#include "layout.hpp"

Layout::Layout(int first_key, int last_key)
{
    int white_pos = 0;
    for (int i = 0; i < last_key - first_key; i++)
    {
        Key key;

        key.left = white_pos * KEY_WIDTH;
        key.right = key.left + KEY_WIDTH - 1;

        keys.push_back(key);

        if (
            i % 12 == 1 ||
            i % 12 == 4 ||
            i % 12 == 6 ||
            i % 12 == 9 ||
            i % 12 == 11)
        {
            key.left -= 50;
            key.right = key.left + (KEY_WIDTH / 2) - 1;
            white_pos--;
            black_key.push_back(true);
        }
        else
        {
            black_key.push_back(false);
        }

        white_pos++;
    }
}