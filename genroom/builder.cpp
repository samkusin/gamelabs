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

#include "builder.hpp"
#include "maptypes.hpp"
#include "tiledatabase.hpp"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstdio>

namespace cinekine { namespace overview {

    Builder::Segment::Segment()
    {
    }

    Builder::Segment::Segment(const Box& box) :
        box(box)
    {
    }

    auto Builder::Region::closestSegmentToPoint(const MapPoint& pt) -> Segment*
    {
        //  pick closest segment to the point
        Segment* closest = nullptr;
        int d2Closest = INT32_MAX;
        for (auto segment: segments)
        {
            MapPoint ptDelta = segment->box.center() - pt;
            int d2 = glm::dot(ptDelta, ptDelta);
            if (d2 < d2Closest)
            {
                closest = segment;
                d2Closest = d2;
            }
        }
        return closest;
    }

    ////////////////////////////////////////////////////////////////////////////

    Builder::Builder(Map& map,
                const TileDatabase& tileTemplates,
                uint32_t roomLimit) :
        _map(map),
        _tileTemplates(tileTemplates)
    {
        _regions.reserve(roomLimit);
        _segments.reserve(roomLimit*8);
        _connections.reserve(roomLimit*8);

        //  clear all tilemaps
        const cinekine::overview::MapBounds& bounds = _map.bounds();

        for (uint32_t z = 0; z < bounds.zUnits; ++z)
        {
            Tilemap* tilemap = _map.tilemapAtZ(z);
            Tile zeroTile = { 0, 0 };
            tilemap->fillWithValue(zeroTile, 0, 0, bounds.yUnits, bounds.xUnits);
        }

    }

    int Builder::makeRegion(const TileBrush& tileBrush,
                    const std::vector<NewRegionInstruction>& instructions)
    {
        //  Send a request for a new room within our entire map bounds
        //  the builder currently is pretty simple - it requests a room and then
        //  paints it.
        //
        int regionIndex = -1;
        Region* region = nullptr;
        bool finalize = false;
        for (auto& cmd : instructions)
        {
            Box segBox = cmd.box;

            switch (cmd.policy)
            {
            case NewRegionInstruction::kRandomize:
                break;
            case NewRegionInstruction::kFixed:
                break;
            default:
                finalize = true;
                break;
            }

            if (_segments.size() >= _segments.capacity())
            {
                finalize = true;
            }

            if (!finalize)
            {
                if (!region)
                {
                    _regions.emplace_back();
                    region = &_regions.back();
                    regionIndex = (int)_regions.size()-1;
                }
                _segments.emplace_back(segBox);
                auto* segment = &_segments.back();

                region->segments.push_back(segment);

                paintBoxOntoMap(tileBrush, segment->box);
            }
            else
            {
                break;
            }
        }

        return regionIndex;
    }

    int Builder::connectRegions(const TileBrush& tileBrush,
                        int startRegionHandle, int endRegionHandle,
                        const std::vector<MapPoint>& connectPoints)
    {
        if (_connections.size() >= _connections.capacity())
            return -1;
        if (startRegionHandle >= _regions.size())
            return -1;
        if (endRegionHandle >= _regions.size())
            return -1;
        if (startRegionHandle == endRegionHandle)
            return -1;

        //  Paint connection onto map
        //  From the closest segment to the end region in the start region to
        //  the closest segment in the start region from the closest end room
        //  segment - these points mark our start and end points for the
        //  region's connection.
        Region& startRegion = _regions[startRegionHandle];
        Region& endRegion = _regions[endRegionHandle];
        Segment* startSegment = startRegion.closestSegmentToPoint(endRegion.bounds.center());
        if (!startSegment)
            return -1;      // only if an empty start region
        Segment* endSegment = endRegion.closestSegmentToPoint(startSegment->box.center());
        if (!endSegment)
            return -1;      // only if an empty end retion
        startSegment = startRegion.closestSegmentToPoint(endSegment->box.center());

        //  paint all connections
        MapPoint startPoint = startSegment->box.center();
        MapPoint endPoint = endSegment->box.center();
        const MapPoint* connectStartPoint = &startPoint;

        for (auto& connectPoint : connectPoints)
        {
            const MapPoint* connectEndPoint = &connectPoint;
            paintConnectionOntoMap(tileBrush, *connectStartPoint, *connectEndPoint);
            connectStartPoint = connectEndPoint;
        }

        paintConnectionOntoMap(tileBrush, *connectStartPoint, endPoint);

        _connections.emplace_back();
        Connection& connection = _connections.back();
        connection.regionA = startRegionHandle;
        connection.regionB = endRegionHandle;

        return (int)_connections.size()-1;
    }

    void Builder::paintConnectionOntoMap(const TileBrush& brush,
                    const MapPoint& p0,
                    const MapPoint& p1)
    {
        //  paint a hallway line using the desired plot method -
        //  by default, the method is orthogonal (half-rectangle) from p0 to p1
        const int32_t kHallSize = 3;
        const Box kHallXBox = {
            Box::Point(-kHallSize/2,-kHallSize/2),
            Box::Point((kHallSize+1)/2,(kHallSize+1)/2)
        };
        MapPoint delta = p1 - p0;
        if (delta.x)
            delta.x /= abs(delta.x);
        if (delta.y)
            delta.y /= abs(delta.y);
        MapPoint ptCurrent = p0;
        int xy = rand() % 2;

        while (ptCurrent != p1)
        {
            paintBoxOntoMap(brush, kHallXBox + ptCurrent);
            if (xy == 0)            // x dominant
            {
                if (ptCurrent.x == p1.x)
                {
                    xy = 1;
                }
                else
                {
                    ptCurrent.x += delta.x;
                }
            }
            else                    // y dominant
            {
                if (ptCurrent.y == p1.y)
                {
                    xy = 0;
                }
                else
                {
                    ptCurrent.y += delta.y;
                }
            }
            paintBoxOntoMap(brush, kHallXBox + ptCurrent);
        }

    }

    //  the segment is guaranteed to lie entirely within the map's bounds
    //  the paint action is a multi step process:
    //      - step 1, paint a rect of floor tiles
    //      - step 2, paint a rect of wall tiles
    //
    void Builder::paintBoxOntoMap(const TileBrush& brush, const Box& box)
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
        tileMap->fillWithValue(tile, box.p0.y, box.p0.x,
                                 box.height(),
                                 box.width());

        //  paint walls into this segment
        //  this is a two step process - painting wall tiles, and a second
        //      pass for wall corners to "fill in the gaps" created from the
        //      first pass.
        //
        uint32_t yPos;
        for (yPos = box.p0.y; yPos < box.p1.y; ++yPos)
        {
            for (uint32_t xPos = box.p0.x; xPos < box.p1.x; ++xPos)
            {
                paintTileWalls(*tileMap, yPos, xPos, brush);
            }
        }
        for (yPos = box.p0.y; yPos < box.p1.y; ++yPos)
        {
            for (uint32_t xPos = box.p0.x; xPos < box.p1.x; ++xPos)
            {
                paintTileWallCorners(*tileMap, yPos, xPos, brush);
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

        if (wallMask)
        {
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

            wallRoleFlags |= kTileRole_Wall;
        }

        TileHandle wallTileHandle =
            _tileTemplates.tileHandleFromDescriptor(brush.tileCategoryId,
                                                brush.tileClassId,
                                                wallRoleFlags);

        Tile& thisTile = tileMap.at(tileY, tileX);
        thisTile.wall = wallTileHandle;
    }

    bool Builder::tileFloorsClassIdEqual(const Tile& tile, uint8_t thisClassId) const
    {
        const TileTemplate& floorTemplate = _tileTemplates.tile(tile.floor);
        return floorTemplate.classId == thisClassId;
    }

    void Builder::paintTileWallCorners(Tilemap& tileMap, uint32_t tileY, uint32_t tileX,
                            const TileBrush& brush)
    {
        uint16_t cornerMask = kTileDirection_N | kTileDirection_W |
                    kTileDirection_S | kTileDirection_E;

        Tile& thisTile = tileMap.at(tileY, tileX);
        if (thisTile.wall)
        {
            // this tile already has a wall - no need to run tests
            return;
        }

        //printf("at (%u,%u) => ", tileX, tileY);

        uint16_t wallRoleFlags = 0;

        if (tileY > 0)
        {
            const Tile& north = tileMap.at(tileY-1, tileX);
            //printf("north:[%u,%u], ", north.floor, north.wall);
            if (!tileWallsEqual(north, kTileDirection_W, brush.tileClassId) &&
                !tileWallsEqual(north, kTileDirection_E, brush.tileClassId) &&
                !tileWallsEqual(north, kTileDirection_NW, brush.tileClassId) &&
                !tileWallsEqual(north, kTileDirection_NE, brush.tileClassId))
            {
                cornerMask &= ~(kTileDirection_N);
            }
        }
        if (tileX > 0)
        {
            const Tile& west = tileMap.at(tileY, tileX-1);
            //printf("west:[%u,%u], ", west.floor, west.wall);
            if (!tileWallsEqual(west, kTileDirection_N, brush.tileClassId) &&
                !tileWallsEqual(west, kTileDirection_S, brush.tileClassId) &&
                !tileWallsEqual(west, kTileDirection_NW, brush.tileClassId) &&
                !tileWallsEqual(west, kTileDirection_SW, brush.tileClassId))
            {
                cornerMask &= ~(kTileDirection_W);
            }
        }
        if (tileY < tileMap.rowCount()-1)
        {
            const Tile& south = tileMap.at(tileY+1, tileX);
            //printf("south:[%u,%u], ", south.floor, south.wall);
            if (!tileWallsEqual(south, kTileDirection_W, brush.tileClassId) &&
                !tileWallsEqual(south, kTileDirection_E, brush.tileClassId) &&
                !tileWallsEqual(south, kTileDirection_SW, brush.tileClassId) &&
                !tileWallsEqual(south, kTileDirection_SE, brush.tileClassId))
            {
                cornerMask &= ~(kTileDirection_S);
            }
        }
        if (tileX < tileMap.columnCount()-1)
        {
            const Tile& east = tileMap.at(tileY, tileX+1);
            //printf("east:[%u,%u], ", east.floor, east.wall);
            if (!tileWallsEqual(east, kTileDirection_S, brush.tileClassId) &&
                !tileWallsEqual(east, kTileDirection_N, brush.tileClassId) &&
                !tileWallsEqual(east, kTileDirection_NE, brush.tileClassId) &&
                !tileWallsEqual(east, kTileDirection_SE, brush.tileClassId))
            {
                cornerMask &= ~(kTileDirection_E);
            }
        }

        if (cornerMask & kTileDirection_W)
        {
            if (cornerMask & kTileDirection_N)
                wallRoleFlags |= kTileDirection_NW;
            else  if (cornerMask & kTileDirection_S)
                wallRoleFlags |= kTileDirection_SW;
        }
        if (!wallRoleFlags && (cornerMask & kTileDirection_N))
        {
            if (cornerMask & kTileDirection_E)
                wallRoleFlags |= kTileDirection_NE;
        }
        if (!wallRoleFlags && (cornerMask & kTileDirection_E))
        {
            if (cornerMask & kTileDirection_S)
                wallRoleFlags |= kTileDirection_SE;
        }
        //printf("cornerFlags, wallRoleFlags (%04x,%04x)\n",
        //    cornerMask, wallRoleFlags);
        wallRoleFlags |= (kTileRole_Wall+kTileRole_Corner);

        TileHandle wallTileHandle =
            _tileTemplates.tileHandleFromDescriptor(brush.tileCategoryId,
                                                brush.tileClassId,
                                                wallRoleFlags);

        thisTile.wall = wallTileHandle;
    }

    bool Builder::tileWallsEqual(const Tile& tile, uint16_t roleFlags, uint8_t classId) const
    {
        const TileTemplate& wallTemplate = _tileTemplates.tile(tile.wall);
        return wallTemplate.classId == classId &&
              (wallTemplate.roleFlags & roleFlags)==roleFlags;
    }

} /* namespace overview */ } /* namespace cinekine */
