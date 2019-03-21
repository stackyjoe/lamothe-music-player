#ifndef DAEMON_DATA_HPP
#define DAEMON_DATA_HPP

#include <functional>
#include <mutex>

#include "ui_mainwindow.h"
#include "music_library.hpp"

class daemon_data
{
public:
    daemon_data(Ui::MainWindow *_ui,
                music_library &lib,
                std::mutex &_volume_lock,
                std::mutex &_seek_bar_lock,
                std::function<void(float)> &&_setSeekBar);
    ~daemon_data() = default;
    [[ noreturn ]] void watch();
public:
    Ui::MainWindow *ui;
    music_library &lib;
    std::mutex &volume_lock;
    std::mutex &seek_bar_lock;
    std::function<void(float)> setSeekBar;

};

#endif // DAEMON_DATA_HPP
