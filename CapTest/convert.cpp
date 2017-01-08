////////////////////////////////////////////////////////////////////////////
//
//
//    Project     : VideoNet version 1.1.
//    Description : Peer to Peer Video Conferencing over the LAN.
//	  Author      :	Nagareshwar Y Talekar ( nsry2002@yahoo.co.in)
//    Date        : 15-6-2004.
//
//
//    File description : 
//    Name    : convert.cpp
//    Details : Conversion routine from RGB24 to YUV420 & YUV420 to RGB24.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "convert.h"

#define LIMIT(x) (unsigned char) (((x > 0xffffff) ? 0xff0000 : ((x <= 0xffff) ? 0 : x & 0xff0000)) >> 16)
//RGB<->YUV420
#define rgbtoyuv(b, g, r, y, u, v) \
	y=(BYTE)(((int)30*r  +(int)59*g +(int)11*b)/100); \
	u=(BYTE)(((int)-17*r  -(int)33*g +(int)50*b+12800)/100); \
	v=(BYTE)(((int)50*r  -(int)42*g -(int)8*b+12800)/100);

void RGBtoYUV420P(const BYTE * rgb,
				  BYTE * yuv,
				  int width,
				  int height,
				  unsigned rgbIncrement,
				  bool flip) 
{
	const unsigned planeSize = width*height;
	const unsigned halfWidth = width >> 1;

	// get pointers to the data
	BYTE * yplane  = yuv;
	BYTE * uplane  = yuv + planeSize;
	BYTE * vplane  = yuv + planeSize + (planeSize >> 2);
	const BYTE * rgbIndex = rgb;

	for (int y = 0; y < height; y++) {
		BYTE * yline  = yplane + (y * width);
		BYTE * uline  = uplane + ((y >> 1) * halfWidth);
		BYTE * vline  = vplane + ((y >> 1) * halfWidth);

		if (flip)
			rgbIndex = rgb + (width*(height-1-y)*rgbIncrement);

		for (int x = 0; x < width; x+=2) {
			rgbtoyuv(rgbIndex[0], rgbIndex[1], rgbIndex[2],*yline, *uline, *vline);
			rgbIndex += rgbIncrement;
			yline++;
			rgbtoyuv(rgbIndex[0], rgbIndex[1], rgbIndex[2],*yline, *uline, *vline);
			rgbIndex += rgbIncrement;
			yline++;
			uline++;
			vline++;
		}
	}

}

void YUV420PtoRGB(const BYTE * srcFrameBuffer,
				  BYTE * dstFrameBuffer,
				  int width,
				  int height,
				  bool     flipVertical) 
{

	unsigned int   size  = width*height;
	const BYTE * yplane  = srcFrameBuffer;
	const BYTE * uplane  = srcFrameBuffer + size;
	const BYTE * vplane  = srcFrameBuffer + size + (size >> 2);
  int stride_y = width; int stride_uv = width/2;
  YUV420PtoRGB(yplane, stride_y, uplane, stride_uv, vplane, stride_uv, 
    dstFrameBuffer, width, height, flipVertical);
}

void YUV420PtoRGB(
  const BYTE * yplane, int stride_y,
  const BYTE * uplane, int stride_u,
  const BYTE * vplane, int stride_v,
  BYTE * dstFrameBuffer, int width, int height,
  bool flipVertical)
{

  unsigned int   size = width*height;

  int   x, y;

  for (y = 0; y < height; y += 2) {

    const BYTE * yline = yplane + (y * stride_y);
    const BYTE * yline2 = yline + stride_y;
    const BYTE * uline = uplane + ((y >> 1) * stride_u);
    const BYTE * vline = vplane + ((y >> 1) * stride_v);

    BYTE * rgb;
    BYTE * rgb2;
    if (flipVertical)
    {
      rgb = dstFrameBuffer + (height - y - 1)*width * 3;
      rgb2 = dstFrameBuffer + (height - y - 2)*width * 3;
    }
    else
    {
      rgb = dstFrameBuffer + y*width * 3;
      rgb2 = dstFrameBuffer + (y + 1)*width * 3;
    }

    for (x = 0; x < width; x += 2) {
      long Cr = *uline++ - 128;     // calculate once for 4 pixels
      long Cb = *vline++ - 128;
      long lrc = 104635 * Cb;
      long lgc = -25690 * Cr + -53294 * Cb;
      long lbc = 132278 * Cr;

      if (TRUE)
      {
        long tmp;     // exchange red component and blue component
        tmp = lrc;
        lrc = lbc;
        lbc = tmp;
      }

      long Y = *yline++ - 16;      // calculate for every pixel
      if (Y < 0)
        Y = 0;
      long l = 76310 * Y;
      long lr = l + lrc;
      long lg = l + lgc;
      long lb = l + lbc;

      *rgb++ = LIMIT(lr);
      *rgb++ = LIMIT(lg);
      *rgb++ = LIMIT(lb);

      Y = *yline++ - 16;       // calculate for every pixel
      if (Y < 0)
        Y = 0;
      l = 76310 * Y;
      lr = l + lrc;
      lg = l + lgc;
      lb = l + lbc;

      *rgb++ = LIMIT(lr);
      *rgb++ = LIMIT(lg);
      *rgb++ = LIMIT(lb);

      Y = *yline2++ - 16;     // calculate for every pixel
      if (Y < 0)
        Y = 0;
      l = 76310 * Y;
      lr = l + lrc;
      lg = l + lgc;
      lb = l + lbc;

      *rgb2++ = LIMIT(lr);
      *rgb2++ = LIMIT(lg);
      *rgb2++ = LIMIT(lb);

      Y = *yline2++ - 16;      // calculate for every pixel
      if (Y < 0)
        Y = 0;
      l = 76310 * Y;
      lr = l + lrc;
      lg = l + lgc;
      lb = l + lbc;

      *rgb2++ = LIMIT(lr);
      *rgb2++ = LIMIT(lg);
      *rgb2++ = LIMIT(lb);
    }

  }

}

const float YCbCrYRF = 0.299F;
const float YCbCrYGF = 0.587F;
const float YCbCrYBF = 0.114F;
const float YCbCrCbRF = -0.168736F;
const float YCbCrCbGF = -0.331264F;
const float YCbCrCbBF = 0.500000F;
const float YCbCrCrRF = 0.500000F;
const float YCbCrCrGF = -0.418688F;
const float YCbCrCrBF = -0.081312F;

const float RGBRYF = 1.00000F;
const float RGBRCbF = 0.0000F;
const float RGBRCrF = 1.40200F;
const float RGBGYF = 1.00000F;
const float RGBGCbF = -0.34414F;
const float RGBGCrF = -0.71414F;
const float RGBBYF = 1.00000F;
const float RGBBCbF = 1.77200F;
const float RGBBCrF = 0.00000F;

const int Shift = 20;
const int HalfShiftValue = 1 << (Shift - 1);

const int YCbCrYRI = (int)(YCbCrYRF * (1 << Shift) + 0.5);
const int YCbCrYGI = (int)(YCbCrYGF * (1 << Shift) + 0.5);
const int YCbCrYBI = (int)(YCbCrYBF * (1 << Shift) + 0.5);
const int YCbCrCbRI = (int)(YCbCrCbRF * (1 << Shift) + 0.5);
const int YCbCrCbGI = (int)(YCbCrCbGF * (1 << Shift) + 0.5);
const int YCbCrCbBI = (int)(YCbCrCbBF * (1 << Shift) + 0.5);
const int YCbCrCrRI = (int)(YCbCrCrRF * (1 << Shift) + 0.5);
const int YCbCrCrGI = (int)(YCbCrCrGF * (1 << Shift) + 0.5);
const int YCbCrCrBI = (int)(YCbCrCrBF * (1 << Shift) + 0.5);

const int RGBRYI = (int)(RGBRYF * (1 << Shift) + 0.5);
const int RGBRCbI = (int)(RGBRCbF * (1 << Shift) + 0.5);
const int RGBRCrI = (int)(RGBRCrF * (1 << Shift) + 0.5);
const int RGBGYI = (int)(RGBGYF * (1 << Shift) + 0.5);
const int RGBGCbI = (int)(RGBGCbF * (1 << Shift) + 0.5);
const int RGBGCrI = (int)(RGBGCrF * (1 << Shift) + 0.5);
const int RGBBYI = (int)(RGBBYF * (1 << Shift) + 0.5);
const int RGBBCbI = (int)(RGBBCbF * (1 << Shift) + 0.5);
const int RGBBCrI = (int)(RGBBCrF * (1 << Shift) + 0.5);

void YCbCrToRGB(BYTE* From, BYTE* To, int length)
{
	if (length < 1) return;
	int Red, Green, Blue;
	int Y, Cb, Cr;
	int i, offset;
	for (i = 0; i < length; i++)
	{
		offset = i * 3;
		Y = From[offset]; Cb = From[offset + 1] - 128; Cr = From[offset + 2] - 128;
		Red = Y + ((RGBRCrI * Cr + HalfShiftValue) >> Shift);
		Green = Y + ((RGBGCbI * Cb + RGBGCrI * Cr + HalfShiftValue) >> Shift);
		Blue = Y + ((RGBBCbI * Cb + HalfShiftValue) >> Shift);
		if (Red > 255) Red = 255; else if (Red < 0) Red = 0;
		if (Green > 255) Green = 255; else if (Green < 0) Green = 0;
		if (Blue > 255) Blue = 255; else if (Blue < 0) Blue = 0;
		//offset = i << 2;
		To[offset] = (BYTE)Blue;
		To[offset + 1] = (BYTE)Green;
		To[offset + 2] = (BYTE)Red;
		//To[offset + 3] = 0xff;
	}
}

void RGBToYCbCr(BYTE* From, BYTE* To, int length)
{
	if (length < 1) return;
	int Red, Green, Blue;
	int i, offset;
	for (i = 0; i < length; i++)
	{
		offset = i * 3;
		Blue = From[offset]; Green = From[offset + 1]; Red = From[offset + 2];
		//offset = (i << 1) + i;
		To[offset] = (BYTE)((YCbCrYRI * Red + YCbCrYGI * Green + YCbCrYBI * Blue + HalfShiftValue) >> Shift);
		To[offset + 1] = (BYTE)(128 + ((YCbCrCbRI * Red + YCbCrCbGI * Green + YCbCrCbBI * Blue + HalfShiftValue) >> Shift));
		To[offset + 2] = (BYTE)(128 + ((YCbCrCrRI * Red + YCbCrCrGI * Green + YCbCrCrBI * Blue + HalfShiftValue) >> Shift));
	}
}
