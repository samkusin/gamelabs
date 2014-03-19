/**
 * \file    overview/map.hpp
 *
 * Shared map data object containing terrain and prop objects.
 *
 * \note    Created by Samir Sinha on 3/2/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_OVERVIEW_MAP_HPP
#define CINEK_OVERVIEW_MAP_HPP

#include "maptypes.hpp"
#include "grid.hpp"

#include <vector>


namespace cinekine {
    namespace overview {

/** Defines a grid of tiles */
typedef Grid<TileHandle> Tilemap;
/** Defines a tilemap section */
typedef GridContainer<Tilemap> TilemapContainer;

/**
 * @class Map map.hpp "cinek/overview/map.hpp"
 * @brief Contains map data for display and interaction by a game simulation.
 *
 * A map is comprised of a terrain grid of tiles, and a quad-tree of props.
 * 
 * Ownership rules:
 * - Map owns its QuadTree
 * - Map owns 
 */
class Map 
{
public:
    /** 
     *  Constructor that creates an empty map with specified dimensions.
     *  
     *  @param  bounds  Map x,y and z bounds.
     */
    Map(const MapBounds& bounds);
    /**
     * Destructor.
     */
    ~Map();

    /**
     * Retrieve map coordinate bounds.
     * Can be used to calculate an index into a tile array.
     * @return  Reference to the map bounds structure.
     */ 
    const MapBounds& bounds() const {
        return _bounds;
    }

    /**
     * Retrieve the tilemap at the specified Z layer.
     * @param z     A z-value in the range [zDown, zUp]
     * @return      A pointer to a tile map.
     */
    Tilemap* tilemapAtZ(int16_t z);
    /**
     * Retrieve the const tilemap at the specified Z layer.
     * @param z     A z-value in the range [zDown, zUp]
     * @return      A const pointer to a tile map.
     */
    const Tilemap* tilemapAtZ(int16_t z) const;

private:
    //  grid xyz
    MapBounds _bounds;
    std::vector<Tilemap> _tilemaps;
};

    } /* overview */ 
} /* cinekine */

#endif
