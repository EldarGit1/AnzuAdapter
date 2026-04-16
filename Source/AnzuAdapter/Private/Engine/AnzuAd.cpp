#include "Engine/AnzuAd.h"
#include "../Core/AnzuCore.h"
#include "../Core/Log/Log.h"
#include "EngineTexture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/WidgetComponent.h"
#include "UI/AnzuAdMetricsWidget.h"

using namespace anzu;

AAnzuAd::AAnzuAd()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.SetTickFunctionEnable(true);
    PrimaryActorTick.bStartWithTickEnabled = true;

    _metricsWidget = nullptr;
    _metricsWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MetricsWidgetComponent"));
    if (_metricsWidgetComponent)
    {
        _metricsWidgetComponent->SetupAttachment(GetStaticMeshComponent());
        _metricsWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
        _metricsWidgetComponent->SetBlendMode(EWidgetBlendMode::Transparent);
        _metricsWidgetComponent->SetTwoSided(true);
        _metricsWidgetComponent->SetDrawAtDesiredSize(false);
        // Draw size and forward offset are finalised at BeginPlay from actual mesh bounds
    }

    _visibility = { 1.0f, 1.0f, 0.0f };
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
        AsyncTask(ENamedThreads::GameThread, [this]() { updateVis(); });
    });

    _channel->OnApplyTexture.Register([this] { applyTexture(); });

    _channel->OnShrinkToFit.Register([this] {
        AsyncTask(ENamedThreads::GameThread, [this]() { applyShrink(); });
    });

    if (_metricsWidgetComponent)
    {
        _metricsWidgetComponent->SetWidget(CreateWidget<UAnzuAdMetricsWidget>(GetWorld()));
        _metricsWidget = Cast<UAnzuAdMetricsWidget>(_metricsWidgetComponent->GetUserWidgetObject());

        // Derive draw size and forward offset from the actor mesh bounds at spawn time.
        // These are fixed for the lifetime of the actor regardless of later scale changes.
        FVector boundsOrigin, boundsExtent;
        GetActorBounds(false, boundsOrigin, boundsExtent);

        // Width = Y extent * 2, Height = Z extent * 2 (Unreal units → widget pixels 1:1)
        const int32 drawW = FMath::Max(FMath::RoundToInt(boundsExtent.Y * 2.0f), 64);
        const int32 drawH = FMath::Max(FMath::RoundToInt(boundsExtent.Z * 2.0f), 32);
        _metricsWidgetComponent->SetDrawSize(FIntPoint(drawW, drawH));

        // Push the widget just past the front face of the mesh (+5 UU gap)
        _widgetForwardOffset = boundsExtent.X + 5.0f;
    }

    Super::BeginPlay();

}

void AAnzuAd::Tick(float DeltaTime)
{
    if (_metricsWidgetComponent)
    {
        APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
        if (cameraManager)
        {
            const FVector widgetLocation = GetActorLocation() + (GetActorForwardVector() * _widgetForwardOffset);
            _metricsWidgetComponent->SetWorldLocation(widgetLocation);
            const FRotator lookAtRotation = (cameraManager->GetCameraLocation() - widgetLocation).Rotation();
            _metricsWidgetComponent->SetWorldRotation(lookAtRotation);
        }
    }

    /*
    ChannelManager::TryUpdateVisibilityStats(
            _channel,
            Visibility);
    */
    applyTexture();

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

void AAnzuAd::updateVis()
{
    calcAngle();
    calcVisiblity();
    calcViewability();

    if (_metricsWidget)
    {
        _metricsWidget->UpdateMetrics(_visibility.Angle, _visibility.Visibility, _visibility.Viewability);
    }

    anzu::Log::Error("Angle is %f", _visibility.Angle);
    anzu::Log::Error("Visibility is %f", _visibility.Visibility);
    anzu::Log::Error("Viewability is %f", _visibility.Viewability);

    ChannelManager::TryUpdateVisibilityStats(
            _channel,
            _visibility);
}

void AAnzuAd::calcAngle()
{
    UStaticMeshComponent* staticMeshComponent = GetStaticMeshComponent();
    if (staticMeshComponent)
    {
        APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
        if (cameraManager)
        {
            const FVector cameraForward = cameraManager->GetActorForwardVector().GetSafeNormal();
            const FVector meshForward = staticMeshComponent->GetForwardVector().GetSafeNormal();
            const float dot = FMath::Clamp(FVector::DotProduct(cameraForward, meshForward), -1.0f, 1.0f);
            _visibility.Angle = FMath::RadiansToDegrees(FMath::Acos(dot));
        }
    }
}

void AAnzuAd::calcVisiblity()
{
    APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
    if (playerController)
    {
        int32 viewportX = 0;
        int32 viewportY = 0;
        playerController->GetViewportSize(viewportX, viewportY);

        if (viewportX > 0 && viewportY > 0)
        {
            FVector boundsOrigin;
            FVector boundsExtent;
            GetActorBounds(true, boundsOrigin, boundsExtent);

            TArray<FVector> corners;
            corners.Reserve(8);
            corners.Add(boundsOrigin + FVector( boundsExtent.X,  boundsExtent.Y,  boundsExtent.Z));
            corners.Add(boundsOrigin + FVector( boundsExtent.X,  boundsExtent.Y, -boundsExtent.Z));
            corners.Add(boundsOrigin + FVector( boundsExtent.X, -boundsExtent.Y,  boundsExtent.Z));
            corners.Add(boundsOrigin + FVector( boundsExtent.X, -boundsExtent.Y, -boundsExtent.Z));
            corners.Add(boundsOrigin + FVector(-boundsExtent.X,  boundsExtent.Y,  boundsExtent.Z));
            corners.Add(boundsOrigin + FVector(-boundsExtent.X,  boundsExtent.Y, -boundsExtent.Z));
            corners.Add(boundsOrigin + FVector(-boundsExtent.X, -boundsExtent.Y,  boundsExtent.Z));
            corners.Add(boundsOrigin + FVector(-boundsExtent.X, -boundsExtent.Y, -boundsExtent.Z));

            bool bHasProjectedPoint = false;
            float minX = TNumericLimits<float>::Max();
            float minY = TNumericLimits<float>::Max();
            float maxX = TNumericLimits<float>::Lowest();
            float maxY = TNumericLimits<float>::Lowest();

            for (const FVector& corner : corners)
            {
                FVector2D screenPos;
                if (playerController->ProjectWorldLocationToScreen(corner, screenPos, true))
                {
                    bHasProjectedPoint = true;
                    minX = FMath::Min(minX, screenPos.X);
                    minY = FMath::Min(minY, screenPos.Y);
                    maxX = FMath::Max(maxX, screenPos.X);
                    maxY = FMath::Max(maxY, screenPos.Y);
                }
            }

            if (bHasProjectedPoint)
            {
                const float clampedMinX = FMath::Clamp(minX, 0.0f, static_cast<float>(viewportX));
                const float clampedMinY = FMath::Clamp(minY, 0.0f, static_cast<float>(viewportY));
                const float clampedMaxX = FMath::Clamp(maxX, 0.0f, static_cast<float>(viewportX));
                const float clampedMaxY = FMath::Clamp(maxY, 0.0f, static_cast<float>(viewportY));

                const float actorScreenWidth = FMath::Max(0.0f, clampedMaxX - clampedMinX);
                const float actorScreenHeight = FMath::Max(0.0f, clampedMaxY - clampedMinY);
                const float actorScreenArea = actorScreenWidth * actorScreenHeight;
                const float totalScreenArea = static_cast<float>(viewportX) * static_cast<float>(viewportY);

                _visibility.Visibility = FMath::Clamp(actorScreenArea / totalScreenArea, 0.0f, 1.0f);
            }
            else
            {
                _visibility.Visibility = 0.0f;
            }
        }
        else
        {
            _visibility.Visibility = 0.0f;
        }
    }
    else
    {
        _visibility.Visibility = 0.0f;
    }
}

void AAnzuAd::calcViewability()
{
    UStaticMeshComponent* staticMeshComponent = GetStaticMeshComponent();
    APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!staticMeshComponent || !playerController)
    {
        _visibility.Viewability = 0.0f;
        return;
    }

    FVector boundsOrigin;
    FVector boundsExtent;
    GetActorBounds(true, boundsOrigin, boundsExtent);

    TArray<FVector> corners;
    corners.Reserve(8);
    corners.Add(boundsOrigin + FVector( boundsExtent.X,  boundsExtent.Y,  boundsExtent.Z));
    corners.Add(boundsOrigin + FVector( boundsExtent.X,  boundsExtent.Y, -boundsExtent.Z));
    corners.Add(boundsOrigin + FVector( boundsExtent.X, -boundsExtent.Y,  boundsExtent.Z));
    corners.Add(boundsOrigin + FVector( boundsExtent.X, -boundsExtent.Y, -boundsExtent.Z));
    corners.Add(boundsOrigin + FVector(-boundsExtent.X,  boundsExtent.Y,  boundsExtent.Z));
    corners.Add(boundsOrigin + FVector(-boundsExtent.X,  boundsExtent.Y, -boundsExtent.Z));
    corners.Add(boundsOrigin + FVector(-boundsExtent.X, -boundsExtent.Y,  boundsExtent.Z));
    corners.Add(boundsOrigin + FVector(-boundsExtent.X, -boundsExtent.Y, -boundsExtent.Z));

    bool bHasProjectedPoint = false;
    float minX = TNumericLimits<float>::Max();
    float minY = TNumericLimits<float>::Max();
    float maxX = TNumericLimits<float>::Lowest();
    float maxY = TNumericLimits<float>::Lowest();

    for (const FVector& corner : corners)
    {
        FVector2D screenPos;
        if (playerController->ProjectWorldLocationToScreen(corner, screenPos, true))
        {
            bHasProjectedPoint = true;
            minX = FMath::Min(minX, screenPos.X);
            minY = FMath::Min(minY, screenPos.Y);
            maxX = FMath::Max(maxX, screenPos.X);
            maxY = FMath::Max(maxY, screenPos.Y);
        }
    }

    if (!bHasProjectedPoint || maxX <= minX || maxY <= minY)
    {
        _visibility.Viewability = 0.0f;
        return;
    }

    int32 viewportX = 0;
    int32 viewportY = 0;
    playerController->GetViewportSize(viewportX, viewportY);
    if (viewportX <= 0 || viewportY <= 0)
    {
        _visibility.Viewability = 0.0f;
        return;
    }

    constexpr int32 samplesX = 8;
    constexpr int32 samplesY = 8;
    int32 totalSamples = 0;
    int32 visibleSamples = 0;

    const float width = maxX - minX;
    const float height = maxY - minY;

    FCollisionQueryParams queryParams(SCENE_QUERY_STAT(AnzuViewability), true);
    queryParams.bReturnPhysicalMaterial = false;

    for (int32 y = 0; y < samplesY; ++y)
    {
        for (int32 x = 0; x < samplesX; ++x)
        {
            ++totalSamples;

            const float sampleX = minX + ((static_cast<float>(x) + 0.5f) / static_cast<float>(samplesX)) * width;
            const float sampleY = minY + ((static_cast<float>(y) + 0.5f) / static_cast<float>(samplesY)) * height;

            if (sampleX < 0.0f || sampleX > static_cast<float>(viewportX) || sampleY < 0.0f || sampleY > static_cast<float>(viewportY))
            {
                continue;
            }

            FVector rayOrigin;
            FVector rayDirection;
            if (!playerController->DeprojectScreenPositionToWorld(sampleX, sampleY, rayOrigin, rayDirection))
            {
                continue;
            }

            FHitResult hit;
            const FVector traceStart = rayOrigin;
            const FVector traceEnd = traceStart + rayDirection.GetSafeNormal() * 100000.0f;
            const bool bHit = GetWorld() && GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, queryParams);

            if (bHit && hit.GetActor() == this)
            {
                ++visibleSamples;
            }
        }
    }

    if (totalSamples <= 0)
    {
        _visibility.Viewability = 0.0f;
        return;
    }

    _visibility.Viewability = FMath::Clamp(static_cast<float>(visibleSamples) / static_cast<float>(totalSamples), 0.0f, 1.0f);
}

void AAnzuAd::applyShrink()
{
    if(!_channel || !_channel->IsShrinkToFit)
    {
        return;
    }
    float newScaleX = original_scale_x;
    float newScaleY = original_scale_y;

    float new_ar = _channel->NextTextureInfo.AspectRatio;
    float orig_ar = newScaleX / newScaleY;

    if(new_ar < orig_ar)
    {
        newScaleX = original_scale_x * new_ar / orig_ar;
    }
    else if(new_ar > orig_ar)
    {
        newScaleY = original_scale_y * orig_ar / new_ar;
    }

    UStaticMeshComponent* staticMeshComponent = GetStaticMeshComponent();
    if(staticMeshComponent)
    {
        FVector currentScale = staticMeshComponent->GetRelativeScale3D();
        staticMeshComponent->SetRelativeScale3D(FVector(currentScale.Z, newScaleX, newScaleY));
    }
}

void AAnzuAd::applyTexture()
{
    UStaticMeshComponent* staticMeshComponent = GetStaticMeshComponent();
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

}