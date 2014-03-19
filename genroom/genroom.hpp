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

#ifndef CK_Sample_GenRoom_Hpp
#define CK_Sample_GenRoom_Hpp

#include "jobqueue/job.hpp"
#include "map.hpp"

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
    
    /// @struct Segment
    /// @brief  A single rectangular segment - the most basic element used when
    ///         building room-based maps
    struct Segment
    {
        int32_t x0,y0;      // pt1 (upper,left)
        int32_t x1,y1;      // pt2 (lower,right)

        bool intersects(const Segment& segment) const;
    };

    /// @class Room
    /// @brief  Contains one or more intersecting segments.  Rooms are often
    ///         connected to other rooms via hallways
    class Room
    {
    public:
        Room();

        void addSegment(const Segment& segment);

    private:
        std::array<Segment, 4> _segments;        
        uint32_t _segmentCount;
    };


    /// @class Architect
    /// @brief An interface for implementing the Builder's Architect, which
    ///        directs the Builder
    ///        
    /// This allows customization of building tasks by an implementing
    /// application.
    /// 
    class Architect
    {
    public:
        virtual ~Architect() {}
        //  The builder requests a new segment.   The segment defines a region
        //  at origin 0,0 to the max bounds possible for the segment.
        //  Implementations must resize and reposition the segment based on
        //  its own criterion
        virtual void onNewSegment(Segment& segment) = 0;
        //  The builder is about to paint the segment, allowing implementations
        //  to select a tileset or fine tune other properties
        virtual void onPaintSegment(TileBrush& brush,
                                    const Room& room, const Segment& segment) = 0;
    };

    /// The Builder Context
    /// Applications pass data to the Builder via the Context
    /// 
    struct Context
    {
        Architect *architect;
    };

    /// @class Builder
    /// @brief The Builder Job responsible for generating Rooms from Segments
    // 
    class Builder : public Job
    {
    public:
        Builder(Map& map,
                const TileDatabase& tileTemplates,
                uint32_t numRooms);

        virtual Result execute(JobScheduler& scheduler,
                               void* context);
    
        virtual int32_t priority() const {
            return 0;
        }

    private:
        void paintSegmentOntoMap(TileBrush& brush, const Segment& segment);

    private:
        Map& _map;
        std::vector<Room> _rooms;
        const TileDatabase& _tileTemplates;

        uint32_t _roomLimit;
    };
} /* namespace overview */ } /* namespace cinekine */

#endif
