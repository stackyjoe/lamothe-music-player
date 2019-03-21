#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStringListModel>

#include "mainwindow.hpp"
#include "song_tile_widget.hpp"

MainWindow::MainWindow(music_player &_player, QWidget *parent) :
    QMainWindow(parent),
    ui(std::make_unique<Ui::MainWindow>()),
    lib(QDir::homePath().toStdString() + "/.local/share/applications/lamothe/user_settings.conf", _player),
    music_daemon(
        daemon_data(ui.get(),
                    lib,
                    volume_lock,
                    seek_bar_lock,
                    [this](float percent){this->setSeekBarPosition(percent);})
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
    connect(ui->seekSlider, &QAbstractSlider::sliderReleased, this, &MainWindow::seek);
    connect(ui->seekSlider, &QAbstractSlider::sliderPressed, this, &MainWindow::lock_seek_bar);
}

void MainWindow::fooBar() {
    // This is just a dummy function for not-yet-implemented functionalities.
    qDebug() << tr("Function not yet implemented.\n");
}

void MainWindow::lock_seek_bar() {
    seek_bar_lock.lock();
}

void MainWindow::unlock_seek_bar() {
    seek_bar_lock.unlock();
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
        auto * p = new song_tile_widget(songs[i],lib); // NOLINT(cppcoreguidelines-owning-memory)
        ui->listView->setIndexWidget(index, p);
    }

    }


void MainWindow::add_file_to_library(const std::string& file_path) {

    if(! lib.contains(file_path)) {
        lib.add(file_path);

        int cur_song_count = ui->listView->model()->rowCount();
        ui->listView->model()->insertRow(cur_song_count);
        QModelIndex index = ui->listView->model()->index(cur_song_count, 0);
        auto * p = new song_tile_widget(file_path, lib); // NOLINT(cppcoreguidelines-owning-memory)
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
