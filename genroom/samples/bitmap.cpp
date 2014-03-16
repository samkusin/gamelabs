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

#include "bitmap.hpp"
#include <vector>
#include <algorithm>

namespace cinekine {

    Sprite::Sprite(const std::string& name,
                int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH,
                int32_t drawOffsetX, int32_t drawOffsetY,
                int32_t drawWidth, int32_t drawHeight,
                int32_t sourceWidth, int32_t sourceHeight) :
        _name(name),
        _fx(frameX), _fy(frameY), _fw(frameW), _fh(frameH),
        _drawX(drawOffsetX), _drawY(drawOffsetY),
        _drawW(drawWidth), _drawH(drawHeight),
        _sourceW(sourceWidth), _sourceH(sourceHeight)
    {
    }

    Sprite::Sprite(const std::string& name) :
        _name(name)
    {
    }
    
    SpriteDictionary::SpriteDictionary(
                rapidjson::GenericDocument<rapidjson::UTF8<>>& document,
                const std::string& nullBitmapName) :
        _nullSprite(nullBitmapName, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1)
    {
        //  find the number of "frames" and reserve space
        const rapidjson::Value& frames = document["frames"];
        size_t frameCount = frames.Size();
        if (frameCount)
        {
            _sprites.reserve(frameCount);

            for (rapidjson::Value::ConstValueIterator it = frames.Begin(), itEnd = frames.End();
                 it != itEnd;
                 ++it)
            {
                const rapidjson::Value& frame = *it;
                if (!frame.HasMember("filename") || !frame.HasMember("frame"))
                {
                    continue;
                }
                const rapidjson::Value& frameRect = frame["frame"];
                const rapidjson::Value& spriteSourceSize = frame["spriteSourceSize"];
                const rapidjson::Value& sourceSize = frame["sourceSize"];
                int32_t frameX = frameRect["x"].GetInt();
                int32_t frameY = frameRect["y"].GetInt();
                int32_t frameW = frameRect["w"].GetInt();
                int32_t frameH = frameRect["h"].GetInt();
                int32_t drawX = spriteSourceSize["x"].GetInt();
                int32_t drawY = spriteSourceSize["y"].GetInt();
                int32_t drawW = spriteSourceSize["w"].GetInt();
                int32_t drawH = spriteSourceSize["h"].GetInt();
                int32_t sourceW = sourceSize["w"].GetInt();
                int32_t sourceH = sourceSize["h"].GetInt();
                std::string name = frame["filename"].GetString();
                
                Sprite sprite(name,
                              frameX, frameY, frameW, frameH,
                              drawX, drawY, drawW, drawH,
                              sourceW, sourceH);
                _sprites.emplace_back(sprite);
                if (name == nullBitmapName)
                {
                    _nullSprite = sprite;
                }
            }
        }
    }

    void SpriteDictionary::mapSprite(int32_t handle, const std::string& name)
    {
        for (auto& sprite : _sprites)
        {
            if (sprite.name() == name)
            {
                _dictionary.emplace(handle, &sprite);
                return;
            }
        }
        _dictionary.emplace(handle, &_nullSprite);
    }

    const Sprite& SpriteDictionary::sprite(int32_t handle) const
    {
        auto it = _dictionary.find(handle);
        if (it == _dictionary.end())
            return _nullSprite;

        return *(*it).second;
    }
}

