#include "audio_backends/sfml_wrapper.hpp"

audio_wrapper::~audio_wrapper() = default;

PlayerStatus sfml_wrapper::getStatus() const {
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

void sfml_wrapper::play() {
    interface.play();
    return;
}

void sfml_wrapper::stop() {
    interface.stop();
    return;
}

void sfml_wrapper::pause() {
    interface.pause();
    return;
}

void sfml_wrapper::setVolume(int new_vol) {
    interface.setVolume(static_cast<float>(new_vol));
    return;
}

float sfml_wrapper::getPercentPlayed() const {
    return static_cast<float>(interface.getPlayingOffset().asMilliseconds())/static_cast<float>(std::max(interface.getDuration().asMilliseconds(),1));
}

void sfml_wrapper::seekByPercent(float percent) {
    interface.setPlayingOffset(percent*interface.getDuration());
    return;
}

bool sfml_wrapper::openFromFile(const std::string &path) {
    return interface.openFromFile(path);
}

audio_interface audio_handle = audio_interface::make<sfml_wrapper>();
