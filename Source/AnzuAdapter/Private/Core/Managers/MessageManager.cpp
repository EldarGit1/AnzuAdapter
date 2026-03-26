#include "MessageManager.h"

namespace anzu
{
    AnzuMiniJson::JSONParser MessageManager::JsonParser;
    Event<void1Args<const Message&>> MessageManager::OnUserMsgEvents;
    Event<void1Args<const Message&>> MessageManager::OnTextureEvents;
    Event<void1Args<const Message&>> MessageManager::OnStatusEvents;
    Event<void1Args<const Message&>> MessageManager::OnPlacementEvents;

    void MessageManager::Initialize()
    {
        Anzu_RegisterMessageCallback(onNewMessageCallback, nullptr);
    }

    void MessageManager::Uninitialize()
    {
        OnUserMsgEvents.Clear();
        OnTextureEvents.Clear();
        OnStatusEvents.Clear();
        OnPlacementEvents.Clear();
    }

    void MessageManager::onNewMessageCallback(void *userdata, const char *msg)
    {
        std::shared_ptr<JSONValue> messageValue = JsonParser.parse(msg);
        
        if (messageValue)
        {
            // All messages include type, subtype, and data fields
            // except for 'usermsg', which lacks them.
            AnzuMiniJson::JSONObject message = messageValue->getObject();
            std::string type = message["type"]->getString();
            std::string subtype = type == "usermsg" ? "" : message["subtype"]->getString();
            std::string data = message["data"]->getString();
            Message m = {type, subtype, data};
            
            if (type == "usermsg")
            {
                OnUserMsgEvents.Invoke(m);
            }
            else if (type == "texture")
            {
                OnTextureEvents.Invoke(m);
            }
            else if (type == "status")
            {
                OnStatusEvents.Invoke(m);
            }
            else if (type == "placement")
            {
                OnPlacementEvents.Invoke(m);
            }
        }
        else
        {
            Log::Error("*** Unknown JSON message: %s.", msg);
        }
    }
}
