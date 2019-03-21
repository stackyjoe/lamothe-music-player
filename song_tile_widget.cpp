#include "song_tile_widget.hpp"

#include <QDebug>
#include <QKeyEvent>

song_tile_widget::song_tile_widget(const std::string &_path, music_library &_lib, QWidget *parent) :
    QWidget(parent),
    song_path(_path),
    lib(_lib),
    tags_handle(song_path.data()),
    ui(std::make_unique<Ui::song_tile_widget>())
{
    ui->setupUi(this);
    ui->track_label->setText(QString::fromUtf8(tags_handle.tag()->title().toCString(true)));
    ui->album_label->setText(QString::fromUtf8(tags_handle.tag()->album().toCString(true)));
    ui->artist_label->setText(QString::fromUtf8(tags_handle.tag()->artist().toCString(true)));
    ui->year_label->setText(QString::number(tags_handle.tag()->year()));

    int duration = tags_handle.file()->audioProperties()->lengthInSeconds();
    int hours = duration / (60*60);
    int minutes = duration / 60;
    int seconds = duration % 60;

    QString time;
    if(hours) {
        time += QString::number(hours) + ":";
    }
    time += QString("%1").arg(minutes, 2, 10, QChar('0')) + ":"
            + QString("%1").arg(seconds,2,10, QChar('0'));

    ui->duration_label->setText(time);
}

void song_tile_widget::mouseDoubleClickEvent( [[ maybe_unused ]]QMouseEvent *ev) {
    lib.playSong(song_path);
    return;
}
