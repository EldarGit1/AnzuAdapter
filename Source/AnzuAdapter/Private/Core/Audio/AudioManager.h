#pragma once

#include <memory>

namespace anzu
{
    class AudioInfoInterface;
    using AudioInfoInterfaceRef = std::shared_ptr<AudioInfoInterface>;
    using AudioInterfaceFactoryCallback = AudioInfoInterfaceRef(*)(void* userdata);

    struct AudioInterfaceFactory
    {
        AudioInterfaceFactoryCallback callback;
        void* userdata;
    };

    class AudioInfoInterface
    {
    public:
        // For the sake of simplicity we use this buffer to fill and allocate/copy to serve sound
        virtual void ClearAudioBufferVector() = 0;
        virtual void ResizeAudioBufferVector(int newAudioBufferSize) = 0;
        virtual bool Write(int channelId) = 0;
        virtual ~AudioInfoInterface() = default;
    };

    class EmptyAudioImplementation : public AudioInfoInterface
    {
    public:
        EmptyAudioImplementation();
        ~EmptyAudioImplementation();

        void ClearAudioBufferVector() override;
        void ResizeAudioBufferVector(int newAudioBufferSize) override;
        bool Write(int channelId) override;
        static AudioInfoInterfaceRef ExampleAudioFactory(void * userdata);
    };

    class AudioManager
    {
    public:
        inline static AudioInterfaceFactory GetAudioFactory() { return {EmptyAudioImplementation::ExampleAudioFactory, nullptr}; };
        AudioManager() = delete;
    };
}
