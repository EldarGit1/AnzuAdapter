

#if 0
#pragma once
#include "Component.h"

#include "AnzuAdapter/Engine/AnzuAd/MeshAd.h"

class AnzuAd : public Component
{
public:
    static AnzuAd* create();

    // Events
    anzu::Event<anzu::func0Args> OnPlaybackEmpty;
    anzu::Event<anzu::func0Args> OnPlaybackInit;
    anzu::Event<anzu::func0Args> OnPlaybackStarted;
    anzu::Event<anzu::func0Args> OnPlaybackComplete;
    anzu::Event<anzu::func0Args> OnChannelImpression;

    // From Component
    void onCreate() override;
    void onDestroy() override;

    void setChannel(anzu::AnzuChannel* anzuChannel);
    void removeChannel();
    void setVisibleOnPlay(bool visible);
    void interact();
    void resync();

    std::string getChannelName();
    std::string getChannelTags();
    std::string getChannelRefreshType();
    int getChannelImpressions();
    int getChannelEmpties();
    float getAngle();
    float getVisibility();
    float getViewability();

    virtual ~AnzuAd() = default;

private:
    bool _isVisibleOnPlay = true;
    AdBase* _derivedAd;
    anzu::AnzuChannel* _channelToSubscribe;

    // Callback Ids
    size_t _onPlaybackEmptyCallbackId = 0;
    size_t _onPlaybackInitCallbackId = 0;
    size_t _onPlaybackStartedCallbackId = 0;
    size_t _onPlaybackCompleteCallbackId = 0;
    size_t _onChannelImpressionCallbackId = 0;

    void initialize();
    void uninitialize();
    void registerEvents();
    void unregisterEvents();
    void executeOnPlaybackStarted();
};
#endif