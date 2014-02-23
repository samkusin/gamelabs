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

/*
 *  This is a C++ implementation of Raymond Hill's Javascript implementation of
 *  Steven Fortune's algorithm to generate Voronoi diagrams.  Note it is roughly
 *  a line-by-line translation of the original Javascript to C++11 (with
 *  obvious optimizations and modifications tailored to C++)
 *
 *  The original license including links to the original source is supplied
 *  below.  Where appropriate, I've kept the original source comments explaining
 *  the algorithm, and highlighted sections I've made changes to (ssinha)
 *
 *  Copyright (C) 2010-2013 Raymond Hill: https://github.com/gorhill/Javascript-Voronoi
 *  MIT License: See https://github.com/gorhill/Javascript-Voronoi/LICENSE.md
 */

#ifndef CK_VORONOI_HPP
#define CK_VORONOI_HPP

#include "./rbtree.hpp"

#include <vector>
#include <cmath>

namespace cinekine
{
    namespace voronoi
    {

    /**
     * @class Vertex
     * @brief A 2D vertex used during voronoi computation
     */
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(float _x, float _y): x(_x), y(_y) {}
        static const Vertex undefined;
        operator bool() const {
            return !std::isnan(x) && !std::isnan(y);
        }
        float x, y;
    };
        
    inline bool operator==(const Vertex& v1, const Vertex& v2)
    {
        return v1.x == v2.x && v1.y == v2.y;
    }

    inline bool operator!=(const Vertex& v1, const Vertex& v2)
    {
        return v1.x != v2.x || v1.y != v2.y;
    }

    /**
     * @struct Site
     * @brief  Extends Site - Site metadata built on top of the site's position
     *         (a 2D vertex)
     */
    struct Site: public Vertex
    {
        int cell;

        Site(const Vertex& v) : Vertex(v.x, v.y), cell(-1) {}
        Site(): cell(-1) {}
    };

    /**
     * @struct Edge
     * @brief  Defines an edge of a Voronoi Cell and its placement
     *         relative to other Sites
     * Note that p0 and p1 are invalid unless explicitly set
     */
    struct Edge
    {
        int leftSite;
        int rightSite;
        Vertex p0;
        Vertex p1;

        Edge() :
            leftSite(-1), rightSite(-1),
            p0(Vertex::undefined),
            p1(Vertex::undefined) {}

        Edge(int lSite, int rSite) :
            leftSite(lSite), rightSite(rSite),
            p0(Vertex::undefined),
            p1(Vertex::undefined) {}
        
        void setStartpoint(int lSite, int rSite,
                           const Vertex& vertex);
        void setEndpoint(int lSite, int rSite,
                         const Vertex& vertex);
    };

    inline void Edge::setStartpoint(int lSite, int rSite,
                             const Vertex& vertex)
    {
        if (!p0 && !p1)
        {
            p0 = vertex;
            leftSite = lSite;
            rightSite = rSite;
        }
        else if (leftSite == rSite)
        {
            p1 = vertex;
        }
        else
        {
            p0 = vertex;
        }
    }

    inline void Edge::setEndpoint(int lSite, int rSite,
                           const Vertex& vertex)
    {
        setStartpoint(rSite, lSite, vertex);
    }

    /**
     * @struct HalfEdge
     * @brief  An edge segment as it relates to a single site (versus a
     *         full edge as it relates to two sites.)
     */
    struct HalfEdge
    {
        int site;
        int edge;
        float angle;
    };

    /** A half edges container */
    typedef std::vector<HalfEdge> HalfEdges;

    /**
     * @struct Cell
     * @brief  A cell containing a site surrounded by edges.
     *
     * It's possible to optimize this, specifying a start and end point to
     * a common HalfEdge vector (or pool?)
     * 
     */
    struct Cell
    {
        int site;
        HalfEdges halfEdges;
        bool closeMe;

        Cell(int s) :
            site(s),
            halfEdges(),
            closeMe(false) {}
    };

    /** A Site container */
    typedef std::vector<Site> Sites;
    /** An edges container */
    typedef std::vector<Edge> Edges;
    /** A cells container */
    typedef std::vector<Cell> Cells;

    class Fortune;

    /**
     * @class Graph
     * @brief A Voronoi cell graph from a collection of sites
     */
    class Graph
    {
    public:
        Graph();
        Graph(float xBound, float yBound, Sites&& sites);
        Graph(Graph&& other);

        Graph& operator=(Graph&& other);

        const Sites& sites() const {
            return _sites;
        }
        const Cells& cells() const {
            return _cells;
        }
        const Edges& edges() const {
            return _edges;
        }

    private:
        friend class Fortune;
        friend Graph build(Sites&& sites, float xBound, float yBound);

        int createEdge(int left, int right,
                       const Vertex& va=Vertex::undefined,
                       const Vertex& vb=Vertex::undefined);
        int createBorderEdge(int site,
                             const Vertex& va, const Vertex& vb);
        HalfEdge createHalfEdge(int edge, int lSite, int rSite);

        void clipEdges();
        bool clipEdge(int32_t edge);
        bool connectEdge(int edgeIdx);
        
        void closeCells();
        bool prepareHalfEdgesForCell(int32_t cell);

        Vertex getHalfEdgeStartpoint(const HalfEdge& halfEdge);
        Vertex getHalfEdgeEndpoint(const HalfEdge& halfEdge);

    private:
        Sites _sites;

        Edges _edges;
        Cells _cells;
    
        float _xBound;
        float _yBound;
    };

    ///////////////////////////////////////////////////////////////////////

    struct BeachArc;

    struct CircleEvent : RBNodeBase<CircleEvent>
    {
        BeachArc* arc;
        int site;
        float x;
        float y;
        float yCenter;

        CircleEvent() :
            arc(nullptr),
            site(-1),
            x(0.0f), y(0.0f), yCenter(0.0f) {}
    };

    struct BeachArc : RBNodeBase<BeachArc>
    {
        int site;
        int edge;
        int refcnt;

        CircleEvent* circleEvent;

        BeachArc(int s) :
            site(s),
            edge(-1),
            refcnt(0),
            circleEvent(nullptr) {}
    };

    class Fortune
    {
    public:
        Fortune(Graph& graph);
        ~Fortune();

        void addBeachSection(int site);
        void removeBeachSection(BeachArc* arc);

        CircleEvent* circleEvent() {
            return _topCircleEvent;
        }

    private:
        Graph& _graph;
        const Sites& _sites;
        Edges& _edges;

        RBTree<BeachArc> _beachline;
        RBTree<CircleEvent> _circleEvents;
        CircleEvent* _topCircleEvent;

        int _arcCnt, _circleCnt;
        
        BeachArc* allocArc(int site) {
            BeachArc* arc = new BeachArc(site);
            ++arc->refcnt;
            ++_arcCnt;
            return arc;
        }
        void releaseArc(BeachArc* arc) {
            if (arc->refcnt > 0)
            {
                --arc->refcnt;
                if (!arc->refcnt)
                {
                    --_arcCnt;
                    delete arc;
                }
            }
            else
            {
                printf("Releasing after 0 refcnt\n");
            }
        }

        CircleEvent* allocCircleEvent(BeachArc* arc) {
            auto event = new CircleEvent();
            event->arc = arc;
            ++event->arc->refcnt;
            ++_circleCnt;
            return event;
        }
        void freeCircleEvent(CircleEvent* event) {
            releaseArc(event->arc);
            --_circleCnt;
            delete event;
        }

        void attachCircleEvent(BeachArc* arc);
        void detachCircleEvent(BeachArc* arc);
        void detachBeachSection(BeachArc* arc);
        float leftBreakPoint(BeachArc* arc, float directrix);
        float rightBreakPoint(BeachArc* arc, float directrix);
    };

    //  Builds a graph given a collection of sites and a bounding box
    //  
    Graph build(Sites&& sites, float xBound, float yBound);

    }   // namespace voronoi
}   // namespace cinekine

#endif
