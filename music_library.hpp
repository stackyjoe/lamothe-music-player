#ifndef MUSIC_LIBRARY_HPP
#define MUSIC_LIBRARY_HPP

#include <optional>
#include <set>
#include <vector>

#include <QString>

#include <SFML/Audio.hpp>

#include "music_player.hpp"

enum class UserDesiredState { error, play, pause, stop};

class music_library
{
public:
    music_library() = default;
    ~music_library() = default;

    music_library(std::string library_path, music_player &_player);

    music_library(const music_library &other) = delete;
    music_library &operator=(const music_library &other) = delete;

    music_library(music_library &&other) = default;
    music_library &operator=(music_library &&other) = default;

    std::string next_song();
    void setState(UserDesiredState _state);
    UserDesiredState getState();
    void setLibraryFile(QString lib_path);
    void find_song(const std::string &path);
    void setVolume(int new_vol);
    float getPercentPlayed();
    void seekByPercent(float percent);
    void stop();
    bool contains(const std::string &song_path) const;
    void add(std::string song_path);
    void remove(const std::string &song_path);
    const std::vector<std::string> &songs();
    const std::string what_file_is_playing();

protected:
    // Optional iterator is set to std::nullopt to indicate iterator invalidation
    std::optional<std::vector<std::string>::iterator> current_song;
    UserDesiredState desired_state;
    music_player &player;
    std::set<std::string> unique_song_paths;
    std::vector<std::string> song_paths;
    std::mutex player_mutex;
};

#endif // MUSIC_LIBRARY_HPP
