#ifndef TAGLIB_HANDLER_HPP
#define TAGLIB_HANDLER_HPP

#include <taglib/fileref.h>

#include "metadata_interface.hpp"

class taglib_wrapper : public metadata_wrapper
{
public:
    taglib_wrapper() = default;
    ~taglib_wrapper() override = default;
protected:
    // Used only by tag_handler::get_metadata_from()
    void openFromFile(const std::string &file_path) override;
    std::string track_title() override;
    std::string album_title() override;
    std::string artist_title() override;
    std::string recording_year() override;
    std::string duration() override;
    bool ownsAFile() override;
private:
    TagLib::FileRef interface;
};

extern metadata_interface metadata_handle;

#endif // TAGLIB_HANDLER_HPP
