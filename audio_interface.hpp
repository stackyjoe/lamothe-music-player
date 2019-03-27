#ifndef AUDIO_INTERFACE_HPP
#define AUDIO_INTERFACE_HPP

#include <optional>

#include "audio_wrapper.hpp"

// This is a thread safe interface for the abstract class music_player.
class  audio_interface {
protected:

    mutable std::mutex access_lock;
    std::unique_ptr<audio_wrapper> implementation;

    template<class audio_implementation>
    explicit audio_interface(std::unique_ptr<audio_implementation> _implementation) {
        static_assert (std::is_base_of<audio_wrapper, audio_implementation>::value,
                       "audio_interface template constructor must be called with a std::unique_ptr to a class derived from audio_player.");
        implementation = std::move(_implementation);
    }

public:
    PlayerStatus get_status() const {
        return implementation->getStatus();
    }

    float get_percent_played() const {
        return implementation->getPercentPlayed();
    }

    audio_interface() = default;
    // Copy constructor and copy assignment are implicitly forbidden by virtue of owning a unique_ptr and mutex.
    audio_interface(audio_interface &&) = default;
    audio_interface &operator=(audio_interface &&) = default;


    template<class audio_implementation>
    static audio_interface make() {
        static_assert (std::is_base_of<audio_wrapper,audio_implementation>::value,
                       "audio_interface::make must be called with an implementation of the audio_wrapper class.");
        return audio_interface(std::make_unique<audio_implementation>());
    }

    // Trick to avoid sharing a mutex for the music_player directly
    template<class operation>
    auto perform(operation o) const -> decltype (o(*implementation)) {
        static_assert (std::is_invocable<decltype(o), audio_wrapper &>::value,
                       "audio_interface::perform should be called on lambda invocable with a audio_wrapper & argument.");
        std::scoped_lock access(access_lock);
        return o(*implementation);
    }

    template<class operation>
    auto try_perform(operation o) const -> std::optional<decltype(o(implementation))> {
        static_assert (std::is_invocable<decltype(o), audio_wrapper &>::value,
                       "audio_interface::try_perform should be called on lambda invocable with a audio_wrapper & argument.");
        std::unique_lock access(access_lock, std::try_to_lock);
        if(not access.owns_lock())
            return std::nullopt;
        return o(implementation);
    }
};

#endif // AUDIO_INTERFACE_HPP
