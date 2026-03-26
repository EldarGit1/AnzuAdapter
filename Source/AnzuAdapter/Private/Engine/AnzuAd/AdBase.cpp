#include "AdBase.h"

// For channel name generator
//#include "SceneManager.h"

#include "AnzuAdapter/Private/Core/AnzuCore.h"
#include "AnzuAdapter/Private/Core/Log/Log.h"
#if 0
using namespace anzu;

bool AdBase::SubscribeToChannel(AnzuChannel* anzuChannel)
{
    bool subscribedSuccessfully = false;

    if (anzuChannel == nullptr)
    {
        Log::Debug("Can't subscribe to a null AnzuChannel.");
    }
    else if (_runningChannel)
    {
        Log::Debug("Already subscribed to AnzuChannel: %s (Id: %d).",
                   _runningChannel->Name.c_str(), _runningChannel->Id);
    }
    else
    {
        bool shouldPlayFallback = false;
        int channelPermissions = getPermissions(*anzuChannel);
        ChannelInfo* channelInfo =
                ChannelManager::CreateOrGetChannel(
                        anzuChannel,
                        channelPermissions);

        if (channelInfo)
        {
            ReadWriteScopeLock lock(_lock, true);

            _anzuChannel = anzuChannel;
            _runningChannel = channelInfo;

            if (_engineEntity)
            {
                detectScale();
                detectTexture();

                bool channelInUse = channelInfo->CurrTextureInfo.Texture != nullptr;

                if (channelInUse)
                {
                    applyTexture();
                }
            }

            subscribedSuccessfully = true;
            registerEvents();
            shouldPlayFallback = channelInfo->IsPlayingFallback;

            Log::Debug("Subscribed successfully to AnzuChannel: %s (Id: %d).",
                       _runningChannel->Name.c_str(), _runningChannel->Id);
        }

        if (shouldPlayFallback)
        {
            ChannelManager::PlayFallbackIfNeeded(channelInfo);
        }
    }

    return subscribedSuccessfully;
}

void AdBase::UnsubscribeFromChannel()
{
    ReadWriteScopeLock lock(_lock, true);

    if (_runningChannel)
    {
        unregisterEvents();

        if (_engineEntity)
        {
            restoreScale();
            restoreTexture();
        }

        resetVisibility();
        ChannelManager::RemovePlacementCountFromChannel(_runningChannel->Name);
        _runningChannel = nullptr;

        if (AnzuCore::GetState() != eAnzuState::Uninitializing)
        {
            _anzuChannel = nullptr;
        }
    }
}

void AdBase::Destroy()
{
    if (_originalTexture)
    {
        _originalTexture.reset();
    }

    if (_onChannelManagerInitializeCallbackId != 0)
    {
        // Unregister here because of ChannelManager initialize right after uninitialize
        ChannelManager::OnInitialize.Unregister(_onChannelManagerInitializeCallbackId);
        _onChannelManagerInitializeCallbackId = 0;
    }

    if (_engineEntity)
    {
        // Release if necessary,
        // depends on engine implementation.
        _engineEntity = nullptr;
    }

    resetVisibility();
}

void AdBase::Interact()
{
    if (_runningChannel && _runningChannel->IsInteractive)
    {
        ChannelManager::Interact(_runningChannel->Id);
    }
}

void AdBase::Resync()
{
    if (_runningChannel)
    {
        ChannelManager::Resync(_runningChannel->Id);
    }
}

void AdBase::registerEvents()
{
    if (_runningChannel)
    {
        _onChannelDestroyCallbackId =
                _runningChannel->OnChannelDestroyed.Register(
                        [this] { UnsubscribeFromChannel(); });

        _onShrinkToFitCallbackId =
                _runningChannel->OnShrinkToFit.Register(
                        [this] { applyShrinkToFit(); });

        _onApplyTextureCallbackId =
                _runningChannel->OnApplyTexture.Register(
                        [this] { applyTexture(); });

        _onUpdateVisibilityCallbackId =
                _runningChannel->OnUpdateVisibility.Register(
                        [this] { onUpdateVisibility(); });

        _onPlaybackEmptyCallbackId =
                _runningChannel->OnPlaybackEmpty.Register(
                        [this] { OnPlaybackEmpty.Invoke(); });

        _onPlaybackInitCallbackId =
                _runningChannel->OnPlaybackInit.Register(
                        [this] { OnPlaybackInit.Invoke(); });

        _onPlaybackStartedCallbackId =
                _runningChannel->OnPlaybackStarted.Register(
                        [this] { OnPlaybackStarted.Invoke(); });

        _onPlaybackCompleteCallbackId =
                _runningChannel->OnPlaybackComplete.Register(
                        [this] { OnPlaybackComplete.Invoke(); });

        _onChannelImpressionCallbackId =
                _runningChannel->OnImpression.Register(
                        [this] { OnChannelImpression.Invoke(); });
    }

    if (_onChannelManagerInitializeCallbackId == 0)
    {
        _onChannelManagerInitializeCallbackId =
                ChannelManager::OnInitialize.Register(
                        [this] { onChannelManagerInitialize(); });
    }
}

void AdBase::unregisterEvents()
{
    if (_runningChannel)
    {
        _runningChannel->OnChannelDestroyed.Unregister(_onChannelDestroyCallbackId);
        _runningChannel->OnShrinkToFit.Unregister(_onShrinkToFitCallbackId);
        _runningChannel->OnApplyTexture.Unregister(_onApplyTextureCallbackId);
        _runningChannel->OnUpdateVisibility.Unregister(_onUpdateVisibilityCallbackId);
        _runningChannel->OnPlaybackEmpty.Unregister(_onPlaybackEmptyCallbackId);
        _runningChannel->OnPlaybackInit.Unregister(_onPlaybackInitCallbackId);
        _runningChannel->OnPlaybackStarted.Unregister(_onPlaybackStartedCallbackId);
        _runningChannel->OnPlaybackComplete.Unregister(_onPlaybackCompleteCallbackId);
        _runningChannel->OnImpression.Unregister(_onChannelImpressionCallbackId); // TODO - Test

        // Reset callback id's
        _onChannelDestroyCallbackId = 0;
        _onShrinkToFitCallbackId = 0;
        _onApplyTextureCallbackId = 0;
        _onUpdateVisibilityCallbackId = 0;
        _onPlaybackEmptyCallbackId = 0;
        _onPlaybackInitCallbackId = 0;
        _onPlaybackStartedCallbackId = 0;
        _onPlaybackCompleteCallbackId = 0;
        _onChannelImpressionCallbackId = 0;
    }
}

void AdBase::onUpdateVisibility()
{
    resetVisibility();

    if (_runningChannel && _engineEntity) // && _engineEntity->isVisible() - // IMPLEMENT
    {
        // IMPLEMENT - All visibility computations need updated corners
        detectCorners();

        float visibilityScore = computeVisibility();
        bool isVisible = visibilityScore > 0.0f;

        if (_isVisible != isVisible)
        {
            _isVisible = isVisible;

            if (_isVisible)
            {
                onBecameVisible();
            }
            else
            {
                onBecameInvisible();
            }
        }

        if (_isVisible)
        {
            Visibility = {
                    visibilityScore,
                    computeViewability(),
                    computeAngle()
            };
        }
    }

    ChannelManager::TryUpdateVisibilityStats(
            _runningChannel,
            Visibility);
}

void AdBase::onBecameVisible()
{
    _runningChannel->VisiblePlacementCounter += 1;

    if (_runningChannel->VisiblePlacementCounter == 1)
    {
        _runningChannel->IsVisible = true;
        ChannelManager::ResumePlaybackAndSetVisible(_runningChannel->Id);
    }
}

void AdBase::onBecameInvisible()
{
    if (_runningChannel->VisiblePlacementCounter > 0)
    {
        _runningChannel->VisiblePlacementCounter -= 1;

        if (_runningChannel->VisiblePlacementCounter == 0)
        {
            _runningChannel->IsVisible = false;
            ChannelManager::PausePlaybackAndSetInvisible(_runningChannel->Id);
        }
    }
}

void AdBase::onChannelManagerInitialize()
{
    if (_anzuChannel)
    {
        SubscribeToChannel(_anzuChannel);
    }
}

void AdBase::resetVisibility()
{
    Visibility = {0, 0, 90};
}

int AdBase::getPermissions(AnzuChannel& anzuChannel)
{
    int permissions = 0;
    bool allowImages = false;
    bool allowVideos = false;
    bool staticContent = false;

    permissions |= Anzu_TexturePermission_t::VTP_RGBA8888;

    if (anzuChannel.IsSupportsBGRA)
    {
        permissions |= Anzu_TexturePermission_t::VTP_SUPPORTS_RB;
    }

    switch (anzuChannel.MediaType)
    {
        case eMediaType::Image:
            permissions |= VTP_IMAGE;
            allowImages = true;
            break;
        case eMediaType::Video:
            permissions |= VTP_VIDEO;
            allowVideos = true;
            break;
        case eMediaType::ImageVideo:
            permissions |= VTP_IMAGE | VTP_VIDEO;
            allowImages = true;
            allowVideos = true;
            break;
        case eMediaType::None:
            permissions |= 0;
            break;
    }
    
    switch (anzuChannel.RefreshType)
    {
        case eRefreshType::Static:
            permissions |= VTP_STATIC;
            staticContent = true;
            break;
        case eRefreshType::Dynamic:
            permissions |= 0;
            break;
    }

    if (anzuChannel.IsInteractive)
    {
        permissions |= VTP_INTERACTIVE;
    }

    if (anzuChannel.FallbackPath.empty() == false)
    {
        // File path for FallbackPath media was provided
        permissions |= VTP_HAS_FALLBACK;
    }

    if (anzuChannel.ChannelName.empty())
    {
        // Random name generator
        permissions |= VTP_GENERATED;

        int precisionVal = 2;
        std::string aspectRatio = std::to_string(anzuChannel.AspectRatio);
        std::string trimmedAspectRatio =
                aspectRatio.substr(0,aspectRatio.find(".") + precisionVal + 1);
        auto scene = SceneManager::getInstance()->getCurrentScene();
        std::ostringstream generatedName;

        generatedName << (scene  ? scene->getName() : "");
        generatedName << "|" << trimmedAspectRatio;
        generatedName << "|" << (allowImages ? "I" : "");
        generatedName << (allowVideos ? "V" : "");
        generatedName << (anzuChannel.IsInteractive ? "N" : "");
        generatedName << (staticContent ? "S" : "");
        generatedName << (anzuChannel.IsShrinkToFit ? "S2F" : "");
        anzuChannel.ChannelName = generatedName.str();
        Log::Error("Channel created without a defined 'ChannelName'."
                   " Temporary name '%s' has been assigned."
                   " For production use you should define explicit channel"
                   " names to avoid unexpected behavior.",
                   generatedName.str().c_str());
    }

    return permissions;
}
#endif