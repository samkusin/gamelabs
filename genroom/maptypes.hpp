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

#ifndef CK_Sample_GenRoom_MapTypes_Hpp
#define CK_Sample_GenRoom_MapTypes_Hpp

#include <cstddef>
#include <cstdint>
#include <vector>

namespace cinekine { namespace overview {
    
    enum
    {
        kTileDirection_N       = 0x0001,   /**< North */
        kTileDirection_NE      = 0x0002,   /**< Northeast */
        kTileDirection_E       = 0x0004,   /**< East */
        kTileDirection_SE      = 0x0008,   /**< Southeast */
        kTileDirection_S       = 0x0010,   /**< South */
        kTileDirection_SW      = 0x0020,   /**< Southwest */
        kTileDirection_W       = 0x0040,   /**< West */
        kTileDirection_NW      = 0x0080,   /**< Northwest */
        kTileDirection_Mask    = 0x00ff    /**< Bitmask defining direction */
    };

    enum
    {
        kTileRole_Wall          = 0x0100,
        kTileRole_Corner        = 0x0200,
        kTileRole_Floor         = 0x1000
    };

    /**
     * @struct  TileTemplate
     * @brief   Information attached to a tile used by the game engine
     */
    struct TileTemplate
    {
        uint32_t bitmapHandle;      /**< The tile's bitmap */
        uint16_t roleFlags;         /**< Role flags (as defined by the app) */
        uint8_t categoryId;         /**< Tile category Id */
        uint8_t classId;            /**< Tile class Id */
        uint32_t params[2];         /**< Application defined values */

        TileTemplate() : bitmapHandle((uint32_t)(-1)) {}
    };

    typedef std::vector<TileTemplate> TileTemplates;

    /**
     * @struct  MapBounds
     * @brief   Defines bounds for a map
     */
    struct MapBounds
    {
        uint32_t xUnits;             /**< X units */
        uint32_t yUnits;             /**< Y units */
        uint32_t zUnits;             /**< Z units (layers) */
    };

} /* namespace overview */ } /* namespace cinekine */

#endif