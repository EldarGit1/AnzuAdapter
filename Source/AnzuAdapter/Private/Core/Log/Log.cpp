#include "Log.h"

namespace anzu
{
    bool Log::_isLoggerSet = false;
    eLogLevel Log::_logLevel = eLogLevel::LL_Warning;
    void2Args<int, const char*> Log::_logger = nullptr;

    void Log::SetOnLogCallback(void2Args<int, const char*> logger)
    {
        bool setLogger = _isLoggerSet == false && logger;

        if (setLogger)
        {
            _logger = logger;
            _isLoggerSet = true;

            Anzu_RegisterLogCallback(onNativeLog, nullptr);
        }
        else if (logger == nullptr)
        {
            _logger = nullptr;
            _isLoggerSet = false;
            _logLevel = eLogLevel::LL_Warning;
        }
    }

    void Log::onNativeLog(void* userdata, Anzu_LogLevel_t logLevel, const char* msg)
    {
        logFromNative(static_cast<eLogLevel>(logLevel), "%s", msg);
    }

    void Log::SetLogLevel(eLogLevel logLevel)
    {
        _logLevel = logLevel;
        Anzu_SetLogLevel(static_cast<Anzu_LogLevel_t>(logLevel));
    }

    void Log::Debug(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logInternal("ANZU-1: ", eLogLevel::LL_Debug, format, args);
        va_end(args);
    }

    void Log::Info(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logInternal("ANZU-1: ", eLogLevel::LL_Info, format, args);
        va_end(args);
    }

    void Log::Warning(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logInternal("ANZU-1: ", eLogLevel::LL_Warning, format, args);
        va_end(args);
    }

    void Log::Error(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logInternal("ANZU-1: ", eLogLevel::LL_Error, format, args);
        va_end(args);
    }

    void Log::logFromNative(eLogLevel logLevel, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        logInternal("ANZU-0: ", logLevel, format, args);
        va_end(args);
    }

    void Log::logInternal(const char* prefix, eLogLevel logLevel, const char* format, va_list args)
    {
        bool shouldLog = _isLoggerSet && _logLevel <= logLevel;

        if (shouldLog)
        {
            char buffer[1024];
            int prefixLen = snprintf(buffer, sizeof(buffer), "%s", prefix);
            bool isLenValid = prefixLen >= 0 && prefixLen < static_cast<int>(sizeof(buffer));

            if (isLenValid)
            {
                vsnprintf(buffer + prefixLen, sizeof(buffer) - prefixLen, format, args);

                if (_logger)
                {
                    _logger(static_cast<int>(logLevel), buffer);
                }
            }
        }
    }

    bool Log::IsLoggerSet()
    {
        return _isLoggerSet;
    }
}
