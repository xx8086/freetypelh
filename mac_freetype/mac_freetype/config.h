//
//  config.h
//  mac_freetype
//
//  Created by Liu,Han(ARL) on 2017/9/27.
//  Copyright © 2017年 Liu,Han(ARL). All rights reserved.
//

#ifndef config_h
#define config_h


#ifdef _MSC_VER
#define MIN __min
#define MAX __max
#else
#define MIN std::min
#define MAX std::max
#endif

#include <memory>

// Define some fixed size types.

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

// Try to figure out what endian this machine is using. Note that the test
// below might fail for cross compilation; additionally, multi-byte
// characters are implementation-defined in C preprocessors.

#if (('1234' >> 24) == '1')
#elif (('4321' >> 24) == '1')
#define BIG_ENDIAN
#else
#error "Couldn't determine the endianness!"
#endif


// A simple 32-bit pixel.

union Pixel32
{
    Pixel32()
    : integer(0)
    { r = g = b = a = 0;}
    Pixel32(uint8 bi, uint8 gi, uint8 ri, uint8 ai = 255)
    {
        b = bi;
        g = gi;
        r = ri;
        a = ai;
    }
    
    uint32 integer;
    
    struct
    {
#ifdef BIG_ENDIAN
        uint8 a, r, g, b;
#else // BIG_ENDIAN
        uint8 b, g, r, a;
#endif // BIG_ENDIAN
    };
};

struct WordPixel{
    //Pixel32* _pixe = nullptr;
    std::shared_ptr<Pixel32> _share_pixe;
    int _width;
    int _hight;
    WordPixel(){}
    WordPixel(std::shared_ptr<Pixel32> piex, int w, int h):
    _share_pixe(piex), _width(w), _hight(h){}
    
    WordPixel& operator= (const WordPixel& ohter)
    {
        _share_pixe = ohter._share_pixe;
        _width = ohter._width;
        _hight = ohter._hight;
        return *this;
    }
private:
    
};

struct Vec2
{
    Vec2() { }
    Vec2(float a, float b)
    : x(a), y(b) { }
    
    float x, y;
};

#endif /* config_h */
