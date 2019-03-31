#include "music_metadata.hpp"

music_metadata::music_metadata(std::string _file_path,
                               std::string track,
                               std::string album,
                               std::string artist,
                               std::string year,
                               std::string dur)
    : file_path(QString::fromStdString(_file_path)),
    track_title(QString::fromStdString(track)),
    album_title(QString::fromStdString(album)),
    artist_title(QString::fromStdString(artist)),
    recording_year(QString::fromStdString(year)),
    duration(QString::fromStdString(dur))
{ }

music_metadata::music_metadata(QFile &lib_file) {
    if(not lib_file.isOpen() or not lib_file.isReadable())
        return;
    file_path = lib_file.readLine().trimmed();
    track_title = lib_file.readLine().trimmed();
    album_title = lib_file.readLine().trimmed();
    artist_title = lib_file.readLine().trimmed();
    recording_year = lib_file.readLine().trimmed();
    duration = lib_file.readLine().trimmed();
}

music_metadata::music_metadata(QStandardItemModel &model, QModelIndex index) :
    file_path(model.data(model.index(index.row(),0), Qt::UserRole).toString()),
    track_title(model.data(model.index(index.row(),0), Qt::DisplayRole).toString()),
    album_title(model.data(model.index(index.row(),1), Qt::DisplayRole).toString()),
    artist_title(model.data(model.index(index.row(),2), Qt::DisplayRole).toString()),
    recording_year(model.data(model.index(index.row(),3), Qt::DisplayRole).toString()),
    duration(model.data(model.index(index.row(),4), Qt::DisplayRole).toString())
{ }

bool music_metadata::write_to(QFile &lib_file) const {
    if(not lib_file.isOpen() or not lib_file.isWritable())
        return false;
    QString serialized_contents =
            file_path + "\n" +
            track_title + "\n" +
            album_title + "\n" +
            artist_title + "\n" +
            recording_year + "\n" +
            duration + "\n";
    lib_file.write(serialized_contents.toUtf8());
    return true;
}

void music_metadata::add_to(QStandardItemModel &model, QModelIndex index) const {
    int row = index.row();
    model.setData(index, file_path, Qt::UserRole);
    model.setData(index, track_title, Qt::DisplayRole);

    index = model.index(row, 1);
    model.setData(index, album_title, Qt::DisplayRole);

    index = model.index(row, 2);
    model.setData(index, artist_title, Qt::DisplayRole);

    index = model.index(row, 3);
    model.setData(index, recording_year, Qt::DisplayRole);

    index = model.index(row, 4);
    model.setData(index, duration, Qt::DisplayRole);
}
