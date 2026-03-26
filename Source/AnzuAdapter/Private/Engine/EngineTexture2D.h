#pragma once

#if 0 

#include "Texture.h"
#include "Renderer.h"

#include "AnzuAdapter/Private/Core/Render/RenderManager.h"

class EngineTexture2D : public anzu::TextureInterface
{
public:
    static void Initialize();
    virtual void UpdateTexture(int width, int height, void* data, int size, bool switchRB) override;
    virtual bool CreateTexture(anzu::TextureInfo& textureInfo) override;
    virtual void* GetTextureHandle() override;
    static anzu::TextureInterfaceRef GetTextureFactory(void*);

    EngineTexture2D();
    virtual ~EngineTexture2D();

private:
    Texture* _texture;
};
#endif