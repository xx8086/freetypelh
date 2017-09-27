//
//  saveimg.cpp
//  mac_freetype
//
//  Created by Liu,Han(ARL) on 2017/9/27.
//  Copyright © 2017年 Liu,Han(ARL). All rights reserved.
//

#include "saveimg.hpp"
#include <iostream>
#include <fstream>
#include <assert.h>

// TGA Header struct to make it simple to dump a TGA to disc.

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

struct TGAHeader
{
    uint8   idLength,           // Length of optional identification sequence.
    paletteType,        // Is a palette present? (1=yes)
    imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
    // 3=grey, +8=rle packed).
    uint16  firstPaletteEntry,  // First palette index, if present.
    numPaletteEntries;  // Number of palette entries, if present.
    uint8   paletteBits;        // Number of bits per palette entry.
    uint16  x,                  // Horiz. pixel coord. of lower left of image.
    y,                  // Vert. pixel coord. of lower left of image.
    width,              // Image width in pixels.
    height;             // Image height in pixels.
    uint8   depth,              // Image color depth (bits per pixel).
    descriptor;         // Image attribute flags.
};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

bool CSaveImg::write_tga(WordPixel& pxl, const char* filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (file)
    {
        TGAHeader header;
        memset(&header, 0, sizeof(TGAHeader));
        header.imageType  = 2;
        header.width = pxl._width;
        header.height = pxl._hight;
        header.depth = 32;
        header.descriptor = 0x20;
        
        file.write((const char *)&header, sizeof(TGAHeader));
        file.write((const char *)pxl._share_pixe.get(), sizeof(Pixel32) * pxl._width * pxl._hight);
        
        return true;
    }
    return false;
}

void CSaveImg::copy_datas(char* buff, char* src, int dest_pos, int dest_line, int src_w, int src_h)
{
    int line = dest_line;
    int insertpos = 0;
    for(int i = 0; i < src_h; i++)
    {
        line += i * _width;
        insertpos = line + dest_pos;
        memcpy(buff + insertpos, src, src_w);
    }
}

bool CSaveImg::write_tga(std::vector<WordPixel> &vecwords, const char* filename, uint16 width, uint16 hight, int maxwordhight)
{
    _str_save_name = filename;
    _width = width;
    _height = hight;
    
    std::ofstream file(_str_save_name.c_str(), std::ios::binary);
    if (file)
    {
        const int elementsize = sizeof(Pixel32);
        char* buffer = new char[_width * _height];
        memset(buffer, 0, _width * _height);

        int lenght = 0;
        int linepos = 0;
        for (std::vector<WordPixel>::iterator iter = vecwords.begin();
             iter != vecwords.end();
             iter++)
        {
            if (lenght + elementsize * iter->_width > _width)
            {
                if (linepos + maxwordhight > _height)
                {
                    break;
                }
                linepos += _width * maxwordhight;
                lenght = 0;
            }
            if (iter->_width * elementsize > _width)
            {
                assert(0);
                break;
            }

            copy_datas(buffer, (char*)(iter->_share_pixe.get()), linepos, lenght, elementsize * iter->_width, iter->_hight);
            lenght += elementsize * iter->_width;
        }

        

        TGAHeader header;
        memset(&header, 0, sizeof(TGAHeader));
        header.imageType = 2;
        header.width = _width;
        header.height = _height;
        header.depth = 32;
        header.descriptor = 0x20;
        file.write((const char *)&header, sizeof(TGAHeader));
        file.write((const char *)buffer, _width * _height);
        delete[] buffer;
        buffer = nullptr;
        return true;
    }
    return false;
}
