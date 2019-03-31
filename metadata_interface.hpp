#ifndef TAG_HANDLER_HPP
#define TAG_HANDLER_HPP

#include <memory>
#include <mutex>
#include <string>
#include <type_traits>

#include "metadata_wrapper.hpp"
#include "music_metadata.hpp"

class metadata_interface
{
protected:
    std::mutex metadata_lock;
    std::unique_ptr<metadata_wrapper> implementation;

    template<class metadata_implementation>
    explicit metadata_interface(std::unique_ptr<metadata_implementation> &&_implementation)
        : implementation(std::move(_implementation)) {
        static_assert (std::is_base_of<metadata_wrapper, metadata_implementation>::value,
                       "metadata_interface template constructor must be called with a std::unique_ptr to a class derived from metadata_wrapper.");
    }

public:
    metadata_interface() = default;
    // Copy constructor and copy assignment are implicitly forbidden by virtue of owning a unique_ptr and mutex.
    metadata_interface(metadata_interface &&) = default;
    metadata_interface &operator=(metadata_interface &&) = default;

    music_metadata get_metadata_of(const std::string &file_path) {
        implementation->openFromFile(file_path);
        if(implementation->ownsAFile())
            return music_metadata(file_path,
                                  implementation->track_title(),
                                  implementation->album_title(),
                                  implementation->artist_title(),
                                  implementation->recording_year(),
                                  implementation->duration());
        return {};
    }

    template<class metadata_implementation>
    static metadata_interface make() {
        static_assert (std::is_base_of<metadata_wrapper,metadata_implementation>::value,
                       "audio_interface::make must be called with an implementation of the audio_wrapper class.");
        return metadata_interface(std::make_unique<metadata_implementation>());
    }
};

#endif // TAG_HANDLER_HPP
