#include "LimitsManager.h"

namespace anzu
{
    LimitsInfo LimitsManager::_limitsInfo = {0.0f, 0.0f, 180.0f};
    size_t LimitsManager::_onStatusEventId = 0;

    void LimitsManager::Initialize()
    {
        _onStatusEventId = MessageManager::OnStatusEvents.Register(onStatusEvent);
    }

    void LimitsManager::Uninitialize()
    {
        MessageManager::OnStatusEvents.Unregister(_onStatusEventId);
        _limitsInfo = {0.0f, 0.0f, 180.0f};
    }

    void LimitsManager::onStatusEvent(const Message& m)
    {
        if (m.Subtype == "limits")
        {
            std::shared_ptr<JSONValue> limitsValue = MessageManager::JsonParser.parse(m.Data);
            AnzuMiniJson::JSONObject limits = limitsValue->getObject();

            _limitsInfo.MinVisibility = (float) limits["min_visibility"]->getNumber();
            _limitsInfo.MinViewability = (float) limits["min_viewability"]->getNumber();
            _limitsInfo.MaxAngle = (float) limits["max_angle"]->getNumber();

            Log::Debug("Got limits data: %s.", m.Data.c_str());
        }
    }

    float LimitsManager::GetMinVisibility()
    {
        return _limitsInfo.MinVisibility;
    }

    float LimitsManager::GetMinViewability()
    {
        return _limitsInfo.MinViewability;
    }

    float LimitsManager::GetMaxAngle()
    {
        return _limitsInfo.MaxAngle;
    }
}
