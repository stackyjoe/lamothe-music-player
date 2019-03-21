#include "sfml_player.hpp"

music_player::~music_player() = default;

PlayerStatus sfml_player::getStatus() {
    PlayerStatus st = PlayerStatus::error;

    switch(interface.getStatus()) {
    case sf::Music::Status::Paused:
        st = PlayerStatus::paused;
        break;

    case sf::Music::Status::Playing:
        st = PlayerStatus::playing;
        break;

    case sf::Music::Status::Stopped:
        st = PlayerStatus::stopped;
        break;

    }

    return st;
}

void sfml_player::play() {
    interface.play();
    return;
}

void sfml_player::stop() {
    interface.stop();
    return;
}

void sfml_player::pause() {
    interface.pause();
    return;
}

void sfml_player::setVolume(int new_vol) {
    interface.setVolume(static_cast<float>(new_vol));
    return;
}

float sfml_player::getPercentPlayed() {
    return static_cast<float>(interface.getPlayingOffset().asMilliseconds())/static_cast<float>(std::max(interface.getDuration().asMilliseconds(),1));
}

void sfml_player::seekByPercent(float percent) {
    interface.setPlayingOffset(percent*interface.getDuration());
    return;
}

bool sfml_player::openFromFile(const std::string &path) {
    return interface.openFromFile(path);
}
