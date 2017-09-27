//
//  effectfonts.cpp
//  mac_freetype
//
//  Created by Liu,Han(ARL) on 2017/9/27.
//  Copyright © 2017年 Liu,Han(ARL). All rights reserved.
//

#include "effectfonts.hpp"
#include <iostream>
#include <fstream>
#include <assert.h>

CEffectFonts::CEffectFonts(){
    ;
}
CEffectFonts::~CEffectFonts(){
    release();
}

void CEffectFonts::release(){
    if(_fontBuffer != nullptr){
        delete [] _fontBuffer;
        _fontBuffer = nullptr;
    }
    
    FT_Done_FreeType(_library);
}

void CEffectFonts::init(const char* fftpath, 
    int size,
    const Pixel32 &fontcol,
    const Pixel32 &outlinecol,
    float outlinewidth){

        _size = size;
        _font_col = fontcol;
        _outline_col = outlinecol;
        _outlinewidth = outlinewidth;

    // Initialize FreeType.
    FT_Init_FreeType(&_library);
    
    // Open up a font file.
    std::ifstream fontFile(fftpath, std::ios::binary);
    if (fontFile)
    {
        // Read the entire file to a memory buffer.
        fontFile.seekg(0, std::ios::end);
        std::fstream::pos_type fontFileSize = fontFile.tellg();
        fontFile.seekg(0);
        unsigned char *fontBuffer = new unsigned char[fontFileSize];
        fontFile.read((char *)fontBuffer, fontFileSize);
        FT_New_Memory_Face(_library, fontBuffer, fontFileSize, 0, &_face);
    }
    else{
        assert(0);
    }
    
}


// Each time the renderer calls us back we just push another span entry on
// our list.

void CEffectFonts::raster_callback(const int y,
                                   const int count,
                                   const FT_Span * const spans,
                                   void * const user)
{
    Spans *sptr = (Spans *)user;
    for (int i = 0; i < count; ++i)
        sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
}


// Set up the raster parameters and render the outline.

void CEffectFonts::render_spans(FT_Library &library,
                                FT_Outline * const outline,
                                Spans *spans)
{
    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));
    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = raster_callback;
    params.user = spans;
    
    FT_Outline_Render(library, outline, &params);
}


// Render the specified character as a colored glyph with a colored outline
// and dump it to a TGA.

void CEffectFonts::load_glyph_tga(std::wstring wstr)
{
    for(std::wstring::iterator iter = wstr.begin();
        iter != wstr.end();
        iter++){
            insert_word(*iter);
    }
}

void CEffectFonts::insert_word(wchar_t ch)
{
    if (_map_words.end() != _map_words.find(ch))
    {
        return;
    }
    
    // Set the size to use.
    if (FT_Set_Char_Size(_face, _size << 6, _size << 6, 90, 90))
    {
        return;
    }
    
    // Load the glyph we are looking for.
    FT_UInt gindex = FT_Get_Char_Index(_face, ch);
    if (FT_Load_Glyph(_face, gindex, FT_LOAD_NO_BITMAP))
    {
        return;
    }
    
    // Need an outline for this to work.
    if (_face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
    {
        return;
    }
    // Render the basic glyph to a span list.
    Spans spans;
    render_spans(_library, &_face->glyph->outline, &spans);

    // Next we need the spans for the outline.
    Spans outlineSpans;

    // Set up a stroker.
    FT_Stroker stroker;
    FT_Stroker_New(_library, &stroker);
    FT_Stroker_Set(stroker,
                   (int)(_outlinewidth * 64),
                   FT_STROKER_LINECAP_ROUND,
                   FT_STROKER_LINEJOIN_ROUND,
                   0);

    FT_Glyph glyph;
    if (FT_Get_Glyph(_face->glyph, &glyph) == 0)
    {
        FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
        // Again, this needs to be an outline to work.
        if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
        {
            // Render the outline spans to the span list
            FT_Outline *o =
                &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
            render_spans(_library, o, &outlineSpans);
        }

        // Clean up afterwards.
        FT_Stroker_Done(stroker);
        FT_Done_Glyph(glyph);

        // Now we need to put it all together.
        if (!spans.empty())
        {
            // Figure out what the bounding rect is for both the span lists.
            Rect rect(spans.front().x,
                      spans.front().y,
                      spans.front().x,
                      spans.front().y);
            for (Spans::iterator s = spans.begin();
                 s != spans.end(); ++s)
            {
                rect.Include(Vec2(s->x, s->y));
                rect.Include(Vec2(s->x + s->width - 1, s->y));
            }
            for (Spans::iterator s = outlineSpans.begin();
                 s != outlineSpans.end(); ++s)
            {
                rect.Include(Vec2(s->x, s->y));
                rect.Include(Vec2(s->x + s->width - 1, s->y));
            }
                
#if 0
                // This is unused in this test but you would need this to draw
                // more than one glyph.
                float bearingX = _face->glyph->metrics.horiBearingX >> 6;
                float bearingY = _face->glyph->metrics.horiBearingY >> 6;
                float advance = _face->glyph->advance.x >> 6;
#endif
                
                // Get some metrics of our image.
                int width = rect.Width();
                int height = rect.Height();
                int imgsize = width * height;
                
                // Allocate data for our image and clear it out to transparent.
                //Pixel32 *pxl = new Pixel32[imgsize];
                //std::shared_ptr<Pixel32> pxl(new Pixel32[10], [](int *p) { delete[] p; });
                std::shared_ptr<Pixel32> pxl(new Pixel32[imgsize], std::default_delete<Pixel32[]>());
            
                //std::unique_ptr<Pixel32[]> pxl(new Pixel32[imgsize]());
                
                
                // Loop over the outline spans and just draw them into the
                // image.
                for (Spans::iterator s = outlineSpans.begin();
                     s != outlineSpans.end(); ++s)
                    for (int w = 0; w < s->width; ++w)
                        pxl.get()[(int)((height - 1 - (s->y - rect.ymin)) * width
                                  + s->x - rect.xmin + w)]=
                        Pixel32(_outline_col.r, _outline_col.g, _outline_col.b,
                                s->coverage);
                
                // Then loop over the regular glyph spans and blend them into
                // the image.
                for (Spans::iterator s = spans.begin();
                     s != spans.end(); ++s)
                    for (int w = 0; w < s->width; ++w)
                    {
                        Pixel32 &dst =
                        pxl.get()[(int)((height - 1 - (s->y - rect.ymin)) * width
                                  + s->x - rect.xmin + w)];
                        Pixel32 src = Pixel32(_font_col.r, _font_col.g, _font_col.b,
                                              s->coverage);
                        dst.r = (int)(dst.r + ((src.r - dst.r) * src.a) / 255.0f);
                        dst.g = (int)(dst.g + ((src.g - dst.g) * src.a) / 255.0f);
                        dst.b = (int)(dst.b + ((src.b - dst.b) * src.a) / 255.0f);
                        dst.a = MIN(255, dst.a + src.a);
                    }
                
                std::pair<MAP_WORDS::iterator, bool> ret;
                ret = _map_words.insert(PAIR_WORD(ch,
                                                  WordPixel(pxl, width, height)));
                
            }
        }
}

bool CEffectFonts::get_wordpixel(wchar_t ch, WordPixel &wp)
{
    MAP_WORDS::iterator iter = _map_words.find(ch);
    if (_map_words.end() == iter)
    {
        insert_word(ch);
        iter = _map_words.find(ch);
        if (_map_words.end() == iter)
        {
            //assert(0);
            return false;
        }
    }

    wp = iter->second;
    return true;
}

int CEffectFonts::get_wordpixel_list(std::wstring wstr, std::vector<WordPixel> &vecwords)
{
    int maxwordhight = 0;
    for (std::wstring::iterator iter = wstr.begin();
         iter != wstr.end();
         iter++)
    {
        WordPixel wp;
        if (get_wordpixel(*iter, wp))
        {
            vecwords.push_back(wp);
            maxwordhight = maxwordhight > wp._hight ? maxwordhight : wp._hight;
        }
    }
    return maxwordhight;
}
