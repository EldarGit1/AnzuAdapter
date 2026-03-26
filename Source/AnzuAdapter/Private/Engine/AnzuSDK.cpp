#include "AnzuSDK.h"

#if 0
#include "Timer.h"

#include "AnzuAdapter/Core/AnzuCore.h"
#include "AnzuAdapter/Engine/EngineTexture2D.h"
#include "AnzuAdapter/Core/Loader/common/AnzuLoader.h"

void AnzuSDK::Initialize(const AppConfig &appConfig)
{
    _anzuLoaded = AnzuLoader::LoadLib();

	if (_anzuLoaded)
	{
		// Engine level initialization
		EngineLogger::Initialize();
		EngineLogger::SetLogLevel(appConfig.LogLevel);
		EngineTexture2D::Initialize();

		// Core level initialization
		anzu::AdapterConfig adapterConfig = {
			appConfig.AppKey,
			appConfig.AppId,
			appConfig.IsDevMode,
			appConfig.DisableCustomTabs,
			appConfig.DisableIDFAPopupOnSDKInit
		};

        anzu::AnzuCore::Initialize(adapterConfig);
	}
}

void AnzuSDK::Uninitialize() const
{
    if (_anzuLoaded)
    {
        EngineLogger::Uninitialize();
        anzu::AnzuCore::Uninitialize();
    }
}

void AnzuSDK::tick()
{
	if (_anzuLoaded)
	{
        anzu::AnzuCore::Update(Timer::DeltaTime());
	}
}

void AnzuSDK::onDestroy()
{
    Uninitialize();
}
#endif