#ifndef TAG_HANDLER_HPP
#define TAG_HANDLER_HPP

#include <string>

class tag_handler
{
public:
    virtual ~tag_handler() = 0;
    virtual void openFromFile(const std::string &file_path) = 0;
    virtual std::string track_title() = 0;
    virtual std::string album_title() = 0;
    virtual std::string artist_title() = 0;
    virtual std::string recording_year() = 0;
    virtual std::string duration() = 0;
    virtual bool ownsAFile() = 0;
};

#endif // TAG_HANDLER_HPP
