#include "EngineLogger.h"

#include "Core/Log/Log.h"

void EngineLogger::Initialize()
{
    if (anzu::Log::IsLoggerSet() == false)
    {
        anzu::Log::SetOnLogCallback(handleLog);
    }
}

void EngineLogger::Uninitialize()
{
    anzu::Log::SetOnLogCallback(nullptr);
}

void EngineLogger::SetLogLevel(anzu::eLogLevel logLevel)
{
    anzu::Log::SetLogLevel(logLevel);
}

void EngineLogger::handleLog(int logLevel, const char* log)
{
    using namespace anzu;
    switch (logLevel)
    {
        case 0:
            Log::Debug(log);
            break;
        case 1:
            Log::Info(log);
            break;
        case 2:
            Log::Warning(log);
            break;
        case 3:
            Log::Error(log);
            break;
        default:
            break;
    }
}
