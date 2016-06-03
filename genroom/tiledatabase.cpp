/*
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

#include "tiledatabase.hpp"

#include <algorithm>
#include <cstdlib>

namespace cinekine { namespace overview {

    TileDatabase::TileDatabase(size_t tileLimit)
    {
        _tiles.resize(tileLimit);
    }

    void TileDatabase::mapTemplate(TileHandle tileHandle,
                                   const TileTemplate& tileTemplate)
    {
        if (tileHandle >= _tiles.size())
            return;

        //  remove any old descriptor mapping associated with the supplied
        //  tileHandle  - it will be remapped once we've mapped a new template
        //  to the handle
        uint32_t descriptor = makeDescriptor(_tiles[tileHandle]);
        _tilesByDescriptor.erase(descriptor);

        //  register new tile and descriptor
        _tiles[tileHandle] = tileTemplate;
        descriptor = makeDescriptor(tileTemplate);
        _tilesByDescriptor.emplace(descriptor, tileHandle);
    }

    const TileTemplate& TileDatabase::tile(TileHandle handle) const
    {
        if (handle >= _tiles.size())
        {
            handle = 0;
        }
        return _tiles[handle];
    }

    TileHandle TileDatabase::tileHandleFromDescriptor(
                                    uint8_t categoryId,
                                    uint8_t classId,
                                    uint16_t roleFlags) const
    {
        TileTemplate tile;
        tile.categoryId = categoryId;
        tile.classId = classId;
        tile.roleFlags = roleFlags;
        uint32_t descriptor = makeDescriptor(tile);
        auto it = _tilesByDescriptor.find(descriptor);
        if (it == _tilesByDescriptor.end())
            return 0;

        return it->second;
    }

    uint32_t TileDatabase::makeDescriptor(const TileTemplate& tile) const
    {
        uint32_t descriptor = tile.roleFlags;
        descriptor <<= 8;
        descriptor += tile.categoryId;
        descriptor <<= 8;
        descriptor += tile.classId;
        return descriptor;
    }


} /* namespace overview */ } /* namespace cinekine */
