#ifndef SFML_PLAYER_HPP
#define SFML_PLAYER_HPP

#include "music_player.hpp"

#include <SFML/Audio.hpp>

class sfml_player : public music_player
{
public:
    sfml_player() = default;
    ~sfml_player() override = default;

    sfml_player(const sfml_player &other) = delete;
    sfml_player &operator=(const sfml_player &other) = delete;

    sfml_player(sfml_player &&other) = default;
    sfml_player &operator=(sfml_player &&other) = default;

    PlayerStatus getStatus() override;
    void play() override;
    void stop() override;
    void setVolume(int new_vol) override;
    float getPercentPlayed() override;
    void pause() override;
    bool openFromFile(const std::string &path) override;
    void seekByPercent(float percent) override;
private:
    sf::Music interface;
};

#endif // SFML_PLAYER_HPP
