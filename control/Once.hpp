#pragma once

#include <atomic>
#include <functional>

namespace geni::control {

    template<typename Function, typename... Args>
    inline
    void once( Function&& f, Args&&... args )
    {
        static std::atomic_flag flag;
        if(!flag.test_and_set()) {
            std::invoke(
                std::forward<Function>(f),
                std::forward<Args>(args)...
            );
        }
    }

}