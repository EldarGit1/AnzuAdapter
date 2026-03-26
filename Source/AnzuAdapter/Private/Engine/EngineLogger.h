#pragma once

#include "AnzuAdapter/Private/Core/Log/Log.h"
DECLARE_LOG_CATEGORY_EXTERN(AnzuAdapter, Log, All);
class EngineLogger
{
public:
    static void Initialize();
    static void Uninitialize();
    static void SetLogLevel(anzu::eLogLevel logLevel);

    EngineLogger() = delete;

private:
    static void handleLog(int logLevel, const char* log);
};
