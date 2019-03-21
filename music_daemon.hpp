#ifndef SFML_MUSIC_PLAYER_HPP
#define SFML_MUSIC_PLAYER_HPP

#include <QAbstractSlider>

#include <mutex>
#include <thread>

#include "ui_mainwindow.h"
#include "daemon_data.hpp"
#include "music_library.hpp"

// Beware the slight spelling difference!
class music_daemon
{
public:
    music_daemon(daemon_data &&_data);
private:
    daemon_data data;
    std::thread daemon_thread;
};

#endif // SFML_MUSIC_PLAYER_HPP
