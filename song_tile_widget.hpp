#ifndef SONG_TILE_WIDGET_HPP
#define SONG_TILE_WIDGET_HPP

#include <QWidget>

#include <taglib/fileref.h>

#include "ui_song_tile_widget.h"

#include "mainwindow.hpp"
#include "music_library.hpp"
#include "tag_handler.hpp"

namespace Ui {
class song_tile_widget;
}

class song_tile_widget : public QWidget
{
    Q_OBJECT

public:
    explicit song_tile_widget(const std::string &_path, music_library &_lib, tag_handler &_tag_interface, MainWindow &_mw);
    ~song_tile_widget() override = default;

protected:
    std::string song_path;
    music_library &lib;
    tag_handler &tag_interface;
    MainWindow &mw;

    void mouseDoubleClickEvent(QMouseEvent *ev) override;

private:
    std::unique_ptr<Ui::song_tile_widget> ui;
};

#endif // SONG_TILE_WIDGET_HPP
