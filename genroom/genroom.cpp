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

#include "genroom.hpp"
#include "maptypes.hpp"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstdio>

namespace cinekine { namespace overview {

    bool Segment::intersects(const Segment& segment) const
    {
        return false;
    }

    Room::Room() :
        _segmentCount(0)
    {
    }

    void Room::addSegment(const Segment& segment)
    {
        if (_segmentCount >= _segments.size())
            return;

        _segments[_segmentCount] = segment;
    }

    Builder::Builder(Map& map,
                     const TileTemplates& tileTemplates,
                     uint32_t numRooms) :
        _map(map),
        _tileTemplates(tileTemplates),
        _roomLimit(numRooms)
    {
        _rooms.reserve(_roomLimit);

        //  clear all tilemaps
        const cinekine::overview::MapBounds& bounds = _map.bounds();

        for (uint32_t z = 0; z < bounds.zUnits; ++z)
        {
            cinekine::overview::Tilemap* tilemap = _map.tilemapAtZ(z);
            tilemap->fillWithValue(0, 0, 0, bounds.yUnits, bounds.xUnits);
        }

    }

    auto Builder::execute(JobScheduler& scheduler,
                          void* context) -> Result
    {
        Context& ctx = *reinterpret_cast<Context*>(context);
        if (!ctx.architect)
            return kTerminate;

        Segment segment;
        segment.x0 = 0;
        segment.y0 = 0;
        segment.x1 = _map.bounds().xUnits - 1;
        segment.y1 = _map.bounds().yUnits - 1;

        //  allow the architect to define the segment's bounds
        ctx.architect->onNewSegment(segment);

        _rooms.emplace_back();
        Room& room = _rooms.back();
        room.addSegment(segment);

        //  draw it onto the map
        TileBrush tileBrush;
        ctx.architect->onPaintSegment(tileBrush, room, segment);
        paintSegmentOntoMap(tileBrush, segment);

        if (_rooms.size() >= _roomLimit)
            return kTerminate;

        return kReschedule;
    }

    //  the segment is guaranteed to lie entirely within the map's bounds
    //  the paint action is a multi step process:
    //      - step 1, paint a rect of floor tiles
    //      - step 2, paint a rect of wall tiles
    //
    void Builder::paintSegmentOntoMap(TileBrush& brush, const Segment& segment)
    {    
        //  paint floor
        uint16_t floorTileId = 0;
        for (auto& tileTemplate: _tileTemplates)
        {
            if (tileTemplate.categoryId == brush.tileCategoryId &&
                tileTemplate.classId == brush.tileClassId)
            {
                if (tileTemplate.roleFlags & kTileRole_Floor)
                {
                    floorTileId = (uint16_t)tileTemplate.bitmapHandle;
                }
            }
        }
        cinekine::overview::Tilemap* gridFloor = _map.tilemapAtZ(0);
        gridFloor->fillWithValue(floorTileId, segment.y0, segment.x0,
                                 (segment.y1 - segment.y0)+1,
                                 (segment.x1 - segment.x0)+1);
        //  paint walls
        cinekine::overview::Tilemap* gridWall = _map.tilemapAtZ(1);

    }
    
} /* namespace overview */ } /* namespace cinekine */
