#include "music_library.hpp"

#include <QDebug>
#include <QFile>
#include <QTextStream>


music_library::music_library( [[ maybe_unused ]] std::string library_path, music_player &_player) :
    current_song(std::nullopt),
    desired_state(UserDesiredState::pause),
    player(_player)
{
    QFile file(QString::fromStdString(library_path));
    QString user_library_path;
    file.open(QFile::ReadOnly);
    QTextStream user_settings(&file);

    user_library_path = user_settings.readLine();
    setLibraryFile(user_library_path);
}

void music_library::setState(UserDesiredState _state)
{
    desired_state = _state;
    return;
}

UserDesiredState music_library::getState() {
    return desired_state;
}

void music_library::setLibraryFile(QString lib_path) {
    QFile file(lib_path);
    std::vector<std::string> new_song_paths;

    if(file.open(QFile::ReadOnly) ) {
        QTextStream lib(&file);
        QString line;
        while(not lib.atEnd()) {
            line = lib.readLine().trimmed();
            new_song_paths.push_back(line.toStdString());
        }
    }
    else {
        qDebug() << "Error opening file: " << lib_path << "\n";
        return;
    }

    std::string cur("");
    if(current_song.has_value())
        cur = *current_song.value();

    current_song.reset();
    song_paths = std::move(new_song_paths);
    unique_song_paths = std::set<std::string>(song_paths.begin(), song_paths.end());

    auto i = song_paths.begin();
    auto end = song_paths.end();

    for(; i != end; ++i) {
        if( *i == cur ) {
            current_song = i;
            return;
        }
    }

    return;
}

void music_library::setVolume(int new_vol) {
    player.setVolume(new_vol);
}

float music_library::getPercentPlayed() {
    return player.getPercentPlayed();
}

void music_library::seekByPercent(float percent) {
    player.seekByPercent(percent);
    return;
}

bool music_library::contains(const std::string &song_path) const {
    if(unique_song_paths.find(song_path) != unique_song_paths.end() )
        return true;
    return false;
}

void music_library::add(std::string new_song_path) {
    if(unique_song_paths.find(new_song_path) != unique_song_paths.end())
        return;

    std::string cur;

    if (current_song.has_value()) {
        cur = *current_song.value();
    }
    else {
        cur = "";
    }

    song_paths.push_back(new_song_path);
    unique_song_paths.insert(new_song_path);
    current_song.reset();

    auto i = song_paths.begin();
    auto end = song_paths.end();

    for(;i != end; ++i) {
        if(*i == cur) {
            current_song = i;
            return;
        }

    }

    return;
}

void music_library::remove(const std::string &bad_song_path) {
    if(unique_song_paths.find(bad_song_path) == unique_song_paths.end())
        return;

    unique_song_paths.erase(bad_song_path);

    for(auto itr = song_paths.begin(); itr != song_paths.end(); ++itr) {
        if(*itr == bad_song_path) {
            song_paths.erase(itr);
            unique_song_paths.erase(bad_song_path);
            return;
        }
    }

    return;
}

const std::vector<std::string> &music_library::songs() {
    return song_paths;
}

void music_library::checkStatus() {
    std::unique_lock lock(player_mutex, std::try_to_lock);
    if(!lock.owns_lock())
        return;

    PlayerStatus player_status = player.getStatus();

    switch(desired_state) {
    case UserDesiredState::play :
        switch(player_status) {
        case PlayerStatus::playing:
            break;
        case PlayerStatus::paused:
            player.play();
            break;

        default:
            if(current_song.has_value() and current_song.value() != song_paths.end() && ++current_song.value() != song_paths.end()) {
                if(not player.openFromFile(*current_song.value()) ) {
                    qDebug() << "Error opening file: " << QString::fromStdString(*current_song.value()) << "\n";
                    setState(UserDesiredState::error);
                    return;
                }
                player.play();
            }
            else {
                setState(UserDesiredState::stop);
            }
            break;
        }
        break;
    case UserDesiredState::pause :
        switch(player_status) {
        case PlayerStatus::playing:
            player.pause();
            break;

        default:
            break;
        }
        break;

    case UserDesiredState::stop :
        switch (player_status) {
        case PlayerStatus::playing:
            player.pause();
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

void music_library::playSong(const std::string &path) {
    std::scoped_lock lock(player_mutex);
    if(unique_song_paths.find(path) == unique_song_paths.end()) {
        qDebug() << "Error in music_library::playSong(" << QString::fromStdString(path) << ")\n";
        return;
    }

    if(not player.openFromFile(path)) {
        qDebug() << "Error opening file: " << QString::fromStdString(path) << "\n";
    }

    player.play();
    setState(UserDesiredState::play);

    auto itr = song_paths.begin();

    while(itr != song_paths.end() and *itr != path)
        ++itr;

    current_song = itr;
}

void music_library::stop() {
    player.stop();
}
