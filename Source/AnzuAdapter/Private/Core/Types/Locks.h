#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <mutex>

namespace anzu
{
    typedef std::mutex SimpleLock;
    typedef std::unique_lock<SimpleLock> SimpleScopeLock;
    typedef std::shared_ptr<SimpleLock> SimpleLockRef;
    typedef std::shared_ptr<SimpleScopeLock> SimpleScopeLockRef; // Not quite sure if we need this at all

    class WritePreferredReadWriteLock
    {
        // -1    : one writer
        // 0     : no reader and no writer
        // n > 0 : n reader
        int32_t _status;
        int32_t _waiting_readers;
        int32_t _waiting_writers;
        SimpleLock _mtx;
        std::condition_variable _read_cv;
        std::condition_variable _write_cv;
    public:
        WritePreferredReadWriteLock() : _status(0), _waiting_readers(0), _waiting_writers(0) {}
        void beginRead()
        {
            SimpleScopeLock lck(_mtx);
            _waiting_readers += 1;
            _read_cv.wait(lck, [&]() { return _waiting_writers == 0 && _status >= 0; });
            _waiting_readers -= 1;
            _status += 1;
        }

        void beginWrite()
        {
            SimpleScopeLock lck(_mtx);
            _waiting_writers += 1;
            _write_cv.wait(lck, [&]() { return _status == 0; });
            _waiting_writers -= 1;
            _status = -1;
        }

        void end()
        {
            SimpleScopeLock lck(_mtx);
            if (_status == -1) {
                _status = 0;
            }
            else {
                _status -= 1;
            }
            if (_waiting_writers > 0) {
                if (_status == 0) {
                    _write_cv.notify_one();
                }
            }
            else {
                _read_cv.notify_all();
            }
        }
    };

    class ReadWriteScopeLock
    {
        WritePreferredReadWriteLock& lock;
        bool is_write;
    public:
        ReadWriteScopeLock(WritePreferredReadWriteLock& _lock, bool _write)
                :lock(_lock)
                , is_write(_write)
        {
            if (is_write) {
                lock.beginWrite();
            }
            else {
                lock.beginRead();
            }
        }
        ~ReadWriteScopeLock()
        {
            if (is_write) {
                lock.end();
            }
            else {
                lock.end();
            }
        }
    };

#define ANZU_LOCK WritePreferredReadWriteLock
#define ANZU_SCOPE_LOCK ReadWriteScopeLock
    typedef std::atomic<int> AtomicInt;
}
