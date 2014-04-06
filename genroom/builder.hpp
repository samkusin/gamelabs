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

#ifndef CK_Sample_GenRoom_Hpp
#define CK_Sample_GenRoom_Hpp

#include "map.hpp"
#include "buildtypes.hpp"

#include <vector>

namespace cinekine {
    namespace overview {
        class TileDatabase;
    }
}

namespace cinekine { namespace overview {

    /// @struct NewRegionRequest
    /// @brief  A request from the Builder to an Architect for a new room
    ///
    struct NewRegionRequest
    {
        /// An Architect chooses room coordinates from within this bounding
        /// segment
        Box mapBounds;
    };

    /// @struct NewRegionInstruction
    /// @brief Instructs the builder to construct a segment within a room
    ///
    struct NewRegionInstruction
    {
        enum Policy
        {
            /// Finalize the generated map signaling the end of map building
            kFinalize,
            /// Attach segment to a random side.  If the first *randomized*
            /// segment in a room then the segment is applied as is, being the
            /// root node of all subsequent segments in the room
            kRandomize,
            /// Create segment as defined - this is a brute force method,
            /// overriding the current segment tree for the room.
            kFixed
        }
        policy;                 ///< The segment's build policy
        Box box;                ///< The segment to build from the box
        bool terminal;          ///< Indicates a leaf in the segment tree
        NewRegionInstruction():
            policy(kFinalize),
            box(Box::kEmpty),
            terminal(false) {}
        NewRegionInstruction(Policy policy) :
            policy(policy),
            box(Box::kEmpty),
            terminal(false) {}
    };

    /// @struct NewRegionResponse
    /// @brief  The Architect's response to Builders instructing how to build
    ///         the region
    ///
    struct NewRegionResponse
    {
        /// Draws the region with the given tile brush parameters
        TileBrush tileBrush;
        /// Segments to build
        std::vector<NewRegionInstruction> instructions;
    };

    /// @class Architect
    /// @brief An interface for implementing the Builder's Architect, the object
    ///        directing the Builder during map construction
    ///
    class Architect
    {
    public:
        virtual ~Architect() {}
        /// Returns instructions for building a new region.
        /// @param  request  Request parameters from a Builder
        /// @return Instructions to the Builder for building the next region
        virtual NewRegionResponse onNewRegionRequest(const NewRegionRequest& request) = 0;
    };

    /// @class Builder
    /// @brief The Builder responsible for generating and connecting Rooms
    //
    class Builder
    {
    public:
        Builder(Architect& architect,
                Map& map,
                const TileDatabase& tileTemplates,
                uint32_t roomLimit);

        void update();

    private:
        void paintSegmentOntoMap(TileBrush& brush, const Segment& segment);
        void paintTileWalls(Tilemap& tileMap, uint32_t tileY, uint32_t tileX,
                            const TileBrush& brush);
        void paintTileWallCorners(Tilemap& tileMap, uint32_t tileY, uint32_t tileX,
                            const TileBrush& brush);
        bool tileFloorsClassIdEqual(const Tile& tile, uint8_t thisClassId) const;
        bool tileWallsEqual(const Tile& tile, uint16_t roleFlags, uint8_t classId) const;

    private:
        Architect& _architect;
        Map& _map;
        const TileDatabase& _tileTemplates;

        struct Region
        {
            Box bounds;
            std::vector<Segment*> segments;

            Region(): bounds() {}
        };

        std::vector<Region> _regions;
        std::vector<Segment> _segments;

        bool _building;
    };
} /* namespace overview */ } /* namespace cinekine */

#endif
