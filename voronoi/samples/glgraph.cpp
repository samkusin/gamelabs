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

#include <glfw3.h>

#include <ctime>
#include <cstdio>
#include <chrono>
#include <cstdlib>

int g_width = 0;
int g_height = 0;
bool g_resized = false;
bool g_redraw = false;

cinekine::voronoi::Graph g_voronoiGraph;

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

void rebuild()
{
    const float xBound = g_width;
    const float yBound = g_height;

    printf("********************************\n");
    printf("Voronoi graph (%.2fx%.2f)\n", xBound, yBound);
    printf("********************************\n");

    const size_t maxSites = 126;

    cinekine::voronoi::Sites sites = createSites((rand() % maxSites)+2, xBound, yBound);

    std::chrono::high_resolution_clock::time_point start =
        std::chrono::high_resolution_clock::now();
    cinekine::voronoi::Graph graph =
        cinekine::voronoi::build(std::move(sites), xBound, yBound);
    std::chrono::high_resolution_clock::time_point end =
        std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> span =
        std::chrono::duration_cast<std::chrono::duration<double>>(end-start);

    g_voronoiGraph = std::move(graph);
    printf("- %lu cells generated in %lf seconds\n", g_voronoiGraph.cells().size(), span.count());
}

void draw()
{
    glClearColor(0.0f, 0.0f, 1.0f, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    const cinekine::voronoi::Cells& cells = g_voronoiGraph.cells();
    const cinekine::voronoi::Edges& edges = g_voronoiGraph.edges();
    const cinekine::voronoi::Sites& sites = g_voronoiGraph.sites();

    glBegin(GL_TRIANGLES);

    for (auto& cell : cells)
    {
        /*const auto& site = sites[cell.site];
        printf("\nCell @ (%.2f,%.2f)\n", sites[cell.site].x, sites[cell.site].y);
        */
        //  reorder vertices to ease triangulation of the cell's polygon
        auto& halfEdges = cell.halfEdges;
        size_t halfEdgeCount = halfEdges.size();

        const cinekine::voronoi::Vertex* vA = nullptr;
        const cinekine::voronoi::Vertex* vB = nullptr;
        const cinekine::voronoi::Vertex* v0 = nullptr;

        float r = 0.2f+(0.8f * rand())/RAND_MAX;
        float g = 0.2f+(0.8f * rand())/RAND_MAX;
        float b = 0.2f+(0.8f * rand())/RAND_MAX;
        glColor3f(r, g, b);

/*
        printf("Cell: (%d) =>(%.2f,%.2f)\n", cell.site, site.x, site.y);
        for (auto& halfEdge : halfEdges)
        {
            auto& edge = edges[halfEdge.edge];
            printf("\tEdge[%d] : (%.2f,%.2f) => (%.2f,%.2f)\n",
                   halfEdge.edge,
                   edge.p0.x, edge.p0.y,
                   edge.p1.x, edge.p1.y);
        }
*/
        for (int he = 0; he < halfEdgeCount-1; ++he)
        {
            auto* thisEdge = &edges[halfEdges[he].edge];
            auto* nextEdge = &edges[halfEdges[(he+1)%halfEdgeCount].edge];

            if (thisEdge->p0 == nextEdge->p0 || thisEdge->p0 == nextEdge->p1)
            {
                vA = &thisEdge->p1;
                vB = &thisEdge->p0;
            }
            else
            {
                vA = &thisEdge->p0;
                vB = &thisEdge->p1;
            }

            if (!v0)
            {
                v0 = vA;
                continue;
            }
            glVertex2f(v0->x, v0->y);
            glVertex2f(vA->x, vA->y);
            glVertex2f(vB->x, vB->y);
//            printf("(%.2f,%.2f,%.2f) => ", r, g, b);
//            printf("(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n",
//                   v0->x, v0->y,
//                   vA->x, vA->y,
//                   vB->x, vB->y);
        }
//        printf("========================\n");
    }

    glEnd();

    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 0.0f);
    for (auto& site : sites)
    {
        glVertex2f(site.x, site.y);
    }
    glEnd();
}

void resizeCb(GLFWwindow* window, int w, int h)
{
    g_width = w;
    g_height = h;
    g_resized = true;
}

void refreshCb(GLFWwindow* window)
{
    g_redraw = true;
}

void errorCb(int error, const char* description)
{
    fputs(description, stderr);
}

void keyCb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_RELEASE)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
            break;

        case GLFW_KEY_V:
            if (action == GLFW_RELEASE)
            {
                g_resized = true;
            }
            break;
        case GLFW_KEY_D:
            if (action == GLFW_RELEASE)
            {
                g_redraw = true;
            }
        default:
            break;
    }
}

//  simple demo with graphics - for demonstration only
//
int main(int argc, char* argv[])
{
	srand((uint32_t)time(NULL));

    glfwSetErrorCallback(errorCb);

    if (!glfwInit())
        return 1;

    GLFWwindow* window = glfwCreateWindow(1024,768, "Voronoi", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, resizeCb);
    glfwSetWindowRefreshCallback(window, refreshCb);
    glfwSetKeyCallback(window, keyCb);

    g_resized = true;
    glfwGetFramebufferSize(window, &g_width, &g_height);

    while (!glfwWindowShouldClose(window))
    {
        if (g_resized)
        {
            g_resized = false;
            g_redraw = true;
            rebuild();
            glPointSize(5.0);
            glViewport(0,0,(GLsizei)g_width, (GLsizei)g_height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, g_width, g_height, 0, -1, 1);
        }
        if (g_redraw)
        {
            draw();
            glfwSwapBuffers(window);
            g_redraw = false;
        }
        glfwWaitEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}
