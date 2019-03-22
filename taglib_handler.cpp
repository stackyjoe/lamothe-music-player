#include "taglib_handler.hpp"

tag_handler::~tag_handler() = default;

void taglib_handler::openFromFile(const std::string &file_path) {
    interface = TagLib::FileRef(file_path.data());
}

std::string taglib_handler::track_title() {
    return std::string(interface.tag()->title().toCString(true));
}
std::string taglib_handler::album_title() {
    return std::string(interface.tag()->album().toCString(true));
}
std::string taglib_handler::artist_title() {
    return std::string(interface.tag()->artist().toCString(true));
}
std::string taglib_handler::recording_year() {
    unsigned int year = interface.tag()->year();
    // 0 functions as the default value if the tag is not initialized in the file.
    if(year > 0)
        return std::to_string(year);
    return std::string("");
}
std::string taglib_handler::duration() {
    const int duration = interface.file()->audioProperties()->lengthInSeconds();
    int hours = duration / (60*60);
    int minutes = duration / 60;
    int seconds = duration % 60;

    std::string time;
    if(hours) {
        time += std::to_string(hours) + ":";
    }
    if(minutes < 10)
        time += "0";
    time += std::to_string(minutes) + ":";
    if(seconds < 10)
        time += "0";
    time += std::to_string(seconds);

    return time;
}

bool taglib_handler::ownsAFile() {
    return not interface.isNull();
}
