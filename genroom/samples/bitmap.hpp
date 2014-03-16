/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Samir Sinha
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

#ifndef CK_Sample_Bitmap_Hpp
#define CK_Sample_Bitmap_Hpp

#include <string>
#include <vector>
#include <map>

#include "rapidjson/document.h"

namespace cinekine {

    class Sprite
    {
    public:
        Sprite(const std::string& name,
               int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH,
               int32_t drawOffsetX, int32_t drawOffsetY,
               int32_t drawWidth, int32_t drawHeight,
               int32_t sourceWidth, int32_t sourceHeight);
        Sprite(const std::string& name);

        const std::string& name() const         { return _name; }
        int32_t drawLeft() const     { return _drawX; }
        int32_t drawTop() const      { return _drawY; }
        int32_t drawRight() const    { return _drawX + _drawW; }
        int32_t drawBottom() const   { return _drawY + _drawH; }
        int32_t sourceWidth() const  { return _sourceW; }
        int32_t sourceHeight() const { return _sourceH; }

        int32_t sourceLeft() const              { return _fx; }
        int32_t sourceTop() const               { return _fy; }
        int32_t sourceRight() const             { return _fx + _fw; }
        int32_t sourceBottom() const            { return _fy + _fh; }

        bool operator<(const Sprite& sprite) const {
            return _name < sprite._name;
        }

    private:
        std::string _name;
        int32_t _fx, _fy, _fw, _fh;
        int32_t _drawX, _drawY;
        int32_t _drawW, _drawH;
        int32_t _sourceW, _sourceH;
    };

    class SpriteDictionary
    {

    public:
        SpriteDictionary(rapidjson::GenericDocument<rapidjson::UTF8<>>& document,
                         const std::string& nullBitmapName);
        
        void mapSprite(int32_t handle, const std::string& name);

        const Sprite& sprite(int32_t handle) const;

    private:
        //  sprites ordered by name
        std::vector<Sprite> _sprites;
        std::map<int32_t, Sprite*> _dictionary;
        Sprite _nullSprite;
    };

}

#endif
