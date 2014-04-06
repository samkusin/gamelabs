/*
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

#ifndef CK_Sample_Room_Hpp
#define CK_Sample_Room_Hpp

#include "maptypes.hpp"

#include <array>

namespace cinekine {
    namespace overview {
        class TileDatabase;
    }
}

namespace cinekine { namespace overview {

    //
    //  Builder types
    //

    // @struct TileBrush
    // @brief
    struct TileBrush
    {
        uint8_t tileCategoryId;
        uint8_t tileClassId;
    };

    struct Box
    {
        static const Box kEmpty;

        int32_t x0,y0;      // pt1 (upper,left)
        int32_t x1,y1;      // pt2 (lower,right)

        operator bool() const {
            return (x1-x0) && (y1-y0);    // is empty?
        }

        int32_t width() const { return (x1 - x0); }
        int32_t height() const { return (y1 - y0); }

        bool inside(const Box& box) const {
            return (x0 >= box.x0 && x1 <= box.x1 && y0 >= box.y0 && y1 <= box.y1);
        }
        bool outside(const Box& box) const {
            return (x0 > box.x1 || box.x0 > x1 || y0 > box.y1 || box.y0 > y1);
        }
        bool intersects(const Box& box) const {
            return !outside(box);
        }
        Box intersection(const Box& box) const;
        Box boundTo(const Box& bounds) const;
        int32_t area() const { return width() * height(); }
        //  returns the box bounded to the specified bounds
        bool operator==(const Box& box) const {
            return x0 == box.x0 && x1 == box.x1 && y0 == box.y0 && y1 == box.y1;
        }
        bool operator!=(const Box& box) const {
            return x0 != box.x0 || x1 != box.x1 || y0 != box.y0 || y1 != box.y1;
        }
    };

    /// @struct Segment
    /// @brief  A box connected to other boxes within a larger construct (room)
    struct Segment
    {
        Box box;
        Segment();
        Segment(const Box& box);
    };


} /* namespace overview */ } /* namespace cinekine */

#endif