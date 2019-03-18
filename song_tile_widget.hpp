#ifndef SONG_TILE_WIDGET_HPP
#define SONG_TILE_WIDGET_HPP

#include <QWidget>

namespace Ui {
class song_tile_widget;
}

class song_tile_widget : public QWidget
{
    Q_OBJECT

public:
    explicit song_tile_widget(QWidget *parent = nullptr);
    ~song_tile_widget();

private:
    Ui::song_tile_widget *ui;
};

#endif // SONG_TILE_WIDGET_HPP
