#pragma once

#include "AnzuAdapter/Private/Core/Types/Event.h"
#include "AnzuAdapter/Private/Core/Types/Types.h"

namespace anzu
{
    class ChannelEventManager
    {
    public:
        static Event<func1Args<int>> OnChannelEmpty;
        static Event<func1Args<int>> OnPlaybackInit;
        static Event<func1Args<int>> OnChannelImpression;
        static Event<func1Args<int>> OnPlaybackComplete;

        ChannelEventManager() = delete;
    };
}
