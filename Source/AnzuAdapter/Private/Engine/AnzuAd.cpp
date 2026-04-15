#include "Engine/AnzuAd.h"
#include "../Core/AnzuCore.h"
#include "../Core/Log/Log.h"
#include "EngineTexture2D.h"
#include "Materials/MaterialInstanceDynamic.h"

using namespace anzu;

AAnzuAd::AAnzuAd()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.SetTickFunctionEnable(true);
    PrimaryActorTick.bStartWithTickEnabled = true;

    Visibility = { 100, 100, 90 };
}

void AAnzuAd::BeginPlay()
{
    UStaticMeshComponent* staticMeshComponent = GetStaticMeshComponent();
    if(staticMeshComponent == nullptr)
    {
        //ERROR
    }
    int32 materialsNumber = staticMeshComponent->GetNumMaterials();
    if(materialsNumber != 1)
    {
        //ERROR
    }
    UMaterialInterface* originalMaterial = staticMeshComponent->GetMaterial(0);
    if(originalMaterial == nullptr)
    {
        //ERROR
    }
    TArray<UTexture*> originalTexturesArray;
    originalMaterial->GetUsedTextures(
        originalTexturesArray
        , EMaterialQualityLevel::Type::Num
        , true
        , ERHIFeatureLevel::Type::Num
        , true
    );
    if (originalTexturesArray.Num() == 0)
    {
        //ERROR
        Log::Error("originalTexturesArray is empty.");                   
    }
    _texture = Cast<UTexture2D>(originalTexturesArray[0]);

    if(_texture == nullptr)
    {
        Log::Error("Texture is null ?!");
        return;
    }
    else
    {
        Log::Error("TEXTURE FOUND!");
    }

    //subscribe to channel..
    AnzuChannel channel;
    channel.ChannelName = "channel1";
    channel.IsShrinkToFit = true;
    int permissions = 0;
    permissions |= VTP_IMAGE | VTP_VIDEO;

    
    _channel = ChannelManager::CreateOrGetChannel(&channel, permissions);

    if(_channel == nullptr)
    {
        Log::Error("Failed to create channel!");
        return;
    }
    else
    {
        Log::Error("Channel created!!");
    }
    

    _channel->IsVisible = true;
    _channel->OnPlaybackEmpty.Register( [this] { /*OnPlaybackEmpty.Invoke();*/ Log::Error("EMPTY"); });

    _channel->OnPlaybackInit.Register( [this] { /*OnPlaybackInit.Invoke();*/ Log::Error("INIT"); });

    _channel->OnPlaybackStarted.Register( [this] { /*OnPlaybackStarted.Invoke();*/ Log::Error("STARTED"); });

    _channel->OnPlaybackComplete.Register([this] { /*OnPlaybackComplete.Invoke();*/ Log::Error("Complete"); });

    _channel->OnImpression.Register([this] { /*OnChannelImpression.Invoke();*/Log::Error("IMPRESSION"); });

    _channel->OnUpdateVisibility.Register([this] {
        ChannelManager::TryUpdateVisibilityStats(
            _channel,
            Visibility);
        }
    );
    Super::BeginPlay();

}

void AAnzuAd::Tick(float DeltaTime)
{
    ChannelManager::TryUpdateVisibilityStats(
            _channel,
            Visibility);

    UStaticMeshComponent* staticMeshComponent = GetStaticMeshComponent();    
    //apply every tick!
    UTexture2D* texPtr = nullptr;
    if (_channel->CurrTextureInfo.Texture)
    {
        auto tex = static_cast<::EngineTexture2D*>(_channel->CurrTextureInfo.Texture.get());
        if (tex != nullptr)
        {
            texPtr = static_cast<UTexture2D*>(tex->GetTextureHandle());
        }
    }

    if (/*texPtr != nullptr*/ true)
    {
        UMaterialInstanceDynamic* dynamicMaterial = staticMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
        if (dynamicMaterial == nullptr)
        {
            Log::Error("Failed to create dynamic material instance.");
        }
        else
        {
            TArray<FMaterialParameterInfo> textureParameterInfos;
            TArray<FGuid> textureParameterGuids;
            dynamicMaterial->GetAllTextureParameterInfo(textureParameterInfos, textureParameterGuids);

            bool bTextureReplaced = false;
            for (const FMaterialParameterInfo& parameterInfo : textureParameterInfos)
            {
                UTexture* parameterTexture = nullptr;
                if (dynamicMaterial->GetTextureParameterValue(parameterInfo, parameterTexture) && parameterTexture == _texture)
                {
                    dynamicMaterial->SetTextureParameterValue(parameterInfo.Name, texPtr);
                    bTextureReplaced = true;
                    break;
                }
            }

            if (!bTextureReplaced && textureParameterInfos.Num() > 0)
            {
                dynamicMaterial->SetTextureParameterValue(textureParameterInfos[0].Name, texPtr);
                bTextureReplaced = true;
            }

            if (!bTextureReplaced)
            {
                Log::Error("Failed to replace material texture: no texture parameter found.");
            }
        }
    }
    else
    {
        Log::Error("texPtr is null, texture swap skipped.");
    }


    Super::Tick(DeltaTime);
}

void AAnzuAd::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AAnzuAd::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}