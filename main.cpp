#include "mainwindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>

#include "audio_interface.hpp"


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
    extern metadata_interface metadata_handle;
    extern audio_interface audio_handle;

    QApplication a(argc, argv);
    MainWindow w(audio_handle, metadata_handle, nullptr);
    w.show();

    return QApplication::exec();
}
