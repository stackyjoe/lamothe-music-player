#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QStringListModel>

#include "mainwindow.hpp"
#include "song_tile_model.hpp"

MainWindow::MainWindow(audio_interface &_audio_handle, metadata_interface &_metadata_handle, QWidget *parent) :
    QMainWindow(parent),
    ui(std::make_unique<Ui::MainWindow>()),
    audio_handle(_audio_handle),
    metadata_handle(_metadata_handle),
    daemon_thread(
        [&](){
            while(1)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(115));

                // Syncs backend with user-expected behavior.
                this->sync_audio_with_library_state();

                this->sync_ui_with_audio_state();
            };
        }
    ),
    state(UserDesiredState::pause)
{
    ui->setupUi(this);
    showMaximized();
    set_up_connections();

    ui->tableView->setModel(new song_tile_model(nullptr)); // NOLINT(cppcoreguidelines-owning-memory)
    ui->tableView->setColumnWidth(0,450);
    ui->tableView->setColumnWidth(1,350);
    ui->tableView->setColumnWidth(2,300);

    QString  conf_path = QDir::homePath() + "/.local/share/applications/lamothe/user_settings.conf";
    QFile conf_file(conf_path);

    if(conf_file.open(QFile::ReadOnly)) {
        std::vector<music_metadata> song_data;
        QTextStream lib_path_file_stream(&conf_file);
        QString lib_path = lib_path_file_stream.readLine().trimmed();

        open_library(lib_path);
    }
}

void MainWindow::fooBar() {
    // This is just a dummy function for not-yet-implemented functionalities.
    qDebug() << tr("Function not yet implemented.\n");
}

void MainWindow::set_up_connections() {
    connect(ui->actionAddSong, &QAction::triggered, this, &MainWindow::add_file_by_dialog);
    connect(ui->actionAddFolder, &QAction::triggered, this, &MainWindow::add_folder_by_dialog);
    connect(ui->actionRecusivelyAddFolder, &QAction::triggered, this, &MainWindow::fooBar);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::fooBar);
    connect(ui->actionOpenLibrary, &QAction::triggered, this, &MainWindow::open_library_via_dialog);
    connect(ui->actionSaveLibrary, &QAction::triggered, this, &MainWindow::save_library_via_dialog);
    connect(ui->actionExitProgram, &QAction::triggered, this, &MainWindow::exitProgram);
    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, this, &MainWindow::changeVolume);
    connect(ui->seekSlider, &QAbstractSlider::sliderReleased, this, [&](){this->seek();});
    connect(ui->seekSlider, &QAbstractSlider::sliderPressed, this, [&](){seek_bar_lock.lock();});
    connect(ui->tableView, &QTableView::doubleClicked, this,
            // Qt::connect doesn't like overloaded signals/slots.
            static_cast<void(QMainWindow::*)(const QModelIndex &)>(&MainWindow::play_song)
            );
    connect(ui->tableView->model(), &QAbstractItemModel::rowsMoved, this, &MainWindow::sync_library_with_tableview);
}

void MainWindow::on_prevButton_clicked() {
    if(not current_song.isValid())
        return;
    current_song = ui->tableView->model()->index(std::max(0,current_song.row()-1),0);
    play_song(current_song);
}

void MainWindow::on_playButton_clicked() {
    switch( state ) {
    case UserDesiredState::play:
        state = UserDesiredState::pause;
        break;
    default:
        state = UserDesiredState::play;
        break;
    }
}

void MainWindow::on_nextButton_clicked() {
    if(not current_song.isValid())
        return;
    play_song(ui->tableView->model()->index(std::min(current_song.row()+1, ui->tableView->model()->rowCount()), 0));
}

void MainWindow::changeVolume(int new_vol) {
    audio_handle.perform([&new_vol](audio_wrapper &player){player.set_volume(new_vol);});
}

void MainWindow::save_library_via_dialog() const {
    QString save_file_path = QFileDialog::getSaveFileName(nullptr,
                                                          tr("Save library to a file"),
                                                          QDir::homePath(),
                                                          tr("Library (*.csv)"));

    save_library(save_file_path);
}

void MainWindow::save_library(QString file_path) const {
    QFile file(file_path);
    if(not file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error writing to file: " << file_path << "\n";
        return;
    }

    std::string contents;

    QModelIndex index;
    int max = ui->tableView->model()->rowCount();
    for(int i = 0; i < max; ++i ) {
        auto * model = dynamic_cast<QStandardItemModel*>(ui->tableView->model());
        index = ui->tableView->model()->index(i,0);
        music_metadata song_datum(*model, index);
        song_datum.write_to(file);
    }
    file.close();
}

void MainWindow::open_library_via_dialog() {
    QString library_path = QFileDialog::getOpenFileName(nullptr,
                                                        tr("Open library from a file"),
                                                        QDir::homePath(),
                                                        tr("Library (*.csv)"));
    open_library(library_path);
}

void MainWindow::open_library(QString library_path) {
    QFile lib_file(library_path);
    std::vector<music_metadata> song_data;

    if(lib_file.open(QFile::ReadOnly) ) {
        while(not lib_file.atEnd()) {
            song_data.push_back(music_metadata(lib_file));
        }
    }
    else {
        qDebug() << "Error opening file: " << library_path << "\n";
    }

    add_to_library(song_data);
}

void MainWindow::seek() {
    float pos = ui->seekSlider->sliderPosition();
    pos = pos/std::max(1,ui->seekSlider->maximum());
    audio_handle.perform([pos](audio_wrapper &player){player.seek_by_percent(pos);});
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
        auto * model = dynamic_cast<QStandardItemModel*>(ui->tableView->model());

        if(model == nullptr) {
            qDebug() << tr("ui->tableView->model() not castable to a QStandardItemModel object.\n");
            return;
        }

        music_metadata metadata = metadata_handle.get_metadata_of(file_path);

        metadata.add_to(*model, index);

        ui->tableView->update();
    }
}

void MainWindow::add_file_by_dialog() {

    QString supported_formats = "Audio files (";
    for(auto &format : audio_handle.supported_file_formats())
        supported_formats += QString::fromStdString(format) + tr(" ");
    supported_formats += tr(")");
    std::string file_path = QFileDialog::getOpenFileName(this, tr("Open audio file"), QDir::homePath(),
                                                supported_formats).toStdString();
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
    QStringList supported_formats;
    for(auto & formats : audio_handle.supported_file_formats())
        supported_formats << formats.data();
    QStringList file_list = folder_path.entryList(supported_formats, QDir::Files);

    foreach(QString file, file_list) { // NOLINT(hicpp-signed-bitwise)
        add_file_to_library((folder_path.absolutePath() + QDir::separator() + file).toStdString());
    }
}

void MainWindow::add_to_library(const std::vector<music_metadata> &song_data) {
    auto * model = dynamic_cast<QStandardItemModel*>(ui->tableView->model());

    if(model == nullptr) {
        qDebug() << tr("ui->tableView->model() not castable to a QStandardItemModel object.\n");
        return;
    }

    if(song_data.size() < static_cast<size_t>(model->rowCount())) {
        int new_max = static_cast<int>(song_data.size());
        int old_max = model->rowCount();
        model->removeRows(new_max, old_max-new_max);
    }
    else {
        int new_max = static_cast<int>(song_data.size());
        int old_max = model->rowCount();
        model->insertRows(old_max, new_max - old_max);
    }

    size_t max = song_data.size();

    for(size_t i = 0; i < max; ++i) {
        QModelIndex index;

        index = model->index(static_cast<int>(i),0);

        song_data[i].add_to(*model, index);
        //metadata.add_to(*model, index);
    }

    ui->tableView->update();
}

void MainWindow::sync_audio_with_library_state() {
    switch(state) {
    case UserDesiredState::play :
        switch(audio_handle.get_status()) {
        case PlayerStatus::playing:
            break;
        case PlayerStatus::paused:
            audio_handle.perform([](audio_wrapper &player){player.play();});
            break;
        default: {
            current_song = ui->tableView->model()->index(current_song.row()+1,0);

            play_song(current_song);
        } break;
        }
        break;
    case UserDesiredState::pause :
        switch(audio_handle.get_status()) {
        case PlayerStatus::playing:
            audio_handle.perform([](audio_wrapper &player){player.pause();});
            break;

        default:
            break;
        }
        break;

    case UserDesiredState::stop :
        switch (audio_handle.get_status()) {
        case PlayerStatus::playing:
            audio_handle.perform([](audio_wrapper &player){player.pause();});
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

void MainWindow::sync_ui_with_audio_state() {

    // This function can get called before things are fully initialized.
    if(ui == nullptr or ui->seekSlider == nullptr or ui->playButton == nullptr)
        return;

    // Synchronizes UI seek bar with audio backend.
    if(ui->seekSlider != nullptr) {
        float time_pos = audio_handle.perform(
                [](audio_wrapper &player){ return player.get_percent_played(); }
            );
       set_seek_bar_position(time_pos);
    }

    // Synchronizes play/pause push button
    switch( state ) {
    case UserDesiredState::play:
        ui->playButton->setIcon(QIcon(":/icons/icons/pause.svg"));
        ui->playButton->update();
        break;
    default:
        ui->playButton->setIcon(QIcon(":/icons/icons/play.svg"));
        ui->playButton->update();
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
    audio_handle.perform(
        [&path](audio_wrapper &player){
            if(not player.open_from_file(path)) {
                qDebug() << "Error opening file: " << QString::fromStdString(path) << "\n";
            }

            player.play();
        }
    );

    state = UserDesiredState::play;
}

[[ noreturn ]] void MainWindow::exitProgram() {
    audio_handle.perform([](audio_wrapper &player){player.stop();});
    std::quick_exit(EXIT_SUCCESS);
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    switch(state) {
    default:
        exitProgram();
    case UserDesiredState::play :
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("LaMothe"),
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
            event->ignore();
        else
            exitProgram();
        break;
    }
}
