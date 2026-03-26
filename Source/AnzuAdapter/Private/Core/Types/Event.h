#pragma once

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace anzu
{
    template<typename T>
    class Event;

    // Specialization for raw pointers
    template<typename Ret, typename... Args>
    class Event<Ret(*)(Args...)>
    {
    public:
        using Callback = Ret(*)(Args...);
        using ListenerId = size_t;

        ListenerId Register(Callback cb)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            if (cb)
            {
                ListenerId id = _nextId++;
                _listeners[id] = cb;
                return id;
            }

            return 0;
        }

        void Unregister(ListenerId id)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _listeners.erase(id);
        }

        void Invoke(Args... args)
        {
            std::unordered_map<ListenerId, Callback> copy;

            {
                std::lock_guard<std::mutex> lock(_mutex);
                copy = _listeners;
            }

            for (typename std::unordered_map<ListenerId, Callback>::const_iterator it = copy.begin(); it != copy.end(); ++it)
            {
                const Callback& cb = it->second;
                if (cb) cb(args...);
            }
        }

        void Clear()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _listeners.clear();
        }

    private:
        std::unordered_map<ListenerId, Callback> _listeners;
        ListenerId _nextId = 1;
        std::mutex _mutex;
    };

    // Specialization for std::function
    template<typename Ret, typename... Args>
    class Event<std::function<Ret(Args...)>>
    {
    public:
        using Callback = std::function<Ret(Args...)>;
        using ListenerId = size_t;

        ListenerId Register(const Callback& cb)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            if (cb)
            {
                ListenerId id = _nextId++;
                _listeners[id] = cb;

                return id;
            }

            return 0;
        }

        void Unregister(ListenerId id)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _listeners.erase(id);
        }

        // For non-void return types
        template<typename R = Ret>
        typename std::enable_if<!std::is_void<R>::value, std::vector<R>>::type
        Invoke(Args... args)
        {
            std::vector<R> results;
            std::unordered_map<ListenerId, Callback> copy;

            {
                std::lock_guard<std::mutex> lock(_mutex);
                copy = _listeners;
            }

            for (typename std::unordered_map<ListenerId, Callback>::const_iterator it = copy.begin(); it != copy.end(); ++it)
            {
                const Callback& cb = it->second;
                if (cb) results.push_back(cb(args...));
            }

            return results;
        }

        // For void return type
        template<typename R = Ret>
        typename std::enable_if<std::is_void<R>::value, void>::type
        Invoke(Args... args)
        {
            std::unordered_map<ListenerId, Callback> copy;

            {
                std::lock_guard<std::mutex> lock(_mutex);
                copy = _listeners;
            }

            for (typename std::unordered_map<ListenerId, Callback>::const_iterator it = copy.begin(); it != copy.end(); ++it)
            {
                const Callback& cb = it->second;
                if (cb) cb(args...);
            }
        }

        void Clear()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _listeners.clear();
        }

    private:
        std::unordered_map<ListenerId, Callback> _listeners;
        ListenerId _nextId = 1;
        std::mutex _mutex;
    };
}
