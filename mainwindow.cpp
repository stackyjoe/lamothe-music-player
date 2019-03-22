#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStringListModel>

#include "daemon_watcher.tpp"
#include "mainwindow.hpp"
#include "song_tile_widget.hpp"

MainWindow::MainWindow(music_player &_audio_interface, tag_handler &_tag_interface, QWidget *parent) :
    QMainWindow(parent),
    ui(std::make_unique<Ui::MainWindow>()),
    lib(QDir::homePath().toStdString() + "/.local/share/applications/lamothe/user_settings.conf", _audio_interface
        ),
    audio_interface(_audio_interface),
    tag_interface(_tag_interface),
    daemon_thread(
        [&](){
            daemon_watcher(
                // daemon_watcher is a template function that just calls its arguments,
                //  in the given order, in a while(1) loop.
                [&]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(125));
                },
                [&]() {
                    // Syncs backend with user-expected behavior.
                    this->sync_ui_with_library();
                },
                [&]() {
                    // Updates seek bar based on backend
                    if(ui->seekSlider != nullptr) {
                        setSeekBarPosition(lib.getPercentPlayed());
                    }
                }
            );
        }
    )
{
    ui->setupUi(this);
    ui->listView->setModel(new QStringListModel(nullptr)); // NOLINT(cppcoreguidelines-owning-memory)
    sync_tiles_with_library();

    connect(ui->actionAddSong, &QAction::triggered, this, &MainWindow::add_file_by_dialog);
    connect(ui->actionAddFolder, &QAction::triggered, this, &MainWindow::add_folder_by_dialog);
    connect(ui->actionRecusivelyAddFolder, &QAction::triggered, this, &MainWindow::fooBar);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::fooBar);
    connect(ui->actionOpenLibrary, &QAction::triggered, this, &MainWindow::fooBar);
    connect(ui->actionSaveLibrary, &QAction::triggered, this, &MainWindow::save_library);
    connect(ui->actionExitProgram, &QAction::triggered, this, &MainWindow::exitProgram);
    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, this, &MainWindow::changeVolume);
    connect(ui->seekSlider, &QAbstractSlider::sliderReleased, this, [&](){this->seek();});
    connect(ui->seekSlider, &QAbstractSlider::sliderPressed, this, [&](){seek_bar_lock.lock();});
}

void MainWindow::fooBar() {
    // This is just a dummy function for not-yet-implemented functionalities.
    qDebug() << tr("Function not yet implemented.\n");
}

void MainWindow::changeVolume(int new_vol) {
    lib.setVolume(new_vol);
}

void MainWindow::save_library() {
    const std::vector<std::string> &files = lib.songs();
    QString save_file_path = QFileDialog::getSaveFileName(this, tr("Save library to file"), QDir::homePath(), tr("Library (*.csv)"));

    QFile file(save_file_path);
    if(not file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error writing to file: " << save_file_path << "\n";
        return;
    }

    std::string contents;
    for(auto & file : files) {
        contents += file + "\n";
    }

    file.write(contents.data());
}

void MainWindow::seek() {
    float pos = ui->seekSlider->sliderPosition();
    pos = pos/std::max(1,ui->seekSlider->maximum());
    lib.seekByPercent(pos);
    seek_bar_lock.unlock();
}

void MainWindow::setCurrentlyPlayingTrackTitle(const std::string& title) {
    ui->currently_playing_label->setText(QString::fromStdString(title));
}

void MainWindow::setSeekBarPosition(float percent) {
    if(seek_bar_lock.try_lock()) {
        ui->seekSlider->setSliderPosition(static_cast<int>(percent * ui->seekSlider->maximum()));
        seek_bar_lock.unlock();
    }
}

void MainWindow::add_folder_by_dialog() {
    QDir folder_path =
            QFileDialog::getExistingDirectory(this,
                                              tr("Choose directory to add"),
                                              QDir::homePath()
                                             );
    QStringList file_list = folder_path.entryList(QStringList() << "*.wav" << "*.ogg" << "*.flac", QDir::Files);

    foreach(QString file, file_list) { // NOLINT(hicpp-signed-bitwise)
        add_file_to_library((folder_path.absolutePath() + QDir::separator() + file).toStdString());
    }
}

void MainWindow::sync_tiles_with_library() {
    const std::vector<std::string> &songs = lib.songs();
    QAbstractItemModel * model = ui->listView->model();

    if(songs.size() < static_cast<size_t>(model->rowCount())) {
        int new_max = static_cast<int>(songs.size());
        int old_max = model->rowCount();
        model->removeRows(new_max, old_max-new_max);
    }
    else {
        int new_max = static_cast<int>(songs.size());
        int old_max = model->rowCount();
        model->insertRows(old_max, new_max - old_max);
    }

    size_t max = songs.size();

    for(size_t i = 0; i < max; ++i) {
        QModelIndex index = model->index(static_cast<int>(i),0);
        auto * p = new song_tile_widget(songs[i],lib, tag_interface, *this); // NOLINT(cppcoreguidelines-owning-memory)
        ui->listView->setIndexWidget(index, p);
    }

}

void MainWindow::sync_ui_with_library() {
    switch(lib.getState()) {
    case UserDesiredState::play :
        switch(audio_interface.getStatus()) {
        case PlayerStatus::playing:
            break;
        case PlayerStatus::paused:
            qDebug() << tr("User wants to play, but backend is paused.\n");
            audio_interface.play();
            break;
        default: {
            std::string next = lib.next_song();
            tag_interface.openFromFile(next);
            if(not tag_interface.ownsAFile()) {
                break;
            }
            ui->currently_playing_label->setText(QString::fromStdString(tag_interface.track_title()));
            playSong(next);
        } break;
        }
        break;
    case UserDesiredState::pause :
        switch(audio_interface.getStatus()) {
        case PlayerStatus::playing:
            audio_interface.pause();
            break;

        default:
            break;
        }
        break;

    case UserDesiredState::stop :
        switch (audio_interface.getStatus()) {
        case PlayerStatus::playing:
            audio_interface.pause();
            break;

        default:
            break;
        }
        break;

    default:
        qDebug() << "music_libary.state has been corrupted.";
        break;
    }

}

void MainWindow::playSong(const std::string &path) {
    std::scoped_lock lock(audio_lock);

    if(not audio_interface.openFromFile(path)) {
        qDebug() << "Error opening file: " << QString::fromStdString(path) << "\n";
    }

    audio_interface.play();
    lib.find_song(path);
    lib.setState(UserDesiredState::play);
}

void MainWindow::add_file_to_library(const std::string& file_path) {

    if(! lib.contains(file_path)) {
        lib.add(file_path);

        int cur_song_count = ui->listView->model()->rowCount();
        ui->listView->model()->insertRow(cur_song_count);
        QModelIndex index = ui->listView->model()->index(cur_song_count, 0);
        auto * p = new song_tile_widget(file_path, lib, tag_interface, *this); // NOLINT(cppcoreguidelines-owning-memory)
        ui->listView->setIndexWidget(index, p);
        ui->listView->update();
    }
}

void MainWindow::add_file_by_dialog() {

    std::string file_path = QFileDialog::getOpenFileName(this, tr("Open audio file"), QDir::homePath(),
                                                tr("Ogg Vorbis files (*.ogg)")).toStdString();
    // Now Open it
    if (file_path.empty()) {
      return;
    }

    add_file_to_library(file_path);
}

[[ noreturn ]] void MainWindow::exitProgram() {
    lib.stop();
    std::quick_exit(EXIT_SUCCESS);
}

void MainWindow::on_playButton_clicked()
{
    lib.setState(UserDesiredState::play);
}

void MainWindow::on_pauseButton_clicked()
{
    lib.setState(UserDesiredState::pause);
}
