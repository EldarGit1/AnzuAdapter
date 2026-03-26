#include "EngineLogger.h"

DEFINE_LOG_CATEGORY(AnzuAdapter);

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
    switch (logLevel)
    {
        case 0:
            UE_LOG(AnzuAdapter, Verbose, TEXT("%s"), log);
            break;
        case 1:
            UE_LOG(AnzuAdapter, Display, TEXT("%s"), log);
            break;
        case 2:
            UE_LOG(AnzuAdapter, Warning, TEXT("%s"), log);
            break;
        case 3:
            UE_LOG(AnzuAdapter, Error, TEXT("%s"), log);
            break;
        default:
            break;
    }
}
