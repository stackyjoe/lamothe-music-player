#ifndef PLAYER_INTERFACE_HPP
#define PLAYER_INTERFACE_HPP

#include <optional>

#include "music_player.hpp"

// This is a thread safe interface for the abstract class music_player.
class  player_interface {
public:
    player_interface() = delete;
    player_interface(const player_interface &) = delete;
    player_interface &operator=(const player_interface &) = delete;

    explicit player_interface(music_player &_player) : player(_player) { }
protected:

    mutable std::mutex access_lock;
    music_player &player;


public:
    PlayerStatus get_status() const {
        return player.getStatus();
    }

    float get_percent_played() const {
        return player.getPercentPlayed();
    }

    // Trick to avoid sharing a mutex for the music_player directly
    template<class operation>
    auto perform(operation o) const -> decltype (o(player)) {
        std::scoped_lock access(access_lock);
        return o(player);
    }

    template<class operation>
    auto try_perform(operation o) const -> std::optional<decltype(o(player))> {
        std::unique_lock access(access_lock, std::try_to_lock);
        if(not access.owns_lock())
            return std::nullopt;
        return o(player);
    }
};

#endif // PLAYER_INTERFACE_HPP
