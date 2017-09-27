//
//  lh_cfont.hpp
//  lh_gl
//
//  Created by Liu,Han(ARL) on 2017/8/30.
//  Copyright © 2017年 Liu,Han(ARL). All rights reserved.
//

#ifndef lh_cfont_hpp
#define lh_cfont_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <GLUT/glut.h>

#include "config.h"
#include "FTGL/ftgl.h"
#include "tb.h"

class CLHFront
{
public:
    CLHFront();
    ~CLHFront();
protected:
    
    
#define EDITING 1
#define INTERACTIVE 2
enum{
 FTGL_BITMAP = 0,
 FTGL_PIXMAP = 1,
 FTGL_OUTLINE = 2,
 FTGL_POLYGON = 3,
 FTGL_EXTRUDE = 4,
 FTGL_TEXTURE = 5,
 FTGL_BUFFER = 6,
};

public:
    void init(const char*);
protected:
    static void SetCamera(void);
    static void setUpLighting();
    static void setUpFonts(const char* file);
    static void renderFontmetrics();
    static void renderFontInfo();
    static void do_display(void);
    static void display(void);
    static void _init(const char* file);
    static void parsekey(unsigned char key, int x, int y);
    static void parseSpecialKey(int key, int x, int y);
    
    static void motion(int x, int y);
    static void mouse(int button, int state, int x, int y);
    static void myReshape(int w, int h);
    
private:
    static  FTFont* fonts[7];
    static  FTPixmapFont* infoFont;// = nullptr;
    static  FTSimpleLayout simpleLayout;
    static  FTLayout *layouts[2];// = { &simpleLayout, NULL };
    
    static  char myString[4096];
    static  std::string fontfile;
    //static  char const* fontfile;// = "/Users/liuhan01/Desktop/lh_gl/Microsoft_Yahei.ttf";
    static  GLint w_win;// = 640;
    static  GLint h_win;// = 480;
    static  GLuint textureID[2];
    
    static  int current_font;// = FTGL_EXTRUDE;
    static  int mode;// = INTERACTIVE;
    static  int carat;// = 0;
    static  int currentLayout;// = 0;
    
    static  int NumLayouts;//= 2;
    static  float InitialLineLength;// = 600.0f;
    static  float OX;// = -300;
    static  float OY;// = 170;
    
    static  float textures[2][48];
};


#endif /* lh_cfont_hpp */
