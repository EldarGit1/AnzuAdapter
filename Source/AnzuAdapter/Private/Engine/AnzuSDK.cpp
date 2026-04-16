#include "AnzuSDK.h"

#include "AnzuAdapter/Private/Core/AnzuCore.h"
#include "AnzuAdapter/Private/Engine/EngineTexture2D.h"
#include "AnzuAdapter/Private/Core/Loader/AnzuLoader.h"
#include "Core/Log/Log.h"
#include "Engine/World.h"

void UAnzuSDK::Initialize(FSubsystemCollectionBase& Collection)
{
	const AppConfig AppConfig = {
		"88455eafeeb2aaeff910feb5",
		"appId",
		true,
		false,
		false,
		anzu::eLogLevel::LL_Debug
	};

	InitializeSdk(AppConfig);
	Super::Initialize(Collection);
}

void UAnzuSDK::Deinitialize()
{
	UninitializeSdk();
	Super::Deinitialize();
}


void UAnzuSDK::InitializeSdk(const AppConfig &appConfig)
{
	if (_isSdkInitialized)
	{
		return;
	}

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
		_isSdkInitialized = true;
	}
}

void UAnzuSDK::UninitializeSdk()
{
	if (_anzuLoaded && _isSdkInitialized)
    {
        anzu::AnzuCore::Uninitialize();
		EngineLogger::Uninitialize();
		_isSdkInitialized = false;
    }
}

void UAnzuSDK::Tick(float DeltaTime)
{
	if (_anzuLoaded && _isSdkInitialized)
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
