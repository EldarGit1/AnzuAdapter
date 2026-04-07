#include "EngineTexture2D.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureDefines.h"
#include "Misc/AssertionMacros.h"

EngineTexture2D::EngineTexture2D()
    : _texture(nullptr)
{
}

EngineTexture2D::~EngineTexture2D()
{
    if (_texture)
    {
        _texture->RemoveFromRoot();
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
    if (_texture && data && size > 0)
    {
        check(width > 0 && height > 0);
        check(size >= width * height * 4);

        FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(0, 0, 0, 0, width, height);
        uint8* uploadData = new uint8[size];
        FMemory::Memcpy(uploadData, data, static_cast<SIZE_T>(size));

        if (switchRB)
        {
            const int32 pixelCount = width * height;
            for (int32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
            {
                const int32 byteIndex = pixelIndex * 4;
                Swap(uploadData[byteIndex], uploadData[byteIndex + 2]);
            }
        }

        _texture->UpdateTextureRegions(
            0,
            1,
            region,
            width * 4,
            4,
            uploadData,
            [](uint8* srcData, const FUpdateTextureRegion2D* regions)
            {
                delete[] srcData;
                delete regions;
            });
    }
}

bool EngineTexture2D::CreateTexture(anzu::TextureInfo& textureInfo)
{
    bool isCreated = false;

    if (textureInfo.Width > 0 && textureInfo.Height > 0)
    {
        if (_texture)
        {
            _texture->RemoveFromRoot();
            _texture = nullptr;
        }

        _texture = UTexture2D::CreateTransient(textureInfo.Width, textureInfo.Height, PF_B8G8R8A8);

        if (_texture)
        {
            _texture->NeverStream = true;
            _texture->SRGB = true;
            _texture->Filter = TF_Bilinear;
#if WITH_EDITORONLY_DATA
            _texture->MipGenSettings = TMGS_NoMipmaps;
#endif
            _texture->UpdateResource();
            _texture->AddToRoot();
            isCreated = true;
        }
    }

    return isCreated;
}

void *EngineTexture2D::GetTextureHandle()
{
    return static_cast<void *>(_texture);
}

anzu::TextureInterfaceRef EngineTexture2D::GetTextureFactory(void*)
{
    return std::static_pointer_cast<anzu::TextureInterface>(
            std::make_shared<EngineTexture2D>());
}
