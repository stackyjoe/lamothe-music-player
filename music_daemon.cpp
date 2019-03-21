#include "music_daemon.hpp"
#include <mutex>

music_daemon::music_daemon(daemon_data &&_data) :
    data(std::move(_data)),
    daemon_thread([this](){this->data.watch();})
{

}

