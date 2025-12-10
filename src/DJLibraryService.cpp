#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {







    }
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    //Todo: Implement buildLibrary method
    for (size_t i = 0; i < library_tracks.size(); i++) {
        
        AudioTrack* track = nullptr;
        
        if (library_tracks[i].type == "MP3") {
            // Create MP3Track
            track = new MP3Track(
                library_tracks[i].title, 

                library_tracks[i].artists,
                library_tracks[i].duration_seconds,
                library_tracks[i].bpm,
                library_tracks[i].extra_param1,      // bitrate
                library_tracks[i].extra_param2 != 0  // has_tags (convert int to bool)
            );
            std::cout << "MP3Track created: " << library_tracks[i].extra_param1 << " kbps\n";
        }
        else if (library_tracks[i].type == "WAV") {

            // Create WAVTrack
            track = new WAVTrack(
                library_tracks[i].title,

                library_tracks[i].artists,
                library_tracks[i].duration_seconds,
                library_tracks[i].bpm,
                library_tracks[i].extra_param1,  // sample_rate
                library_tracks[i].extra_param2   // bit_depth
            );
            std::cout << "WAVTrack created: " << library_tracks[i].extra_param1 << "Hz/" << library_tracks[i].extra_param2 << "bit\n";
        }
        
        if (track != nullptr) {

            library.push_back(track);
        }

    }
    
    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded\n";

}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    // Your implementation here
     return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    // Your implementation here
    std::cout << "[INFO] Loading playlist: " << playlist_name << "\n";
    
    // Clear existing playlist to avoid memory leak
    while (!playlist.is_empty()) {
        PlaylistNode* head = playlist.get_head();
        if (head != nullptr && head->track != nullptr) {
            playlist.remove_track(head->track->get_title());
        } else {
            break;
        }
    }
    
    // Create new playlist with the name
    playlist = Playlist(playlist_name);
    
    for (size_t i = 0; i < track_indices.size(); i++) {
        int index = track_indices[i];
        
        // Check if index is valid (1-based)
        if (index < 1 || index > (int)library.size()) {
            std::cout << "[WARNING] Invalid track index: " << index << "\n";
            continue;
        }
        
        // Get track from library (convert to 0-based)
        AudioTrack* originalTrack = library[index - 1];
        
        
        PointerWrapper<AudioTrack> cloned = originalTrack->clone();
        
        if (!cloned) {
            std::cout << "[ERROR] Failed to clone track\n";
            continue;
        }
        
       
        cloned.get()->load();
        cloned.get()->analyze_beatgrid();
        
        
        playlist.add_track(cloned.release());
        std::cout << "Added '" << originalTrack->get_title() << "' to playlist '" << playlist_name << "'\n";
    }
    
    std::cout << "[INFO] Playlist loaded: " << playlist_name << " (" << playlist.get_track_count() << " tracks)\n";
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */


std::vector<std::string> DJLibraryService::getTrackTitles() const {
    // Your implementation here
    std::vector<std::string> titles;

    PlaylistNode* current = playlist.get_head();
    while (current != nullptr) {

        titles.push_back(current->track->get_title());

        current = current->next;
    }
    
    return titles;
}

