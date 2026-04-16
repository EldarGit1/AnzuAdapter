#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "AnzuAdapter/Private/Engine/EngineLogger.h"
#include "AnzuSDK.generated.h"

struct AppConfig
{
    const char* AppKey;
    const char* AppId;
    bool IsDevMode;
    bool DisableCustomTabs;
    bool DisableIDFAPopupOnSDKInit;
    anzu::eLogLevel LogLevel;
};

UCLASS()
class ANZUADAPTER_API UAnzuSDK : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()
public:
    // UGameInstanceSubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;

    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UMyObject, STATGROUP_Tickables);
    }

    virtual bool IsTickable() const override
    {
        return true;
    }

    virtual bool IsTickableInEditor() const override
    {
        return false;
    }

	//void BeginDestroy() override;

private:
    void InitializeSdk(const AppConfig& appConfig);
    void UninitializeSdk();


	bool _anzuLoaded = false;
    bool _isSdkInitialized = false;

};
