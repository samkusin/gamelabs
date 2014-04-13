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

#include "glm/glm.hpp"

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
        typedef glm::ivec2 Point;
        Point p0;
        Point p1;

        operator bool() const {
            return (p1.x-p0.x) && (p1.y-p0.y);    // is empty?
        }

        int32_t width() const { return (p1.x-p0.x); }
        int32_t height() const { return (p1.y-p0.y); }

        bool inside(const Box& box) const {
            return (p0.x >= box.p0.x && p1.x <= box.p1.x && p0.y >= box.p0.y && p1.y <= box.p1.y);
        }
        bool outside(const Box& box) const {
            return (p0.x > box.p1.x || box.p0.x > p1.x || p0.y > box.p1.y || box.p0.y > p1.y);
        }
        bool intersects(const Box& box) const {
            return !outside(box);
        }
        Box intersection(const Box& box) const;
        Box boundTo(const Box& bounds) const;
        int32_t area() const { return width() * height(); }
        //  returns the box bounded to the specified bounds
        bool operator==(const Box& box) const {
            return p0 == box.p0 && p1 == box.p1;
        }
        bool operator!=(const Box& box) const {
            return p0 != box.p0 || p1 != box.p1;
        }
        void clear() { p0 = Point(); p1 = Point(); }
    };


} /* namespace overview */ } /* namespace cinekine */

#endif