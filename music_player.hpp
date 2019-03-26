#ifndef MUSIC_PLAYER_HPP
#define MUSIC_PLAYER_HPP

#include <mutex>
#include <optional>
#include <string>

enum class PlayerStatus { error, playing, paused, stopped };

class player_interface;

class music_player
{
public:
    music_player() = default;
    virtual ~music_player() = 0;

    virtual PlayerStatus getStatus() const = 0;
    virtual float getPercentPlayed() const = 0;

    virtual void pause() = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual bool openFromFile(const std::string &path) = 0;
    virtual void seekByPercent(float percent) = 0;
    virtual void setVolume( int new_vol) = 0;
};

#endif // MUSIC_PLAYER_HPP
