#ifndef TAG_HANDLER_HPP
#define TAG_HANDLER_HPP

#include <string>

#include "music_metadata.hpp"

class metadata_interface
{
public:
    music_metadata get_metadata_of(const std::string &file_path) {
        openFromFile(file_path);
        if(ownsAFile())
            return music_metadata(track_title(),album_title(),artist_title(),recording_year(),duration());
        return music_metadata();
    }

protected:
    virtual ~metadata_interface() = 0;
    virtual void openFromFile(const std::string &file_path) = 0;
    virtual std::string track_title() = 0;
    virtual std::string album_title() = 0;
    virtual std::string artist_title() = 0;
    virtual std::string recording_year() = 0;
    virtual std::string duration() = 0;
    virtual bool ownsAFile() = 0;
};

#endif // TAG_HANDLER_HPP
