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

    // clear old stuff first
    for (size_t i = 0; i < library.size(); i++) {
        delete library[i];
    }
    library.clear();

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

    // need to clear old playlist first or we get memory leaks
    std::vector<AudioTrack*> old_tracks = playlist.getTracks();
    for (size_t i = 0; i < old_tracks.size(); i++) {
        playlist.remove_track(old_tracks[i]->get_title());
    }

    playlist = Playlist(playlist_name);
    
    for (size_t i = 0; i < track_indices.size(); i++) {
        int index = track_indices[i];
        
        if (index < 1 || index > (int)library.size()) {
            std::cout << "[WARNING] Invalid track index: " << index << "\n";
            continue;
        }
        
        AudioTrack* originalTrack = library[index - 1];
        
        PointerWrapper<AudioTrack> cloned = originalTrack->clone();
        
        if (!cloned) {
            std::cout << "[ERROR] Failed to clone track\n";
            continue;
        }
        
        cloned.get()->load();
        cloned.get()->analyze_beatgrid();
        
        playlist.add_track(cloned.release());
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
    
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    for (size_t i = 0; i < tracks.size(); i++) {
        titles.push_back(tracks[i]->get_title());
    }
    
    return titles;
}

