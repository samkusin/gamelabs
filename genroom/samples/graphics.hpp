/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Samir Sinha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CK_Sample_Graphics_Hpp
#define CK_Sample_Graphics_Hpp

#include "bitmap.hpp"

#include <memory>
#include <glfw3.h>

////////////////////////////////////////////////////////////////////////////
//
//  Graphics
//
//  Our use of GL 2 is not to encourage deprecated GL functionality in
//  production code.  For sample purposes, where we're prototyping concepts
//  that aren't graphic-rendering specific, GL 2 is a bit easier to set up,
//  and we're not concerned about optimizing framerates for very simple 2d
//  graphics
//
class Graphics
{
    std::unique_ptr<cinekine::SpriteDictionary> _spriteDb;
    GLuint _glTexture;
    float _texWidth;
    float _texHeight;

public:
    Graphics(const char* spriteDbName);
    ~Graphics();

    void mapTile(int32_t handle, const char* name);

    void clear();

    void beginTiles();
    void endTiles();

    void drawTile(int l, int t, int32_t handle);
};


#endif
