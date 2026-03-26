#include "ChannelManager.h"

#include "AnzuAdapter/Private/Core/Log/Log.h"
#include "AnzuAdapter/Private/Core/Managers/MessageManager.h"
#include "AnzuAdapter/Private/Core/Channel/ChannelEventManager.h"

namespace anzu
{
// Windows defines its own min/max macros, which break std::min/std::max.
// We have to undefine them here to allow using the standard versions.
#ifdef max
#define ANZU_RESTORE_MAX max
#undef max
#endif

#ifdef min
#define ANZU_RESTORE_MIN min
#undef min
#endif

    ANZU_LOCK ChannelManager::RunningChannelsRWLock;
    ANZU_LOCK ChannelManager::RunningTexturesRWLock;

    float ChannelManager::_time = 0.0f;
    float ChannelManager::_updateVisibilityInterval = 1.0f / CHECK_VISIBILITY_TIMES_PER_SECOND;

    std::map<const int, ChannelInfo*> ChannelManager::_runningChannelsById;
    std::map<const void*, int> ChannelManager::_runningChannelsByTexturePtr;

    // Events
    Event<func0Args> ChannelManager::OnInitialize;

    // Event ids
    size_t ChannelManager::_onTextureEventId = 0;
    size_t ChannelManager::_onPlacementEventId = 0;

    void ChannelManager::Initialize()
    {
        Anzu_RegisterTextureInitCallback(onNativeTextureInit, nullptr);
        Anzu_RegisterTextureUpdateCallback(onNativeTextureUpdated, nullptr);
        Anzu_RegisterTextureImpressionCallback(onNativeTextureImpression, nullptr);
        Anzu_RegisterTexturePlacementProgressCallback(/*onNativeTexturePlacementProgress*/nullptr, nullptr); // Need to implement
        Anzu_RegisterTexturePlaybackProgressCallback(/*onNativeTexturePlaybackProgress*/nullptr, nullptr); // Need to implement
        Anzu_RegisterTexturePlaybackCompleteCallback(/*onNativeTexturePlaybackComplete*/nullptr, nullptr); // Need to implement

        _onTextureEventId = MessageManager::OnTextureEvents.Register(onTextureEvent);
        _onPlacementEventId = MessageManager::OnPlacementEvents.Register(onPlacementEvent);

        OnInitialize.Invoke();
    }

    void ChannelManager::Uninitialize()
    {
        MessageManager::OnTextureEvents.Unregister(_onTextureEventId);
        MessageManager::OnPlacementEvents.Unregister(_onPlacementEventId);

        DestroyAllChannels();
    }

    void ChannelManager::DestroyAllChannels()
    {
        std::vector<ChannelInfo*> channelsToDestroy;

        {
            ANZU_SCOPE_LOCK lock(RunningChannelsRWLock, false);

            for (auto& it : _runningChannelsById)
            {
                channelsToDestroy.push_back(it.second);
            }
        }

        for (const auto& channelInfo : channelsToDestroy)
        {
            channelInfo->OnChannelDestroyed.Invoke();
        }
    }

    void ChannelManager::Update(float deltaTime)
    {
        _time += deltaTime;

        if (_time > _updateVisibilityInterval)
        {
            updateVisibilityInfoOnAllChannels();

            _time = 0.0f;
        }

        updateTexture();
        updateFallbackTimer(deltaTime);
    }

    void ChannelManager::PausePlaybackAndSetInvisible(int channelId)
    {
        Anzu__Texture_PausePlayback(channelId);
        Anzu__Texture_SetVisibility(channelId, false);
    }

    void ChannelManager::ResumePlaybackAndSetVisible(int channelId)
    {
        Anzu__Texture_ResumePlayback(channelId);
        Anzu__Texture_SetVisibility(channelId, true);
    }

    ChannelInfo* ChannelManager::CreateOrGetChannel(AnzuChannel* anzuChannel, int channelPermissions)
    {
        if (anzuChannel == nullptr) return nullptr;

        ChannelInfo* retCode;
        ChannelInfo* channelInfo = findChannelInfoByName(anzuChannel->ChannelName);
        ANZU_SCOPE_LOCK lock(RunningChannelsRWLock, true);

        if (channelInfo == nullptr)
        {
            // This is a new channel.
            // Anzu__Texture_CreateInstance is using the channel name as unique identifier for a channel.
            // - When called with the same channel name, returns the reference counted object for this channel.
            // - You should not alter tabs/permissions once set or results are unpredictable.
            // - GetWidth and height are not in pixels. Instead, the GetHeight should always be 1, and width should represents the calculated aspect ratio.
            int channelId = Anzu__Texture_CreateInstanceForTexture(anzuChannel->ChannelName.c_str(),
                                                                   anzuChannel->Tags.c_str(),
                                                                   channelPermissions,
                                                                   anzuChannel->AspectRatio,
                                                                   1,
                                                                   anzuChannel->IsShrinkToFit,
                                                                   anzuChannel->AudioSampleRate,
                                                                   0,
                                                                   0);

            // This is the sole point where ChannelInfo is created
            retCode = new ChannelInfo();
            retCode->Id = channelId;
            retCode->Impressions = 0;
            retCode->Empties = 0;
            retCode->PlacementCounter = 1;
            retCode->Name = anzuChannel->ChannelName;
            retCode->Tags = anzuChannel->Tags;
            retCode->FallbackPath = anzuChannel->FallbackPath;
            retCode->FallbackTimer = anzuChannel->FallbackTimer;
            retCode->IsPlayingFallback = false;
            retCode->MediaType = anzuChannel->MediaType;
            retCode->RefreshType = anzuChannel->RefreshType;
            retCode->IsVisible = false;
            retCode->IsInteractive = anzuChannel->IsInteractive;
            retCode->IsShrinkToFit = anzuChannel->IsShrinkToFit;
            retCode->AspectRatio = anzuChannel->AspectRatio;
            retCode->MediaState = eMediaState::Waiting;

            // Add new ChannelInfo to map
            _runningChannelsById.insert(std::pair<const int, ChannelInfo*>((const int)retCode->Id, retCode));
        }
        else
        {
            channelInfo->PlacementCounter++;
            retCode = channelInfo;
        }

        Log::Debug("Channel: %s (Id: %d) Placement Count: %d",
                   anzuChannel->ChannelName.c_str(), retCode->Id, retCode->PlacementCounter);

        return retCode;
    }

    ChannelInfo* ChannelManager::GetChannelInfoById(int channelId)
    {
        ChannelInfo* retCode = nullptr;
        ANZU_SCOPE_LOCK lock(RunningChannelsRWLock, false);

        auto it = _runningChannelsById.find(channelId);
        if (it != _runningChannelsById.end())
        {
            retCode = it->second;
        }

        return retCode;
    }

    void ChannelManager::TryUpdateVisibilityStats(ChannelInfo* channelInfo, const VisibilityInfo &adVis)
    {
        if (channelInfo == nullptr) return;

        VisibilityInfo newChannelVis(channelInfo->VisibilityInfo);
        bool adPass = VisibilityPassMinimum(adVis);
        bool channelPass = VisibilityPassMinimum(newChannelVis);

        if (adPass && channelPass == false)
        {
            newChannelVis = adVis;
        }
        else if (adPass && channelPass)
        {
            if (adVis.Viewability > newChannelVis.Viewability)
            {
                newChannelVis = adVis;
            }
            else if (adVis.Viewability == newChannelVis.Viewability)
            {
                if (adVis.Visibility > newChannelVis.Visibility)
                {
                    newChannelVis = adVis;
                }
                else if (adVis.Visibility == newChannelVis.Visibility)
                {
                    if (adVis.Angle < newChannelVis.Angle)
                    {
                        newChannelVis = adVis;
                    }
                }
            }
        }
        else if (adPass == false && channelPass == false)
        {
            if (adVis.Visibility > newChannelVis.Visibility)
            {
                newChannelVis = adVis;
            }
        }

        // Clamping
        newChannelVis.Visibility = std::max(0.0f, std::min(1.0f, newChannelVis.Visibility));
        newChannelVis.Viewability = std::max(0.0f, std::min(1.0f, newChannelVis.Viewability));
        newChannelVis.Angle = std::min(180.0f, std::max(0.0f, newChannelVis.Angle));

        // Pass score to the SDK safely
        Anzu__Texture_SetVisibilityScore(channelInfo->Id,
                                         newChannelVis.Visibility,
                                         newChannelVis.Viewability,
                                         newChannelVis.Angle);

        channelInfo->VisibilityInfo = newChannelVis;
    }

    bool ChannelManager::VisibilityPassMinimum(const VisibilityInfo& vis)
    {
        // We got the limits from App on Anzu dashboard
        return LimitsManager::GetMaxAngle() >= vis.Angle &&
               LimitsManager::GetMinVisibility() <= vis.Visibility &&
               LimitsManager::GetMinViewability() <= vis.Viewability;
    }

    void ChannelManager::RemovePlacementCountFromChannel(const std::string& channelName)
    {
        bool shouldDestroyChannel = false;
        ChannelInfo* channelInfo = findChannelInfoByName(channelName);

        if (channelInfo)
        {
            {
                ANZU_SCOPE_LOCK lock(channelInfo->Lock, true);

                channelInfo->PlacementCounter -= 1;
                shouldDestroyChannel = (channelInfo->PlacementCounter < 1);
                Log::Debug("Channel: %s (Id: %d) Placement Count: %d", channelName.c_str(),
                           channelInfo->Id, channelInfo->PlacementCounter);
            }

            if (shouldDestroyChannel)
            {
                destroyChannel(channelInfo);
            }
        }
    }

    void ChannelManager::AddNativeTexturePtrToMap(const void* nativeTexturePtr, const int channelId)
    {
        if (nativeTexturePtr)
        {
            ANZU_SCOPE_LOCK lock(RunningTexturesRWLock, true);
            _runningChannelsByTexturePtr.insert({nativeTexturePtr, channelId});
        }
    }

    // Called when NextTextureInfo is ready to replace the current texture.
    // Cleans up old GPU resources and removes texture from all pointer maps.
    void ChannelManager::OnApplyTexture(int channelId)
    {
        ChannelInfo* channelInfo = GetChannelInfoById(channelId);

        if (channelInfo)
        {
            if (channelInfo->CurrTextureInfo.Texture)
            {
                removeNativeTexturePtrFromMap(channelInfo->CurrTextureInfo.Texture->GetTextureHandle());
            }

            // Hold old texture to avoid iOS Cocos2d-x race in Texture2D destructor.
            // Prevents destruction before new texture is set.
            auto tempTexture = channelInfo->CurrTextureInfo.Texture;

            channelInfo->CurrTextureInfo = channelInfo->NextTextureInfo;
            RenderManager::ResetTextureInfo(channelInfo->NextTextureInfo);

            // New data uploaded to GPU and ready for rendering
            channelInfo->CurrTextureInfo.PlaybackState = ePlaybackState::UsedDataForSameTexture;
            channelInfo->OnApplyTexture.Invoke();
        }
    }

    // TODO: implement example interaction and pass game object name to send to the server
    void ChannelManager::Interact(int channelId)
    {
        Anzu__Texture_Interact(channelId, 0 /*main click*/, "");
    }

    void ChannelManager::Resync(int channelId)
    {
        Anzu__Texture_Resync(channelId);
    }

    // Callback for "Update With Engine", passed from game engine to SDK
    void ChannelManager::CustomEngineTextureUpdateHandler_helper(void* userdata, void* nativeTexturePtr, int width, int height, void* data, int size)
    {
        ChannelInfo* channelInfo = findChannelInfoByNativePtr(nativeTexturePtr);

        if (channelInfo)
        {
            if (channelInfo->NextTextureInfo.Texture)
            {
                if (channelInfo->NextTextureInfo.Texture->GetTextureHandle() == nativeTexturePtr)
                {
                    channelInfo->NextTextureInfo.Texture->UpdateTexture(width, height, data, size,
                                                                        channelInfo->NextTextureInfo.SwitchRB);
                }
            }

            if (channelInfo->CurrTextureInfo.Texture)
            {
                if (channelInfo->CurrTextureInfo.Texture->GetTextureHandle() == nativeTexturePtr)
                {
                    channelInfo->CurrTextureInfo.Texture->UpdateTexture(width, height, data, size,
                                                                        channelInfo->CurrTextureInfo.SwitchRB);
                }
            }
        }
    }

    void ChannelManager::updateVisibilityInfoOnAllChannels()
    {
        ANZU_SCOPE_LOCK lock(RunningChannelsRWLock, false);

        // Iterate over running channels, asking them to report current visibility scores
        for (auto entry : _runningChannelsById)
        {
            ChannelInfo* channelInfo = entry.second;

            // Reset channel visibility stats
            channelInfo->VisibilityInfo.Angle = 180.0f;
            channelInfo->VisibilityInfo.Visibility = 0.0f;
            channelInfo->VisibilityInfo.Viewability = 0.0f;

            // Each placements calculates its visibility and tries to update the channel.
            // The best stats are always kept in the channel’s VisibilityInfo.
            // Only one placements ultimately updates the channel’s data.
            channelInfo->OnUpdateVisibility.Invoke();
        }
    }

    ChannelInfo* ChannelManager::findChannelInfoByName(const std::string& channelName)
    {
        ChannelInfo* retCode = nullptr;
        ChannelInfo* channelInfo = nullptr;
        ANZU_SCOPE_LOCK lock(RunningChannelsRWLock, false);

        for (const auto& entry : _runningChannelsById)
        {
            channelInfo = entry.second;

            if (channelInfo && channelInfo->Name == channelName)
            {
                retCode = channelInfo;
                break;
            }
        }

        return retCode;
    }

    ChannelInfo* ChannelManager::findChannelInfoByNativePtr(const void* nativePtr)
    {
        ChannelInfo* retCode = nullptr;
        ANZU_SCOPE_LOCK lock(RunningTexturesRWLock, false);

        auto it = _runningChannelsByTexturePtr.find(nativePtr);
        if (it != _runningChannelsByTexturePtr.end())
        {
            retCode = GetChannelInfoById(it->second);
        }

        return retCode;
    }

    bool ChannelManager::destroyChannel(ChannelInfo* channelInfo)
    {
        bool isDestroyed = false;

        if (channelInfo)
        {
            isDestroyed = Anzu__Texture_RemoveInstance(channelInfo->Id);

            if (isDestroyed)
            {
                removeChannelInfoFromMaps(channelInfo);
            }
        }

        return isDestroyed;
    }

    void ChannelManager::removeChannelInfoFromMaps(ChannelInfo* channelInfo)
    {
        if (channelInfo)
        {
            const std::string channelName(channelInfo->Name);
            const int channelId = channelInfo->Id;

            if (channelInfo->NextTextureInfo.Texture)
            {
                removeNativeTexturePtrFromMap(channelInfo->NextTextureInfo.Texture->GetTextureHandle());
            }

            if (channelInfo->CurrTextureInfo.Texture)
            {
                removeNativeTexturePtrFromMap(channelInfo->CurrTextureInfo.Texture->GetTextureHandle());
            }

            ANZU_SCOPE_LOCK lock(RunningChannelsRWLock, true);
            _runningChannelsById.erase(channelInfo->Id);

            // This is the sole point where ChannelInfo is deleted
            delete channelInfo;

            Log::Debug("Channel: %s (Id: %d) was deleted and removed from maps.", channelName.c_str(), channelId);
        }
    }

    void ChannelManager::removeNativeTexturePtrFromMap(const void* nativeTexturePtr)
    {
        if (nativeTexturePtr)
        {
            ANZU_SCOPE_LOCK lock(RunningTexturesRWLock, true);
            _runningChannelsByTexturePtr.erase(nativeTexturePtr);
        }
    }

    void ChannelManager::onTextureEvent(const Message &m)
    {
        if (m.Subtype.empty())
        {
            Log::Error("*** Missing texture subtype.");
        }
        else
        {
            std::shared_ptr<JSONValue> textureValue = MessageManager::JsonParser.parse(m.Data);

            if (textureValue)
            {
                AnzuMiniJson::JSONObject texture = textureValue->getObject();
                int id = (int) texture["id"]->getNumber();
                int token = (int) texture["token"]->getNumber();

                if (m.Subtype == "init")
                {
                    executeOnTextureInit(id, token);
                }
                else if (m.Subtype == "info")
                {
                    executeOnTextureInfo(id, token, texture);
                }
                else if (m.Subtype == "error")
                {
                    executeOnTextureError(id, token);
                }
                else if (m.Subtype == "complete")
                {
                    executeOnTextureComplete(id, token);
                }
            }
            else
            {
                Log::Error("*** SDK ERROR: invalid json: %s.", m.Data.c_str());
            }
        }
    }

    void ChannelManager::onPlacementEvent(const Message &m)
    {
        if (m.Subtype.empty())
        {
            Log::Error("*** Missing placement subtype.");
        }
        else
        {
            int id = atoi(m.Data.c_str());

            if (m.Subtype == "impression")
            {
                executeOnPlacementImpression(id);
            }
            else if (m.Subtype == "empty")
            {
                executeOnPlacementEmpty(id);
            }
        }
    }

    void ChannelManager::executeOnTextureInit(int channelId, int token)
    {
        // Initialization doesn't guarantee creative data, we might still get EMPTY
        ChannelInfo* channelInfo = GetChannelInfoById(channelId);

        if (channelInfo)
        {
            auto tf = RenderManager::GetTextureFactory();
            auto af = AudioManager::GetAudioFactory();
            auto texture = tf.callback(tf.userdata);
            auto audio = af.callback(af.userdata);
            ANZU_SCOPE_LOCK lock(channelInfo->Lock, true);

            // Store the token. Only texture messages with this token affect this playback.
            RenderManager::InitializeTextureInfo(channelInfo->NextTextureInfo, token, channelId, texture, audio);
            channelInfo->MediaState = eMediaState::Waiting;
            channelInfo->OnPlaybackInit.Invoke();
            ChannelEventManager::OnPlaybackInit.Invoke(channelId);
        }
    }

    void ChannelManager::executeOnTextureInfo(int channelId, int token, AnzuMiniJson::JSONObject& textureInfoData)
    {
        ChannelInfo* channelInfo = GetChannelInfoById(channelId);

        if (channelInfo && channelInfo->NextTextureInfo.Token == token)
        {
            // Can't generate GPU texture on this thread. Instead, the engine will
            // process it later by reading the state of the texture info.
            ANZU_SCOPE_LOCK lock(channelInfo->Lock, true);

            RenderManager::OnTextureInfo(textureInfoData, channelInfo->NextTextureInfo);
            channelInfo->OnShrinkToFit.Invoke();
            channelInfo->NextTextureInfo.PlaybackState = ePlaybackState::TextureInfoReceived;
            channelInfo->MediaState = eMediaState::Creative;

            // Token 0 indicates we might be playing fallback media
            if (channelInfo->IsPlayingFallback && token != 0)
            {
                // TODO - Test
                channelInfo->IsPlayingFallback = false;
            }
        }
    }

    void ChannelManager::executeOnTextureError(int channelId, int token)
    {
        ChannelInfo* channelInfo = GetChannelInfoById(channelId);

        if (channelInfo)
        {
            ANZU_SCOPE_LOCK lock(channelInfo->Lock, false);

            if (channelInfo->CurrTextureInfo.Token == token ||
                channelInfo->NextTextureInfo.Token == token)
            {
                Log::Error("Got playback error on channel: %d.", channelId);
            }
        }
    }

    void ChannelManager::executeOnTextureComplete(int channelId, int token)
    {
        ChannelInfo* channelInfo = GetChannelInfoById(channelId);

        if (channelInfo)
        {
            ANZU_SCOPE_LOCK lock(channelInfo->Lock, false);

            if (channelInfo->NextTextureInfo.Token == token ||
                channelInfo->CurrTextureInfo.Token == token)
            {
                channelInfo->OnPlaybackComplete.Invoke();
                ChannelEventManager::OnPlaybackComplete.Invoke(channelId);
            }
        }
    }

    void ChannelManager::executeOnPlacementImpression(int channelId)
    {
        ChannelInfo* channelInfo = GetChannelInfoById(channelId);

        if (channelInfo)
        {
            Log::Debug("Got an impression on channel: %d.", channelId);
            channelInfo->Impressions += 1;
            channelInfo->OnImpression.Invoke();
            ChannelEventManager::OnChannelImpression.Invoke(channelId);
        }
    }

    void ChannelManager::executeOnPlacementEmpty(int channelId)
    {
        ChannelInfo* channelInfo = GetChannelInfoById(channelId);

        if (channelInfo)
        {
            bool shouldPlayFallback = false;

            {
                ANZU_SCOPE_LOCK lock(channelInfo->Lock, true);

                // Might need to access also when eMediaState == Empty
                if (channelInfo->MediaState == eMediaState::Waiting)
                {
                    channelInfo->MediaState = eMediaState::Empty;
                    channelInfo->NextTextureInfo.PlaybackState = ePlaybackState::Init;
                    shouldPlayFallback = true;
                }

                channelInfo->Empties += 1;
            }

            if (shouldPlayFallback)
            {
                // Call right before the object is displayed to ensure FallbackPath media loads if
                // no server media arrives. Fallback files must be bundled with the game
                // (bmp, png, jpg, psd, dds, gif, ogv supported everywhere. wmv not on ps4, mp4 not on linux).
                PlayFallbackIfNeeded(channelInfo);
            }

            channelInfo->OnPlaybackEmpty.Invoke();
            ChannelEventManager::OnChannelEmpty.Invoke(channelId);
            Log::Debug("Got an empty on channel: %d.\nNo suitable media was "
                       "returned from the server for this channel.", channelId);
        }
    }

    void ChannelManager::updateTexture()
    {
        ANZU_SCOPE_LOCK lock (RunningChannelsRWLock, false);

        for (const auto& entry : _runningChannelsById)
        {
            ChannelInfo* channelInfo = entry.second;

            if (channelInfo)
            {
                RenderManager::UpdateTexture(channelInfo->CurrTextureInfo);
                RenderManager::UpdateTexture(channelInfo->NextTextureInfo);
            }
        }
    }

    void ChannelManager::updateFallbackTimer(float deltaTime)
    {
        ANZU_SCOPE_LOCK readingChannelsLock (RunningChannelsRWLock, false);

        for (const auto& entry : _runningChannelsById)
        {
            ChannelInfo* channelInfo = entry.second;

            if (channelInfo && channelInfo->FallbackTimer > 0.0f)
            {
                {
                    ANZU_SCOPE_LOCK lock (channelInfo->Lock, true);
                    channelInfo->FallbackTimer -= deltaTime;
                }

                if (channelInfo->FallbackTimer <= 0.0f)
                {
                    PlayFallbackIfNeeded(channelInfo);
                }
            }
        }
    }

    void ChannelManager::PlayFallbackIfNeeded(ChannelInfo* channelInfo)
    {
        if (channelInfo)
        {
            bool shouldPlayFallback =
                    channelInfo->FallbackPath.empty() == false &&
                    channelInfo->FallbackTimer <= 0.0f;

            if (shouldPlayFallback)
            {
                {
                    ANZU_SCOPE_LOCK lock (channelInfo->Lock, true);
                    channelInfo->MediaState = eMediaState::Fallback;
                    channelInfo->IsPlayingFallback = true;
                }

                Anzu__Texture_PlayFilename(
                        channelInfo->Id,
                        channelInfo->FallbackPath.c_str(),
                        true,
                        true);
            }
        }
    }

    // The 1st callback - gives us a token for this creative lifecycle
    void ChannelManager::onNativeTextureInit(void* userdata, int channelId, int token)
    {
        executeOnTextureInit(channelId, token);
    }

    // The 3rd callback (many more for videos/modules), signaling a new buffer needs to update the GPU texture.
    // Gets a direct GPU handle (renderId) or copies the buffer at the right time.
    // The render pipeline then uses renderId or the buffer to update the texture in DrawGame when dirty.
    void ChannelManager::onNativeTextureUpdated(void* userdata, int channelId, int token)
    {
        ChannelInfo* channelInfo = GetChannelInfoById(channelId);

        if (channelInfo)
        {
            if (channelInfo->NextTextureInfo.Token == token) // New creative first frame/image has been decoded
            {
                channelInfo->NextTextureInfo.IsTextureDataFresh = true;
            }
            else if (channelInfo->CurrTextureInfo.Token == token) // Next video frame has been decoded
            {
                channelInfo->CurrTextureInfo.IsTextureDataFresh = true;
            }
        }
    }

    void ChannelManager::onNativeTextureImpression(void* userdata, int channelId, int token)
    {
        executeOnPlacementImpression(channelId);
    }

// Finally restore min/max if any
#ifdef ANZU_RESTORE_MIN
#define min ANZU_RESTORE_MIN
#undef ANZU_RESTORE_MIN
#endif // ANZU_RESTORE_MIN

#ifdef ANZU_RESTORE_MAX
#define max ANZU_RESTORE_MAX
#undef ANZU_RESTORE_MAX
#endif // ANZU_RESTORE_MAX
}
