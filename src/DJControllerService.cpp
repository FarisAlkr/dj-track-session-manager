#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    
//Check if already in cache (HIT)
    if (cache.contains(track.get_title())) {

        cache.get(track.get_title());  // Update MRU
        return 1;  //hit
    }
    
    //MISS - need to insert
    // Clone the track
    PointerWrapper<AudioTrack> cloned = track.clone();
    
    //Check if clone succeeded
    if (!cloned) {
        std::cout << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone\n";
        return 0;  // Failed
    }
    
    //Load and analyze the cloned track
    cloned.get()->load();
    cloned.get()->analyze_beatgrid();
    
    //Put in cache
    bool evicted = cache.put(std::move(cloned));
    
    //Return based on eviction
    if (evicted) {
        return -1;  // MISS with eviction
    }
    return 0;  // MISS without eviction
}




void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    // Your implementation here


     return cache.get(track_title);
    
}
