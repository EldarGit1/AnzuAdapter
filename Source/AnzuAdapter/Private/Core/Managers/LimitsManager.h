#pragma once

#include "anzuSDK/include/Anzu.h"
#include "AnzuAdapter/Private/Core/Json/minijsonparse.h"
#include "AnzuAdapter/Private/Core/Types/Event.h"
#include "AnzuAdapter/Private/Core/Log/Log.h"
#include "AnzuAdapter/Private/Core/Managers/MessageManager.h"

using namespace AnzuMiniJson;

namespace anzu
{
    struct Message;
    
    struct LimitsInfo
    {
        float MinVisibility;
        float MinViewability;
        float MaxAngle;
    };

    class LimitsManager
    {
    public:
        static void Initialize();
        static void Uninitialize();
        static float GetMinVisibility();
        static float GetMinViewability();
        static float GetMaxAngle();

        LimitsManager() = delete;

    private:
        static LimitsInfo _limitsInfo;
        static size_t _onStatusEventId;

        static void onStatusEvent(const Message& m);
    };
}
