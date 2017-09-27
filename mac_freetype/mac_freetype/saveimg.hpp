//
//  saveimg.hpp
//  mac_freetype
//
//  Created by Liu,Han(ARL) on 2017/9/27.
//  Copyright © 2017年 Liu,Han(ARL). All rights reserved.
//

#ifndef saveimg_hpp
#define saveimg_hpp

#include "config.h"
#include <string>
#include <stdio.h>
#include <vector>

class CSaveImg
{
public:
    CSaveImg(){}
    ~CSaveImg(){};
public:
    bool write_tga(std::vector<WordPixel> &vecwords,
                   const char* filename,
                   uint16 width,
                   uint16 hight,
                   int maxwordhight);//error.
    bool write_tga(WordPixel&, const char* filename);
private:
    void copy_datas(char* buff, char* src, int dest_pos, int dest_line, int src_w, int src_h);
private:
    uint16 _width = 0;
    uint16 _height = 0;
    std::string _str_save_name;
};

#endif /* saveimg_hpp */
