#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "ui_mainwindow.h"

#include <QMainWindow>

#include <mutex>
#include <set>
#include <thread>

#include <SFML/Audio.hpp>


#include "metadata_interface.hpp"
#include "music_metadata.hpp"
#include "audio_interface.hpp"
#include "user_desired_state.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(audio_interface &_audio_handle, metadata_interface &_metadata_handle, QWidget *parent = nullptr);
    MainWindow(const MainWindow &other) = delete;
    MainWindow &operator=(const MainWindow &other) = delete;

    ~MainWindow() override = default;

protected:
    void fooBar();

    void add_file_to_library(const std::string& file_path);
    void add_files_to_library(const std::vector<std::string> &song_paths);

    void add_file_by_dialog();
    void add_folder_by_dialog();

    void changeVolume(int new_vol);
    void play_song(const std::string &path);
    void play_song(const QModelIndex &index);
    void seek();
    void set_seek_bar_position(float percent);
    void set_currently_playing_track_title(const std::string& title);

    void save_library() const;
    void sync_audio_with_library_state();
    void sync_library_with_tableview();

    [[ noreturn ]] void exitProgram();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    audio_interface &audio_handle;
    metadata_interface &metadata_handle;

    mutable std::mutex seek_bar_lock;
    mutable std::mutex volume_lock;
    mutable std::mutex tag_lock;
    std::thread daemon_thread;

    std::set<std::string> songs;
    QPersistentModelIndex current_song;
    UserDesiredState state;


private slots:
    void on_playButton_clicked();
    void on_pauseButton_clicked();
};

#endif // MAINWINDOW_HPP
