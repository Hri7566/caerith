#include "midi.hpp"
#include "util.hpp"

#include <iostream>
#include <list>
#include <algorithm>

MIDI::MIDI()
{
    tempo = 0;
    bpm = 0;
}

MIDI::~MIDI()
{
}

void MIDI::parse_file(std::string path)
{
    smf.open(path, std::fstream::in | std::ios::binary);

    // load header chunk
    MIDIHeaderChunk header;
    smf.read((char *)&header, sizeof(MIDIHeaderChunk));

    // fix things
    header.chunk_header.length = swap32(header.chunk_header.length);
    header.format = swap16(header.format);
    header.num_tracks = swap16(header.num_tracks);
    header.division = swap16(header.division);

    if (header.division & 0x8000 == 0)
    {
        // bits 14-0 quarter note
    }
    else
    {
        // bits 14-8 contian a negative number
        // bits 7-0 contain ticks per frame
    }

    for (uint16_t i = 0; i < header.num_tracks; i++)
    {
        MIDITrackChunk mtrack;
        // AerithTrack track;
        // smf.read((char *)&track, sizeof(MIDITrackChunk));
        smf.read((char *)&mtrack.chunk_header, sizeof(MIDIChunk));

        bool end_of_track = false;
        uint8_t previous_status = 0;

        tracks.push_back(AerithTrack());

        // std::cout << "Track " << i << std::endl;

        while (!end_of_track && !smf.eof())
        {
            MIDIEvent event;
            event.delta = read_variable_quantity(&smf);
            event.status = smf.get();
            event.type = event.status & 0xf0;

            if (event.status < 0x80)
            {
                // no event thingy, go back and remember
                event.status = previous_status;
                smf.seekg(-1, std::ios_base::cur);
            }

            // std::cout << "Event delta: " << event.delta << std::endl;

            if (event.type == MIDIEventStatus::NoteOff)
            {
                previous_status = event.status;
                uint8_t channel = event.status & 0x0f;
                uint8_t note_id = smf.get();
                uint8_t velocity = smf.get();

                AerithEvent evt;
                evt.event = AerithEvent::Type::NoteOff;
                evt.delta_tick = event.delta;
                evt.key = note_id;
                evt.velocity = velocity;

                tracks[i].events.push_back(evt);
            }
            else if (event.type == MIDIEventStatus::NoteOn)
            {
                previous_status = event.status;
                uint8_t channel = event.status & 0x0f;
                uint8_t note_id = smf.get();
                uint8_t velocity = smf.get();

                if (velocity == 0)
                {
                    AerithEvent evt;
                    evt.event = AerithEvent::Type::NoteOff;
                    evt.delta_tick = event.delta;
                    evt.key = note_id;
                    evt.velocity = velocity;

                    tracks[i].events.push_back(evt);
                }
                else
                {
                    AerithEvent evt;
                    evt.event = AerithEvent::Type::NoteOn;
                    evt.delta_tick = event.delta;
                    evt.key = note_id;
                    evt.velocity = velocity;

                    tracks[i].events.push_back(evt);
                }
            }
            else if (event.type == MIDIEventStatus::Aftertouch)
            {
                previous_status = event.status;
                uint8_t channel = event.status & 0x0f;
                uint8_t note_id = smf.get();
                uint8_t velocity = smf.get();
                tracks[i].events.push_back({AerithEvent::Type::Other});
            }
            else if (event.type == MIDIEventStatus::ControlChange)
            {
                previous_status = event.status;
                uint8_t channel = event.status & 0x0f;
                uint8_t note_id = smf.get();
                uint8_t velocity = smf.get();
                tracks[i].events.push_back({AerithEvent::Type::Other});
            }
            else if (event.type == MIDIEventStatus::ProgramChange)
            {
                previous_status = event.status;
                uint8_t channel = event.status & 0x0f;
                uint8_t program_id = smf.get();
                tracks[i].events.push_back({AerithEvent::Type::Other});
            }
            else if (event.type == MIDIEventStatus::ChannelPressure)
            {
                previous_status = event.status;
                uint8_t channel = event.status & 0x0f;
                uint8_t channel_pressure = smf.get();
                tracks[i].events.push_back({AerithEvent::Type::Other});
            }
            else if (event.type == MIDIEventStatus::PitchBend)
            {
                previous_status = event.status;
                uint8_t channel = event.status & 0x0f;
                uint8_t ls7b = smf.get();
                uint8_t ms7b = smf.get();
                tracks[i].events.push_back({AerithEvent::Type::Other});
            }
            else if (event.type == MIDIEventStatus::SystemExclusive)
            {
                if (event.status == 0xf0)
                {
                    uint32_t stuff = read_variable_quantity(&smf);
                    // std::cout << "SystemExclusive start: " << stuff << std::endl;
                }

                if (event.status == 0xf7)
                {
                    uint32_t stuff = read_variable_quantity(&smf);
                    // std::cout << "SystemExclusive end: " << stuff << std::endl;
                }

                if (event.status == 0xff)
                {
                    uint8_t type = smf.get();
                    uint8_t length = read_variable_quantity(&smf);

                    // std::cout << "SystemExclusive detected: 0x" << std::hex << (uint32_t)type << ", length: 0x" << std::hex << (uint32_t)length << std::endl;

                    if (type == MIDIMeta::Sequence)
                    {
                        uint8_t thing1 = smf.get();
                        uint8_t thing2 = smf.get();
                        // std::cout << "Sequence number: " << thing1 << thing2 << std::endl;
                    }
                    else if (type == MIDIMeta::Text)
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Text: " << stuff << std::endl;
                    }
                    else if (type == MIDIMeta::Copyright)
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Copyright: " << stuff << std::endl;
                    }
                    else if (type == MIDIMeta::TrackName)
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Track name: " << stuff << std::endl;
                        tracks[i].name = stuff;
                    }
                    else if (type == MIDIMeta::InstrumentName)
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Instrument name: " << stuff << std::endl;
                    }
                    else if (type == MIDIMeta::Lyrics)
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Lyrics: " << stuff << std::endl;
                    }
                    else if (type == MIDIMeta::Marker)
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Marker: " << stuff << std::endl;
                    }
                    else if (type == MIDIMeta::CuePoint)
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Cue: " << stuff << std::endl;
                    }
                    else if (type == MIDIMeta::ChannelPrefix)
                    {
                        uint8_t prefix = smf.get();
                        // std::cout << "Prefix: " << smf.get() << std::endl;
                    }
                    else if (type == MIDIMeta::EndOfTrack)
                    {
                        // std::cout << "End of track" << std::endl;
                        end_of_track = true;
                    }
                    else if (type == MIDIMeta::SetTempo)
                    {
                        uint8_t b1 = smf.get();
                        uint8_t b2 = smf.get();
                        uint8_t b3 = smf.get();

                        if (tempo == 0)
                        {
                            (tempo |= (b1 << 16));
                            (tempo |= (b2 << 8));
                            (tempo |= (b3 << 0));
                            bpm = (60000000 / tempo);
                        }

                        // std::cout << "Tempo: " << std::dec << tempo << " (" << bpm << "bpm)" << std::endl;
                    }
                    else if (type == MIDIMeta::SMPTEOffset)
                    {
                        uint8_t h = smf.get();
                        uint8_t m = smf.get();
                        uint8_t s = smf.get();
                        uint8_t fr = smf.get();
                        uint8_t ff = smf.get();
                        // std::cout << "SMPTE: H:" << smf.get() << " M:" << smf.get() << " S:" << smf.get() << " FR:" << smf.get() << " FF:" << smf.get() << std::endl;
                    }
                    else if (type == MIDIMeta::TimeSignature)
                    {
                        uint8_t time_signature_top = smf.get();
                        uint8_t time_signature_bottom = 2 << smf.get();
                        uint8_t clocks_per_tick = smf.get();
                        uint8_t clocks_32per24 = smf.get();
                        // std::cout << "Time signature: " << smf.get() << "/" << (2 << smf.get()) << std::endl;
                        // std::cout << "Clocks per tick: " << smf.get() << std::endl;
                        // std::cout << "32 per 24 clocks: " << smf.get() << std::endl;
                    }
                    else if (type == MIDIMeta::KeySignature)
                    {
                        uint8_t key_signature = smf.get();
                        uint8_t minor_key = smf.get();
                        // std::cout << "Key signature: " << smf.get() << std::endl;
                        // std::cout << "Minor key: " << smf.get() << std::endl;
                    }
                    else if (type == MIDIMeta::SequencerSpecific)
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Sequencer specific: " << stuff << std::endl;
                    }
                    else
                    {
                        std::string stuff = read_string(&smf, length);
                        // std::cout << "Unknown SystemExclusive thing: 0x" << std::hex << (uint32_t)type << ", length: " << (uint32_t)length << ", stuff: " << stuff << std::endl;
                    }
                }
            }
            else
            {
                std::cout << "Unknown MIDI event: 0x" << std::hex << (uint32_t)event.status << std::endl;
            }
            // break;
            // mtrack.events.push_back(event);
        }

        // std::cout << "Track chunk type: " << track.chunk_header.chunk_type << std::endl;
        // break;
        // tracks.push_back(track);
    }

    for (auto &track : tracks)
    {
        uint32_t wall_time = 0;
        std::list<AerithNote> current_notes;

        for (auto &event : track.events)
        {
            wall_time += event.delta_tick;

            if (event.event == AerithEvent::Type::NoteOn)
            {
                current_notes.push_back({event.key, event.velocity, wall_time, 0});
            }

            if (event.event == AerithEvent::Type::NoteOff)
            {
                // detect un-stopped note
                auto note = std::find_if(current_notes.begin(), current_notes.end(), [&](const AerithNote &n)
                                         { return n.key == event.key; });

                if (note != current_notes.end())
                {
                    note->duration = wall_time - note->start_time;
                    track.notes.push_back(*note);

                    // track min/max
                    track.min_note = std::min(track.min_note, note->key);
                    track.max_note = std::max(track.max_note, note->key);

                    // global min/max
                    min_note = std::min(min_note, note->key);
                    max_note = std::max(max_note, note->key);

                    current_notes.erase(note);
                }
            }
        }
    }
}
