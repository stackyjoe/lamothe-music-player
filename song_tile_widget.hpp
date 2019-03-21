#ifndef SONG_TILE_WIDGET_HPP
#define SONG_TILE_WIDGET_HPP

#include <QWidget>

#include <taglib/fileref.h>

#include "ui_song_tile_widget.h"

#include "music_library.hpp"

namespace Ui {
class song_tile_widget;
}

class song_tile_widget : public QWidget
{
    Q_OBJECT

public:
    explicit song_tile_widget(const std::string &_path, music_library &_lib, QWidget *parent = nullptr);
    ~song_tile_widget() override = default;

protected:
    std::string song_path;
    music_library &lib;
    TagLib::FileRef tags_handle;

    void mouseDoubleClickEvent(QMouseEvent *ev) override;

private:
    std::unique_ptr<Ui::song_tile_widget> ui;
};

#endif // SONG_TILE_WIDGET_HPP
