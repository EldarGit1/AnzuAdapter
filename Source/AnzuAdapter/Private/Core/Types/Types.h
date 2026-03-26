#pragma once

#include <functional>

namespace anzu
{
    using func0Args = std::function<void()>;
    
    template <typename T>
    using func1Args = std::function<void(T)>;

    template <typename T1, typename T2>
    using func2Args = std::function<void(T1, T2)>;

    template <typename T1, typename T2, typename T3>
    using func3Args = std::function<void(T1, T2, T3)>;

    using void0Args = void(*)();
    
    template <typename T>
    using void1Args = void(*)(T);

    template <typename T1, typename T2>
    using void2Args = void(*)(T1, T2);

    template <typename T1, typename T2, typename T3>
    using void3Args = void(*)(T1, T2, T3);
}
