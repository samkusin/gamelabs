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

//#include "voronoi/voronoi.hpp"

#include <GLFW/glfw3.h>

#include <ctime>
#include <cstdio>
#include <chrono>
#include <cstdlib>
#include <array>

#include "jobqueue/jobqueue.hpp"
#include "jobqueue/jobscheduler.hpp"

#include "map.hpp"
#include "genroom.hpp"
#include "bitmap.hpp"
#include "graphics.hpp"
#include "tiledatabase.hpp"


///////////////////////////////////////////////////////////////////////////////

struct Tile
{
    const char* name;
    int32_t handle;
    uint8_t categoryId;
    uint8_t classId;
    uint16_t roleFlags;
    //Tile(const char* name, int32_t handle) : name(name), handle(handle) {}
    //Tile(): name(nullptr), handle(0) {}
};

//  The Category is a hint to the room generator, identifying what tiles to use
//  for a particular locale, situation, etc
const uint8_t kTileCategory_Dungeon             = 1;

//  The Class represents the set of all tiles belonging to 'stone', 'brick', 
//  etc.   For example, all wall and floor tiles that are 'stone' are in one
//  class, and are often plotted next to each other
const uint8_t kTileClass_Stone                  = 1;

//  A configuraiton array loading a tile database into its templates
//
std::array<Tile, 256> _tileToSprites = {
    Tile { "n_wall",                        1,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileDirection_N },
    Tile { "ne_wall",                       2,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileDirection_NE },
    Tile { "e_wall",                        3,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileDirection_E },
    Tile { "se_wall",                       4,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileDirection_SE },
    Tile { "s_wall",                        5,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileDirection_S },
    Tile { "sw_wall",                       6,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileDirection_SW },
    Tile { "w_wall",                        7,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileDirection_W },
    Tile { "nw_wall",                       8,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileDirection_NW },
    Tile { "nw_corner_wall",                9,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileRole_Corner |
            cinekine::overview::kTileDirection_NW },
    Tile { "ne_corner_wall",                10,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileRole_Corner |
            cinekine::overview::kTileDirection_NE },
    Tile { "nw_corner_wall",                11,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileRole_Corner |
            cinekine::overview::kTileDirection_NW },
    Tile { "se_corner_wall",                12,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileRole_Corner |
            cinekine::overview::kTileDirection_SE },
    Tile { "sw_corner_wall",                13,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Wall | cinekine::overview::kTileRole_Corner |
            cinekine::overview::kTileDirection_SW },
    Tile { "floor",                         64,
            kTileCategory_Dungeon,          kTileClass_Stone,
            cinekine::overview::kTileRole_Floor },
    Tile { nullptr, 0, 0, 0, 0 }
};

///////////////////////////////////////////////////////////////////////////////
//
//  Builder Architect Implementation wrapper
//  
//  Note: our use of GL 2 is not to encourage deprecated GL functionality
//  in production code.  For sample purposes, where we're prototyping concepts
//  that aren't graphic-rendering specific, GL 2 is a bit easier to set up,
//  and we're not concerned about optimizing framerates for very simple 2d
//  graphics
// 
class TheArchitect : public cinekine::overview::Architect
{
public:
    TheArchitect()
    {
    }

    void onNewSegment(cinekine::overview::Segment& segment)
    {
        //  Determine width and height of our target segment/room
        int32_t xSize = 3 + (rand() % 3);
        int32_t ySize = 3 + (rand() % 3);
        int32_t xMinRange = std::max(0, segment.x1 - xSize + 1);
        int32_t yMinRange = std::max(0, segment.y1 - ySize + 1);

        //  define a random segment within the supplied bounding segment
        segment.x0 = xMinRange ? (rand() % xMinRange) : 0;
        segment.y0 = yMinRange ? (rand() % yMinRange) : 0;
        segment.x1 = segment.x0 + xSize;
        segment.y1 = segment.y0 + ySize;      
    }
   
    void onPaintSegment(cinekine::overview::TileBrush& brush,
                        const cinekine::overview::Room& room,
                        const cinekine::overview::Segment& segment)
    {
        //  set our brush
        brush.tileCategoryId = kTileCategory_Dungeon;
        brush.tileClassId = kTileClass_Stone;
    }
};

///////////////////////////////////////////////////////////////////////////////
//
//  Application wrapper
//  
class Application
{
    Graphics& _graphics;

    cinekine::JobQueue _jobQueue;
    cinekine::JobHandle _builderJob;
    cinekine::overview::Context _context;

    cinekine::overview::TileDatabase _tileTemplates;
    std::unique_ptr<cinekine::overview::Map> _map;
    std::unique_ptr<cinekine::overview::Architect> _architect;

    static const uint32_t kNumRooms = 12;

public:
    Application(Graphics& graphics) :
        _graphics(graphics),
        _jobQueue(32),
        _builderJob(cinekine::kNullJobHandle),
        _tileTemplates(256),
        _map()
    {
        for (auto& tile : _tileToSprites)
        {
            if (!tile.name)
                break;
            _graphics.mapTile(tile.handle, tile.name);

            cinekine::overview::TileTemplate tileTemplate;
            tileTemplate.bitmapHandle = tile.handle;
            tileTemplate.roleFlags = tile.roleFlags;
            tileTemplate.classId = tile.classId;
            tileTemplate.categoryId = tile.categoryId;
            tileTemplate.params[0] = 0;
            tileTemplate.params[1] = 0;
            _tileTemplates.mapTemplate(tile.handle, tileTemplate);
        }
    }

    ~Application()
    {
        _jobQueue.cancel(_builderJob);
        _builderJob = cinekine::kNullJobHandle;
    }

    void update(float deltaTime)
    {
        _context.architect = _architect.get();

        _jobQueue.schedule();
        while (_jobQueue.dispatch(&_context)) {}
    }

    void drawTilemap(cinekine::overview::Tilemap* grid)
    {
        if (!grid)
            return;
        for (uint32_t y = 0; y < grid->rowCount(); ++y)
        {
            for (uint32_t x = 0; x < grid->columnCount(); ++x)
            {
                uint16_t tileId = grid->at(y, x);
                _graphics.drawTile(x*32,y*32, tileId);
            }
        }   
    }

    void draw()
    {
        _graphics.clear();

        if (_map)
        {
            _graphics.beginTiles();
            drawTilemap(_map->tilemapAtZ(0));
            drawTilemap(_map->tilemapAtZ(1));
            _graphics.endTiles();
        }   
    }

    void buildMap()
    {
        if (_jobQueue.validJob(_builderJob))
        {
            _jobQueue.cancel(_builderJob);
        }

        _map = std::unique_ptr<cinekine::overview::Map>(
            new cinekine::overview::Map(
                    cinekine::overview::MapBounds{ 32,32,2 }
                    )
                );
        std::unique_ptr<cinekine::Job> builder(
            new cinekine::overview::Builder(
                    *_map,
                    _tileTemplates,
                    kNumRooms)
                );
        _builderJob = _jobQueue.add(std::move(builder));
        _architect =
            std::unique_ptr<cinekine::overview::Architect>(
                new TheArchitect()
            );
    }

};


///////////////////////////////////////////////////////////////////////////////
///

bool gUpdateSignaled = false;

void errorCb(int error, const char* description)
{
    fputs(description, stderr);
}

void resizeCb(GLFWwindow* window, int w, int h)
{
    glViewport(0,0,(GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
}

void keyCb(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* app =
        reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_RELEASE)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
            break;

        case GLFW_KEY_B:
            if (action == GLFW_RELEASE)
            {
                app->buildMap();
                gUpdateSignaled = true;
            }

        case GLFW_KEY_SPACE:
            if (action == GLFW_RELEASE)
            {
                gUpdateSignaled = true;
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

    int result = 1;
    GLFWwindow* window = glfwCreateWindow(1024,768, "Room Generator", NULL, NULL);
    if (window)
    {
        glfwMakeContextCurrent(window);
        
        Graphics graphics("tiles.json");
        
        int frameW, frameH;
        glfwGetFramebufferSize(window, &frameW, &frameH);
        resizeCb(window, frameW, frameH);
        glfwSetFramebufferSizeCallback(window, resizeCb);
        glfwSetKeyCallback(window, keyCb);

        Application app(graphics);
        glfwSetWindowUserPointer(window, &app);
        
        float lastFrameTime = (float)glfwGetTime();
        float deltaTime = 0.f;

        app.buildMap();
        gUpdateSignaled = true;

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            if (gUpdateSignaled)
            {
                app.update(deltaTime);
                gUpdateSignaled = false;
            }
            app.draw();
            glfwSwapBuffers(window);

            deltaTime = (float)glfwGetTime() - lastFrameTime;
            lastFrameTime = (float)glfwGetTime();
        }

        result = 0;

        glfwDestroyWindow(window);
    }

    glfwTerminate();

    return result;
}
