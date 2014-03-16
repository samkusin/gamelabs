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

#include "graphics.hpp"

#include "rapidjson/document.h"
#include "rapidjson/filestream.h"
#include "stb/stb_image.h"

#include <cstdio>

static bool parseJsonDoc(rapidjson::Document& document, const char* filename)
{
    FILE *fp = fopen("tiles.json", "rb");
    if (!fp)
    {
        printf("Failed to open tiles.json");
        return false;
    }

    //  parse the atlas/dictionary
    rapidjson::FileStream stream(fp);
    document.ParseStream<0>(stream);

    fclose(fp);
    return true;
}

//
//  Graphics
//
Graphics::Graphics(const char* spriteDbName) :
        _glTexture(0),
        _texWidth(0.f),
        _texHeight(0.f)
{   
    rapidjson::Document spriteDbDoc;
    if (!parseJsonDoc(spriteDbDoc, spriteDbName))
        return;

    _spriteDb = std::unique_ptr<cinekine::SpriteDictionary>(
                    new cinekine::SpriteDictionary(spriteDbDoc, "null")
                );

    //  initialize the texture used for our atlas
    const rapidjson::Value& spriteDbInfo = spriteDbDoc["meta"];

    int w, h, comp;
    uint8_t* imgData = stbi_load(spriteDbInfo["image"].GetString(),
                                 &w, &h,
                                 &comp, 4);
    glGenTextures(1, &_glTexture);
    glBindTexture(GL_TEXTURE_2D, _glTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, _glTexture);
    stbi_image_free(imgData);

    _texWidth = w;
    _texHeight = h;
}

Graphics::~Graphics()
{
    glDeleteTextures(1, &_glTexture);
    _glTexture = 0;
}

void Graphics::mapTile(int32_t handle, const char* name)
{
    _spriteDb->mapSprite(handle, name);
}

void Graphics::clear()
{
    glClearColor(0.0f, 0.0f, 1.0f, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND); 
}

void Graphics::beginTiles()
{
    glBindTexture(GL_TEXTURE_2D, _glTexture);
    glBegin(GL_QUADS);
}

void Graphics::drawTile(int l, int t, int32_t handle)
{
    const cinekine::Sprite& sprite = _spriteDb->sprite(handle);

    float s0 = sprite.sourceLeft() / _texWidth;
    float t0 = sprite.sourceTop() / _texHeight;
    float s1 = sprite.sourceRight() / _texWidth;
    float t1 = sprite.sourceBottom() / _texHeight;
    float x0 = l + sprite.drawLeft();
    float x1 = l + sprite.drawRight();
    float y0 = t + sprite.drawTop();
    float y1 = t + sprite.drawBottom();

    glTexCoord2f(s0, t0);
    glVertex2f(x0, y0);
    glTexCoord2f(s0, t1);
    glVertex2f(x0, y1);
    glTexCoord2f(s1, t1);
    glVertex2f(x1, y1);
    glTexCoord2f(s1, t0);
    glVertex2f(x1, y0);
}

void Graphics::endTiles()
{
    glEnd();
}
