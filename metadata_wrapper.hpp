#ifndef METADATA_WRAPPER_H
#define METADATA_WRAPPER_H

#include <string>

class metadata_wrapper {
public:
    metadata_wrapper() = default;
    virtual ~metadata_wrapper() = 0;
    virtual void openFromFile(const std::string &file_path) = 0;
    virtual std::string track_title() = 0;
    virtual std::string album_title() = 0;
    virtual std::string artist_title() = 0;
    virtual std::string recording_year() = 0;
    virtual std::string duration() = 0;
    virtual bool ownsAFile() = 0;
};
#endif // METADATA_WRAPPER_H
