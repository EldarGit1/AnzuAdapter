#include "AudioManager.h"

namespace anzu
{
    void EmptyAudioImplementation::ClearAudioBufferVector()
    {

    }

    void EmptyAudioImplementation::ResizeAudioBufferVector(int newAudioBufferSize)
    {

    }

    bool EmptyAudioImplementation::Write(int channelId)
    {
        return false;
    }

    EmptyAudioImplementation::EmptyAudioImplementation() : AudioInfoInterface()
    {

    }

    AudioInfoInterfaceRef EmptyAudioImplementation::ExampleAudioFactory(void* userdata)
    {
        return std::make_shared<EmptyAudioImplementation>();
    }

    EmptyAudioImplementation::~EmptyAudioImplementation() = default;
}
