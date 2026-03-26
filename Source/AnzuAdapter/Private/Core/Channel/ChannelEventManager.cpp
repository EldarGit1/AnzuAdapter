#include "ChannelEventManager.h"

namespace anzu
{
    Event<func1Args<int>> ChannelEventManager::OnChannelEmpty;
    Event<func1Args<int>> ChannelEventManager::OnPlaybackInit;
    Event<func1Args<int>> ChannelEventManager::OnChannelImpression;
    Event<func1Args<int>> ChannelEventManager::OnPlaybackComplete;
}
