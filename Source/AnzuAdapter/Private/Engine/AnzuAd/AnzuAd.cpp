#include "AnzuAd.h"
#if 0
#include "AnzuAdapter/Core/Log/Log.h"

AnzuAd* AnzuAd::create()
{
    auto anzuAd = new (std::nothrow) AnzuAd();

    if (anzuAd)
    {
        anzuAd->Name = "AnzuAd";
    }

    return anzuAd;
}

void AnzuAd::onCreate()
{
    // Call base if necessary
    initialize();
}

void AnzuAd::onDestroy()
{
    // Call base if necessary
    uninitialize();
}

void AnzuAd::setChannel(anzu::AnzuChannel* anzuChannel)
{
    if (anzuChannel)
    {
        _channelToSubscribe = anzuChannel;

        if (_derivedAd)
        {
            _derivedAd->SubscribeToChannel(anzuChannel);
        }
    }
}

void AnzuAd::removeChannel()
{
    if (_derivedAd)
    {
        _derivedAd->UnsubscribeFromChannel();
    }

    _channelToSubscribe = nullptr;
}

void AnzuAd::setVisibleOnPlay(bool visible)
{
    if(_isVisibleOnPlay != visible)
    {
        _isVisibleOnPlay = visible;
    }
}

void AnzuAd::interact()
{
    if (_derivedAd)
    {
        _derivedAd->Interact();
    }
}

void AnzuAd::resync()
{
    if (_derivedAd)
    {
        _derivedAd->Resync();
    }
}

std::string AnzuAd::getChannelName()
{
    return _channelToSubscribe ? _channelToSubscribe->ChannelName : "Null";
}

std::string AnzuAd::getChannelTags()
{
    std::string channelTags = "Untagged";

    if (_channelToSubscribe)
    {
        std::string tags = _channelToSubscribe->Tags;

        if (tags.empty() == false)
        {
            channelTags = tags;
        }
    }

    return channelTags;
}

std::string AnzuAd::getChannelRefreshType()
{
    std::string refreshTypeStr = "Null";

    if (_channelToSubscribe)
    {
        anzu::eRefreshType refreshType =
            _channelToSubscribe->RefreshType;

        switch (refreshType)
        {
            case anzu::eRefreshType::Dynamic:
                refreshTypeStr = "Dynamic";
                break;
            case anzu::eRefreshType::Static:
                refreshTypeStr = "Static";
                break;
        }
    }

    return refreshTypeStr;
}

int AnzuAd::getChannelImpressions()
{
    int impressions = 0;

    if (_derivedAd)
    {
        impressions = _derivedAd->GetImpressions();
    }

    return impressions;
}

int AnzuAd::getChannelEmpties()
{
    int empties = 0;

    if (_derivedAd)
    {
        empties = _derivedAd->GetEmpties();
    }

    return empties;
}

float AnzuAd::getAngle()
{
    float angle = 90.0f;

    if (_derivedAd)
    {
        angle = _derivedAd->Visibility.Angle;
    }

    return angle;
}

float AnzuAd::getVisibility()
{
    float visibility = 0.0f;

    if (_derivedAd)
    {
        visibility = _derivedAd->Visibility.Visibility;
    }

    return visibility;
}

float AnzuAd::getViewability()
{
    float viewability = 0.0f;

    if (_derivedAd)
    {
        viewability = _derivedAd->Visibility.Viewability;
    }

    return viewability;
}

void AnzuAd::initialize()
{
    if (_derivedAd == nullptr)
    {
        _derivedAd = MeshAd::Create(owner);

        if (_derivedAd)
        {
            registerEvents();
        }
    }

    bool shouldSubscribeToChannel =
            _derivedAd && _channelToSubscribe;

    if (shouldSubscribeToChannel)
    {
        _derivedAd->SubscribeToChannel(_channelToSubscribe);
    }
}

void AnzuAd::uninitialize()
{
    if (_derivedAd)
    {
        unregisterEvents();
        _derivedAd->Destroy();
        delete _derivedAd;
        _derivedAd = nullptr;
    }
}

void AnzuAd::registerEvents()
{
    _onPlaybackEmptyCallbackId =
            _derivedAd->OnPlaybackEmpty.Register(
                    [this]() { OnPlaybackEmpty.Invoke(); });

    _onPlaybackInitCallbackId =
            _derivedAd->OnPlaybackInit.Register(
                    [this]() { OnPlaybackInit.Invoke(); });

    _onPlaybackStartedCallbackId =
            _derivedAd->OnPlaybackStarted.Register(
                    [this]() { executeOnPlaybackStarted(); });

    _onPlaybackCompleteCallbackId =
            _derivedAd->OnPlaybackComplete.Register(
                    [this]() { OnPlaybackComplete.Invoke(); });

    _onChannelImpressionCallbackId =
            _derivedAd->OnChannelImpression.Register(
                    [this]() { OnChannelImpression.Invoke(); });
}

void AnzuAd::unregisterEvents()
{
    _derivedAd->OnPlaybackEmpty.Unregister(_onPlaybackEmptyCallbackId);
    _derivedAd->OnPlaybackInit.Unregister(_onPlaybackInitCallbackId);
    _derivedAd->OnPlaybackStarted.Unregister(_onPlaybackStartedCallbackId);
    _derivedAd->OnPlaybackComplete.Unregister(_onPlaybackCompleteCallbackId);
    _derivedAd->OnChannelImpression.Unregister(_onChannelImpressionCallbackId);

    // Reset callback id's
    _onPlaybackEmptyCallbackId = 0;
    _onPlaybackInitCallbackId = 0;
    _onPlaybackStartedCallbackId = 0;
    _onPlaybackCompleteCallbackId = 0;
    _onChannelImpressionCallbackId = 0;
}

void AnzuAd::executeOnPlaybackStarted()
{
    bool makePlacementVisible = owner && _isVisibleOnPlay;

    if (makePlacementVisible)
    {
        //owner->setVisible(true); // TODO - implement
    }

    OnPlaybackStarted.Invoke();
}
#endif