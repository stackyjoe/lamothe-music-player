#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <mutex>
#include <SFML/Audio.hpp>
#include <thread>

#include "music_library.hpp"
#include "music_player.hpp"
#include "tag_handler.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(music_player &_audio_interface, tag_handler &_tag_interface, QWidget *parent = nullptr);
    MainWindow(const MainWindow &other) = delete;
    MainWindow &operator=(const MainWindow &other) = delete;

    MainWindow(MainWindow &&other) = default;
    MainWindow &operator=(MainWindow &&other) = default;
    ~MainWindow() override = default;

    void playSong(const std::string &path);
    void setSeekBarPosition(float percent);
    void setCurrentlyPlayingTrackTitle(const std::string& title);

protected:
    void fooBar();
    void add_file_to_library(const std::string& file_path);
    void add_file_by_dialog();
    void add_folder_by_dialog();
    void save_library();
    void sync_tiles_with_library();
    void sync_ui_with_library();
    void changeVolume(int new_vol);
    void seek();

    [[ noreturn ]] void exitProgram();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    music_library lib;
    music_player &audio_interface;
    tag_handler &tag_interface;

    std::mutex audio_lock;
    std::mutex seek_bar_lock;
    std::mutex volume_lock;
    std::thread daemon_thread;

private slots:
    void on_playButton_clicked();
    void on_pauseButton_clicked();
};

#endif // MAINWINDOW_HPP
