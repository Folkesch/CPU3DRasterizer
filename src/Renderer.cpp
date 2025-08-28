#include "Renderer.h"

#include "glm/glm.hpp"


#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define max(a,b)    (((a) > (b)) ? (a) : (b))

void Renderer::Render(Scene& scene, RenderTarget& renderTarget)
{
    for (auto& model : scene.Models)
    {
        for (size_t i = 0; i < model.TriangleIndices.size(); i += 3)
        {
            glm::vec3 a = WorldToScreen(model.Vertices[model.TriangleIndices[i].vertexIdx], model.ModelTransform, scene.Cam, { renderTarget.Width, renderTarget.Height });
            glm::vec3 b = WorldToScreen(model.Vertices[model.TriangleIndices[i + 1].vertexIdx], model.ModelTransform, scene.Cam, { renderTarget.Width, renderTarget.Height });
            glm::vec3 c = WorldToScreen(model.Vertices[model.TriangleIndices[i + 2].vertexIdx], model.ModelTransform, scene.Cam, { renderTarget.Width, renderTarget.Height });
            if (a.z <= 0 || b.z <= 0 || c.z <= 0) continue;

            // triangel bounds
            // +- 1 to make sure we cover the whole triangel


            uint32_t minX = glm::floor(min(min(a.x, b.x), c.x));
            uint32_t minY = glm::floor(min(min(a.y, b.y), c.y));
            uint32_t maxX = glm::ceil(max(max(a.x, b.x), c.x));
            uint32_t maxY = glm::ceil(max(max(a.y, b.y), c.y));

            //if (minX < 0) minX = 0;
            //if (minY < 0) minY = 0;
            //if (maxX < 0) maxX = 0;
            //if (maxY < 0) maxY = 0;

            // pixel block covering the triangel bounds
            size_t blockStartX = glm::clamp<uint32_t>(minX, 0, (uint32_t)renderTarget.Width - 1);
            size_t blockStartY = glm::clamp<uint32_t>(minY, 0, (uint32_t)renderTarget.Height - 1);
            size_t blockEndX = glm::clamp<uint32_t>(maxX, 0, (uint32_t)renderTarget.Width - 1);
            size_t blockEndY = glm::clamp<uint32_t>(maxY, 0, (uint32_t)renderTarget.Height - 1);

            for (size_t y = blockStartY; y < blockEndY; y++)
            {
                for (size_t x = blockStartX; x < blockEndX; x++)
                {
                    // + 0.5 to get center of pixel and not edge of pixel
                    glm::vec2 p = { (float)x + 0.5f, (float)y + 0.5f };
                    glm::vec3 weights = {};

                    if (PointInTriangel(a, b, c, p, weights))
                    {
                        glm::vec3 depths = { a.z, b.z, c.z };
                        float depth = 1 / glm::dot(glm::vec3(1, 1, 1) / depths, weights);

                        if (depth > renderTarget.DepthBuffer[y * renderTarget.Width + x])
                            continue;

                        glm::vec2 texCoord = { 0.0f, 0.0f };
                        glm::vec3 normalDir = { 0.0f, 0.0f, 0.0f };

                        if (model.HasTexture())
                        {
                            // interpolate texture coordinates with weights and 1/z (perspective correct interpolation (I don't know what that means))
                            texCoord += model.TextureCoords[model.TriangleIndices[i + 0].textureCoordIdx] * weights.x / depths[0];
                            texCoord += model.TextureCoords[model.TriangleIndices[i + 1].textureCoordIdx] * weights.y / depths[1];
                            texCoord += model.TextureCoords[model.TriangleIndices[i + 2].textureCoordIdx] * weights.z / depths[2];
                            texCoord *= depth; // because of the 1/z interpolation (I don't know what that means)
                        }

                        if (model.HasNormals())
                        {
                            // interpolate normal vector with weights and 1/z (perspective correct interpolation (I don't know what that means))
                            normalDir += model.Normals[model.TriangleIndices[i + 0].normalIdx] * weights.x / depths[0];
                            normalDir += model.Normals[model.TriangleIndices[i + 1].normalIdx] * weights.y / depths[1];
                            normalDir += model.Normals[model.TriangleIndices[i + 2].normalIdx] * weights.z / depths[2];
                            normalDir *= depth; // because of the 1/z interpolation (I don't know what that means)

                            normalDir = normalDir * model.ModelTransform.GetRotationMat();
                        }

						renderTarget.ColurBuff[y * renderTarget.Width + x] = scene.Shader.GetPixelColor(model, texCoord, normalDir);
                        renderTarget.DepthBuffer[y * renderTarget.Width + x] = depth;
                    }

                }
            }
        }
    }
}

glm::vec3 Renderer::WorldToScreen(const glm::vec3& vertex, const Transform& transform, const Camera& Cam, const glm::vec2& numPixels)
{
    glm::vec3 vertex_world = transform.ToWorldPoint(vertex);
    glm::vec3 vertex_view = Cam.CameraTransform.ToLocalPoint(vertex_world);

    float screenHeight_world = glm::tan(glm::radians(Cam.Fov) / 2) * 2;
    float pixelPerWorldUnit = numPixels.y / screenHeight_world / vertex_view.z;

    glm::vec2 pixelOffset = glm::vec2(vertex_view.x, vertex_view.y) * pixelPerWorldUnit;
    glm::vec2 vertex_screen = (numPixels * 0.5f) + pixelOffset;
    return glm::vec3(vertex_screen.x, vertex_screen.y, vertex_view.z);
}

bool Renderer::PointInTriangel(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p, glm::vec3& weights)
{
    // if all the triangels that are being created are in a clockwise orientation, the point is on the right of all lines and in the triangel
    float areaABP = SignedTriangelArea(a, b, p);
    float areaBCP = SignedTriangelArea(b, c, p);
    float areaCAP = SignedTriangelArea(c, a, p);
    bool inTry = (areaABP >= 0.0f && areaBCP >= 0.0f && areaCAP >= 0.0f);

    // (barycentric coordinates?)
    float totalAria = (areaABP + areaBCP + areaCAP);
    float invAreaSum = 1 / totalAria;
    float weightA = areaBCP * invAreaSum;
    float weightB = areaCAP * invAreaSum;
    float weightC = areaABP * invAreaSum;
    weights = { weightA, weightB, weightC };

    return inTry && totalAria > 0.0f;
}

float Renderer::SignedTriangelArea(glm::vec2 a, glm::vec2 b, glm::vec2 p)
{
    glm::vec2 AP = p - a;
    glm::vec2 ABRotated = rotate90DegreesRight(b - a);
    return glm::dot(ABRotated, AP) / 2.0f;
}
