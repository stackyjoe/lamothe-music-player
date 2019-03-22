#ifndef DAEMON_DATA_TPP
#define DAEMON_DATA_TPP

#include <any>
#include <thread>
#include <type_traits>

template<typename... Args>
[[noreturn]] void daemon_watcher(Args ... args) {
    // Checks that its arguments are all functions with no needed arguments.
    static_assert ( (std::is_invocable<decltype(args)>::value
                     && ... && true),
                    "daemon_watch instantiated with template argument that is not a function of no arguments.\n");
    while(1) {
        ((void)args(), ...);
    }
}

#endif // DAEMON_DATA_TPP
