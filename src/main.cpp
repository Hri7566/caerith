#include "midi.hpp"
#include "screen.hpp"

int main(int argc, char **argv)
{
    MIDI smf = MIDI();
    smf.parse_file("./test.mid");

    start_screen(&smf);
}
