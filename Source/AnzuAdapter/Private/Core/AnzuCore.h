#pragma once

#include "AnzuAdapter/Private/Core/Audio/AudioManager.h"
#include "AnzuAdapter/Private/Core/Channel/ChannelManager.h"
#include "AnzuAdapter/Private/Core/Log/Log.h"
#include "AnzuAdapter/Private/Core/Managers/MessageManager.h"
#include "AnzuAdapter/Private/Core/Render/RenderManager.h"

#if _WINDOWS
#define ANZU_NOOP __noop
#else // if TARGET_PLATFORM in [ANDROID, IOS, MAC, LINUX]
#define ANZU_NOOP (void)0
#endif

#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IOS
extern "C" void Anzu__Internal__iOS14__AllowAdvertisingIdentifierEnable (bool isAllowed);
#endif
#endif

// WARNING!!! You must define your OS type in preprocessor definition
// _WINDOWS | ANDROID | __arm__ | __APPLE__ | TARGET_OS_IOS |
// See "Platform specific calling convention" in Anzu.h, Line ~18
// Failing to do so, will result errors in anzu.h

#define ANZU_NATIVE_ADAPTER_VERSION_STR "2.00"

namespace anzu
{
    enum class eAnzuState
    {
        Uninitialized   = 0,
        Initializing    = 1,
        Active          = 2,
        Paused          = 3,
        Uninitializing  = 4,
    };

    struct AdapterConfig
    {
        const char* AppKey;
        const char* AppId;
        bool IsDevMode;
        bool DisableCustomTabs;         // Android
        bool DisableIDFAPopupOnSDKInit; // IOS
    };

    struct Message;

    class AnzuCore
    {
    public:
        // Events
        static Event<void0Args> OnLogicReady;
        static Event<void1Args<const std::string&>> OnSessionEvent;

        static void Initialize(const AdapterConfig& adapterConfig);
        static void Uninitialize();
        static void Pause();
        static void Resume();
        static void Update(float deltaTime);
        static void SetGDPRConsent(bool hasConsent, const std::string& consentString = nullptr);
        static void DisableIDFAPopupOnSDKInitForIOS(bool disableIDFARequestOnIOS14 = false);
        inline static eAnzuState GetState() { return _anzuState; }

        AnzuCore() = delete;

    private:
        static eAnzuState _anzuState;
        static bool _shouldPauseSDK;

        // Event ids
        static size_t _onStatusEventId;

        static void setState(eAnzuState newState);
        static void onSDKInitialized();
        static void onSDKUninitialized();
        static void onStatusEvent(const Message& m);
    };
}
