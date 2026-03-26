#pragma once

#include "anzuSDK/include/Anzu.h"
#include "AnzuAdapter/Private/Core/Json/minijsonparse.h"
#include "AnzuAdapter/Private/Core/Types/Event.h"
#include "AnzuAdapter/Private/Core/Types/Types.h"
#include "AnzuAdapter/Private/Core/Log/Log.h"
#include "AnzuAdapter/Private/Core/Managers/LimitsManager.h"

#include <string>

using namespace AnzuMiniJson;

namespace anzu
{
    struct Message
    {
        std::string Type;
        std::string Subtype;
        std::string Data;
    };

    class MessageManager
    {
    public:
        static AnzuMiniJson::JSONParser JsonParser;
        static Event<void1Args<const Message&>> OnUserMsgEvents;
        static Event<void1Args<const Message&>> OnTextureEvents;
        static Event<void1Args<const Message&>> OnStatusEvents;
        static Event<void1Args<const Message&>> OnPlacementEvents;
        
        static void Initialize();
        static void Uninitialize();
        
        MessageManager() = delete;
        
    private:
        static void ANZU_CALLCONV onNewMessageCallback(void* userdata, const char* msg);
    };
}
