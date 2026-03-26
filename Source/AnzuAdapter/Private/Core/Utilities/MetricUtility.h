#pragma once

#include <string>

#include "anzuSDK/include/Anzu.h"

namespace anzu
{
    enum class eMetricOperation
    {
        Set = 0,
        Add,
        Sub,
        Avg,
        Str,
        Date
    };

    enum class eMetricHashAlgorithm
    {
        SHA1 = 0,
        SHA256
    };
    
    class MetricUtility
    {
    public:
        static bool IsMetricKeyExists(const std::string& name);
        static float SetMetric(const std::string& name, float value, eMetricOperation operation);
        static float SetSystemMetric(const std::string& name, float value, eMetricOperation operation);
        static void SetMetricString(const std::string& name, const std::string& value, eMetricOperation operation = eMetricOperation::Str);
        static void SetSystemMetricString(const std::string& name, const std::string& value, eMetricOperation operation = eMetricOperation::Str);
        static void SetHashedMetricString(const std::string& name, const std::string& value, eMetricHashAlgorithm hashAlgorithm = eMetricHashAlgorithm::SHA256);
        static float GetMetric(const std::string& name);
        static float GetCampaignMetric(const std::string& campaignId, const std::string& name);
        static float GetSystemMetric(const std::string& name);
        static std::string GetMetricString(const std::string& name);
        static std::string GetCampaignMetricString(const std::string& CampaignId, const std::string& name);
        static std::string GetSystemMetricString(const std::string& name);

        MetricUtility() = delete;
    };
}
