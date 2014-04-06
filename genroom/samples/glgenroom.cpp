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

#include "map.hpp"
#include "builder.hpp"
#include "bitmap.hpp"
#include "graphics.hpp"
#include "tiledatabase.hpp"

const int32_t kMapWidth = 56;
const int32_t kMapHeight = 40;


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
//  The Architect implementation's primary responsibility is to generate
//  region building instructions and pass them to the Builder upon request.
//
//  How it does this relies on the buliding strategy - in this simple
//  implementation we randomize room placement and submit build instructions
//  assuming one room = one building region.
//
class TheArchitect : public cinekine::overview::Architect
{
    struct Room
    {
        cinekine::overview::Box box;
    };

    const cinekine::overview::TileDatabase& _tileTemplates;
    std::unique_ptr<cinekine::overview::Builder> _builder;
    std::vector<Room> _rooms;

    //  determines whether we can build a room from the specified box region
    bool canBuildRoom(const cinekine::overview::Box& bounds,
        const cinekine::overview::Box& box) const
    {
        if (!box.inside(bounds))
            return false;

        for (auto& room : _rooms)
        {
            if (room.box.intersects(box))
                return false;
        }
        printf("canBuildRoom!\n");
        return true;
    }

    //  randomize a box.  one or more boxes comprise a room
    auto generateBox(int32_t wMin, int32_t hMin,
                int32_t wMax, int32_t hMax) -> cinekine::overview::Box
    {
        cinekine::overview::Box box;
        int32_t xSize = wMin + (rand() % (wMax-wMin)+1);
        int32_t ySize = hMin + (rand() % (hMax-hMin)+1);
        //  define a random segment within the supplied bounding segment
        box.x0 = 0;
        box.y0 = 0;
        box.x1 = xSize;
        box.y1 = ySize;
        return box;
    }

public:
    TheArchitect(cinekine::overview::Map& map,
                 const cinekine::overview::TileDatabase& tileTemplates) :
        _tileTemplates(tileTemplates)
    {
        _builder = std::unique_ptr<cinekine::overview::Builder>(
            new cinekine::overview::Builder(
                    *this,
                    map,
                    _tileTemplates,
                    256)
                );
    }

    void update()
    {
        _builder->update();
    }

    auto onNewRegionRequest(const cinekine::overview::NewRegionRequest& request) ->
        cinekine::overview::NewRegionResponse
    {
        cinekine::overview::NewRegionResponse resp;
        resp.tileBrush = { kTileCategory_Dungeon, kTileClass_Stone };

        //  prioritize large rooms.  If our randomized room selection cannot
        //  fit a 3x3 room minimum, then we (likely) have no room to build
        int32_t boxMinW = 12, boxMinH = 12;
        bool widthDominant = true;
        auto roomBox = cinekine::overview::Box::kEmpty;

        while (boxMinW >= 3 && boxMinH >= 3)
        {
            roomBox = generateBox(
                    boxMinW, boxMinH,
                    boxMinW + boxMinW/2,
                    boxMinH + boxMinH/2);

            //  randomize our box origin - this is the simplest way to
            //  position a box.  basically in this implementation, there's no
            //  design to our room layout
            int xVariance = request.mapBounds.width() - roomBox.width();
            int yVariance = request.mapBounds.height() - roomBox.height();
            int attempts = 0;
            bool boxAllowed = false;
            int32_t roomX1 = roomBox.x1;
            int32_t roomY1 = roomBox.y1;
            const int kMaxAttempts = 16;
            do
            {
                roomBox.x0 = request.mapBounds.x0;
                roomBox.y0 = request.mapBounds.y0;
                if (xVariance > 0)
                    roomBox.x0 += (rand() % (xVariance+1));
                if (yVariance > 0)
                    roomBox.y0 += (rand() % (yVariance+1));
                roomBox.x1 = roomX1 + roomBox.x0;
                roomBox.y1 = roomY1 + roomBox.y0;
                ++attempts;
            }
            while(!(boxAllowed = canBuildRoom(request.mapBounds, roomBox))
                    && attempts < kMaxAttempts);
            if (boxAllowed)
                break;

            roomBox = cinekine::overview::Box::kEmpty;

            //  shrink our box size since our (admittedly weak) box test
            //  failed.  then try again.
            if (widthDominant)
            {
                boxMinH -= 3;
            }
            else
            {
                boxMinW -= 3;
            }
            widthDominant = !widthDominant;
        }

        if (roomBox)
        {
            Room room;
            room.box = roomBox;
            _rooms.push_back(room);

            auto policy = cinekine::overview::NewRegionInstruction::kRandomize;
            resp.instructions.emplace_back(policy);

            auto& instruction = resp.instructions.back();
            instruction.box = roomBox;
            instruction.terminal = true;
        }
        else
        {
            //  done
            resp.instructions.emplace_back(cinekine::overview::NewRegionInstruction::kFinalize);
        }

        return resp;
    }

};

///////////////////////////////////////////////////////////////////////////////
//
//  Application wrapper
//
class Application
{
    Graphics& _graphics;

    cinekine::overview::TileDatabase _tileTemplates;
    std::unique_ptr<cinekine::overview::Map> _map;
    std::unique_ptr<TheArchitect> _architect;

public:
    Application(Graphics& graphics) :
        _graphics(graphics),
        _tileTemplates(256)
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

    void update(float deltaTime)
    {
        if (_architect)
        {
            _architect->update();
        }
    }

    void drawTilemap(cinekine::overview::Tilemap* grid)
    {
        if (!grid)
            return;
        for (uint32_t y = 0; y < grid->rowCount(); ++y)
        {
            for (uint32_t x = 0; x < grid->columnCount(); ++x)
            {
                const cinekine::overview::Tile& tile = grid->at(y,x);
                _graphics.drawTile(x*32,y*32, tile.floor);
                _graphics.drawTile(x*32,y*32, tile.wall);
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
            _graphics.endTiles();
        }
    }

    void buildMap()
    {
        _map = std::unique_ptr<cinekine::overview::Map>(
            new cinekine::overview::Map(
                    cinekine::overview::MapBounds{ kMapWidth,kMapHeight,1 }
                    )
                );
        _architect =
            std::unique_ptr<TheArchitect>(
                new TheArchitect(*_map, _tileTemplates)
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
