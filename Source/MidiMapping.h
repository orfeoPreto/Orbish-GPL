#pragma once
#include <array>
#include <atomic>
#include <cstdint>

// Every mappable action in Orbish gets a unique ID.
// The first entries correspond to APVTS parameter IDs.
// The tail entries are non-parameter "editor actions" handled by the processor directly.
enum class MidiAction : uint16_t
{
    // --- APVTS boolean parameters ---
    Record = 0, Play, Stop, Mute, Solo, Monitor, Reverse, Reset,
    NextTrack, PreviousTrack, NewTrack, RemoveTrack,
    NextLoop, PreviousLoop, NewLoop, RemoveLoop,
    MuteAll, StopAll, StartAll, PauseAll, ResetAll,
    AddToGroup, RemoveFromGroup, NextSnapMode, NextRecMode,
    Click, Bounce, Trigger, IncFixed, Undo, Redo,

    // --- APVTS float parameters ---
    GlobalMix, ClickLevel, InputLevel, OutputLevel, Latency,

    // --- APVTS choice/int parameters ---
    Snap, Mode, SelectGroup, TrackSelect, LoopSelect, Fixed,

    // --- Non-parameter actions (processor methods) ---
    TempoUp, TempoDown, PitchUp, PitchDown,

    COUNT  // sentinel - total number of actions
};

// Maps MidiAction → APVTS parameter ID string. Returns nullptr for non-parameter actions.
const char* midiActionToParamID(MidiAction action);

// Human-readable display name for UI.
const char* midiActionDisplayName(MidiAction action);

// A single MIDI-to-action binding. 8 bytes, trivially copyable, cache-friendly.
struct MidiMappingEntry
{
    uint8_t  channel  = 0;  // 1-16, or 0 = any channel
    uint8_t  type     = 0;  // 0 = CC, 1 = Note On/Off
    uint8_t  number   = 0;  // CC number or note number (0-127)
    uint8_t  reserved = 0;  // padding / future flags
    uint16_t action   = 0;  // MidiAction cast to uint16_t
    uint16_t flags    = 0;  // bit 0: toggle mode for booleans
};

// Fixed-capacity mapping table. No heap allocation on lookup.
// Maximum 128 mappings - more than enough for any practical setup.
static constexpr int kMaxMidiMappings = 128;

struct MidiMappingTable
{
    std::array<MidiMappingEntry, kMaxMidiMappings> entries{};
    std::atomic<int> count{0};

    // RT-safe O(n) scan. n <= 128, each entry is 8 bytes = ~1KB total, fits L1 cache.
    // Returns index of matching entry, or -1 if not found.
    int find(uint8_t channel, uint8_t type, uint8_t number) const;

    // Called from message thread only.
    bool add(const MidiMappingEntry& entry);
    bool remove(int index);
    void clear();
};

// Atomic state machine for the MIDI learn handshake between audio thread and UI.
struct MidiLearnState
{
    std::atomic<int>      active{0};           // 0 = inactive, 1 = waiting for MIDI
    std::atomic<uint16_t> targetAction{0};     // which MidiAction we're learning
    std::atomic<uint8_t>  capturedChannel{0};  // processor writes these when it captures
    std::atomic<uint8_t>  capturedType{0};
    std::atomic<uint8_t>  capturedNumber{0};
    std::atomic<int>      captured{0};         // 0 = not yet, 1 = captured
};
