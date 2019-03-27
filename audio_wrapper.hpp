#ifndef AUDIO_WRAPPER_HPP
#define AUDIO_WRAPPER_HPP

#include <mutex>
#include <optional>
#include <string>

enum class PlayerStatus { error, playing, paused, stopped };

class audio_wrapper
{
public:
    audio_wrapper() = default;
    virtual ~audio_wrapper() = 0;

    virtual PlayerStatus getStatus() const = 0;
    virtual float getPercentPlayed() const = 0;

    virtual void pause() = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual bool openFromFile(const std::string &path) = 0;
    virtual void seekByPercent(float percent) = 0;
    virtual void setVolume( int new_vol) = 0;
};

#endif // AUDIO_WRAPPER_HPP
