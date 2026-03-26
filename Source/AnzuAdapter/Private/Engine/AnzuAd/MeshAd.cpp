#include "MeshAd.h"
#if 0 
#include "AnzuAdapter/Private/Core/Utilities/Math/AnzuMath.h"
#include "AnzuAdapter/Private/Engine/EngineTexture2D.h"

MeshAd* MeshAd::Create(Entity* engineEntity)
{
    auto spriteAd = new (std::nothrow) MeshAd();
    bool createdSuccessfully =
            spriteAd &&
            spriteAd->initialize(engineEntity);

    if (createdSuccessfully == false)
    {
        delete spriteAd;
        spriteAd = nullptr;
    }

    return spriteAd;
}

void MeshAd::Destroy()
{
    UnsubscribeFromChannel();

    if (_engineEntity)
    {
        // Clean/release, if so
        // either here or in AdBase,
        // depends on engine implementation.
    }

    AdBase::Destroy();
}

bool MeshAd::initialize(Entity* engineEntity)
{
    bool isInitialized = false;

    if (engineEntity)
    {
        _engineEntity = engineEntity;

        detectScale();
        detectTexture();
        resetVisibility();
        isInitialized = true;
    }

    return isInitialized;
}

void MeshAd::detectScale()
{
    auto scale = _engineEntity->transform.scale;

    // Detect for the use of shrink to fit
    _originalScaleX = scale.x;
    _originalScaleY = scale.y;
}

void MeshAd::detectTexture()
{
    auto renderer = _engineEntity->getComponent<Renderer>();

    // Retain if necessary, release in AdBase destruction
    _originalTexture = renderer->material.texture;
}

void MeshAd::detectCorners()
{
    // IMPLEMENT
}

void MeshAd::restoreScale()
{
    _engineEntity->transform.scale =
            Vector3(_originalScaleX, _originalScaleY, 1);
}

void MeshAd::restoreTexture()
{
    _engineEntity->getComponent<Renderer>()->material.texture =
            _originalTexture;
}

void MeshAd::applyTexture()
{
    if (_runningChannel)
    {
        auto tex = (EngineTexture2D*)_runningChannel->CurrTextureInfo.Texture.get();
        auto texPtr = static_cast<Texture*>(tex->GetTextureHandle());

        _engineEntity->getComponent<Renderer>()->material.texture =
                std::shared_ptr<Texture>(texPtr, [](Texture*) {});

        // TODO - investigate or remove.
        // Some platforms decode mp4 frames with alpha = 0 by default.
        // TODO - usually visibleNormWidth is 1.0, matching texture width.
        //  For some videos padded to macroblock size, renderer uses only part of it.
        //  Compute actual visible width as:
        //     visibleWidth = visibleNormWidth * textureWidth
        //  Ensures correct display when buffer > content.
    }
}

void MeshAd::applyShrinkToFit()
{
    if (_runningChannel && _runningChannel->IsShrinkToFit)
    {
        float newScaleX = _originalScaleX;
        float newScaleY = _originalScaleY;
        const float incomingAspectRatio = _runningChannel->NextTextureInfo.AspectRatio;
        const float originalAspectRatio = newScaleX / newScaleY;

        anzu::ReadWriteScopeLock lock(_lock, true);

        if (incomingAspectRatio < originalAspectRatio)
        {
            newScaleX = _originalScaleX * incomingAspectRatio / originalAspectRatio;
        }
        else if (incomingAspectRatio > originalAspectRatio)
        {
            newScaleY = _originalScaleY * originalAspectRatio / incomingAspectRatio;
        }

        _engineEntity->transform.scale = Vector3(newScaleX, newScaleY, 1);
    }
}

// IMPLEMENT - The fraction of the ad within the
// screen - screen coverage is normalized (0 to 1).
float MeshAd::computeVisibility()
{
    //            Ad
    //    X----------------X
    //    |                |
    //    |                |
    //    X----------------X
    //
    // - AdCorners = The ad polygon corner points in SCREEN-space.
    // - ViewBounds = The SCREEN-space polygon representing the visible area
    //   for the active view/camera (viewport/display/visibleRect depending on platform).
    // - Use the AnzuMath class to compute the visibility score:
    //      SCORE = GetPolygonScreenCoverage(AdCorners, ViewBounds)

    return 1.0f;
}

// IMPLEMENT - The fraction of the ad unobstructed from the camera
// (occlusion-based viewability) - normalized (0 to 1).
float MeshAd::computeViewability()
{
    //           Ad
    //    o--------------o
    //    |  x  x  x  x  |
    //    |  x  x  x  x  |
    //    |  x  x  x  x  |
    //    |  x  x  x  x  |
    //    o--------------o
    //
    // We sample a 4 x 4 grid of points on the ad and raycast from the camera
    // to each point to test occlusion. The score is the fraction of points
    // whose first hit is the ad.
    //
    // AdCorners = The ad polygon corner points in WORLD-space.
    // CameraPosition = The position of the camera in WORLD-space.
    //
    // - Use the AnzuMath class to cast the viewability score:
    //      TotalRaycastPoints = GetRaycastScanWidth() * GetRaycastScanHeight().
    //      RaycastPoints = GenerateRaycastGridPoints(AdCorners), WORLD-space points on the ad.
    //
    // - For each Point in RaycastPoints:
    //      DirectionVector = Point - CameraPosition (normalize).
    //      RayEnd = CameraPosition + DirectionVector * FarPlane (frustum's far plane).
    //      HitResult = Cast a ray from CameraPosition to RayEnd.
    //
    //      If HitResult is the ad collider (first hit):
    //          Hits++
    //
    // SCORE = Hits / TotalRaycastPoints

    return 1.0f;
}

// IMPLEMENT - The angle between the ad's facing direction and the camera view direction.
// Angle 0: Ad faces the camera (BEST score).
// Angle 90: Ad is perpendicular to the camera (worst score).
// Angle 180: Ad faces away from the camera (worst score).
float MeshAd::computeAngle()
{
    // Notes: Working in WORLD space.
    //
    //   (A)-----------------(B)
    //    |                   |
    //    |        Ad         |
    //    |                   |
    //   (D)-----------------(C)
    //
    // Edge1 = B - A
    // Edge2 = D - A
    // AdNormal = -Normalize(Edge1 Cross Edge2)
    // CameraForwardDir = Normalize(CameraForwardWorld), Camera forward/look direction in WORLD space.
    // AdCenter = (A + B + C + D) / 4
    // CameraToAdCenterDir = Normalize(AdCenter - CameraPosition)
    // AngleCameraForwardToAdNormal = Angle(CameraForwardDir, AdNormal)
    // AngleAdNormalToCenterRay = Angle(AdNormal, CameraToAdCenterDir)
    // AngleCameraForwardToCenterRay = Angle(CameraForwardDir, CameraToAdCenterDir)
    // PreciseAngle = 55 degrees, tuning constant.
    //
    // If AngleCameraForwardToCenterRay == 0:
    //      SCORE = AngleAdNormalToCenterRay
    // Else:
    //      PreciseScore = PreciseAngle * (AngleCameraForwardToAdNormal / (AngleCameraForwardToCenterRay + PreciseAngle))
    //      If AngleCameraForwardToAdNormal <= AngleAdNormalToCenterRay:
    //          If AngleAdNormalToCenterRay <= AngleCameraForwardToCenterRay:
    //              SCORE = PreciseScore
    //          Else:
    //              Weight = Clamp((AngleAdNormalToCenterRay - AngleCameraForwardToCenterRay) /
    //                            (PreciseAngle - AngleCameraForwardToCenterRay), 0..1)
    //              SCORE = Weight * AngleAdNormalToCenterRay + (1 - Weight) * AngleCameraForwardToAdNormal
    //      Else:
    //          If AngleAdNormalToCenterRay <= AngleCameraForwardToCenterRay:
    //              SCORE = PreciseScore
    //          Else:
    //              Weight = Clamp((AngleAdNormalToCenterRay - AngleCameraForwardToCenterRay) /
    //                            (PreciseAngle - AngleCameraForwardToCenterRay), 0..1)
    //              SCORE = Weight * AngleAdNormalToCenterRay + (1 - Weight) * PreciseScore

    return 0.0f;
}
#endif