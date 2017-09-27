//
//  lh_cfont.cpp
//  lh_gl
//
//  Created by Liu,Han(ARL) on 2017/8/30.
//  Copyright © 2017年 Liu,Han(ARL). All rights reserved.
//
#include "lh_cfont.hpp"

FTPixmapFont* CLHFront::infoFont = nullptr;
FTLayout* CLHFront::layouts[2] = { &simpleLayout, NULL };

GLint CLHFront::w_win = 640;
GLint CLHFront::h_win = 480;

int CLHFront::current_font = FTGL_EXTRUDE;
int CLHFront::mode = INTERACTIVE;
int CLHFront::carat = 0;
int CLHFront::currentLayout = 0;
int CLHFront::NumLayouts = 2;

float CLHFront::InitialLineLength = 600.0f;
float CLHFront::OX = -300;
float CLHFront::OY = 170;
float CLHFront::textures[2][48] = {
    {
        1.0, 1.0, 1.0, 0.7, 0.7, 0.7, 1.0, 1.0, 1.0, 0.7, 0.7, 0.7,
        0.7, 0.7, 0.7, 0.4, 0.4, 0.4, 0.7, 0.7, 0.7, 0.4, 0.4, 0.4,
        1.0, 1.0, 1.0, 0.7, 0.7, 0.7, 1.0, 1.0, 1.0, 0.7, 0.7, 0.7,
        0.7, 0.7, 0.7, 0.4, 0.4, 0.4, 0.7, 0.7, 0.7, 0.4, 0.4, 0.4,
    },
    {
        0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
        0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3,
        0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
        0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3,
    }
};

FTFont* CLHFront::fonts[7];
FTSimpleLayout CLHFront::simpleLayout;
char CLHFront::myString[4096];
std::string CLHFront::fontfile;
GLuint CLHFront::textureID[2];


CLHFront::CLHFront()
{}

CLHFront::~CLHFront()
{}

void CLHFront::setUpLighting()
{
    
    // Set up lighting.
    /*
    float light1_ambient[4]  = { 0.5, 0.5, 0.5, 1.0 };
    float light1_diffuse[4]  = { 1.0, 0.9, 0.9, 1.0 };
    float light1_specular[4] = { 1.0, 0.7, 0.7, 1.0 };
    float light1_position[4] = { 400.0, 400.0, 100.0, 1.0 };
    */
    float light1_ambient[4]  = { 1.0, 0.0, 0.0, 1.0 };
    float light1_diffuse[4]  = { 0.0, 1.0, 0.0, 1.0 };
    float light1_specular[4] = { 0.0, 0.0, 1.0, 1.0 };
    float light1_position[4] = { 400.0, 400.0, 100.0, 1.0 };
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
    //glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glEnable(GL_LIGHT1);
    
    float front_emission[4] = { 0.5, 0.4, 0.3, 0.0 };
    float front_ambient[4]  = { 0.4, 0.4, 0.4, 0.0 };
    float front_diffuse[4]  = { 0.95, 0.95, 0.8, 0.0 };
    float front_specular[4] = { 0.8, 0.8, 0.8, 0.0 };
    //glMaterialfv(GL_FRONT, GL_EMISSION, front_emission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, front_ambient);
    //glMaterialfv(GL_FRONT, GL_DIFFUSE, front_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, front_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 25.0);
    glColor4fv(front_diffuse);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    
    glEnable(GL_LIGHTING);
}


void CLHFront::setUpFonts(const char* file)
{
    fonts[FTGL_BITMAP] = new FTBitmapFont(file);
    fonts[FTGL_PIXMAP] = new FTPixmapFont(file);
    fonts[FTGL_OUTLINE] = new FTOutlineFont(file);
    fonts[FTGL_POLYGON] = new FTPolygonFont(file);
    fonts[FTGL_EXTRUDE] = new FTExtrudeFont(file);
    fonts[FTGL_TEXTURE] = new FTTextureFont(file);
    fonts[FTGL_BUFFER] = new FTBufferFont(file);
    
    for(int x = 0; x < 7; ++x)
    {
        if(fonts[x]->Error())
        {
            fprintf(stderr, "Failed to open font %s", file);
            exit(1);
        }
        
        if(!fonts[x]->FaceSize(50))
        {
            fprintf(stderr, "Failed to set size");
            exit(1);
        }
        
        fonts[x]->Depth(6.);
        fonts[x]->Outset(-1.0, 1.0);
        fonts[x]->CharMap(ft_encoding_unicode);
    }
    
    infoFont = new FTPixmapFont(file);
    if(infoFont->Error())
    {
        fprintf(stderr, "Failed to open font %s", file);
        exit(1);
    }
    
    infoFont->FaceSize(18);
    strcpy(myString,
           "开始绘制图形之前，我们必须先给OpenGL输入一些顶点。OpenGL是一个3D图形库，所以我们在OpenGL中指定的所有坐标都是3D坐标（x、y和z）。OpenGL不是简单地把所有的3D坐标变换为屏幕上的2D像素；");
    
}



void CLHFront::renderFontmetrics()
{
    FTBBox bbox;
    float x1, y1, z1, x2, y2, z2;
    
    // If there is a layout, use it to compute the bbox, otherwise query as
    // a string.
    if(layouts[currentLayout])
        bbox = layouts[currentLayout]->BBox(myString);
    else
        bbox = fonts[current_font]->BBox(myString);
    
    x1 = bbox.Lower().Xf(); y1 = bbox.Lower().Yf(); z1 = bbox.Lower().Zf();
    x2 = bbox.Upper().Xf(); y2 = bbox.Upper().Yf(); z2 = bbox.Upper().Zf();
    
    // Draw the bounding box
    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // GL_ONE_MINUS_SRC_ALPHA
 /*
   //上下横线
    glColor3f(0.0, 1.0, 0.0);
    // Draw the front face
    glBegin(GL_LINE_LOOP);
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y2, z1);
    glVertex3f(x2, y2, z1);
    glVertex3f(x2, y1, z1);
    glEnd();
    // Draw the back face
    if(current_font == FTGL_EXTRUDE && z1 != z2)
    {
        glBegin(GL_LINE_LOOP);
        glVertex3f(x1, y1, z2);
        glVertex3f(x1, y2, z2);
        glVertex3f(x2, y2, z2);
        glVertex3f(x2, y1, z2);
        glEnd();
        // Join the faces
        glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x1, y1, z2);
        
        glVertex3f(x1, y2, z1);
        glVertex3f(x1, y2, z2);
        
        glVertex3f(x2, y2, z1);
        glVertex3f(x2, y2, z2);
        
        glVertex3f(x2, y1, z1);
        glVertex3f(x2, y1, z2);
        glEnd();
    }
*/

/*
 //竖线
    // Render layout-specific metrics
    if(!layouts[currentLayout])
    {
        // There is no layout. Draw the baseline, Ascender and Descender
        glBegin(GL_LINES);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(fonts[current_font]->Advance(myString), 0.0, 0.0);
        glVertex3f(0.0, fonts[current_font]->Ascender(), 0.0);
        glVertex3f(0.0, fonts[current_font]->Descender(), 0.0);
        glEnd();
    }
    else if(layouts[currentLayout]
            && (dynamic_cast <FTSimpleLayout *>(layouts[currentLayout])))
    {
        float lineWidth = ((FTSimpleLayout *)layouts[currentLayout])->GetLineLength();
        
        // The layout is a SimpleLayout.  Render guides that mark the edges
        // of the wrap region.
        glColor3f(0.5, 1.0, 1.0);
        glBegin(GL_LINES);
        glVertex3f(0, 10000, 0);
        glVertex3f(0, -10000, 0);
        glVertex3f(lineWidth, 10000, 0);
        glVertex3f(lineWidth, -10000, 0);
        glEnd();
    }
*/
    
    /*
    // 坐标轴
    // Draw the origin
    glTranslatef(-OX, -OY,0);
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(5.0);
    glBegin(GL_POINTS);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();
    // Draw the axis
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(100,0,0);
    glEnd();
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,100,0);
    glEnd();
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,0,100);
    glEnd();
    */
}


void CLHFront::renderFontInfo()
{
    return;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w_win, 0, h_win);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    
    // draw mode
    glColor3f(0.0, 1.0, 0.0);
    glRasterPos2f(20.0f , h_win - (20.0f + infoFont->Ascender()));
    
    switch(mode)
    {
        case EDITING:
            infoFont->Render("Edit Mode");
            break;
        case INTERACTIVE:
            break;
    }
    
    // draw font type
    glRasterPos2i(20 , 20);
    switch(current_font)
    {
        case FTGL_BITMAP:
            infoFont->Render("Bitmap Font");
            break;
        case FTGL_PIXMAP:
            infoFont->Render("Pixmap Font");
            break;
        case FTGL_OUTLINE:
            infoFont->Render("Outline Font");
            break;
        case FTGL_POLYGON:
            infoFont->Render("Polygon Font");
            break;
        case FTGL_EXTRUDE:
            infoFont->Render("Extruded Font");
            break;
        case FTGL_TEXTURE:
            infoFont->Render("Texture Font");
            break;
        case FTGL_BUFFER:
            infoFont->Render("Buffer Font");
            break;
    }
    
    glRasterPos2f(20.0f , 20.0f + infoFont->LineHeight());
    infoFont->Render(fontfile.c_str());
    
    // If the current layout is a SimpleLayout, output the alignemnt mode
    if(layouts[currentLayout]
       && (dynamic_cast <FTSimpleLayout *>(layouts[currentLayout])))
    {
        glRasterPos2f(20.0f , 20.0f + 2*(infoFont->Ascender() - infoFont->Descender()));
        // Output the alignment mode of the layout
        switch (((FTSimpleLayout *)layouts[currentLayout])->GetAlignment())
        {
            case FTGL::ALIGN_LEFT:
                infoFont->Render("Align Left");
                break;
            case FTGL::ALIGN_RIGHT:
                infoFont->Render("Align Right");
                break;
            case FTGL::ALIGN_CENTER:
                infoFont->Render("Align Center");
                break;
            case FTGL::ALIGN_JUSTIFY:
                infoFont->Render("Align Justified");
                break;
        }
    }
}



void CLHFront::do_display(void)
{
    switch(current_font)
    {
        case FTGL_BITMAP:
        case FTGL_PIXMAP:
        case FTGL_OUTLINE:
            glDisable(GL_TEXTURE_2D);
            break;
        case FTGL_POLYGON:
            glDisable(GL_TEXTURE_2D);
            setUpLighting();
            break;
        case FTGL_EXTRUDE:
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            setUpLighting();
            glBindTexture(GL_TEXTURE_2D, textureID[0]);
            break;
        case FTGL_TEXTURE:
        case FTGL_BUFFER:
            glEnable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            setUpLighting();
            glNormal3f(0.0, 0.0, 1.0);
            break;
    }
    glTranslatef(OX, OY,0);
    
    // If you do want to switch the color of bitmaps rendered with glBitmap,
    // you will need to explicitly call glRasterPos (or its ilk) to lock
    // in a changed current color.
    
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    int renderMode = FTGL::RENDER_FRONT | FTGL::RENDER_BACK;
    if(layouts[currentLayout])
        layouts[currentLayout]->Render(myString, -1,
                                       FTPoint(), renderMode);
    else
        fonts[current_font]->Render(myString, -1,
                                    FTPoint(), FTPoint(), renderMode);
    
    if(current_font == FTGL_EXTRUDE)
    {
        glBindTexture(GL_TEXTURE_2D, textureID[1]);
        renderMode = FTGL::RENDER_SIDE;
        if(layouts[currentLayout])
            layouts[currentLayout]->Render(myString, -1,
                                           FTPoint(), renderMode);
        else
            fonts[current_font]->Render(myString, -1,
                                        FTPoint(), FTPoint(), renderMode);
    }
    glPopMatrix();
    glPushMatrix();
    renderFontmetrics();
    glPopMatrix();
    renderFontInfo();
}


void CLHFront::display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SetCamera();
    
    switch(current_font)
    {
        case FTGL_BITMAP:
        case FTGL_PIXMAP:
            glRasterPos2i((long)(w_win / 2 + OX), (long)(h_win / 2 + OY));
            glTranslatef(w_win / 2, h_win / 2, 0.0);
            break;
        case FTGL_OUTLINE:
        case FTGL_POLYGON:
        case FTGL_EXTRUDE:
        case FTGL_TEXTURE:
        case FTGL_BUFFER:
            tbMatrix();
            break;
    }
    
    glPushMatrix();
    do_display();
    glPopMatrix();
    glutSwapBuffers();
}


void CLHFront::_init(const char* file)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(1.0, 1.0); // ????
    
    SetCamera();
    tbInit(GLUT_LEFT_BUTTON);
    tbAnimate(GL_FALSE);
    setUpFonts(file);
    
    // Configure the SimpleLayout
    simpleLayout.SetLineLength(InitialLineLength);
    simpleLayout.SetFont(fonts[current_font]);
    glGenTextures(2, textureID);
    for(int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, textureID[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_FLOAT,
                     textures[i]);
    }
}


void CLHFront::parsekey(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
            exit(0);
            break;
        case 13:
            if(mode == EDITING)
            {
                mode = INTERACTIVE;
            }
            else
            {
                mode = EDITING;
                carat = 0;
            }
            break;
        case '\t':
            // If current layout is a SimpleLayout, change its alignment properties
            if(layouts[currentLayout]
               && (dynamic_cast <FTSimpleLayout *>(layouts[currentLayout])))
            {
                FTSimpleLayout *l = (FTSimpleLayout *)layouts[currentLayout];
                // Decrement the layout
                switch (l->GetAlignment())
                {
                    case FTGL::ALIGN_LEFT:
                        l->SetAlignment(FTGL::ALIGN_RIGHT);
                        break;
                    case FTGL::ALIGN_RIGHT:
                        l->SetAlignment(FTGL::ALIGN_CENTER);
                        break;
                    case FTGL::ALIGN_CENTER:
                        l->SetAlignment(FTGL::ALIGN_JUSTIFY);
                        break;
                    case FTGL::ALIGN_JUSTIFY:
                        l->SetAlignment(FTGL::ALIGN_LEFT);
                        break;
                }
            }
            break;
        default:
            if(mode == INTERACTIVE)
            {
                myString[0] = key;
                myString[1] = 0;
            }
            else
            {
                myString[carat] = key;
                myString[carat + 1] = 0;
                carat = carat > 2000 ? 2000 : carat + 1;
            }
            break;
    }
    
    glutPostRedisplay();
}


void CLHFront::parseSpecialKey(int key, int x, int y)
{
    FTSimpleLayout *l = NULL;
    float s;
    
    // If the currentLayout is a SimpleLayout store a pointer in l
    if(layouts[currentLayout]
       && (dynamic_cast <FTSimpleLayout *>(layouts[currentLayout])))
    {
        l = (FTSimpleLayout *)layouts[currentLayout];
    }
    
    switch (key)
    {
        case GLUT_KEY_UP:
            current_font = (current_font + 1) % 7;
            break;
        case GLUT_KEY_DOWN:
            current_font = (current_font + 6) % 7;
            break;
        case GLUT_KEY_PAGE_UP:
            currentLayout = (currentLayout + 1) % NumLayouts;
            break;
        case GLUT_KEY_PAGE_DOWN:
            currentLayout = (currentLayout + NumLayouts - 1) % NumLayouts;
            break;
        case GLUT_KEY_HOME:
            /* If the current layout is simple decrement its line length */
            if (l) l->SetLineLength(l->GetLineLength() - 10.0f);
            break;
        case GLUT_KEY_END:
            /* If the current layout is simple increment its line length */
            if (l) l->SetLineLength(l->GetLineLength() + 10.0f);
            break;
        case GLUT_KEY_LEFT:
            s = fonts[current_font]->FaceSize();
            if(s >= 2)
                fonts[current_font]->FaceSize(s - 1);
            break;
        case GLUT_KEY_RIGHT:
            fonts[current_font]->FaceSize(fonts[current_font]->FaceSize() + 1);
            break;
    }
    
    // If the current layout is a SimpleLayout, update its font.
    if(l)
    {
        l->SetFont(fonts[current_font]);
    }
    
    glutPostRedisplay();
}

void CLHFront::SetCamera(void)
{
    switch(current_font)
    {
        case FTGL_BITMAP:
        case FTGL_PIXMAP:
            /*
             glMatrixMode(GL_PROJECTION);
             glLoadIdentity();
             gluOrtho2D(0, w_win, 0, h_win);
             glMatrixMode(GL_MODELVIEW);
             glLoadIdentity();
             break;
             */
        case FTGL_OUTLINE:
        case FTGL_POLYGON:
        case FTGL_EXTRUDE:
        case FTGL_TEXTURE:
        case FTGL_BUFFER:
            glMatrixMode (GL_PROJECTION);
            glLoadIdentity ();
            gluPerspective(90, (float)w_win / (float)h_win, 1, 1000);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0.0, 0.0, (float)h_win / 2.0f, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            break;
    }
}




void CLHFront::motion(int x, int y)
{
    tbMotion(x, y);
}

void CLHFront::mouse(int button, int state, int x, int y)
{
    tbMouse(button, state, x, y);
}

void CLHFront::myReshape(int w, int h)
{
    glMatrixMode (GL_MODELVIEW);
    glViewport (0, 0, w, h);
    glLoadIdentity();
    
    w_win = w;
    h_win = h;
    SetCamera();
    tbReshape(w_win, h_win);
}

void CLHFront::init(const char* cfont)
{
    if (!cfont)
    {
        fprintf(stderr, "A font file must be specified on the command line\n");
        exit(1);
    }
    
    fontfile = cfont;
    glutInitWindowSize(w_win, h_win);
    glutDisplayFunc(display);
    glutKeyboardFunc(parsekey);
    glutSpecialFunc(parseSpecialKey);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(myReshape);
    glutIdleFunc(display);
    _init(cfont);
}












