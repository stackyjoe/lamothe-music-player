#include "mainwindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>

#include "player_interface.hpp"
#include "sfml_player.hpp"
#include "taglib_interface.hpp"

int main(int argc, char *argv[])
{
    QString local_share_path = QDir::homePath() +
            QString("/.local/share/applications/lamothe/");
    QDir dir(QDir::home());
    if(!dir.mkpath(local_share_path)) {
        qDebug() << QObject::tr("Error creating") << local_share_path << "\n";
        std::quick_exit(EXIT_FAILURE);
    }

    // Change these two to change audio backend.
    sfml_player player;
    taglib_interface tag_interface;

    player_interface audio_interface(player);

    QApplication a(argc, argv);
    MainWindow w(audio_interface, tag_interface, nullptr);
    w.show();

    return QApplication::exec();
}
