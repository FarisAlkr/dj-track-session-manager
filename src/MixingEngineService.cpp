#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(0), auto_sync(false), bpm_tolerance(0)
{
    // Your implementation here
     decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks.\n";
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std::cout << "[MixingEngineService] Cleaning up decks...\n";
    delete decks[0];
    decks[0] = nullptr;
    
    delete decks[1];
    decks[1] = nullptr;
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    // Your implementation here
  std::cout << "\n=== Loading Track to Deck ===\n";
    
    // Clone the track
    PointerWrapper<AudioTrack> cloned=track.clone();
    
    // Check if clone worked
    if (!cloned) {
        std::cout << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone\n";
        return -1;
    }
    
    // Find target deck 
    int target = 1 -active_deck;
    
    // First track special case - both decks empty
    if (decks[0] == nullptr && decks[1] == nullptr) {
        target = 0;
    }
    
    std::cout << "[Deck Switch] Target deck: " << target << "\n";
    
    // If target deck has something, delete it first
    if (decks[target] != nullptr) {
        delete decks[target];
        decks[target] = nullptr;
    }
    
    // Load and analyze
    cloned.get()->load();
    cloned.get()->analyze_beatgrid();
    
    // BPM sync if needed
    if (decks[active_deck] != nullptr && auto_sync == true) {
        bool canMix = can_mix_tracks(cloned);
        if (canMix == false) {
            sync_bpm(cloned);
        }
    }
    
    // Put the track on the deck
    decks[target] = cloned.release();
    std::cout << "[Load Complete] '" << decks[target]->get_title() << "' is now loaded on deck " << target << "\n";
    
    // Unload the old active deck
    if (active_deck != target && decks[active_deck] != nullptr) {
        std::cout << "[Unload] Unloading previous deck " << active_deck << " (" << decks[active_deck]->get_title() << ")\n";
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }
    
    // Switch to new deck
    active_deck = target;
    std::cout << "[Active Deck] Switched to deck " << target << "\n";
    
    return target;
}



/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {

    // Your implementation here

    // Check if active deck has a track
    if(decks[active_deck] == nullptr) {
        return false;
    }
    if (!track) {
        return false;
    }
    
    // Get both BPMs
    int deckBpm = decks[active_deck]->get_bpm();
    int trackBpm = track.get()->get_bpm();
    
    // Calculate difference
    int difference = deckBpm - trackBpm;
    
    // Make it positive (absolute value)
    if (difference < 0) {
        difference = -difference;
    }
    
    // Check if within tolerance
    if (difference <= bpm_tolerance) {
        return true;
    }
    
    return false;
}


/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    
     // Check both are valid
    if (decks[active_deck] == nullptr) {
        return;
    }
    if (!track) {
        return;
    }
    
    
    int originalBpm = track.get()->get_bpm();
    
    
    int deckBpm = decks[active_deck]->get_bpm();
    
    // Calculate average
    int newBpm = (originalBpm + deckBpm) / 2;
    
    
    track.get()->set_bpm(newBpm);

    std::cout << "[Sync BPM] Syncing BPM from " << originalBpm << " to " << newBpm << "\n";

}
