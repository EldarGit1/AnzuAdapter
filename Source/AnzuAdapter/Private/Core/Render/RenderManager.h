#pragma once

#include "anzuSDK/include/Anzu.h"
#include "AnzuAdapter/Private/Core/Json/minijsonparse.h"
#include "AnzuAdapter/Private/Core/Types/Locks.h"
#include "AnzuAdapter/Private/Core/Audio/AudioManager.h"

namespace anzu
{
    struct TextureInfo;
    class TextureInterface;
    using TextureInterfaceRef = std::shared_ptr<TextureInterface>;
    using TextureInterfaceFactoryCallback = TextureInterfaceRef(*)(void* userdata);
    using OnRenderingEvent = void(ANZU_CALLCONV*)(int renderId); // __cdecl
    using NativeUpdate = Anzu_RenderingEventReturnCodes_t(ANZU_CALLCONV*)(int renderId);

    enum class eTextureUpdateStrategy
    {
        DirectWithSdk = 0,
        DirectWithEngine,
        CopyToLocalBuffer
    };

    enum class ePlaybackState
    {
        PreInit = 0,
        Init,
        TextureInfoReceived,
        TextureReadyNoData,      // initial gpu resource was generated, but data hasn't been received
        FreshDataForSameTexture, // received data, copy it to the old texture ( reuse ), mostly for videos
        FreshDataForNewTexture,  // received data, copy it to newly generated texture
        UsedDataForNewTexture,   // data has been copied, need to "apply" the texture and switch curr and next info
        UsedDataForSameTexture,  // data was applied, nothing to do
        Complete
    };

    enum class eMediaClass
    {
        ImageMedia = 0,
        VideoMedia,
        ModuleMedia
    };

#pragma region TextureInterface

    struct TextureInterfaceFactory
    {
        TextureInterfaceFactoryCallback callback;
        void* userdata;
    };

    class TextureInterface
    {
    public:
        virtual void UpdateTexture(int width, int height, void* data, int size, bool switchRB) = 0;
        virtual bool CreateTexture(TextureInfo& textureInfo) = 0;
        virtual void* GetTextureHandle() = 0;
        virtual ~TextureInterface() = default;
    };

#pragma endregion // TextureInterface
#pragma region TextureInfo

    struct TextureInfo
    {
        int Token; // Unique identifier for showing specific creative on specific channel, comes from logic
        int ChannelId;
        int RenderID; // We need this only if we render using direct texture updates
        int Width;
        int Height;
        float AspectRatio;
        float VisibleNormalWidth;
        bool SwitchRB;
        bool HasAudio;
        bool IsTextureDataFresh;
        eMediaClass MediaClass;
        ePlaybackState PlaybackState;
        std::vector<char> TextureBuffer; // We need this only if we render using buffer copy (indirect texture updates)
        TextureInterfaceRef Texture; // Reference to Texture Handler class
        AudioInfoInterfaceRef AudioInfo;
        // Heads up - Context: copy constructor - simply copying render Id  will never work because render Id specific native
        // texture handle, perhaps find a way to fix this? (if something brakes reset render Id to 0.
    };

#pragma endregion // TextureInfo
#pragma region RenderManager

    class RenderManager
    {
    public:
        static void Initialize(eTextureUpdateStrategy textureUpdateStrategy = eTextureUpdateStrategy::DirectWithEngine, AnzuCustomTextureUpdateCallback_t customTextureUpdateCallback = nullptr);
        static void Uninitialize();
        static void SetRedBlueOrder(int format);
        static TextureInterfaceFactory GetTextureFactory();
        static void SetTextureFactory(TextureInterfaceFactory factory);

        // TextureInfo managing
        static void InitializeTextureInfo(TextureInfo& textureInfo, int token, int channelId, TextureInterfaceRef texture, AudioInfoInterfaceRef audio);
        static void OnTextureInfo(AnzuMiniJson::JSONObject& textureInfoData, TextureInfo& textureInfo);
        static void ResetTextureInfo(TextureInfo& textureInfo);
        static bool CreateTexture(TextureInfo& textureInfo);
        static void UpdateTexture(TextureInfo& textureInfo);

        RenderManager() = delete;

    private:
        static bool _isTextureFactorySet;
        static TextureInterfaceFactory _textureFactory;
        static eTextureUpdateStrategy _textureUpdateStrategy;
        static OnRenderingEvent _onRenderingEvent;
        static AnzuCustomTextureUpdateCallback_t _customTextureUpdateCallback;
        static NativeUpdate _nativeUpdate;

        static void handleTexture(TextureInfo& textureInfo);
        static bool tryScheduleTextureRender(TextureInfo& textureInfo);
    };

#pragma endregion // RenderManager
}
