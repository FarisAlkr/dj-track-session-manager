#include "LRUCache.h"
#include <iostream>

LRUCache::LRUCache(size_t capacity)
    : slots(capacity), max_size(capacity), access_counter(0) {}

bool LRUCache::contains(const std::string& track_id) const {
    return findSlot(track_id) != max_size;
}

AudioTrack* LRUCache::get(const std::string& track_id) {
    size_t idx = findSlot(track_id);
    if (idx == max_size) return nullptr;
    return slots[idx].access(++access_counter);
}

/**
 * TODO: Implement the put() method for LRUCache
 */
bool LRUCache::put(PointerWrapper<AudioTrack> track) {
    
    
    if (track==nullptr) {
        return false;
    }
    
    //Check if track already exists in cache
    size_t existing = findSlot(track.get()->get_title());
    if (existing != max_size) {
        // Track exists update its access time
        slots[existing].access(++access_counter);
        return false;
    }
    
    //If cache is full, evict LRU first
    bool evicted = false;
    if (isFull()) {
        evictLRU();
        evicted = true;
    }
    
    
    size_t empty = findEmptySlot();
    
    // Store the track in empty slot
    slots[empty].store(std::move(track), ++access_counter);
    
    
    return evicted;

}


bool LRUCache::evictLRU() {
    size_t lru = findLRUSlot();
    if (lru == max_size || !slots[lru].isOccupied()) return false;
    slots[lru].clear();
    return true;
}

size_t LRUCache::size() const {
    size_t count = 0;
    for (const auto& slot : slots) if (slot.isOccupied()) ++count;
    return count;
}

void LRUCache::clear() {
    for (auto& slot : slots) {
        slot.clear();
    }
}

void LRUCache::displayStatus() const {
    std::cout << "[LRUCache] Status: " << size() << "/" << max_size << " slots used\n";
    for (size_t i = 0; i < max_size; ++i) {
        if(slots[i].isOccupied()){
            std::cout << "  Slot " << i << ": " << slots[i].getTrack()->get_title()
                      << " (last access: " << slots[i].getLastAccessTime() << ")\n";
        } else {
            std::cout << "  Slot " << i << ": [EMPTY]\n";
        }
    }
}

size_t LRUCache::findSlot(const std::string& track_id) const {
    for (size_t i = 0; i < max_size; ++i) {
        if (slots[i].isOccupied() && slots[i].getTrack()->get_title() == track_id) return i;
    }
    return max_size;

}

/**
 * TODO: Implement the findLRUSlot() method for LRUCache
 */
size_t LRUCache::findLRUSlot() const {

    size_t lru_index = max_size;  //Means Invalid index (not found)
    
    for (size_t i = 0;i< max_size;i++) {

        if (slots[i].isOccupied()) {
            // If this is our first occupied slot, OR it has smaller time
            if ((lru_index == max_size) || slots[i].getLastAccessTime()<slots[lru_index].getLastAccessTime()) {
                lru_index = i;
            }
        }
    }
    return lru_index; // Placeholder
    // fixed it
}

size_t LRUCache::findEmptySlot() const {
    for (size_t i = 0; i < max_size; ++i) {
        if (!slots[i].isOccupied()) return i;
    }
    return max_size;
}

void LRUCache::set_capacity(size_t capacity){
    if (max_size == capacity)
        return;
    //udpate max size
    max_size = capacity;
    //update the slots vector
    slots.resize(capacity);
}