#pragma once

#include "../Core/Channel/ChannelManager.h"
#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "AnzuAd.generated.h"

UCLASS()
class ANZUADAPTER_API AAnzuAd : public AStaticMeshActor
{
	GENERATED_BODY()

    UTexture2D* _texture;
    anzu::ChannelInfo* _channel;
    anzu::VisibilityInfo _visibility;

    float original_scale_x = 1.f;
    float original_scale_y = 1.f;

    void applyTexture();
    void updateVis();
    void applyShrink();

    void calcAngle();
    void calcVisiblity();
    void calcViewability();

public:
	AAnzuAd();

    anzu::Event<anzu::func0Args> OnPlaybackEmpty;
    anzu::Event<anzu::func0Args> OnPlaybackInit;
    anzu::Event<anzu::func0Args> OnPlaybackStarted;
    anzu::Event<anzu::func0Args> OnPlaybackComplete;
    anzu::Event<anzu::func0Args> OnChannelImpression;

    // AStaticMeshActor methods
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void PostInitializeComponents() override;
    // AStaticMeshActor methods

};
