#include "MetricUtility.h"

namespace anzu
{
    bool MetricUtility::IsMetricKeyExists(const std::string &name)
    {
        return Anzu_MetricKeyExists(name.c_str());
    }

    float MetricUtility::SetMetric(const std::string& name, float value, eMetricOperation operation)
    {
        return Anzu_MetricSet(name.c_str(), value, static_cast<Anzu_MetricOperation_t>(operation));
    }

    float MetricUtility::SetSystemMetric(const std::string& name, float value, eMetricOperation operation)
    {
        return Anzu_SystemMetricSet(name.c_str(), value, static_cast<Anzu_MetricOperation_t>(operation));
    }

    void MetricUtility::SetMetricString(const std::string& name, const std::string& value, eMetricOperation operation)
    {
        Anzu_MetricSetS(name.c_str(),
                        value.c_str(),
                        static_cast<Anzu_MetricOperation_t>(operation));
    }

    void MetricUtility::SetSystemMetricString(const std::string& name, const std::string& value, eMetricOperation operation)
    {
        Anzu_SystemMetricSetS(name.c_str(),
                              value.c_str(),
                              static_cast<Anzu_MetricOperation_t>(operation));
    }

    void MetricUtility::SetHashedMetricString(const std::string& name, const std::string& value, eMetricHashAlgorithm hashAlgorithm)
    {
        Anzu_HashedMetricSetS(name.c_str(),
                              value.c_str(),
                              static_cast<Anzu_MetricHashAlgorithm_t>(hashAlgorithm));
    }

    float MetricUtility::GetMetric(const std::string& name)
    {
        return Anzu_MetricGet(name.c_str());
    }

    float MetricUtility::GetCampaignMetric(const std::string& campaignId, const std::string& name)
    {
        return Anzu_CampaignMetricGet(campaignId.c_str(), name.c_str());
    }

    float MetricUtility::GetSystemMetric(const std::string& name)
    {
        return Anzu_SystemMetricGet(name.c_str());
    }
    
    std::string MetricUtility::GetMetricString(const std::string& name)
    {
        std::string retValue;
        const char* freeMe = Anzu_MetricGetS(name.c_str());

        if (freeMe)
        {
            retValue = freeMe;
            Anzu_FreeString((void*)freeMe);
        }

        return retValue;
    }

    std::string MetricUtility::GetCampaignMetricString(const std::string& campaignId, const std::string& name)
    {
        std::string retValue;
        const char* freeMe = Anzu_CampaignMetricGetS(campaignId.c_str(), name.c_str());

        if (freeMe)
        {
            retValue = freeMe;
            Anzu_FreeString((void*)freeMe);
        }

        return retValue;
    }

    std::string MetricUtility::GetSystemMetricString(const std::string& name)
    {
        std::string retValue;
        const char* freeMe = Anzu_SystemMetricGetS(name.c_str());

        if (freeMe)
        {
            retValue = freeMe;
            Anzu_FreeString((void*)freeMe);
        }

        return retValue;
    }
}
