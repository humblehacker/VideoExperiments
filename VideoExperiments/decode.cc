//
//  decode
//  VideoExperiments
//
//  Created by david on 7/28/13.
//  Copyright (c) 2013 David Whetstone All rights reserved.
//

#include "decode.hh"

RGBA __unused allBlue(int, int, int);
RGBA __unused allGreen(int, int, int);
RGBA __unused allRed(int, int, int);
RGBA __unused decodeYUVtoRGB(int Y, int U, int V);
RGBA __unused decodeYPbPrToRGB(int Y, int Pb, int Pr);
RGBA __unused decodeYCbCrToRGB(int Y, int Cb, int Cr);
RGBA __unused decodeYCbCrToRGBAlt(int Y, int Cb, int Cr);
RGBA __unused decodeYPbPRToRGB_HDTV(int Y, int Pb, int Pr);
RGBA __unused decodeYCbCrToRGB_HDTV(int Y, int Cb, int Cr);
RGBA __unused decodeYCbCrToRGB_HDTVAlt(int Y, int Cb, int Cr);
RGBA __unused testConvertYCbCrToRGBAlt(byte Y, byte Cb, byte Cr);

// This function originally from:
// https://code.google.com/p/ketai/source/browse/trunk/ketai/src/ketai/camera/KetaiCamera.java
// It has been annotated and made less terse in order to understand the optimizations made.
void decodeYUV420spNV21ToRGBA_optimized(ByteVector const & input, int width, int height, RGBAVector &rgb)
{
    int const size = width * height;

    RGBAVector::iterator rgbIterator = rgb.begin();

    int yIndex = 0;
    int uvPlaneOffset = size;

    for (int row = 0; row < height; row++)
    {
        const int uvRow = row >> 1;
        int uvIndex = uvPlaneOffset + uvRow * width;
        int u = 0, v = 0;

        for (int col = 0; col < width; col++, yIndex++)
        {
            int y = (0xff & ((int) input[yIndex])) - 16;
            if (y < 0)
                y = 0;

            bool const columnIsOdd = (col & 1) == 0;
            if (columnIsOdd)
            {
                v = (0xff & input[uvIndex++]) - 128;
                u = (0xff & input[uvIndex++]) - 128;
            }

            // Optimization: To avoid floating point math, all floating point values
            // are shifted left by 10 and truncated to integer values.

            // 1192 = 1.164*2<<10
            // 1634 = 1.596*2<<10
            //  833 = 0.813*2<<10
            //  400 = 0.366*2<<10
            // 2066 = 1.732*2<<10
            // 262143 = 256*2<<10

            int y1192 = 1192 * y;
            int r = (y1192 + 1634 * v);
            int g = (y1192 - 833 * v - 400 * u);
            int b = (y1192 + 2066 * u);

            // clamp values between 0 and 256*2<<10
            if (r < 0)
                r = 0;
            else if (r > 262143)
                r = 262143;

            if (g < 0)
                g = 0;
            else if (g > 262143)
                g = 262143;

            if (b < 0)
                b = 0;
            else if (b > 262143)
                b = 262143;

            // Unpacking the optimization: results are shifted right by 10 and truncated to 8bits
            *(rgbIterator+yIndex) = { .red=(byte)(r>>10), .green=(byte)(g>>10), .blue=(byte)(b>>10), .alpha=0xff };
        }
    }
}

RGBAVectorPtr __unused decodeYUV420spNV21ToRGBA_optimized(ByteVector const & input, int width, int height)
{
    RGBAVectorPtr rgb(new RGBAVector(width*height));

    decodeYUV420spNV21ToRGBA_optimized(input, width, height, *rgb);

    return rgb;
}


RGBAVectorPtr __unused decodeYUV420spNV21ToRGBA(ByteVector const & input, int width, int height)
{
    RGBAVectorPtr rgb(new RGBAVector(width*height));

    decodeYUV420spNV21ToRGBA(input, width, height, *rgb);

    return rgb;
}

void decodeYUV420spNV21ToRGBA(ByteVector const & input, int width, int height, RGBAVector &rgb)
{
    int size = width * height;

    RGBAVector::iterator rgbIterator = rgb.begin();

    ByteVector::const_iterator lumaPlane = input.begin();
    ByteVector::const_iterator chromaPlane = input.begin() + size;

    for (int i = 0, k = 0; i < size; i += 2, k += 2)
    {
        int Y1 = 0xff & ((int)*(lumaPlane + i));
        int Y2 = 0xff & ((int)*(lumaPlane + i + 1));
        int Y3 = 0xff & ((int)*(lumaPlane + width + i));
        int Y4 = 0xff & ((int)*(lumaPlane + width + i + 1));

        int Cb = 0xff & *(chromaPlane + k);
        int Cr = 0xff & *(chromaPlane + k + 1);

        RGBA (*decode)(int, int, int);

        // decode = allGreen;
        // decode = decodeYUVtoRGB;
        // decode = decodeYPbPrToRGB;
        // decode = decodeYCbCrToRGB;
        decode = decodeYCbCrToRGBAlt;
        // decode = decodeYPbPRToRGB_HDTV;
        // decode = decodeYCbCrToRGB_HDTV;
        // decode = decodeYCbCrToRGB_HDTVAlt;

        *(rgbIterator + i) = decode(Y1, Cb, Cr);
        *(rgbIterator + i + 1) = decode(Y2, Cb, Cr);
        *(rgbIterator + width + i) = decode(Y3, Cb, Cr);
        *(rgbIterator + width + i + 1) = decode(Y4, Cb, Cr);

        if (i != 0 && (i + 2) % width == 0)
            i += width;
    }
}

inline byte clampToByte(int value) { return (byte)std::min(std::max(value, 0), UINT8_MAX); }
inline byte quantize(float value, int min, int max) { return (byte) (value*max+min); }

RGBA __unused allBlue(int, int, int)
{
    return { .red=0x00, .green=0x00, .blue=0xFF, .alpha=0xFF };
}

RGBA __unused allRed(int, int, int)
{
    return { .red=0xFF, .green=0x00, .blue=0x00, .alpha=0xFF };
}

RGBA __unused allGreen(int, int, int)
{
    return { .red=0x00, .green=0xFF, .blue=0x00, .alpha=0xFF };
}

//  The following decode... methods derive from the book "Video Demystified" 5e by Keith Jack ISBN: 978-0-7506-8395-1

#pragma mark - YUV Color Space (p17)
#pragma mark -

#pragma mark YUV to RGB (p18)

RGBA  __unused decodeYUVtoRGB(int Y, int U, int V)
{
    int red   = (int)(Y + 1.140f*V);
    int green = (int)(Y - 0.395f*V - 0.581f*U);
    int blue  = (int)(Y + 2.032f*U);

    return { .red=clampToByte(red), .green=clampToByte(green), .blue=clampToByte(blue), .alpha=255 };
}

#pragma mark - YCbCr Color Space (p19)

#pragma mark - RGB-YCbCr Equations: SDTV (p19)
#pragma mark -

#pragma mark YCbCr to RGB: Analog Equations (p20)

RGBA __unused decodeYPbPrToRGB(int Y, int Pb, int Pr)
{
    // Many specifications assume the source is analog YPbPr.
    // This is first converted to analog R'G'B'.

    float red   = Y + 1.402f*Pr;
    float green = Y - 0.714f*Pr - 0.344f*Pb;
    float blue  = Y + 1.772f*Pb;

    // To generate 8-bit R'G'B' with a 16–235 nominal range (Studio R'G'B'),
    // R'G'B' is then quantized to 8 bits

    return { .red=quantize(red, 16, 219), .green=quantize(green, 16, 219), .blue=quantize(blue, 16, 219), .alpha=255 };
}

#pragma mark YCbCr to RGB: Digital Equations (p20)

RGBA __unused decodeYCbCrToRGB(int Y, int Cb, int Cr)
{
    Cb -= 128;
    Cr -= 128;

    int red   = (int) (Y + 1.371f*Cr);
    int green = (int) (Y - 0.698f*Cr - 0.366f*Cb);
    int blue  = (int) (Y + 1.732f*Cb);

    return { .red=clampToByte(red), .green=clampToByte(green), .blue=clampToByte(blue), .alpha=255 };
}

#pragma mark Computer Systems Considerations (p20)

RGBA __unused decodeYCbCrToRGBAlt(int Y, int Cb, int Cr)
{
    // If the R'G'B' data has a range of 0–255, as is commonly found in computer systems,
    // the following equations may be more convenient to use:

    Cb -= 128;
    Cr -= 128;
    Y   = std::max(0, Y-16);

    int red   = (int) (1.164f*Y + 1.596f*Cr);
    int green = (int) (1.164f*Y - 0.813f*Cr - 0.391f*Cb);
    int blue  = (int) (1.164f*Y + 2.018f*Cb);

    return { .red=clampToByte(red), .green=clampToByte(green), .blue=clampToByte(blue), .alpha=255 };
}

#pragma mark - RGB-YCbCr Equations: HDTV (p20)
#pragma mark -

#pragma mark YCbCr to RGB: Analog Equations (p21)

RGBA __unused decodeYPbPRToRGB_HDTV(int Y, int Pb, int Pr)
{
    // Many specifications assume the source is analog YPbPr.
    // This is first converted to analog R'G'B'.

    float red   = Y + 1.575f*Pr;
    float green = Y - 0.468f*Pr - 0.187f*Pb;
    float blue  = Y + 1.856f*Pb;

    // To generate 8-bit R'G'B' with a 16–235 nominal range (Studio R'G'B'),
    // R'G'B' is then quantized to 8 bits

    return { .red=quantize(red, 16, 219), .green=quantize(green, 16, 219), .blue=quantize(blue, 16, 219), .alpha=255 };
}

#pragma mark YCbCr to RGB: Digital Equations (p21)

RGBA __unused decodeYCbCrToRGB_HDTV(int Y, int Cb, int Cr)
{
    Cb -= 128;
    Cr -= 128;

    int red   = (int) (Y + 1.540f*Cr);
    int green = (int) (Y - 0.459f*Cr - 0.183f*Cb);
    int blue  = (int) (Y + 1.816f*Cb);

    return { .red=clampToByte(red), .green=clampToByte(green), .blue=clampToByte(blue), .alpha=255 };
}

#pragma mark Computer Systems Considerations (p21)

RGBA __unused decodeYCbCrToRGB_HDTVAlt(int Y, int Cb, int Cr)
{
    // If the R'G'B' data has a range of 0–255, as is commonly found in computer systems,
    // the following equations may be more convenient to use:

    Cb -= 128;
    Cr -= 128;
    Y   = std::max(0, Y-16);

    int red   = (int) (1.164f*Y + 1.793f*Cr);
    int green = (int) (1.164f*Y - 0.534f*Cr - 0.213f*Cb);
    int blue  = (int) (1.164f*Y + 2.115f*Cb);

    return { .red=clampToByte(red), .green=clampToByte(green), .blue=clampToByte(blue), .alpha=255 };
}

