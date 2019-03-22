#include "mainwindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>

#include "sfml_player.hpp"
#include "taglib_handler.hpp"

int main(int argc, char *argv[])
{
    QString local_share_path = QDir::homePath() +
            QString("/.local/share/applications/lamothe/");
    QDir dir(QDir::home());
    if(!dir.mkpath(local_share_path)) {
        qDebug() << QObject::tr("Error creating") << local_share_path << "\n";
        std::quick_exit(EXIT_FAILURE);
    }

    sfml_player player;
    taglib_handler tag_interface;
    QApplication a(argc, argv);
    MainWindow w(player, tag_interface, nullptr);
    w.show();

    return QApplication::exec();
}
