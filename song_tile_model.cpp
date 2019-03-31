#include "song_tile_model.hpp"


song_tile_model::song_tile_model(QObject *parent) : QStandardItemModel(parent) {
    QStringList header_labels;
    header_labels.push_back(QString("Track"));
    header_labels.push_back(QString("Album"));
    header_labels.push_back(QString("Artist"));
    header_labels.push_back(QString("Year"));
    header_labels.push_back(QString("Duration"));
    setHorizontalHeaderLabels(header_labels);
}
Qt::ItemFlags song_tile_model::flags(const QModelIndex &index) const {
    if(index.isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
    return Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

bool song_tile_model::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, [[ maybe_unused ]] int column, const QModelIndex &parent) {
    // Should be able to add the ability to drag and drop files into the player by modifying this function. TBD

    // If you don't ignore the column variable, when the user drag-n-drops a row it'll insert the row offset by the columns
    return QStandardItemModel::dropMimeData(data, action, row, 0, parent);
}

