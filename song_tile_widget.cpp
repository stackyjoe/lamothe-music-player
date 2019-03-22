#include "song_tile_widget.hpp"

#include <QDebug>
#include <QKeyEvent>

song_tile_widget::song_tile_widget(const std::string &_path, music_library &_lib, tag_handler &_tag_interface, MainWindow &_mw) :
    QWidget(nullptr),
    song_path(_path),
    lib(_lib),
    tag_interface(_tag_interface),
    mw(_mw),
    ui(std::make_unique<Ui::song_tile_widget>())
{
    ui->setupUi(this);
    tag_interface.openFromFile(song_path);
    ui->track_label->setText(QString::fromStdString(tag_interface.track_title()));
    ui->album_label->setText(QString::fromStdString(tag_interface.album_title()));
    ui->artist_label->setText(QString::fromStdString(tag_interface.artist_title()));
    ui->year_label->setText(QString::fromStdString(tag_interface.recording_year()));
    ui->duration_label->setText(QString::fromStdString(tag_interface.duration()));
}

void song_tile_widget::mouseDoubleClickEvent( [[ maybe_unused ]] QMouseEvent *ev) {
    mw.playSong(song_path);
    mw.setCurrentlyPlayingTrackTitle(ui->track_label->text().toStdString());
    return;
}
