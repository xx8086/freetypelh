//
//  main.cpp
//  mac_freetype
//
//  Created by Liu,Han(ARL) on 2017/9/27.
//  Copyright © 2017年 Liu,Han(ARL). All rights reserved.
//
// example2.cpp

// This file demonstrates how to render a coloured glyph with a differently
// coloured outline.
//
// Written Feb. 2009 by Erik Möller,
// with slight modifications by Werner Lemberg
//
// Public domain.
//
// Eric uses similar code in real applications; see
//
//   http://www.timetrap.se
//   http://www.emberwind.se
//
// for more.
#include "effectfonts.hpp"
#include "saveimg.hpp"

int
main(int argc,
     char **argv)
{
    std::wstring stecontext(L"啊");
    CEffectFonts effect_fonts;
    effect_fonts.init("/Users/baidu/Microsoft_Yahei.ttf",
                      10,
                      Pixel32(255, 90, 30),
                      Pixel32(255, 255, 255),
                      3.0f);
    effect_fonts.load_glyph_tga(stecontext);
    std::vector<WordPixel> vecwords;
    int maxhight = effect_fonts.get_wordpixel_list(stecontext, vecwords);
    CSaveImg img_tag;
    img_tag.write_tga(vecwords[0], "/Users/baidu/lh.tga");
    //img_tag.write_tga(vecwords, "/Users/baidu/lh.tga", 512, 512, maxhight);
    //img_tag.write_tga(pix);

    return 1;
}

// Local Variables:
// coding: utf-8
// End:

