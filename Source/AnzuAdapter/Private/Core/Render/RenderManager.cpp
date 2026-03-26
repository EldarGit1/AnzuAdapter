#include "RenderManager.h"

#include "AnzuAdapter/Private/Core/Channel/ChannelManager.h"
#include "AnzuAdapter/Private/Core/Log/Log.h"

#include <cassert>
#include <utility>

namespace anzu
{
    bool RenderManager::_isTextureFactorySet = false;
    TextureInterfaceFactory RenderManager::_textureFactory;
    eTextureUpdateStrategy RenderManager::_textureUpdateStrategy;
    OnRenderingEvent RenderManager::_onRenderingEvent;
    AnzuCustomTextureUpdateCallback_t RenderManager::_customTextureUpdateCallback;
    NativeUpdate RenderManager::_nativeUpdate;

    void RenderManager::Initialize(eTextureUpdateStrategy textureUpdateStrategy, AnzuCustomTextureUpdateCallback_t customTextureUpdateCallback)
    {
        _textureUpdateStrategy = textureUpdateStrategy;
        _onRenderingEvent = nullptr;
        _customTextureUpdateCallback = ChannelManager::CustomEngineTextureUpdateHandler_helper;
        _nativeUpdate = nullptr;
        bool canGenerateNativeTexture = false;
        if (customTextureUpdateCallback)
        {
            _customTextureUpdateCallback = customTextureUpdateCallback;
        }

        switch (_textureUpdateStrategy)
        {
            case eTextureUpdateStrategy::DirectWithSdk:
                // Anzu__Texture_NativeRenderer_CanGenerateNativeTexture
                // should have been already called
                // from game engine side and supplied with
                // the appropriate render API.
                break;
            case eTextureUpdateStrategy::DirectWithEngine:
                assert(_customTextureUpdateCallback != nullptr);
                // This will disable native texture updates in Anzu SDK
                Anzu__Texture_NativeRenderer_CanGenerateNativeTexture("", nullptr);
                // This will pass Anzu SDK the function it will use to update textures when the data is ready
                // Since we don't know what rendering API is used,
                // we will use this class to create textures for us,
                // and if we use 'DirectWithEngine'
                // we will use it to update textures for us
                Anzu__Texture_NativeRenderer_AssignCustomHandler(_customTextureUpdateCallback, nullptr);
                _onRenderingEvent = (OnRenderingEvent)Anzu__Texture_NativeRenderer_GetUpdaterCallback();
                assert(GetTextureFactory().callback != nullptr); // Got null here
                break;
            case eTextureUpdateStrategy::CopyToLocalBuffer:
                // This is handled completely in engine
                _onRenderingEvent = (OnRenderingEvent)nullptr;
                break;
            default:
                break;
        }

        SetRedBlueOrder(0);
    }

    void RenderManager::Uninitialize()
    {
		_textureUpdateStrategy = eTextureUpdateStrategy::DirectWithEngine;
        _onRenderingEvent = nullptr;
        _customTextureUpdateCallback = nullptr;
    }

    void RenderManager::SetRedBlueOrder(int format)
    {
		// 0 - RGBA format, 1 - BGRA format. Should investigate
		Anzu__Texture_NativeRenderer_SetExpectedFormat(format);
    }

    TextureInterfaceFactory RenderManager::GetTextureFactory()
    {
        if (_isTextureFactorySet == false)
        {
            Log::Error("Texture Factory is not set. Please implement and set factory.");
        }

        return _textureFactory;
    }

    void RenderManager::SetTextureFactory(TextureInterfaceFactory factory)
    {
        if (factory.callback)
        {
            _textureFactory = factory;
            _isTextureFactorySet = true;
        }
        else
        {
            _textureFactory = {nullptr, nullptr};
            _isTextureFactorySet = false;
            Log::Error("Texture Factory callback is nullptr. Please implement and set factory callback.");
        }
    }

    void RenderManager::InitializeTextureInfo(TextureInfo& textureInfo, int token, int channelId, TextureInterfaceRef texture, AudioInfoInterfaceRef audio)
    {
        textureInfo.Token = token;
        textureInfo.ChannelId = channelId;
        textureInfo.RenderID = -1;
        textureInfo.Width = -1;
        textureInfo.Height = -1;
        textureInfo.AspectRatio = 0;
        textureInfo.VisibleNormalWidth = 0;
        textureInfo.SwitchRB = false;
        textureInfo.HasAudio = false;
        textureInfo.IsTextureDataFresh = false;
        textureInfo.PlaybackState = ePlaybackState::Init;
        textureInfo.TextureBuffer.clear();
        textureInfo.Texture = std::move(texture);
        textureInfo.AudioInfo = std::move(audio);
        textureInfo.AudioInfo->ClearAudioBufferVector();
    }

    void RenderManager::OnTextureInfo(AnzuMiniJson::JSONObject& textureInfoData, TextureInfo& textureInfo)
    {
        // Media class - 0 means image, 1 means video
        textureInfo.MediaClass = (eMediaClass)(int)textureInfoData["class"]->getNumber();
        textureInfo.Width = (int)textureInfoData["width"]->getNumber();
        textureInfo.Height = (int)textureInfoData["height"]->getNumber();
        textureInfo.AspectRatio = (float)textureInfoData["aspectRatio"]->getNumber();
        textureInfo.VisibleNormalWidth = (float)textureInfoData["visibleNormWidth"]->getNumber();
        textureInfo.SwitchRB = textureInfoData["shouldSwitchRB"]->getBoolean();
        textureInfo.HasAudio = textureInfoData["audio"]->getBoolean();

        if (_textureUpdateStrategy == eTextureUpdateStrategy::CopyToLocalBuffer)
        {
            // Resize local buffer. The buffer is RGBA, and updating it does not updates GPU.
            // This memcpy can be avoided, if other UpdateTexture method is used.
            textureInfo.TextureBuffer.resize(textureInfo.Width * textureInfo.Height * 4);
        }
    }

    void RenderManager::ResetTextureInfo(TextureInfo& textureInfo)
    {
        textureInfo.Token = -1;
        textureInfo.ChannelId = -1;
        textureInfo.RenderID = -1;
        textureInfo.Width = -1;
        textureInfo.Height = -1;
        textureInfo.AspectRatio = 0;
        textureInfo.VisibleNormalWidth = 0;
        textureInfo.SwitchRB = false;
        textureInfo.HasAudio = false;
        textureInfo.IsTextureDataFresh = false;
        textureInfo.PlaybackState = ePlaybackState::PreInit;
        textureInfo.TextureBuffer.clear();
        textureInfo.Texture.reset();
        textureInfo.AudioInfo.reset();
    }

    bool RenderManager::CreateTexture(TextureInfo& textureInfo)
    {
        bool isCreated = textureInfo.Texture->CreateTexture(textureInfo);

        if (isCreated)
        {
            auto id = textureInfo.ChannelId;
            auto handler = textureInfo.Texture->GetTextureHandle();

            ChannelManager::AddNativeTexturePtrToMap(handler, id);
            textureInfo.PlaybackState =
                    ePlaybackState::TextureReadyNoData;
        }

        return isCreated;
    }

    void RenderManager::UpdateTexture(TextureInfo& textureInfo)
    {
        ePlaybackState& playbackState = textureInfo.PlaybackState;

        if (playbackState >= ePlaybackState::TextureInfoReceived)
        {
            if (playbackState == ePlaybackState::TextureInfoReceived)
            {
                if (CreateTexture(textureInfo))
                {
                    // Created successfully. Signal that the texture is
                    // ready, so that decoder can start decoding the first frame.
                    auto id = textureInfo.ChannelId;
                    auto channel = ChannelManager::GetChannelInfoById(id);

                    if (channel)
                    {
                        Anzu__Texture_ResumePlayback(id);
                        channel->OnPlaybackStarted.Invoke();

                        if (channel->IsVisible == false)
                        {
                            // If the channel isn't visible,
                            // capture the first frame and pause.
                            Anzu__Texture_PausePlayback(id);
                        }
                    }
                }
            }

            if (textureInfo.IsTextureDataFresh)
            {
                if (playbackState >= ePlaybackState::TextureReadyNoData)
                {
                    bool shouldUpdateTexture = tryScheduleTextureRender(textureInfo);

                    if (shouldUpdateTexture)
                    {
                        if (playbackState == ePlaybackState::TextureReadyNoData)
                        {
                            playbackState = ePlaybackState::FreshDataForNewTexture;
                            // TODO: figure out a way to turn this into a state
                            textureInfo.IsTextureDataFresh = false;
                        }
                        else if (playbackState == ePlaybackState::UsedDataForSameTexture)
                        {
                            playbackState = ePlaybackState::FreshDataForSameTexture;
                            textureInfo.IsTextureDataFresh = false;
                        }
                    }
                }
            }

            if (playbackState == ePlaybackState::FreshDataForNewTexture ||
                playbackState == ePlaybackState::FreshDataForSameTexture)
            {
                handleTexture(textureInfo);

                // Mark that the data was applied to GPU texture and await for the next content callback
                if (playbackState == ePlaybackState::FreshDataForSameTexture)
                {
                    playbackState = ePlaybackState::UsedDataForSameTexture;
                }
                else
                {
                    playbackState = ePlaybackState::UsedDataForNewTexture;
                }
            }

            // TODO: should i merge this with previous statement and call for apply texture immediately as I have updated the gpu resource?
            // Should be called only for the first frame of video / image
            if (playbackState == ePlaybackState::UsedDataForNewTexture)
            {
                ChannelManager::OnApplyTexture(textureInfo.ChannelId);
            }
        }
    }

    void RenderManager::handleTexture(TextureInfo& textureInfo)
    {
        // Handle texture -
        // This is the 4th step in the channel update flow.
        // The data is fresh which means that either:
        // 1. the rendering event is ready on native side
        // 2. the rendering data is ready in channel->buffer
        // You might choose other places to do this depending on gfx pipeline
        // This step uploads the data to GPU resource
        switch (_textureUpdateStrategy)
        {
            // Invoke SDK function that will update the texture on GPU device directly
            case eTextureUpdateStrategy::DirectWithSdk:
                // In this case the InvokeTextureUpdate function will invoke an inner
                // Anzu SDK function that will work directly with rendering API.
                // This is possible only if Anzu SDK supports the given rendering API
                break;
            case eTextureUpdateStrategy::DirectWithEngine:
                // In this case the InvokeTextureUpdate function will invoke a function that
                // was provided by the game developer. This function purpose is
                // to upload the data to the GPU from the data
                // pointer that will be given to it.
                _onRenderingEvent(textureInfo.RenderID);
                textureInfo.RenderID = 0;
                break;
                // The data was copied to local buffer, we will upload it to GPU
            case eTextureUpdateStrategy::CopyToLocalBuffer:
                // In this case the data that needs to be uploaded to
                // the GPU has been copied into an internal channel buffer
                // and is waiting to be applied to the GPU texture by the game developer.
                // This method is not recommended because it forces us
                // to make a memcpy that could have been avoided
                ChannelManager::GetChannelInfoById(textureInfo.ChannelId)->OnManualTextureUpdate.Invoke(textureInfo);
                break;

            default:
                break;
        }
    }

    bool RenderManager::tryScheduleTextureRender(TextureInfo& textureInfo)
    {
        bool shouldUpdateTexture = false;
        // Schedule rendering for appropriate texture
        switch (_textureUpdateStrategy)
        {
            case eTextureUpdateStrategy::DirectWithSdk:
                break;
            case eTextureUpdateStrategy::DirectWithEngine:
                // This will prepare render task on SDK side. Next we will render it using SDK or game
                textureInfo.RenderID = Anzu__Texture_NativeRenderer_GetRenderID(textureInfo.ChannelId,
                                                                                textureInfo.Texture->GetTextureHandle(),
                                                                                textureInfo.Width,
                                                                                textureInfo.Height,
                                                                                nullptr,
                                                                                0);
                shouldUpdateTexture = textureInfo.RenderID != 0;
                break;
            case eTextureUpdateStrategy::CopyToLocalBuffer:
                // Get buffer data and store it in game
                // 0 if no update is needed, 1 if texture data was dirty
                shouldUpdateTexture = 0 != Anzu__Texture_UpdateData(textureInfo.ChannelId,
                                                                    &textureInfo.TextureBuffer[0],
                                                                    textureInfo.Width,
                                                                    textureInfo.Height);
                break;
            default:
                break;
        }

        return shouldUpdateTexture;
    }
}
