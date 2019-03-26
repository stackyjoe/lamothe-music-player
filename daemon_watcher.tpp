#ifndef DAEMON_WATCHER_TPP
#define DAEMON_WATCHER_TPP

#include <type_traits>

template<typename... Args>
[[noreturn]] void daemon_watcher(Args ... args) {
    // Checks that its arguments are all functions with no needed arguments.
    static_assert ( (std::is_invocable<decltype(args)>::value
                     && ... && true),
                    "daemon_watcher instantiated with a non-void function as template argument.\n");
    while(1) {
        ((void)args(), ...);
    }
}

#endif // DAEMON_WATCHER_TPP
