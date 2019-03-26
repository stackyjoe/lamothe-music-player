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
#include "player_interface.hpp"
#include "user_desired_state.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(player_interface &_audio_interface, metadata_interface &_tag_interface, QWidget *parent = nullptr);
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
    player_interface &audio_interface;
    metadata_interface &tag_interface;
    QPersistentModelIndex current_song;

    mutable std::mutex seek_bar_lock;
    mutable std::mutex volume_lock;
    mutable std::mutex tag_lock;
    std::thread daemon_thread;

    //std::vector<std::string> song_paths;
    std::set<std::string> songs;
    UserDesiredState state;

private slots:
    void on_playButton_clicked();
    void on_pauseButton_clicked();
};

#endif // MAINWINDOW_HPP
