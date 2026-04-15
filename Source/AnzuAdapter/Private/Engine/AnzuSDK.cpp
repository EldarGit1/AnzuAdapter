#include "AnzuSDK.h"

#include "AnzuAdapter/Private/Core/AnzuCore.h"
#include "AnzuAdapter/Private/Engine/EngineTexture2D.h"
#include "AnzuAdapter/Private/Core/Loader/AnzuLoader.h"
#include "Core/Log/Log.h"

void UAnzuSDK::Initialize(const AppConfig &appConfig)
{
    _anzuLoaded = true;//AnzuLoader::LoadLib();

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

void UAnzuSDK::Uninitialize()
{
    if (_anzuLoaded)
    {
        EngineLogger::Uninitialize();
        anzu::AnzuCore::Uninitialize();
    }
}

void UAnzuSDK::Tick(float DeltaTime)
{
	if (_anzuLoaded)
	{
        anzu::AnzuCore::Update(DeltaTime);
	}
}
/*
void UAnzuSDK::BeginDestroy()
{
    Uninitialize();
}
*/
