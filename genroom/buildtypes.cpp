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

#include "buildtypes.hpp"
#include "maptypes.hpp"
#include "tiledatabase.hpp"


namespace cinekine { namespace overview {

    const Box Box::kEmpty = { 0, 0, 0, 0 };

    Box Box::intersection(const Box& box) const
    {
        Box result = { 0, 0, 0, 0 };
        if (outside(box))
            return result;

        result.x0 = std::max(x0, box.x0);
        result.x1 = std::min(x1, box.x1);
        result.y0 = std::max(y0, box.y0);
        result.y1 = std::min(y1, box.y1);

        return result;
    }

    Box Box::boundTo(const Box& bounds) const
    {
        Box result = { x0, y0, x1, y1 };
        if (result.x0 < bounds.x0)
            result.x0 = bounds.x0;
        if (result.x1 > bounds.x1)
            result.x1 = bounds.x1;
        if (result.y0 < bounds.y0)
            result.y0 = bounds.y0;
        if (result.y1 > bounds.y1)
            result.y1 = bounds.y1;
        return result;
    }

    Segment::Segment() :
        box { 0, 0, 0, 0 }
    {
    }

    Segment::Segment(const Box& box) :
        box(box)
    {
    }

} /* namespace overview */ } /* namespace cinekine */