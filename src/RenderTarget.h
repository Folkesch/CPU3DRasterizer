#pragma once
#include <glm/glm.hpp>

class RenderTarget
{
public:
    RenderTarget(size_t width, size_t height);

    ~RenderTarget();

    RenderTarget(const RenderTarget& copy);

    void ClearBuffer();

    void ResizeAndClear(size_t width, size_t height);

    uint8_t* GetBGRAArray();

    glm::vec3* ColurBuff = nullptr;
    float* DepthBuffer = nullptr;

    size_t Width;
    size_t Height;
};