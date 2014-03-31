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
#include "tiledatabase.hpp"

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
                     const TileDatabase& tileTemplates,
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
            Tilemap* tilemap = _map.tilemapAtZ(z);
            Tile zeroTile = { 0, 0 };
            tilemap->fillWithValue(zeroTile, 0, 0, bounds.yUnits, bounds.xUnits);
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
        TileHandle floorTileId = _tileTemplates.tileHandleFromDescriptor(
                                        brush.tileCategoryId,
                                        brush.tileClassId,
                                        kTileRole_Floor);

        Tile tile;
        tile.floor = floorTileId;
        tile.wall = 0;

        cinekine::overview::Tilemap* tileMap = _map.tilemapAtZ(0);
        tileMap->fillWithValue(tile, segment.y0, segment.x0,
                                 (segment.y1 - segment.y0)+1,
                                 (segment.x1 - segment.x0)+1);

        //  paint walls into this segment
        //  iterate through all the cells in this segment
        //      evaluate wall segment based on the adjacent wall and floor tiles
        //
        for (uint32_t yPos = segment.y0; yPos <= segment.y1; ++yPos)
        {
            for (uint32_t xPos = segment.x0; xPos <= segment.x1; ++xPos)
            {
                paintTileWalls(*tileMap, yPos, xPos, brush);
            }
        }
    }

    void Builder::paintTileWalls(Tilemap& tileMap, uint32_t tileY, uint32_t tileX,
                                const TileBrush& brush)
    {
        const TileTemplate& thisFloorTemplate =
            _tileTemplates.tile(tileMap.at(tileY, tileX).floor);

        //  calculate wall masks, which are used to determine what wall tiles
        //  to display.
        uint16_t wallMask = kTileDirection_N | kTileDirection_E |
            kTileDirection_W | kTileDirection_S;

        if (tileY > 0 &&
            tileFloorsClassIdEqual(tileMap.at(tileY-1, tileX), thisFloorTemplate.classId))
        {
            wallMask &= ~(kTileDirection_N);
        }
        if (tileX > 0 &&
            tileFloorsClassIdEqual(tileMap.at(tileY, tileX-1), thisFloorTemplate.classId))
        {
            wallMask &= ~(kTileDirection_W);
        }
        if (tileY < tileMap.rowCount()-1 &&
            tileFloorsClassIdEqual(tileMap.at(tileY+1, tileX), thisFloorTemplate.classId))
        {
            wallMask &= ~(kTileDirection_S);
        }
        if (tileX < tileMap.columnCount()-1 &&
            tileFloorsClassIdEqual(tileMap.at(tileY, tileX+1), thisFloorTemplate.classId))
        {
            wallMask &= ~(kTileDirection_E);
        }

        uint16_t wallRoleFlags = 0;
        if (wallMask & kTileDirection_W)
        {
            if (wallMask & kTileDirection_N)
                wallRoleFlags |= kTileDirection_NW;
            else if (wallMask & kTileDirection_S)
                wallRoleFlags |= kTileDirection_SW;
            else
                wallRoleFlags |= kTileDirection_W;
        }
        if (!wallRoleFlags && (wallMask & kTileDirection_N))
        {
            //  we've already evaluated for West, so only need to eval East
            if (wallMask & kTileDirection_E)
                wallRoleFlags |= kTileDirection_NE;
            else
                wallRoleFlags |= kTileDirection_N;
        }
        if (!wallRoleFlags && (wallMask & kTileDirection_E))
        {
            //  we've already evaluated North, so only care about South
            if (wallMask & kTileDirection_S)
                wallRoleFlags |= kTileDirection_SE;
            else
                wallRoleFlags |= kTileDirection_E;
        }
        if (!wallRoleFlags && (wallMask & kTileDirection_S))
        {
            //  we've already evaluated East and West, so...
            wallRoleFlags |= kTileDirection_S;
        }

        TileHandle wallTileHandle =
            _tileTemplates.tileHandleFromDescriptor(brush.tileCategoryId,
                                                brush.tileClassId,
                                                wallRoleFlags | kTileRole_Wall);

        Tile& thisTile = tileMap.at(tileY, tileX);
        thisTile.wall = wallTileHandle;

        //uint32_t cornerMask;
    }

    bool Builder::tileFloorsClassIdEqual(const Tile& tile, uint8_t thisClassId) const
    {
        const TileTemplate& floorTemplate = _tileTemplates.tile(tile.floor);
        return floorTemplate.classId == thisClassId;
    }

} /* namespace overview */ } /* namespace cinekine */
