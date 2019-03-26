#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStringListModel>

#include "daemon_watcher.tpp"
#include "mainwindow.hpp"
#include "song_tile_model.hpp"

MainWindow::MainWindow(player_interface &_audio_interface, metadata_interface &_tag_interface, QWidget *parent) :
    QMainWindow(parent),
    ui(std::make_unique<Ui::MainWindow>()),
    audio_interface(_audio_interface),
    tag_interface(_tag_interface),
    daemon_thread(
        [&](){
            daemon_watcher(
                // daemon_watcher is a variadic template function that just calls its arguments,
                //  in the given order, in a while(1) loop.
                // It's not really necessary, but it's fun!

                [&]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(125));
                },
                [&]() {
                    // Syncs backend with user-expected behavior.
                    this->sync_audio_with_library_state();
                },
                [&]() {
                    // Updates seek bar based on backend
                    if(ui->seekSlider != nullptr) {
                        float time_pos = audio_interface.perform(
                                    [](music_player &player){ return player.getPercentPlayed(); }
                                );
                        set_seek_bar_position(time_pos);
                    }
                }
            );
        }
    ),
    state(UserDesiredState::pause)
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
    connect(ui->tableView, &QTableView::doubleClicked, this,
            // Qt::connect doesn't like overloaded signals/slots.
            static_cast<void(QMainWindow::*)(const QModelIndex &)>(&MainWindow::play_song)
            );
    connect(ui->tableView->model(), &QAbstractItemModel::rowsMoved, this, &MainWindow::sync_library_with_tableview);

    std::vector<std::string> song_paths;

    QString  conf_path = QDir::homePath() + "/.local/share/applications/lamothe/user_settings.conf";
    QFile conf_file(conf_path);

    if(conf_file.open(QFile::ReadOnly)) {
        QTextStream lib_path_file_stream(&conf_file);
        QString lib_path = lib_path_file_stream.readLine().trimmed();

        QFile lib_file(lib_path);

        if(lib_file.open(QFile::ReadOnly) ) {
            QTextStream lib(&lib_file);
            QString line;
            while(not lib.atEnd()) {
                line = lib.readLine().trimmed();
                song_paths.push_back(line.toStdString());
            }
        }
        else {
            qDebug() << "Error opening file: " << lib_path << "\n";
        }

        add_files_to_library(song_paths);
    }
    ui->tableView->setColumnWidth(0,450);
    ui->tableView->setColumnWidth(1,350);
    ui->tableView->setColumnWidth(2,300);
}

void MainWindow::fooBar() {
    // This is just a dummy function for not-yet-implemented functionalities.
    qDebug() << tr("Function not yet implemented.\n");
}


[[ noreturn ]] void MainWindow::exitProgram() {
    audio_interface.perform([](music_player &player){player.stop();});
    std::quick_exit(EXIT_SUCCESS);
}

void MainWindow::on_playButton_clicked()
{
    state = UserDesiredState::play;
}

void MainWindow::on_pauseButton_clicked()
{
    state =UserDesiredState::pause;
}


void MainWindow::changeVolume(int new_vol) {
    audio_interface.perform([&new_vol](music_player &player){player.setVolume(new_vol);});
}

void MainWindow::save_library() const {
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

    QModelIndex index;
    int max = ui->tableView->model()->rowCount();
    for(int i = 0; i < max; ++i ) {
        index = ui->tableView->model()->index(i,0);
        contents += ui->tableView->model()->data(index,Qt::UserRole).toString().toStdString() + "\n";
    }

    file.write(contents.data());
}

void MainWindow::seek() {
    float pos = ui->seekSlider->sliderPosition();
    pos = pos/std::max(1,ui->seekSlider->maximum());
    audio_interface.perform([pos](music_player &player){player.seekByPercent(pos);});
    seek_bar_lock.unlock();
}

void MainWindow::set_currently_playing_track_title(const std::string& title) {
    ui->currently_playing_label->setText(QString::fromStdString(title));
}

void MainWindow::set_seek_bar_position(float percent) {
    if(seek_bar_lock.try_lock()) {
        ui->seekSlider->setSliderPosition(static_cast<int>(percent * ui->seekSlider->maximum()));
        seek_bar_lock.unlock();
    }
}

void MainWindow::add_file_to_library(const std::string& file_path) {

    if(songs.find(file_path) == songs.end()) {
        songs.insert(file_path);

        int cur_song_count = ui->tableView->model()->rowCount();
        ui->tableView->model()->insertRow(cur_song_count);
        QModelIndex index = ui->tableView->model()->index(cur_song_count, 0);
        QAbstractItemModel * model = ui->tableView->model();
        model->setData(index, QString::fromStdString(file_path), Qt::UserRole);

        music_metadata metadata = tag_interface.get_metadata_of(file_path);

        model->setData(index, metadata.track_title, Qt::DisplayRole);

        index = ui->tableView->model()->index(cur_song_count, 1);
        model->setData(index, metadata.album_title, Qt::DisplayRole);

        index = ui->tableView->model()->index(cur_song_count, 2);
        model->setData(index, metadata.artist_title, Qt::DisplayRole);

        index = ui->tableView->model()->index(cur_song_count, 3);
        model->setData(index, metadata.recording_year, Qt::DisplayRole);

        index = ui->tableView->model()->index(cur_song_count, 4);
        model->setData(index, metadata.duration, Qt::DisplayRole);

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

void MainWindow::add_files_to_library(const std::vector<std::string> &song_paths) {
    auto * model = dynamic_cast<QStandardItemModel*>(ui->tableView->model());

    if(song_paths.size() < static_cast<size_t>(model->rowCount())) {
        int new_max = static_cast<int>(song_paths.size());
        int old_max = model->rowCount();
        model->removeRows(new_max, old_max-new_max);
    }
    else {
        int new_max = static_cast<int>(song_paths.size());
        int old_max = model->rowCount();
        model->insertRows(old_max, new_max - old_max);
    }

    size_t max = song_paths.size();

    for(size_t i = 0; i < max; ++i) {
        QModelIndex index;

        index = model->index(static_cast<int>(i),0);
        QString path = QString::fromStdString(song_paths[i]);
        ui->tableView->model()->setData(index, path, Qt::UserRole);

        music_metadata metadata = tag_interface.get_metadata_of(song_paths[i]);

        model->setData(index, metadata.track_title, Qt::DisplayRole);

        index = model->index(static_cast<int>(i),1);
        model->setData(index, metadata.album_title, Qt::DisplayRole);

        index = model->index(static_cast<int>(i),2);
        model->setData(index, metadata.artist_title, Qt::DisplayRole);

        index = model->index(static_cast<int>(i),3);
        model->setData(index, metadata.recording_year, Qt::DisplayRole);

        index = model->index(static_cast<int>(i),4);
        model->setData(index, metadata.duration, Qt::DisplayRole);
    }

    ui->tableView->update();
}

void MainWindow::sync_audio_with_library_state() {
    switch(state) {
    case UserDesiredState::play :
        switch(audio_interface.get_status()) {
        case PlayerStatus::playing:
            break;
        case PlayerStatus::paused:
            audio_interface.perform([](music_player &player){player.play();});
            break;
        default: {
            current_song = ui->tableView->model()->index(current_song.row()+1,0);

            play_song(current_song);
        } break;
        }
        break;
    case UserDesiredState::pause :
        switch(audio_interface.get_status()) {
        case PlayerStatus::playing:
            audio_interface.perform([](music_player &player){player.pause();});
            break;

        default:
            break;
        }
        break;

    case UserDesiredState::stop :
        switch (audio_interface.get_status()) {
        case PlayerStatus::playing:
            audio_interface.perform([](music_player &player){player.pause();});
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

void MainWindow::sync_library_with_tableview() {
    int song_count = ui->tableView->model()->rowCount();
    QModelIndex index;
    std::vector<std::string> song_paths(static_cast<size_t>(song_count));

    for(int i = 0; i < song_count; ++i) {
        index = ui->tableView->model()->index(i,0);
        song_paths.push_back(ui->tableView->model()->data(index, Qt::UserRole).toString().toStdString());
    }
}

void MainWindow::play_song(const QModelIndex &index) {
    if(not index.isValid()) {
        state = UserDesiredState::pause;
        return;
    }
    current_song = index;
    auto * model = index.model();
    QString filename = model->data(model->index(index.row(),0),Qt::UserRole).toString();
    QString track_name = model->data(model->index(index.row(),0), Qt::DisplayRole).toString();
    set_currently_playing_track_title(track_name.toStdString());
    play_song(filename.toStdString());
}

void MainWindow::play_song(const std::string &path) {
    audio_interface.perform(
        [&path](music_player &player){
            if(not player.openFromFile(path)) {
                qDebug() << "Error opening file: " << QString::fromStdString(path) << "\n";
            }

            player.play();
        }
    );

    state = UserDesiredState::play;
}

