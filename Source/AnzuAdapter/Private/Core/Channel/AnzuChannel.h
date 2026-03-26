#pragma once

#include <string>

namespace anzu
{
    enum class eMediaType
    {
        Image = 0,
        Video,
        ImageVideo,
        None
    };

    enum class eRefreshType
    {
        Dynamic = 0,
        Static // For new creative call AnzuAd resync
    };

	struct AnzuChannel
    {
        std::string ChannelName = "";
        std::string Tags = "";
        eMediaType MediaType = eMediaType::ImageVideo;
        eRefreshType RefreshType = eRefreshType::Dynamic;
        bool IsInteractive = false;
        bool IsShrinkToFit = false;
        float AspectRatio = 1.0f;
        bool IsSupportsBGRA = false;
        int AudioSampleRate = 0;
        std::string FallbackPath = "";
        float FallbackTimer = 0.0f;
    };
}
