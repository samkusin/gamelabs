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

#ifndef CK_Sample_GenRoom_TileDatabase_Hpp
#define CK_Sample_GenRoom_TileDatabase_Hpp

#include "maptypes.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <deque>
#include <unordered_map>

namespace cinekine { namespace overview {

    class TileDatabase
    {
    public:
        TileDatabase(size_t tileLimit);

        /**
         * Maps a TileTemplate to the handle specified in the template
         * @param handle       The handle to map the template to (from 1 to
         *                     tileLimit)
         * @param tileTemplate The template data
         */
        void mapTemplate(TileHandle handle, const TileTemplate& tileTemplate);
        /**
         * Retrieve a tile by handle ID
         * @param  handle The handle
         * @return        The matching TileTemplate.  If there is no tile
         *                found, the returned template has an handle of 0
         *                which usually indicates an empty tile
         */
        const TileTemplate& tile(TileHandle handle) const;
        /**
         * Retrieves a TileTemplate from the given description/selection
         * parameters
         * @param  categoryId Tile category
         * @param  classId    Tile class
         * @param  roleFlags  Tile role
         * @return            The matching TileTemplate.  If there is no tile
         *                    found, the returned template has an handle of 0
         *                    which usually indicates an empty tile
         */
        TileHandle tileHandleFromDescriptor(uint8_t categoryId,
                                    uint8_t classId,
                                    uint16_t roleFlags) const;

    private:
        std::vector<TileTemplate> _tiles;
        std::unordered_map<uint32_t, TileHandle> _tilesByDescriptor;

        uint32_t makeDescriptor(const TileTemplate& tile) const;
    };


} /* namespace overview */ } /* namespace cinekine */

#endif
