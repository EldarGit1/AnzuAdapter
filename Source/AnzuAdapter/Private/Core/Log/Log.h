#pragma once

#include "anzuSDK/include/Anzu.h"
#include "AnzuAdapter/Private/Core/Types/Types.h"

#include <cstdarg>

namespace anzu
{
    enum class eLogLevel
    {
        LL_Debug = 0,       // Log everything.
        LL_Info = 1,        // Log non-debug messages.
        LL_Warning = 2,     // Log only warning and error messages.
        LL_Error = 3,       // Log only error messages.
        LL_None = 4         // No logging.
    };

    class Log
    {
    public:
        static void SetOnLogCallback(void2Args<int, const char*> logger);
        static void SetLogLevel(eLogLevel logLevel);        
        static void Debug(const char* format, ...);
        static void Info(const char* format, ...);
        static void Warning(const char* format, ...);
        static void Error(const char* format, ...);
        static bool IsLoggerSet();

        Log() = delete;

    private:
        static bool _isLoggerSet;
        static eLogLevel _logLevel;
        static void2Args<int, const char*> _logger;

        static void ANZU_CALLCONV onNativeLog(void* userdata, Anzu_LogLevel_t level, const char* msg);
        static void logFromNative(eLogLevel logLevel, const char* format, ...);
        static void logInternal(const char* prefix, eLogLevel logLevel, const char* format, va_list args);
    };
}
