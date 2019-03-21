#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <mutex>
#include <SFML/Audio.hpp>

#include "music_library.hpp"
#include "music_daemon.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(music_player &_player, QWidget *parent = nullptr);
    MainWindow(const MainWindow &other) = delete;
    MainWindow &operator=(const MainWindow &other) = delete;

    MainWindow(MainWindow &&other) = default;
    MainWindow &operator=(MainWindow &&other) = default;
    ~MainWindow() override = default;

    void playSong(const QString &path);
    void setSeekBarPosition(float percent);

protected:
    void fooBar();
    void add_file_to_library(const std::string& file_path);
    void add_file_by_dialog();
    void add_folder_by_dialog();
    void save_library();
    void sync_tiles_with_library();
    void changeVolume(int new_vol);
    void seek();

    // Connected with the seekSlider so that the slider isn't moved while user interacts.
    void lock_seek_bar();
    void unlock_seek_bar();

    [[ noreturn ]] void exitProgram();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    music_library lib;
    std::mutex seek_bar_lock;
    std::mutex volume_lock;
    music_daemon music_daemon;

private slots:
    void on_playButton_clicked();
    void on_pauseButton_clicked();
};

#endif // MAINWINDOW_HPP
