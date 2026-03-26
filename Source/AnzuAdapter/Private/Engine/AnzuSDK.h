#pragma once

//#include "Component.h"

#include "AnzuAdapter/Private/Engine/EngineLogger.h"

struct AppConfig
{
    const char* AppKey;
    const char* AppId;
    bool IsDevMode;
    bool DisableCustomTabs;
    bool DisableIDFAPopupOnSDKInit;
    anzu::eLogLevel LogLevel;
};
#if 0
class AnzuSDK : public Component
{
public:
    void Initialize(const AppConfig& appConfig);
    void Uninitialize() const;

    // From component
	void tick() override;
	void onDestroy() override;

private:
	bool _anzuLoaded = false;
};
#endif