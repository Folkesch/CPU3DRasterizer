#define _USE_MATH_DEFINES

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <math.h>
#include <chrono>


#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <glm/glm.hpp>

#include "SWindow.h"
#include "RenderTarget.h"
#include "Transform.h"
#include "Scene.h"
#include "Renderer.h"

#include <iostream>

#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define max(a,b)    (((a) > (b)) ? (a) : (b))

union float3
{
    struct { float x, y, z; };
    struct { float r, g, b; };
    float data[3];
};

struct float2
{
    float x, y;
};


void initBMPFile(std::ofstream& oFile, int width, int height)
{
    // BMP header format struct
    struct BmpHeader {
        char bitmapSignatureBytes[2] = { 'B', 'M' };
        uint32_t sizeOfBitmapFile = 0; // = 54 + IMGWidth * IMGHEIGHT * 3;
        uint32_t reservedBytes = 0;
        uint32_t pixelDataOffset = 54;
    } bmpHeader;
    bmpHeader.sizeOfBitmapFile = 54 + width * height * 3;

    struct BmpInfoHeader {
        uint32_t sizeOfThisHeader = 40;
        int32_t width = 0; // = IMGWidth; // in pixels
        int32_t height = 0; // = IMGHEIGHT; // in pixels
        uint16_t numberOfColorPlanes = 1; // must be 1
        uint16_t colorDepth = 24;
        uint32_t compressionMethod = 0;
        uint32_t rawBitmapDataSize = 0; // generally ignored
        int32_t horizontalResolution = 3780; // in pixel per meter
        int32_t verticalResolution = 3780; // in pixel per meter
        uint32_t colorTableEntries = 0;
        uint32_t importantColors = 0;
    } bmpInfoHeader;
    bmpInfoHeader.width = width;
    bmpInfoHeader.height = height;

    oFile.write(bmpHeader.bitmapSignatureBytes, 2);
    oFile.write((char*)&bmpHeader.sizeOfBitmapFile, sizeof(uint32_t));
    oFile.write((char*)&bmpHeader.reservedBytes, sizeof(uint32_t));
    oFile.write((char*)&bmpHeader.pixelDataOffset, sizeof(uint32_t));

    oFile.write((char*)&bmpInfoHeader.sizeOfThisHeader, sizeof(uint32_t));
    oFile.write((char*)&bmpInfoHeader.width, sizeof(int32_t));
    oFile.write((char*)&bmpInfoHeader.height, sizeof(int32_t));
    oFile.write((char*)&bmpInfoHeader.numberOfColorPlanes, sizeof(uint16_t));
    oFile.write((char*)&bmpInfoHeader.colorDepth, sizeof(uint16_t));
    oFile.write((char*)&bmpInfoHeader.compressionMethod, sizeof(uint32_t));
    oFile.write((char*)&bmpInfoHeader.rawBitmapDataSize, sizeof(uint32_t));
    oFile.write((char*)&bmpInfoHeader.horizontalResolution, sizeof(int32_t));
    oFile.write((char*)&bmpInfoHeader.verticalResolution, sizeof(int32_t));
    oFile.write((char*)&bmpInfoHeader.colorTableEntries, sizeof(uint32_t));
    oFile.write((char*)&bmpInfoHeader.importantColors, sizeof(uint32_t));
}

void writeImageToFile(glm::vec3 inpArray[], std::string fileName, int width, int height)
{
    std::ofstream oFile(fileName + ".bmp", std::ofstream::binary);
    initBMPFile(oFile, width, height);

    for (size_t i = 0; i < width * height; i++)
    {
        oFile.put((uint8_t)(inpArray[i].b * 255));
        oFile.put((uint8_t)(inpArray[i].g * 255));
        oFile.put((uint8_t)(inpArray[i].r * 255));
    }
    
    oFile.close();
}


void Run(Scene& scene)
{
    using namespace std::chrono_literals;

    SWindow* swindow = new SWindow();
    RenderTarget renderTarget(g_WindowWidth, g_WindowHeight);
	Renderer renderer;

    auto newTime = std::chrono::steady_clock::now();
    auto oldTime = std::chrono::steady_clock::now();
    float DeltaTimeSec = 0;

    // game loop
    bool done = false;
    while (!done)
    {
        newTime = std::chrono::steady_clock::now();
        DeltaTimeSec = std::chrono::duration<float>(newTime - oldTime).count();
        oldTime = newTime;

        if (!swindow->ProcessMessages())
        {
            done = true;
        }

        renderTarget.ResizeAndClear(g_WindowWidth, g_WindowHeight);

        scene.Update(DeltaTimeSec);

        renderer.Render(scene, renderTarget);

        
        const uint8_t* PixelBuffer = renderTarget.GetBGRAArray();
        swindow->DrawBuffer(PixelBuffer);

        delete[] PixelBuffer;

        Sleep(16);

    }

    delete swindow;

}



std::vector<glm::vec3> loadObjFile(std::string objFilePath)
{
    std::ifstream obj(objFilePath);
    std::string line;

    std::vector<glm::vec3> allPoints;
    std::vector<glm::vec3> triangelePoints;

    std::smatch match;
    std::regex pointsReg("v ([-+\.0-9]+) ([-+\.0-9]+) ([-+\.0-9]+)");
    std::regex triangelePointsReg("(\\d+)\\/\\d+\\/\\d+");

    while (std::getline(obj, line))
    {
        if (std::regex_search(line, match, pointsReg))
        {
            allPoints.emplace_back(std::stof(match[1]), std::stof(match[2]), std::stof(match[3]));
        }
        else if (line.starts_with("f"))
        {
            std::vector<uint32_t> indices;
            while (std::regex_search(line, match, triangelePointsReg))
            {
                indices.emplace_back(std::stoi(match[1]));
                line = match.suffix().str();
            }

            for (uint32_t i = 0; i < indices.size(); i++)
            {
                uint32_t pointIndex = indices[i] - 1; // -1 because indeces start att one in obj

                if (i >= 3)
                {
                    triangelePoints.push_back(allPoints[indices[0] - 1]);
                    triangelePoints.push_back(allPoints[indices[i - 1] - 1]);
                }
                triangelePoints.push_back(allPoints[pointIndex]);
            }
        }
    }

    return triangelePoints;
}



int main()
{
    // seed random
    srand(time(NULL));

    Scene scene;

    //uint32_t monkeyIndex = scene.LoadModelObj("../../models/blender_monkey.obj");
    int32_t cubeIndex = scene.LoadModelObj("../../models/cube.obj");
    //int32_t cottageIndex = scene.LoadModelObj("../../models/85-cottage_obj/cottage_obj.obj");
    //int32_t dragonIndex = scene.LoadModelObj("../../models/SimpleDragon.obj");

    //scene.Models[monkeyIndex].ModelTransform.Position = { 0, -2.0f, 5.0f };
    //scene.Models[cottageIndex].ModelTransform.Position = { 0, 0.0f, 100.0f };
    //scene.Models[monkeyIndex].ModelTransform.Yaw = 3.14f;

    scene.Models[cubeIndex].ModelTransform.Position = { 0.0, 0.0f, 7.0f };

    Run(scene);

}