#include "song_tile_widget.hpp"
#include "ui_song_tile_widget.h"

song_tile_widget::song_tile_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::song_tile_widget)
{
    ui->setupUi(this);
}

song_tile_widget::~song_tile_widget()
{
    delete ui;
}
