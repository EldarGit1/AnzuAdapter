#include "EngineTexture2D.h"
#if 0
EngineTexture2D::EngineTexture2D()
        : TextureInterface(),
          _texture(nullptr)
{

}

EngineTexture2D::~EngineTexture2D()
{
    if (_texture)
    {
        if (_texture->rawData)
        {
            delete[] _texture->rawData;
            _texture->rawData = nullptr;
        }

        delete _texture;
        _texture = nullptr;
    }
}

void EngineTexture2D::Initialize()
{
    anzu::RenderManager::SetTextureFactory({
                EngineTexture2D::GetTextureFactory,
                nullptr
            });
}

void EngineTexture2D::UpdateTexture(int width,
                                    int height,
                                    void *data,
                                    int size,
                                    bool switchRB)
{
    if (_texture)
    {
        const size_t newSize = static_cast<size_t>(size);
        const size_t oldSize =
                static_cast<size_t>(_texture->width) *
                static_cast<size_t>(_texture->height) * 4u;

        bool needsAlloc = _texture->rawData == nullptr || newSize > oldSize;

        _texture->width = width;
        _texture->height = height;
        _texture->nrChannels = 4;

        if (data && size > 0)
        {
            if (needsAlloc)
            {
                // delete[] nullptr is safe
                delete[] reinterpret_cast<unsigned char*>(_texture->rawData);
                _texture->rawData = new unsigned char[size];
            }

            memcpy(_texture->rawData, data, static_cast<size_t>(size));

            if (switchRB)
            {
                auto bytes = reinterpret_cast<unsigned char*>(_texture->rawData);
                int pixelCount = width * height;
                int index = 0;

                while (index < pixelCount)
                {
                    int byteIndex = index * 4;
                    unsigned char r = bytes[byteIndex + 0];

                    bytes[byteIndex + 0] = bytes[byteIndex + 2];
                    bytes[byteIndex + 2] = r;

                    index++;
                }
            }
        }
    }
}

bool EngineTexture2D::CreateTexture(anzu::TextureInfo& textureInfo)
{
    bool isCreated = false;

    if (textureInfo.Width > 0 && textureInfo.Height > 0)
    {
        const size_t newTextureSize =
                static_cast<size_t>(textureInfo.Width) *
                static_cast<size_t>(textureInfo.Height) * 4u;

        if (_texture)
        {
            if (_texture->rawData)
            {
                delete[] _texture->rawData;
                _texture->rawData = nullptr;
            }

            delete _texture;
            _texture = nullptr;
        }

        _texture = new Texture();
        _texture->width = textureInfo.Width;
        _texture->height = textureInfo.Height;
        _texture->nrChannels = 4;
        _texture->rawData = new unsigned char[newTextureSize]();

        isCreated = true;
    }

    return isCreated;
}

void *EngineTexture2D::GetTextureHandle()
{
    return static_cast<void *>(_texture);
}

anzu::TextureInterfaceRef EngineTexture2D::GetTextureFactory(void* userdata)
{
    return std::static_pointer_cast<TextureInterface>(
            std::make_shared<EngineTexture2D>());
}
#endif