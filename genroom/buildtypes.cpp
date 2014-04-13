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

    Box Box::intersection(const Box& box) const
    {
        Box result;
        if (outside(box))
            return result;
        result.p0 = Point(std::max(p0.x, box.p0.x), std::max(p0.y, box.p0.y));
        result.p1 = Point(std::min(p1.x, box.p1.x), std::min(p1.y, box.p1.y));
        return result;
    }

    Box Box::boundTo(const Box& bounds) const
    {
        Box result = { p0, p1 };
        if (result.p0.x < bounds.p0.x)
            result.p0.x = bounds.p0.x;
        if (result.p1.x > bounds.p1.x)
            result.p1.x = bounds.p1.x;
        if (result.p0.y < bounds.p0.y)
            result.p0.y = bounds.p0.y;
        if (result.p1.y > bounds.p1.y)
            result.p1.y = bounds.p1.y;
        return result;
    }

} /* namespace overview */ } /* namespace cinekine */