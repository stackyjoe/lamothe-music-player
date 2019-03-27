#ifndef SFML_PLAYER_HPP
#define SFML_PLAYER_HPP

#include "audio_interface.hpp"

#include <SFML/Audio.hpp>

class sfml_wrapper : public audio_wrapper
{
public:
    sfml_wrapper() = default;
    ~sfml_wrapper() override = default;

    sfml_wrapper(const sfml_wrapper &other) = delete;
    sfml_wrapper &operator=(const sfml_wrapper &other) = delete;

    sfml_wrapper(sfml_wrapper &&other) = default;
    sfml_wrapper &operator=(sfml_wrapper &&other) = default;

    PlayerStatus getStatus() const override;

    float getPercentPlayed() const override;

protected:
    void pause() override;
    void play() override;
    void stop() override;
    bool openFromFile(const std::string &path) override;
    void seekByPercent(float percent) override;
    void setVolume(int new_vol) override;

private:
    sf::Music interface;
};

extern audio_interface audio_handle;

#endif // SFML_PLAYER_HPP
