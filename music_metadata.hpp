#ifndef MUSIC_METADATA_HPP
#define MUSIC_METADATA_HPP

#include <QString>
#include <string>

class music_metadata {
public:
    music_metadata(std::string track, std::string album, std::string artist, std::string year, std::string dur)
        : track_title(QString::fromStdString(track)),
        album_title(QString::fromStdString(album)),
        artist_title(QString::fromStdString(artist)),
        recording_year(QString::fromStdString(year)),
        duration(QString::fromStdString(dur))
    { }
    music_metadata() = default;
    music_metadata(const music_metadata &) = default;
    music_metadata &operator=(const music_metadata &) = default;
    music_metadata(music_metadata &&) = default;
    music_metadata &operator=(music_metadata &&) = default;

    QString track_title;
    QString album_title;
    QString artist_title;
    QString recording_year;
    QString duration;
};

#endif // MUSIC_METADATA_HPP
