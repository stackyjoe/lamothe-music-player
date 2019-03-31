#ifndef MUSIC_METADATA_HPP
#define MUSIC_METADATA_HPP

#include <QStandardItemModel>
#include <QFile>
#include <QString>
#include <string>

class music_metadata {
public:
    music_metadata() = default;
    music_metadata(std::string _file_path,
                   std::string track,
                   std::string album,
                   std::string artist,
                   std::string year,
                   std::string dur);
    music_metadata(QFile &lib_file);
    music_metadata(QStandardItemModel &model, QModelIndex index);

    bool write_to(QFile &lib_file) const;
    void add_to(QStandardItemModel &model, QModelIndex index) const;
protected:
    QString file_path;
    QString track_title;
    QString album_title;
    QString artist_title;
    QString recording_year;
    QString duration;
};

#endif // MUSIC_METADATA_HPP
