#ifndef SONG_TILE_MODEL_HPP
#define SONG_TILE_MODEL_HPP

#include <QStandardItemModel>

class song_tile_model : public QStandardItemModel
{
public:
    explicit song_tile_model(QObject *parent);
    Qt::ItemFlags flags(const QModelIndex&index) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
};

#endif // SONG_TILE_MODEL_HPP
