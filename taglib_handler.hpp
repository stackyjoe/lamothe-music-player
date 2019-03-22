#ifndef TAGLIB_HANDLER_HPP
#define TAGLIB_HANDLER_HPP

#include <taglib/fileref.h>

#include "tag_handler.hpp"


class taglib_handler : public tag_handler
{
public:
    taglib_handler() = default;
    ~taglib_handler() override = default;
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

#endif // TAGLIB_HANDLER_HPP
