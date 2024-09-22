#include "midi.hpp"
#include "screen.hpp"

#include <iostream>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <standard MIDI file>" << std::endl;
        return 1;
    }

    MIDI smf = MIDI();
    smf.parse_file(argv[1]);

    start_screen(&smf);

    return 0;
}
