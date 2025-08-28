#include "Scene.h"

#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <stdlib.h>     // rand 
#include <time.h>       // time 
#include <format>

#include <glm/glm.hpp>

#include "SWindow.h"

#include <iostream>

void Camera::MoveForward(float movementAmount)
{
    glm::vec3 forward = glm::vec3(0, 0, 1) * CameraTransform.GetRotationMat();

    CameraTransform.Position += forward * movementAmount;
}

void Camera::MoveRight(float movementAmount)
{
    glm::vec3 forward = glm::vec3(1, 0, 0) * CameraTransform.GetRotationMat();

    CameraTransform.Position += forward * movementAmount;

}

void Camera::MoveUp(float movementAmount)
{
    glm::vec3 forward = glm::vec3(0, 1, 0) * CameraTransform.GetRotationMat();

    CameraTransform.Position += forward * movementAmount;
}

void Scene::Update(float deltaTimeSec)
{

    //Models[0].ModelTransform.Pitch += deltaTimeSec;
    if (g_Keyboard[VK_SPACE] == true) Models[0].ModelTransform.Pitch += deltaTimeSec;

    constexpr float rotateSpeed = 1.0f;
    constexpr float movmentSpeed = 3.0f;

    if (g_Keyboard[VK_UP] == true) Cam.CameraTransform.Pitch += deltaTimeSec * rotateSpeed;
    if (g_Keyboard[VK_DOWN] == true) Cam.CameraTransform.Pitch -= deltaTimeSec * rotateSpeed;
    if (g_Keyboard[VK_RIGHT] == true) Cam.CameraTransform.Yaw += deltaTimeSec * rotateSpeed;
    if (g_Keyboard[VK_LEFT] == true) Cam.CameraTransform.Yaw -= deltaTimeSec * rotateSpeed;

    if (g_Keyboard['W'] == true) Cam.MoveForward(deltaTimeSec * movmentSpeed);
    if (g_Keyboard['S'] == true) Cam.MoveForward(-deltaTimeSec * movmentSpeed);     // minus to move backward
    if (g_Keyboard['D'] == true) Cam.MoveRight(deltaTimeSec * movmentSpeed);
    if (g_Keyboard['A'] == true) Cam.MoveRight(-deltaTimeSec * movmentSpeed);       // minus to move left
    if (g_Keyboard[VK_SHIFT] == true) Cam.MoveUp(deltaTimeSec * movmentSpeed);
    if (g_Keyboard[VK_CONTROL] == true) Cam.MoveUp(-deltaTimeSec * movmentSpeed);   // minus to move left

    if (g_Keyboard['B'] == true) Models[0].ModelTransform.Yaw += 0.1f;

}


static std::string GetFolderPath(const std::string& str)
{
    int16_t firstSlashFromEndIndex = INT16_MAX;

    for (int16_t i = str.size() - 1; i >= 0; i--)
    {
        if (str[i] == '/')
        {
            firstSlashFromEndIndex = i;
            break;
        }
    }

    return str.substr(0, firstSlashFromEndIndex);
}

static bool NextCharsAre(const std::string& str, uint64_t strPos, const std::string& chars)
{
    if (strPos + chars.size() > str.size())
        return false;

    for (size_t i = 0; i < chars.size(); i++)
    {
        if (str[strPos + i] != chars[i]) return false;
    }

    return true;
}

// Returns -1 if no newline character was found
static int64_t NextLine(const std::string& str, uint64_t& strPos)
{
    while (strPos < str.size() && str[strPos] != '\n' && str[strPos] != '\r')
    {
        strPos++;
    }

    if (str[strPos] == '\r' && str[strPos + 1] == '\n')
    {
        strPos += 2; // skip both newline character
    }
    else
    {
        strPos++; // skip the newline character
    }

    if (strPos >= str.size())
        return -1; // no more lines

    return 0;
}

// returns the path to the .mtl file in the .obj file reletiv to the .obj file path
// The string is not altered if no file was found
// Returns -1 if something went wrong
static int GetOBJMetadata(const std::string& str, std::string& mtlFilePath)
{
    uint64_t strPos = 0;

    // get metadata from obj
    while (true)
    {

        if (NextCharsAre(str, strPos, "mtllib"))
        {
			uint64_t StartOfFileName = strPos + 7; // 7 is the length of "mtllib "

            while (str[strPos] != '\n' && str[strPos] != '\r' && strPos < str.size())
            {
                strPos++;
            }

            mtlFilePath = str.substr(StartOfFileName, strPos - StartOfFileName);
            if (NextLine(str, strPos) == -1)
                return 0;
        }
        else
        {
            if (NextLine(str, strPos) == -1)
                return 0;
        }
    }

    return 0;
}

static int GetOBJData(const std::string& OBJStr, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& textureCoords, std::vector<glm::vec3>& normals, std::vector<TVIndices>& triangleIndices)
{
    uint64_t strPos = 0;

    while (true)
    {

        if (NextCharsAre(OBJStr, strPos, "v "))
        {
			uint64_t endOfLine = OBJStr.find_first_of("\n\r", strPos);
            if (endOfLine == std::string::npos)
				endOfLine = OBJStr.size();

			float x, y, z;

            auto result = std::from_chars(&OBJStr[strPos + 2], &OBJStr[endOfLine], x);
            if (result.ec != std::errc())
				return -1;
            result = std::from_chars(result.ptr + 1, &OBJStr[endOfLine], y);
            if (result.ec != std::errc())
                return -1;
            result = std::from_chars(result.ptr + 1, &OBJStr[endOfLine], z);
            if (result.ec != std::errc())
                return -1;

			vertices.emplace_back(x, y, z);

            if (NextLine(OBJStr, strPos) == -1)
                return 0;
        }
        else if (NextCharsAre(OBJStr, strPos, "vt "))
        {
            uint64_t endOfLine = OBJStr.find_first_of("\n\r", strPos);
            if (endOfLine == std::string::npos)
                endOfLine = OBJStr.size();

            float x, y;

            auto result = std::from_chars(&OBJStr[strPos + 3], &OBJStr[endOfLine], x);
            if (result.ec != std::errc())
                return -1;
            result = std::from_chars(result.ptr + 1, &OBJStr[endOfLine], y);
            if (result.ec != std::errc())
                return -1;

			textureCoords.emplace_back(x, y);

            if (NextLine(OBJStr, strPos) == -1)
                return 0;
        }
        else if (NextCharsAre(OBJStr, strPos, "vn "))
        {
            uint64_t endOfLine = OBJStr.find_first_of("\n\r", strPos);
            if (endOfLine == std::string::npos)
                endOfLine = OBJStr.size();

            float x, y, z;

            auto result = std::from_chars(&OBJStr[strPos + 3], &OBJStr[endOfLine], x);
            if (result.ec != std::errc())
                return -1;
            result = std::from_chars(result.ptr + 1, &OBJStr[endOfLine], y);
            if (result.ec != std::errc())
                return -1;
            result = std::from_chars(result.ptr + 1, &OBJStr[endOfLine], z);
            if (result.ec != std::errc())
                return -1;

			normals.emplace_back(glm::normalize(glm::vec3(x, y, z)));

            if (NextLine(OBJStr, strPos) == -1)
                return 0;
        }
        else if (NextCharsAre(OBJStr, strPos, "f "))
        {
            uint64_t endOfLineIdx = OBJStr.find_first_of("\n\r", strPos);
            if (endOfLineIdx == std::string::npos)
                endOfLineIdx = OBJStr.size();
			const char* endOfLinePtr = &OBJStr[endOfLineIdx];

            strPos += 2; // move past "f " to start of number
            const char* strPtr = &OBJStr[strPos];

            std::vector<TVIndices> indices;
            std::from_chars_result result;
            do
            {
                int vertexIdx = -1, textureCoordIdx = -1, normalIdx = -1;

                result = std::from_chars(strPtr, endOfLinePtr, vertexIdx);
                if (result.ec != std::errc())
                    return -1;

                if (*result.ptr == '/')
                {
					result.ptr++; // skip the slash
                    if (*result.ptr != '/'
                     && *result.ptr != ' '
                      && result.ptr < endOfLinePtr)
                    {
                        result = std::from_chars(result.ptr, endOfLinePtr, textureCoordIdx);
                    }
                    if (result.ptr < endOfLinePtr && *result.ptr == '/')
                    {
                        if (result.ptr < endOfLinePtr && *result.ptr != ' ')
                        {
                            result = std::from_chars(result.ptr + 1, endOfLinePtr, normalIdx);
                        }
                    }
                }

                indices.push_back({ vertexIdx - 1, textureCoordIdx - 1, normalIdx - 1 }); // -1 because indices start at one in obj

                strPtr = result.ptr + 1; // skip the space after the number
            } while (strPtr < endOfLinePtr);


			// Convert shapes with more sides than triangles two triangles
            for (uint32_t i = 0; i < indices.size(); i++)
            {
                if (i >= 3)
                {
                    triangleIndices.push_back(indices[0]);
                    triangleIndices.push_back(indices[i - 1]);
                }
                triangleIndices.push_back(indices[i]);
            }

            if (NextLine(OBJStr, strPos) == -1)
                return 0;
        }
        else
        {
            if (NextLine(OBJStr, strPos) == -1)
                return 0;
        }
    }
}

static int GetMTLMetaData(const std::string& mtlFilePath, std::string& bmpFileName)
{
    std::ifstream mtl(mtlFilePath, std::ios::binary | std::ios::ate); // open in binary mode and go to the end of the file to get the size
    if (!mtl.is_open()) return -1;

    uint64_t mtlFileSize = static_cast<uint64_t>(mtl.tellg());
    mtl.seekg(0, std::ios::beg);

    std::string mtlString(mtlFileSize, '\0'); // create a string with the size of the file
    mtl.read(&mtlString[0], mtlFileSize); // read the file into the string

    mtl.close();

    uint64_t strPos = 0;

    while (true)
    {
        if (NextCharsAre(mtlString, strPos, "map_Kd "))
        {
            uint64_t StartOfFileName = strPos + 7; // 7 is the length of "map_Kd "

            while (mtlString[strPos] != '\n' && mtlString[strPos] != '\r' && strPos < mtlString.size())
            {
                strPos++;
            }

            bmpFileName = mtlString.substr(StartOfFileName, strPos - StartOfFileName);
            if (NextLine(mtlString, strPos) == -1)
                return 0;
        }
        else
        {
            if (NextLine(mtlString, strPos) == -1)
                return 0;
        }
    }
}

static int GetMTLData(const std::string& mtlFilePath, Texture& texture)
{
#pragma pack(push, 1) // no padding so the struct is exactly the size of the data in the file

    struct BmpHeader {
        char bitmapSignatureBytes[2];
        uint32_t sizeOfBitmapFile;
        uint32_t reservedBytes;
        uint32_t pixelDataOffset;
    };

    struct BmpInfoHeader {
        uint32_t sizeOfThisHeader;
        int32_t width; // = IMGWidth; // in pixels
        int32_t height; // = IMGHEIGHT; // in pixels
        uint16_t numberOfColorPlanes; // must be 1
        uint16_t colorDepth;
        uint32_t compressionMethod;
        uint32_t rawBitmapDataSize; // generally ignored
        int32_t horizontalResolution; // in pixel per meter
        int32_t verticalResolution; // in pixel per meter
        uint32_t colorTableEntries;
        uint32_t importantColors;
    };

#pragma pack(pop)

	// get the path to the .bmp file in the .mtl file reletiv to the .mtl file path
	std::string bmpFileName = "";
	if (GetMTLMetaData(mtlFilePath, bmpFileName) == -1) 
        return -1;

    if (bmpFileName.empty())
		return -1;

    // load the .bmp file ==================================================
	std::string folderPath = GetFolderPath(mtlFilePath);
	std::string bmpFilePath = folderPath + "/" + bmpFileName;

	std::ifstream bmp(bmpFilePath, std::ios::binary);
	if (!bmp.is_open()) return -1;

	// read bmp header
    BmpHeader header;
	BmpInfoHeader infoHeader;
	bmp.read((char*)&header, sizeof(header));
	bmp.read((char*)&infoHeader, sizeof(infoHeader));
	bmp.seekg(header.pixelDataOffset, std::ios::beg);

	// Get Width and Height
    texture.Width = infoHeader.width;
	texture.Height = infoHeader.height;



	// read pixel data
	std::vector<glm::u8vec3> colorVec((header.sizeOfBitmapFile - header.pixelDataOffset) / 3);
	bmp.read((char*)colorVec.data(), header.sizeOfBitmapFile - header.pixelDataOffset);

    for (size_t i = 0; i < colorVec.size(); i++)
    {
        // convert from BGR to RGB
        texture.ColorVec.push_back({ colorVec[i].b, colorVec[i].g, colorVec[i].r, 255 });
	}

	return 0;
}

int32_t Scene::LoadModelObj(std::string objFilePath)
{

	// Convert std::ifstream to a std::string ==============================================

	std::ifstream obj(objFilePath, std::ios::binary | std::ios::ate); // open in binary mode and go to the end of the file to get the size
    if (!obj.is_open()) return -1;

    uint64_t OBJFileSize = static_cast<uint64_t>( obj.tellg() );
	obj.seekg(0, std::ios::beg);

	std::string OBJString(OBJFileSize, '\0'); // create a string with the size of the file
	obj.read(&OBJString[0], OBJFileSize); // read the file into the string

	obj.close();


    // Get the path to the .mtl file in the .obj file reletiv to the .obj file path
	// The string is not altered if no file was found
    std::string mtlFileName = "";
	GetOBJMetadata(OBJString, mtlFileName);

	// Get OBJ data =========================================================

    std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoords;
	std::vector<glm::vec3> normals;
    std::vector<TVIndices> triangleIndices;

    GetOBJData(OBJString, vertices, textureCoords, normals, triangleIndices);

    std::string OBJStringData = std::format("Loaded {} vertices, {} texture coordinates, {} normals, and {} triangle indices from {}\n",
        vertices.size(), textureCoords.size(), normals.size(), triangleIndices.size(), objFilePath);

    printf(OBJStringData.c_str());

	// Try to get a texture from the .mtl file
    std::string folderPath = GetFolderPath(objFilePath);
	std::string mtlFilePath = folderPath + "/" + mtlFileName;
    Texture texture(0, 0, std::vector<glm::u8vec4>());

    GetMTLData(mtlFilePath, texture);

    Models.emplace_back(vertices, textureCoords, normals, triangleIndices, texture);
   
    // return index of new Model
    return Models.size() - 1;
    
}