#pragma once
#include "CoreMinimal.h"
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
class ANZUADAPTER_API UAnzuSDK : public UObject, public FTickableGameObject
{
    GENERATED_BODY()
public:
    void Initialize(const AppConfig& appConfig);
    void Uninitialize();

    // From component
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

	void BeginDestroy() override;


private:
	bool _anzuLoaded = false;
};
