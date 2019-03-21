#include "daemon_data.hpp"
#include <thread>


daemon_data::daemon_data( Ui::MainWindow *_ui,
                          music_library &_lib,
                          std::mutex &_volume_lock,
                          std::mutex &_seek_bar_lock,
                          std::function<void(float)> &&_setSeekBar )
    : ui(_ui),
      lib(_lib),
      volume_lock(_volume_lock),
      seek_bar_lock(_seek_bar_lock),
      setSeekBar(_setSeekBar)
{

}

[[ noreturn ]] void daemon_data::watch() {
    while(1) {
        lib.checkStatus();
        if(ui->seekSlider != nullptr)
            setSeekBar(lib.getPercentPlayed());
        ui->currently_playing_label->setText();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
