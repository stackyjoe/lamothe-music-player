#ifndef AUDIO_WRAPPER_HPP
#define AUDIO_WRAPPER_HPP

#include <mutex>
#include <optional>
#include <string>

class audio_interface;
enum class PlayerStatus { error, playing, paused, stopped };

/// \class audio_wrapper
/// \brief The audio_wrapper class is an interface for different backends. Currently,
///  only an SFML audio backend is implemented, however it should be pretty easy to
/// implement others as needed. Implementations MUST declare
///     friend std::unique_ptr<impl_class_name> std::make_unique<impl_class_name>();
/// for the template make() function in audio_interface to compile.

class audio_wrapper
{
public:
    virtual ~audio_wrapper() = 0;

    /// \brief Get a list of accepted file formats for the audio backend.
    /// \return A const reference to a vector of strings. The implementation
    /// is expected to own the memory.
    virtual const std::vector<std::string> &supported_file_formats()  const noexcept = 0;
    virtual PlayerStatus getStatus() const noexcept = 0;
    virtual float getPercentPlayed() const noexcept = 0;
    virtual std::chrono::milliseconds get_elapsed_time() const noexcept = 0;

    virtual void pause() = 0;
    virtual void play() = 0;
    virtual void stop() = 0;

    /// \brief openFromFile
    /// \param A string path giving the absolute path to the audio file.
    /// \return A bool indicating success or failure opening and streaming the
    /// audio file.
    virtual bool openFromFile(const std::string &path) = 0;
    virtual void seekByPercent(float percent) = 0;
    virtual void setVolume( int new_vol) = 0;
protected:
    audio_wrapper() = default;
};

#endif // AUDIO_WRAPPER_HPP
