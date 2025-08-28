#include "RenderTarget.h"

#include "SWindow.h"

#include "glm/glm.hpp"

RenderTarget::RenderTarget(size_t width, size_t height)
{
	ColurBuff = new glm::vec3[width * height];
	DepthBuffer = new float[width * height];
	Width = width;
	Height = height;

	for (size_t i = 0; i < width * height; i++)
	{
		ColurBuff[i] = { 0, 0, 0 };
		DepthBuffer[i] = FLT_MAX;
	}
}

RenderTarget::~RenderTarget()
{
	delete[] ColurBuff;
}

RenderTarget::RenderTarget(const RenderTarget& copy)
{
	ColurBuff = new glm::vec3[copy.Width * copy.Height];
	Width = copy.Width;
	Height = copy.Height;

	for (size_t i = 0; i < Width * Height; i++)
	{
		ColurBuff[i] = copy.ColurBuff[i];
	}
}

void RenderTarget::ClearBuffer()
{
	for (size_t i = 0; i < Width * Height; i++)
	{
		ColurBuff[i] = { 0, 0, 0 };
	}
}

void RenderTarget::ResizeAndClear(size_t width, size_t height)
{
	if (width == Width && height == Height)
	{
		for (size_t i = 0; i < Width * Height; i++)
		{
			ColurBuff[i] = { 0, 0, 0 };
			DepthBuffer[i] = FLT_MAX;
		}
		//size_t size = Width * Height * sizeof(DepthBuffer[0]);
		//memset(DepthBuffer, FLT_MAX, size);
	}
	else
	{
		delete[] ColurBuff;
		delete[] DepthBuffer;

		ColurBuff = new glm::vec3[width * height];
		DepthBuffer = new float[width * height];
		Width = width;
		Height = height;

		for (size_t i = 0; i < width * height; i++)
		{
			ColurBuff[i] = { 0, 0, 0 };
			DepthBuffer[i] = FLT_MAX;
		}
		//memset(DepthBuffer, FLT_MAX, sizeof(DepthBuffer[0]));
	}
}

uint8_t* RenderTarget::GetBGRAArray()
{

	uint8_t* pixelBuffer = new uint8_t[Width * Height * 4];

	for (size_t i = 0; i < Width * Height; i++)
	{
		pixelBuffer[i * 4 + 0] = ColurBuff[i].b;
		pixelBuffer[i * 4 + 1] = ColurBuff[i].g;
		pixelBuffer[i * 4 + 2] = ColurBuff[i].r;
		pixelBuffer[i * 4 + 3] = 1; // alfa
	}

	return pixelBuffer;
}
