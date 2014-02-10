/**
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

#include "../voronoi.hpp"

#include <ctime>
#include <cstdio>
#include <chrono>
#include <cstdlib>


cinekine::voronoi::Sites createSites(size_t count, float xBound, float yBound)
{
	cinekine::voronoi::Sites sites;
	sites.reserve(count);
	
	//	generate
	while (count--)
	{
		sites.emplace_back(
			cinekine::voronoi::Vertex(rand()*xBound/RAND_MAX,
								      rand()*yBound/RAND_MAX));
	}

	return sites;
}


//  ssinha - simple non graphical demo.
//
int main(int argc, const char* argv[])
{
	srand((uint32_t)time(NULL));

	const float xBound = 100.0f;
	const float yBound = 100.0f;
    
    printf("********************************\n");
    printf("Voronoi graph (%.2fx%.2f)\n", xBound, yBound);
    printf("********************************\n");

	cinekine::voronoi::Sites sites = createSites(16, xBound, yBound);

    std::chrono::high_resolution_clock::time_point start =
        std::chrono::high_resolution_clock::now();
	cinekine::voronoi::Graph graph =
		cinekine::voronoi::build(std::move(sites), xBound, yBound);
    std::chrono::high_resolution_clock::time_point end =
        std::chrono::high_resolution_clock::now();
   
    std::chrono::duration<double> span =
        std::chrono::duration_cast<std::chrono::duration<double>>(end-start);
    printf("- %lu cells generated in %lf seconds\n", graph.sites().size(), span.count());

    auto& cellsites = graph.sites();
    auto& cells = graph.cells();
    auto& edges = graph.edges();
    
    for (auto& cell: cells)
    {
    	auto& site = cellsites[cell.site];

    	printf("Cell[%d]: ", site.cell);
    	printf("(%.2f,%.2f), edges[%lu]=> [\n", site.x, site.y, cell.halfEdges.size());
    	for (auto& halfedge : cell.halfEdges)
    	{
    		auto& edge = edges[halfedge.edge];
    		printf("    { start:(%.2f,%.2f), end:(%.2f,%.2f) }\n",
               edge.p0.x, edge.p0.y,
               edge.p1.x, edge.p1.y);
    	}
    	printf("]\n\n");
    }
    
	return 0;
}
