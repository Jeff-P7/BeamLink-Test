#include "NexState.h"
#include <Arduino.h>

namespace nexstate {

// Global NexState instance
std::unique_ptr<NexState> g_nexState = nullptr;

NexState::NexState(const NexStateConfig& config) : config(config) {
    lastOutputTime = millis();
}

bool initialize(const NexStateConfig& config) {
    if (g_nexState) {
        return false; // Already initialized
    }
    
    g_nexState = std::make_unique<NexState>(config);
    return true;
}

NexState& State() {
    if (!g_nexState) {
        // Initialize with default config if not already initialized
        initialize();
    }
    return *g_nexState;
}

void update() {
    if (g_nexState) {
        g_nexState->update();
    }
}

void shutdown() {
    g_nexState.reset();
}

} // namespace nexstate
