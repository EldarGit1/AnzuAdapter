#include "AnzuCore.h"

namespace anzu
{
    eAnzuState AnzuCore::_anzuState = eAnzuState::Uninitialized;
    bool AnzuCore::_shouldPauseSDK = false;

    // Events
    Event<void0Args> AnzuCore::OnLogicReady;
    Event<void1Args<const std::string&>> AnzuCore::OnSessionEvent;

    // Event ids
    size_t AnzuCore::_onStatusEventId = 0;

    void AnzuCore::Initialize(const AdapterConfig &adapterConfig)
    {
        if (_anzuState != eAnzuState::Uninitialized)
        {
            Log::Info("Cannot 'Initialize' Anzu if "
                      "state is not 'Uninitialized'.");
        }
        else
        {
            setState(eAnzuState::Initializing);
            MessageManager::Initialize();
            _onStatusEventId = MessageManager::OnStatusEvents.Register(onStatusEvent);
            LimitsManager::Initialize();
            RenderManager::Initialize();
            ChannelManager::Initialize();

            Anzu_InternalDebugging(
                    (void *) (0xc0de5afe + 61),
                    (void *) adapterConfig.DisableCustomTabs);

            Anzu_Initialize(
                    adapterConfig.AppId,
                    adapterConfig.AppKey,
                    adapterConfig.IsDevMode);

            DisableIDFAPopupOnSDKInitForIOS(
                    adapterConfig.DisableIDFAPopupOnSDKInit);

            Anzu_ApplicationActive(true);
        }
    }

    void AnzuCore::Uninitialize()
    {
        if (_anzuState != eAnzuState::Uninitialized)
        {
            setState(eAnzuState::Uninitializing);
            ChannelManager::Uninitialize();
            RenderManager::Uninitialize();
            LimitsManager::Uninitialize();

            Anzu_ApplicationQuit();
        }
        else
        {
            Log::Debug("Cannot 'Destroy' Anzu if state "
                       "is already 'Uninitialized'.");
        }
    }

    void AnzuCore::Pause()
    {
        if (_anzuState == eAnzuState::Active)
        {
            _shouldPauseSDK = false;
            Anzu_ApplicationActive(false);
            setState(eAnzuState::Paused);
            Log::Debug("Anzu paused.");
        }
        else if (_anzuState == eAnzuState::Initializing)
        {
            _shouldPauseSDK = true;
        }
    }

    void AnzuCore::Resume()
    {
        if (_anzuState == eAnzuState::Paused)
        {
            _shouldPauseSDK = false;
            Anzu_ApplicationActive(true);
            setState(eAnzuState::Active);
            Log::Debug("Anzu resumed.");
        }
        else if (_anzuState == eAnzuState::Initializing)
        {
            _shouldPauseSDK = false;
        }
    }

    void AnzuCore::Update(float deltaTime)
    {
        // We compare to support fallback media if
        // the init was not successful yet
        bool isRunning =
                _anzuState == eAnzuState::Active ||
                _anzuState == eAnzuState::Initializing;

        if (isRunning)
        {
            ChannelManager::Update(deltaTime);
        }
    }

    void AnzuCore::SetGDPRConsent(bool hasConsent, const std::string& consentString)
    {
        std::string consent = consentString.empty() ? "" : consentString;
        Anzu_SetGDPRConsent(hasConsent, consent.c_str());
    }

    void AnzuCore::DisableIDFAPopupOnSDKInitForIOS(bool disableIDFARequestOnIOS14)
    {
#if TARGET_OS_IOS
        Anzu__Internal__iOS14__AllowAdvertisingIdentifierEnable(!disableIDFARequestOnIOS14);
#endif
    }

    void AnzuCore::setState(eAnzuState newState)
    {
        Log::Debug("State changed from '%d' to '%d'.",
                   _anzuState, newState);
        _anzuState = newState;
    }

    void AnzuCore::onSDKInitialized()
    {
        // In case a user goes to background while initializing
        if (_shouldPauseSDK)
        {
            Anzu_ApplicationActive(false);
            setState(eAnzuState::Paused);
            _shouldPauseSDK = false;
        }
        else
        {
            setState(eAnzuState::Active);
        }
    }

    void AnzuCore::onSDKUninitialized()
    {
        setState(eAnzuState::Uninitialized);
        MessageManager::OnStatusEvents.Unregister(_onStatusEventId);
        MessageManager::Uninitialize();
    }

    void AnzuCore::onStatusEvent(const Message &m)
    {
        if (m.Subtype.empty())
        {
            Log::Error("[Status] *** Missing status subtype.");
        }
        else
        {
            if (m.Subtype == "initialize")
            {
                // Data should always be equal to 'ok' in this message
                if (m.Data == "ok")
                {
                    Log::Info("[Status][Initialize] Initializing "
                              "Anzu SDK...");
                }
                else
                {
                    Log::Error("[Status][Initialize] Got initialize"
                               " status: %s.", m.Data.c_str());
                }
            }
            else if (m.Subtype == "config")
            {
                // L2 refers to a secondary (Level 2) configuration URL used for ad logic
                // Data can be:
                // - none  : if no L2 URL is provided
                // - load  : currently loading L2 configuration
                // - init  : preparing L2-based configuration
                // - ready : L2 configuration processed, logic started
                // - error : retrieving L2 config from server (will retry)

                if (m.Data == "load")
                {
                    Log::Info("[Status][Config-1] Loading L2 configuration.");
                }
                else if (m.Data == "init")
                {
                    // No action needed during initialization stage
                    Log::Info("[Status][Config-2] Preparing L2 configuration.");
                }
                else if (m.Data == "ready")
                {
                    Log::Info("[Status][Config-3] L2 configuration "
                              "processed, logic started.");

                    onSDKInitialized();
                    OnLogicReady.Invoke();
                }
                else
                {
                    Log::Info("[Status] SDK has successfully received the"
                              " server configuration. Got config status: %s.",
                               m.Data.c_str());
                }
            }
            else if (m.Subtype == "limits")
            {
                // Taking care of in LimitsManager
            }
            else if (m.Subtype == "error")
            {
                Log::Error("[Status][Error] Got error data: %s.",
                           m.Data.c_str());
            }
            else if (m.Subtype == "session")
            {
                OnSessionEvent.Invoke(m.Data);
                Log::Info("[Status][Session] Got session data: %s.",
                          m.Data.c_str());
            }
            else if (m.Subtype == "uninitialize")
            {
                Log::Info("[Status][Uninitialize] Uninitializing "
                          "Anzu SDK...");
                onSDKUninitialized();
            }
            else if (m.Subtype == "regulations")
            {
                Log::Info("[Status][Regulations] Got regulations "
                          "data: %s.", m.Data.c_str());
            }
        }
    }
}
