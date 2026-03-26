#pragma once

#include "Core/Utilities/Math/Vec2.h"

#include "AnzuAdapter/Private/Engine/AnzuAd/AdBase.h"
#if 0
class MeshAd : public AdBase
{
public:
    static MeshAd* Create(Entity* engineEntity);

    // From AdBase
    void Destroy() override;

    ~MeshAd() override = default;

private:
    bool initialize(Entity* engineEntity);

    // From AdBase
    void detectScale() override;
    void detectTexture() override;
    void detectCorners() override;
    void restoreScale() override;
    void restoreTexture() override;
    void applyTexture() override;
    void applyShrinkToFit() override;
    float computeVisibility() override;
    float computeViewability() override;
    float computeAngle() override;
};
#endif