/**
 * \file    overview/map.cpp
 *
 * Shared map data object containing terrain and prop objects.
 *
 * \note    Created by Samir Sinha on 3/2/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "map.hpp"

namespace cinekine {
    namespace overview {

Map::Map(const MapBounds& bounds) :
    _bounds(bounds),
    _tilemaps()
{
    //  validation
    //
    uint32_t layerCount = _bounds.zUnits;
    if (layerCount==0)
    {
        _bounds.zUnits = 1;
        layerCount = 1;
    }
    if (!_bounds.xUnits || !_bounds.yUnits)
    {
        _bounds.xUnits = 1;
        _bounds.yUnits = 1;
    }

    //  allocate tilemaps
    //      Tilemaps are not copyable - so can't initialize via initializer lists.
    _tilemaps.reserve(layerCount);
    for (uint32_t i = 0; i < layerCount; ++i)
    {
        _tilemaps.push_back({ _bounds.xUnits, _bounds.yUnits} );
    }
}


Map::~Map()
{
}


Tilemap* Map::tilemapAtZ(int16_t z)
{
    return const_cast<Tilemap*>(
            reinterpret_cast<const Map*>(this)->tilemapAtZ(z)
        );
}

const Tilemap* Map::tilemapAtZ(int16_t z) const
{
    if (z < 0 || z >= _bounds.zUnits)
        return nullptr;
    return &_tilemaps[z];    
}

    } /* overview */ 
} /* cinekine */
