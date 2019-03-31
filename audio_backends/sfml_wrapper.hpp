#ifndef SFML_WRAPPER_HPP
#define SFML_WRAPPER_HPP

#include <SFML/Audio.hpp>

#include "audio_interface.hpp"

class sfml_wrapper : public audio_wrapper
{
friend std::unique_ptr<sfml_wrapper> std::make_unique<sfml_wrapper>();
public:
    ~sfml_wrapper() override = default;

    sfml_wrapper(const sfml_wrapper &other) = delete;
    sfml_wrapper &operator=(const sfml_wrapper &other) = delete;

    sfml_wrapper(sfml_wrapper &&other) = default;
    sfml_wrapper &operator=(sfml_wrapper &&other) = default;

    std::chrono::milliseconds get_elapsed_time() const noexcept override;
    float getPercentPlayed() const noexcept override;
    PlayerStatus getStatus() const noexcept override;
    const std::vector<std::string> & supported_file_formats() const noexcept override;


    void pause() override;
    void play() override;
    void stop() override;

    bool openFromFile(const std::string &path) override;
    void seekByPercent(float percent) override;
    void setVolume(int new_vol) override;

protected:
    sfml_wrapper();

private:
    std::vector<std::string> supported_formats;
    sf::Music interface;
};

extern audio_interface audio_handle;

#endif // SFML_WRAPPER_HPP
