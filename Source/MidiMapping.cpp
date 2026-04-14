#include "MidiMapping.h"
#include <cstring>

// --- MidiAction → APVTS parameter ID ---

static const char* const kParamIDs[] = {
    // Booleans (must match MidiAction enum order exactly)
    "record", "play", "stop", "mute", "solo", "monitor", "reverse", "reset",
    "nextTrack", "previousTrack", "newTrack", "removeTrack",
    "nextLoop", "previousLoop", "newLoop", "removeLoop",
    "muteAll", "stopAll", "startAll", "pauseAll", "resetAll",
    "addToGroup", "removeFromGroup", "nextSnapMode", "nextRecMode",
    "click", "bounce", "trigger", "incFixed", "undo", "redo",
    // Floats
    "globalMix", "clickLevel", "inputLevel", "outputLevel", "latency",
    // Choice/Int
    "snap", "mode", "selectGroup", "trackSelect", "loopSelect", "fixed",
    // Non-param actions - nullptr
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr,
};

const char* midiActionToParamID(MidiAction action)
{
    auto idx = static_cast<uint16_t>(action);
    if (idx >= static_cast<uint16_t>(MidiAction::COUNT))
        return nullptr;
    return kParamIDs[idx];
}

// --- MidiAction → display name ---

static const char* const kDisplayNames[] = {
    "Record", "Play", "Stop", "Mute", "Solo", "Monitor", "Reverse", "Clear",
    "Next Track", "Previous Track", "New Track", "Remove Track",
    "Next Loop", "Previous Loop", "New Loop", "Remove Loop",
    "Mute All", "Stop All", "Start All", "Pause All", "Reset All",
    "Add to Group", "Remove from Group", "Next Snap Mode", "Next Rec Mode",
    "Click", "Bounce", "Trigger", "Inc Fixed", "Undo", "Redo",
    "Global Mix", "Click Level", "Input Level", "Output Level", "Latency",
    "Snap", "Mode", "Select Group", "Track Select", "Loop Select", "Fixed",
    "Tempo Up", "Tempo Down", "Pitch Up", "Pitch Down",
    "Previous Group", "Next Group", "Toggle Layout",
};

const char* midiActionDisplayName(MidiAction action)
{
    auto idx = static_cast<uint16_t>(action);
    if (idx >= static_cast<uint16_t>(MidiAction::COUNT))
        return "Unknown";
    return kDisplayNames[idx];
}

// --- MidiMappingTable ---

int MidiMappingTable::find(uint8_t channel, uint8_t type, uint8_t number) const
{
    int n = count.load(std::memory_order_acquire);
    for (int i = 0; i < n; ++i)
    {
        auto& e = entries[i];
        if (e.type == type && e.number == number
            && (e.channel == 0 || e.channel == channel))
        {
            return i;
        }
    }
    return -1;
}

bool MidiMappingTable::add(const MidiMappingEntry& entry)
{
    int n = count.load(std::memory_order_relaxed);

    // Check for duplicate (same channel/type/number) - replace if found
    for (int i = 0; i < n; ++i)
    {
        auto& e = entries[i];
        if (e.type == entry.type && e.number == entry.number
            && e.channel == entry.channel)
        {
            entries[i] = entry;
            return true;
        }
    }

    if (n >= kMaxMidiMappings)
        return false;

    entries[n] = entry;
    count.store(n + 1, std::memory_order_release);
    return true;
}

bool MidiMappingTable::remove(int index)
{
    int n = count.load(std::memory_order_relaxed);
    if (index < 0 || index >= n)
        return false;

    // Move last entry into the removed slot
    if (index < n - 1)
        entries[index] = entries[n - 1];

    count.store(n - 1, std::memory_order_release);
    return true;
}

void MidiMappingTable::clear()
{
    count.store(0, std::memory_order_release);
}
