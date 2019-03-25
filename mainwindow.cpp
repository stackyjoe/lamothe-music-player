#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStringListModel>

#include "daemon_watcher.tpp"
#include "mainwindow.hpp"
#include "song_tile_model.hpp"

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
    showMaximized();

    ui->tableView->setModel(new song_tile_model(nullptr)); // NOLINT(cppcoreguidelines-owning-memory)

    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setDefaultDropAction(Qt::MoveAction);
    ui->tableView->setDragDropMode(QAbstractItemView::InternalMove);
    ui->tableView->setAcceptDrops(true);
    ui->tableView->setDragDropOverwriteMode(false);
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
    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::play_song_from_index);
    show();
    ui->tableView->show();
    ui->tableView->setColumnWidth(0, static_cast<int>(width()));
    ui->tableView->setColumnWidth(1,static_cast<int>(0.25*width()));
    ui->tableView->setColumnWidth(2,static_cast<int>(0.25*width()));
}

void MainWindow::fooBar() {
    // This is just a dummy function for not-yet-implemented functionalities.
    qDebug() << tr("Function not yet implemented.\n");
}

void MainWindow::changeVolume(int new_vol) {
    lib.setVolume(new_vol);
}

void MainWindow::save_library() const {
    const std::vector<std::string> &files = lib.songs();
    QString save_file_path = QFileDialog::getSaveFileName(nullptr,
                                                          tr("Save library to file"),
                                                          QDir::homePath(),
                                                          tr("Library (*.csv)"));

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
            QFileDialog::getExistingDirectory(static_cast<QMainWindow*>(this),
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
    QAbstractItemModel * model = ui->tableView->model();

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
        QModelIndex index;
        index = model->index(static_cast<int>(i),0);
        ui->tableView->model()->setData(index, QString::fromStdString((songs[i])), Qt::UserRole);

        std::scoped_lock tag_interface_lock(tag_lock);

        tag_interface.openFromFile(songs[i]);
        if(not tag_interface.ownsAFile())
            continue;
        ui->tableView->model()->setData(index, QString::fromStdString(tag_interface.track_title()), Qt::DisplayRole);

        index = model->index(static_cast<int>(i),1);
        ui->tableView->model()->setData(index, QString::fromStdString(tag_interface.album_title()), Qt::DisplayRole);

        index = model->index(static_cast<int>(i),2);
        ui->tableView->model()->setData(index, QString::fromStdString(tag_interface.artist_title()), Qt::DisplayRole);

        index = model->index(static_cast<int>(i),3);
        ui->tableView->model()->setData(index, QString::fromStdString(tag_interface.recording_year()), Qt::DisplayRole);

        index = model->index(static_cast<int>(i),4);
        ui->tableView->model()->setData(index, QString::fromStdString(tag_interface.duration()), Qt::DisplayRole);



    }

}

void MainWindow::sync_ui_with_library() {
    switch(lib.getState()) {
    case UserDesiredState::play :
        switch(audio_interface.getStatus()) {
        case PlayerStatus::playing:
            break;
        case PlayerStatus::paused:
            audio_interface.play();
            break;
        default: {
            std::string next = lib.next_song();
            tag_interface.openFromFile(next);
            if(not tag_interface.ownsAFile()) {
                break;
            }
            //ui->currently_playing_label->setText(QString::fromStdString(tag_interface.track_title()));
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

void MainWindow::play_song_from_index(const QModelIndex &index) {
    auto * model = index.model();
    QString filename = model->data(model->index(index.row(),0),Qt::UserRole).toString();
    QString track_name = model->data(model->index(index.row(),0), Qt::DisplayRole).toString();
    setCurrentlyPlayingTrackTitle(track_name.toStdString());
    playSong(filename.toStdString());
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

        int cur_song_count = ui->tableView->model()->rowCount();
        ui->tableView->model()->insertRow(cur_song_count);
        QModelIndex index = ui->tableView->model()->index(cur_song_count, 0);
        QAbstractItemModel * model = ui->tableView->model();
        model->setData(index, QString::fromStdString(file_path), Qt::UserRole);
        std::scoped_lock lock(tag_lock);
        tag_interface.openFromFile(file_path);

        model->setData(index, QString::fromStdString(tag_interface.track_title()), Qt::DisplayRole);

        index = ui->tableView->model()->index(cur_song_count, 1);
        model->setData(index, QString::fromStdString(tag_interface.album_title()), Qt::DisplayRole);

        index = ui->tableView->model()->index(cur_song_count, 2);
        model->setData(index, QString::fromStdString(tag_interface.artist_title()), Qt::DisplayRole);

        index = ui->tableView->model()->index(cur_song_count, 3);
        model->setData(index, QString::fromStdString(tag_interface.recording_year()), Qt::DisplayRole);

        index = ui->tableView->model()->index(cur_song_count, 4);
        model->setData(index, QString::fromStdString(tag_interface.duration()), Qt::DisplayRole);

        ui->tableView->update();
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
