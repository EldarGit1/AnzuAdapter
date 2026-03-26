#pragma once

//#include "Entity.h"
#include "Core/Utilities/Math/Vec3.h"
#include <vector>
//#include "Texture.h"
//#include "Renderer.h"

#include "AnzuAdapter/Private/Core/Channel/ChannelManager.h"

class AdBase
{
public:
    anzu::Event<anzu::func0Args> OnPlaybackEmpty;
    anzu::Event<anzu::func0Args> OnPlaybackInit;
    anzu::Event<anzu::func0Args> OnPlaybackStarted;
    anzu::Event<anzu::func0Args> OnPlaybackComplete;
    anzu::Event<anzu::func0Args> OnChannelImpression;

    anzu::VisibilityInfo Visibility;

    bool SubscribeToChannel(anzu::AnzuChannel* anzuChannel);
    void UnsubscribeFromChannel();
    int GetImpressions() const { return _runningChannel ? _runningChannel->Impressions : 0; };
    int GetEmpties() const { return _runningChannel ? _runningChannel->Empties : 0; };

    // Handles user interaction for interactive channels (e.g., open link)
    void Interact();
    void Resync();

    virtual void Destroy();
    virtual ~AdBase() = default;

protected:
    anzu::ANZU_LOCK _lock;
    anzu::AnzuChannel* _anzuChannel;
    anzu::ChannelInfo* _runningChannel;
    float _originalScaleX;
    float _originalScaleY;
    //std::vector<Vector3> _corners;
    //std::vector<Vector3> _raycastPoints;
    bool _isVisible = false;

    // Engine entities
    //Entity* _engineEntity;
    //std::shared_ptr<Texture> _originalTexture;

    // Callback IDs used to remove registrations
    size_t _onChannelDestroyCallbackId = 0;
    size_t _onShrinkToFitCallbackId = 0;
    size_t _onApplyTextureCallbackId = 0;
    size_t _onUpdateVisibilityCallbackId = 0;
    size_t _onChannelManagerInitializeCallbackId = 0;
    size_t _onPlaybackEmptyCallbackId = 0;
    size_t _onPlaybackInitCallbackId = 0;
    size_t _onPlaybackStartedCallbackId = 0;
    size_t _onPlaybackCompleteCallbackId = 0;
    size_t _onChannelImpressionCallbackId = 0;

    void registerEvents();
    void unregisterEvents();
    void onUpdateVisibility();
    void onBecameVisible();
    void onBecameInvisible();
    void onChannelManagerInitialize();
    void resetVisibility();
    int getPermissions(anzu::AnzuChannel& anzuChannel);

    virtual void detectScale() = 0;
    virtual void detectTexture() = 0;
    virtual void detectCorners() = 0;
    virtual void restoreScale() = 0;
    virtual void restoreTexture() = 0;
    virtual void applyTexture() = 0;
    virtual void applyShrinkToFit() = 0;
    virtual float computeVisibility() = 0;
    virtual float computeViewability() = 0;
    virtual float computeAngle() = 0;
};
