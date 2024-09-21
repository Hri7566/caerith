#ifndef _ae_midi_h
#define _ae_midi_h

#include <string>
#include <stdint.h>
#include <vector>
#include <fstream>

enum MIDIEventStatus : uint8_t
{
    NoteOff = 0x80,
    NoteOn = 0x90,
    Aftertouch = 0xa0,
    ControlChange = 0xb0,
    ProgramChange = 0xc0,
    ChannelPressure = 0xd0,
    PitchBend = 0xe0,
    SystemExclusive = 0xf0
};

enum MIDIMeta : uint8_t
{
    Sequence = 0x00,
    Text = 0x01,
    Copyright = 0x02,
    TrackName = 0x03,
    InstrumentName = 0x04,
    Lyrics = 0x05,
    Marker = 0x06,
    CuePoint = 0x07,
    ChannelPrefix = 0x20,
    EndOfTrack = 0x2f,
    SetTempo = 0x51,
    SMPTEOffset = 0x54,
    TimeSignature = 0x58,
    KeySignature = 0x59,
    SequencerSpecific = 0x7f
};

typedef struct
{
    char chunk_type[4];
    uint32_t length;
} MIDIChunk;

typedef struct
{
    MIDIChunk chunk_header;
    uint16_t format;
    uint16_t num_tracks;
    uint16_t division;
} __attribute__((packed)) MIDIHeaderChunk;

typedef struct
{
    uint32_t delta;
    uint8_t status;
    uint8_t type;
} __attribute__((packed)) MIDIEvent;

typedef struct
{
    MIDIChunk chunk_header;
    std::vector<MIDIEvent> events;
} MIDITrackChunk;

typedef struct
{
    enum class Type
    {
        NoteOff,
        NoteOn,
        Other
    } event;

    uint8_t key = 0;
    uint8_t velocity = 0;
    uint32_t delta_tick = 0;
} AerithEvent;

typedef struct
{
    uint8_t key = 0;
    uint8_t velocity = 0;

    uint32_t start_time = 0;
    uint32_t duration = 0;
} AerithNote;

typedef struct
{
    std::string name;
    std::string instrument;

    std::vector<AerithEvent> events;
    std::vector<AerithNote> notes;

    uint8_t max_note = 64;
    uint8_t min_note = 64;
} AerithTrack;

class MIDI
{
public:
    MIDIHeaderChunk header;
    std::ifstream smf;
    uint32_t tempo;
    uint32_t bpm;

    MIDI();
    ~MIDI();

    std::vector<AerithTrack> tracks;

    uint8_t max_note = 64;
    uint8_t min_note = 64;

    void parse_file(std::string path);
};

#endif
