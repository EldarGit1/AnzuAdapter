#pragma once

#include "anzuSDK/include/Anzu.h"
#include "AnzuAdapter/Private/Core/Types/Event.h"
#include "AnzuAdapter/Private/Core/Types/Locks.h"
#include "AnzuAdapter/Private/Core/Types/Types.h"
#include "AnzuAdapter/Private/Core/Channel/AnzuChannel.h"
#include "AnzuAdapter/Private/Core/Render/RenderManager.h"

#include <map>
#include <random>

#define CHECK_VISIBILITY_TIMES_PER_SECOND 5

namespace anzu
{
    enum class eMediaState
    {
        Waiting = 0,
        Empty,
        Fallback,
        Creative
    };

    struct VisibilityInfo
    {
        float Visibility;
        float Viewability;
        float Angle;
    };

    struct ChannelInfo
    {
        int Id;
        int Impressions = 0;
        int Empties = 0;
        int PlacementCounter = 0;
        int VisiblePlacementCounter = 0;
        std::string Name;
        std::string Tags;
        std::string FallbackPath;
        float FallbackTimer;
        bool IsPlayingFallback;
        bool IsVisible; // May be set by any placement of this channel
        bool IsInteractive;
        bool IsShrinkToFit;
        float AspectRatio;
        eMediaType MediaType;
        eRefreshType RefreshType;
        eMediaState MediaState;
        VisibilityInfo VisibilityInfo;
        TextureInfo CurrTextureInfo;
        TextureInfo NextTextureInfo;
        ANZU_LOCK Lock;

        Event<func0Args> OnChannelDestroyed;
        Event<func0Args> OnPlaybackEmpty;
        Event<func0Args> OnPlaybackInit; // Token
        Event<func0Args> OnPlaybackStarted; // When the ad subscribe to the channel
        Event<func0Args> OnPlaybackComplete; // Token
        Event<func0Args> OnApplyTexture;
        Event<func1Args<TextureInfo&>> OnManualTextureUpdate; // textureInfo - takes a param because it is not implemented yet, need to check how to handle
        Event<func0Args> OnShrinkToFit;
        Event<func0Args> OnUpdateVisibility;
        Event<func0Args> OnImpression;
    };

    struct Message;

    class ChannelManager
    {
    public:
        static ANZU_LOCK RunningChannelsRWLock;
        static ANZU_LOCK RunningTexturesRWLock;

        // Events
        static Event<func0Args> OnInitialize;

        static void Initialize();
        static void Uninitialize();
        static void DestroyAllChannels();
        static void Update(float deltaTime);
        static void PausePlaybackAndSetInvisible(int channelId);
        static void ResumePlaybackAndSetVisible(int channelId);
        static ChannelInfo* CreateOrGetChannel(AnzuChannel* anzuChannel, int channelPermissions);
        static ChannelInfo* GetChannelInfoById(int channelId);
        static void TryUpdateVisibilityStats(ChannelInfo* channelInfo, const VisibilityInfo &adVis);
        static bool VisibilityPassMinimum(const VisibilityInfo& vis);
        static void RemovePlacementCountFromChannel(const std::string& channelName);
        static void AddNativeTexturePtrToMap(const void* nativeTexturePtr, int channelId);
        static void OnApplyTexture(int channelId);
        static void Interact(int channelId);
        static void Resync(int channelId);
        static void PlayFallbackIfNeeded(ChannelInfo* channelInfo);

        static void ANZU_CALLCONV CustomEngineTextureUpdateHandler_helper(void* userdata, void* nativeTexturePtr, int width, int height, void* data, int size);

        ChannelManager() = delete;

    private:
        static float _time;
        static float _updateVisibilityInterval;
        static std::map<const int, ChannelInfo*> _runningChannelsById; // channelId
        static std::map<const void*, int> _runningChannelsByTexturePtr; // textureNativePtr, channelId

        // Event ids
        static size_t _onTextureEventId;
        static size_t _onPlacementEventId;

        static void updateVisibilityInfoOnAllChannels();
        static void updateChannelsVisibility();
        static void updateTexture();
        static void updateFallbackTimer(float deltaTime);

        static ChannelInfo* findChannelInfoByName(const std::string& channelName);
        static ChannelInfo* findChannelInfoByNativePtr(const void* nativePtr);
        static bool destroyChannel(ChannelInfo* channelInfo);
        static void removeChannelInfoFromMaps(ChannelInfo* channelInfo);
        static void removeNativeTexturePtrFromMap(const void* nativeTexturePtr);

        static void onTextureEvent(const Message& m);
        static void onPlacementEvent(const Message& m);
        static void executeOnTextureInit(int channelId, int token);
        static void executeOnTextureInfo(int channelId, int token, AnzuMiniJson::JSONObject& texture);
        static void executeOnTextureError(int channelId, int token);
        static void executeOnTextureComplete(int channelId, int token);
        static void executeOnPlacementImpression(int channelId);
        static void executeOnPlacementEmpty(int channelId);

        static void ANZU_CALLCONV onNativeTextureInit(void* userdata, int channelId, int token);
        static void ANZU_CALLCONV onNativeTextureUpdated(void* userdata, int channelId, int token);
        static void ANZU_CALLCONV onNativeTextureImpression(void* userdata, int channelId, int token);
    };
}
