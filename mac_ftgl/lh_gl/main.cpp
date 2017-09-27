//
//  main.cpp
//  lh_gl
//
//  Created by Liu,Han(ARL) on 2017/8/29.
//  Copyright © 2017年 Liu,Han(ARL). All rights reserved.
//

#include "lh_cfont.hpp"

int main(int argc, char *argv[])
{
    CLHFront gf;
    //
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("FTGL TEST");
    gf.init("/Users/baidu/Microsoft_Yahei.ttf");

    glutMainLoop();
    
    return 0;
}
