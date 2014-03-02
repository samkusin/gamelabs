/**
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

#include "jobqueue.hpp"
#include "job.hpp"
#include "jobscheduler.hpp"

#include "gameobjects.hpp"

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>

//  Context shared by the application and its jobs
//  

struct PlayerNameElements
{
    std::vector<std::string> prefixes;
    std::vector<std::string> suffixes;

    std::string generate()
    {
        return prefixes[std::rand() % prefixes.size()] +
               suffixes[std::rand() % suffixes.size()];
    }
};


struct SimContext
{ 
    std::array<PlayerNameElements, kRegion_Count> regionNameElements;
    std::array<PlayerRole, kRole_Count> roles;

    std::vector<Weapon> lightWeapons;
    std::vector<Weapon> mediumWeapons;
    std::vector<Weapon> heavyWeapons;
    std::vector<Armor> lightArmor;
    std::vector<Armor> mediumArmor;
    std::vector<Armor> heavyArmor;

    typedef std::vector<Spell> Spells;
    std::map<int32_t, Spells> spells;


    std::vector<std::shared_ptr<Player>> players;
    std::vector<std::shared_ptr<Party>> parties;
    int nextPartyId;

    static const int kGridHorizontal = 27;
    static const int kGridVertical = 13;

    char grid[kGridVertical][kGridHorizontal];
    int turns;
    bool generatingCharacters;
};

std::ostream& operator<< (std::ostream& stream, Player& player)
{
    stream  << " name: " << player.name() << " ("
            << " Level: " << player.level() << " " << player.role().name
            << " )" << std::endl
            << " str: " << player.coreStats().strength << ","
            << " int: " << player.coreStats().intelligence << ","
            << " dex: " << player.coreStats().dexterity << ","
            << " end: " << player.coreStats().endurance << std::endl
            << " health: " << player.health() << ","
            << " magic: " << player.magic() << std::endl
            << " spell: " << player.spellRating() << std::endl
            << " weapon: " << player.weapon()->name << std::endl
            << " armor: " << player.armor()->name;

    return stream;
}

///////////////////////////////////////////////////////////////////////////////

//  This job will run a single turn of the main game simulation for a
//  single party.
//  
//  When two parties encounter each other, they either fight or grouped
//  together
//  
class GameClient : public cinekine::Job
{
    std::shared_ptr<Party> _party;

    void move(SimContext& context)
    {
        int dir = std::rand() % 4;
        int xoff = 0, yoff = 0;
        if (dir == 0)
            yoff = -1;
        else if (dir == 1)
            xoff = 1;
        else if (dir == 2)
            yoff = 1;
        else if (dir == 3)
            xoff = -1;

        if ((_party->x() + xoff) < 0 || 
            (_party->x() + xoff) >= SimContext::kGridHorizontal)
            xoff = 0;
        if ((_party->y() + yoff) < 0 || 
            (_party->y() + yoff) >= SimContext::kGridVertical)
            yoff = 0;
        
        _party->setXY(_party->x()+xoff, _party->y()+yoff);
    }

public:
    GameClient(std::shared_ptr<Party> party) :
        _party(party)
    {
        std::cout << "GAME_CLIENT_START" << std::endl;
    }

    ~GameClient()
    {
        std::cout << "GAME_CLIENT_END" << std::endl;
    }

    Result execute(cinekine::JobScheduler& scheduler,
                   void* context)
    {
        SimContext& ctx = *reinterpret_cast<SimContext*>(context);

        if (_party->empty())
            return Result::kTerminate;

        // move action
        if (!_party->combat())
        {
            move(ctx);
        }

        //  check if this party is in a cell with another party
        //  if so, then we'll either fight or join that party
        //
        bool fighting = false;
        auto it = ctx.parties.begin();
        for ( ; it != ctx.parties.end(); ++it)
        {
            Party* other = (*it).get();
            if (other->empty())
                continue;

            if (other != _party.get())
            {
                if (other->x() == _party->x() &&
                    other->y() == _party->y())
                {
                    if (!_party->combat())
                    {
                        //  either join or fight.  once we enter combat, 
                        //  only non-combatants can join us
                        if (other->canJoin())
                        {
                            //  have our player join this party, and wipe
                            //  our old party
                            while (_party->count())
                            {
                                std::shared_ptr<Player> player = _party->popPlayer();
                                std::cout << player->name() 
                                          << " joins Party[" << other->id() << "]"
                                          << std::endl;
                                other->addPlayer(player);
                            }
                            return Result::kTerminate;
                        }
                    }
                    _party->fight(*other);
                    fighting = true;
                }
            }
        }
        //  turn off combat if any parties we were fighting fled or were killed
        if (!fighting)
        {
            _party->clearCombat();
        }

        return Result::kReschedule;
    }

    int32_t priority() const
    {
        return 0;
    }
};


//  This Job will generate players for our simulation context.
//  When its created the number of players requested of it (_playersLeft) it
//  will schedule a Game job to run the simulation.
//
class GeneratePlayers : public cinekine::Job
{
    int _playersLeft;

public:
    GeneratePlayers(int numPlayers) :
        _playersLeft(numPlayers)
    {
        std::cout << "GENERATE_PLAYERS_START" << std::endl;
    }

    ~GeneratePlayers()
    {
        std::cout << "GENERATE_PLAYERS_END" << std::endl;
    }

    Result execute(cinekine::JobScheduler& scheduler,
                   void* context)
    {
        SimContext& ctx = *reinterpret_cast<SimContext*>(context);

        if (_playersLeft)
        {
            //  Generate player - in an alternate implementation,
            //      this job could manage its own queue, and queue up 
            //      "GeneratePlayer" jobs, run them until the queue is empty.
            //      If the jobqueue ran jobs concurrently (via threads), all
            //      the better.
            Region region = static_cast<Region>(std::rand() % kRegion_Count);
            Role role = static_cast<Role>(std::rand() % kRole_Count);

            std::shared_ptr<Player> player(
                new Player(
                    ctx.regionNameElements[region].generate().c_str(),
                    ctx.roles[role],
                    region,
                    10));

            if (player->role().type == kRole_Fighter)
            {
                player->setWeapon(&ctx.mediumWeapons[0]);
                player->setArmor(&ctx.mediumArmor[0]);
            }
            else if (player->role().type == kRole_Mage)
            {
                player->setWeapon(&ctx.lightWeapons[0]);
                player->setArmor(&ctx.lightArmor[0]);
            }
            else if (player->role().type == kRole_Thief)
            {
                player->setWeapon(&ctx.mediumWeapons[0]);
                player->setArmor(&ctx.lightArmor[0]);
            }

            ctx.players.push_back(player);
            std::cout << "Creating character:" << std::endl
                      << *player << std::endl;
      
            //  schedule the main game job
            //
            int x = std::rand() % SimContext::kGridHorizontal;
            int y = std::rand() % SimContext::kGridVertical;
            std::shared_ptr<Party> party(
                new Party(ctx.nextPartyId++, player, x, y)
            );
            ctx.parties.push_back(party);
            std::unique_ptr<cinekine::Job> job(
                new GameClient(party)
            );
            scheduler.add(std::move(job));
            --_playersLeft;
        }

        ctx.generatingCharacters = (_playersLeft > 0);

        return _playersLeft ? Result::kReschedule : Result::kTerminate;
    }

    int32_t priority() const
    {
        return 0;
    }

};


///////////////////////////////////////////////////////////////////////////////

void initContext(SimContext& context)
{
    PlayerNameElements nameElements;
    nameElements.prefixes.push_back("Bilan");
    nameElements.prefixes.push_back("Melin");
    nameElements.prefixes.push_back("Doran");
    nameElements.prefixes.push_back("Elan");
    nameElements.suffixes.push_back("dra");
    nameElements.suffixes.push_back("der");
    nameElements.suffixes.push_back("norn");
    nameElements.suffixes.push_back("nisa");
    nameElements.suffixes.push_back("gor");
    nameElements.suffixes.push_back("eny");
    context.regionNameElements[kRegion_North] = std::move(nameElements);

    nameElements.prefixes.push_back("Vict");
    nameElements.prefixes.push_back("Marc");
    nameElements.prefixes.push_back("Fer");
    nameElements.prefixes.push_back("Ser");
    nameElements.suffixes.push_back("ana");
    nameElements.suffixes.push_back("enni");
    nameElements.suffixes.push_back("iron");
    nameElements.suffixes.push_back("issa");
    nameElements.suffixes.push_back("onna");
    nameElements.suffixes.push_back("orry");
    context.regionNameElements[kRegion_Central] = std::move(nameElements);

    nameElements.prefixes.push_back("Us");
    nameElements.prefixes.push_back("Tess");
    nameElements.prefixes.push_back("Quar");
    nameElements.prefixes.push_back("Genn");
    nameElements.suffixes.push_back("a");
    nameElements.suffixes.push_back("ol");
    nameElements.suffixes.push_back("ora");
    nameElements.suffixes.push_back("en");
    nameElements.suffixes.push_back("enna");
    nameElements.suffixes.push_back("i");
    context.regionNameElements[kRegion_South] = std::move(nameElements);

    context.roles[kRole_Fighter].name = "Fighter";
    context.roles[kRole_Fighter].type = kRole_Fighter;
    context.roles[kRole_Fighter].coreStats = { 13, 10, 7, 11 };
    context.roles[kRole_Fighter].healthScalar = 12;
    context.roles[kRole_Fighter].magicScalar = 0;
    context.roles[kRole_Fighter].spellScalar = 0;
    context.roles[kRole_Fighter].weapon = { true, true, true };
    context.roles[kRole_Fighter].armor = { true, true, true };
    context.roles[kRole_Fighter].spell = { false, false, false };

    context.roles[kRole_Mage].name = "Mage";
    context.roles[kRole_Mage].type = kRole_Mage;
    context.roles[kRole_Mage].coreStats = { 8, 10, 13, 10 };
    context.roles[kRole_Mage].healthScalar = 8;
    context.roles[kRole_Mage].magicScalar = 4;
    context.roles[kRole_Mage].spellScalar = 3;
    context.roles[kRole_Mage].weapon = { true, false, false };
    context.roles[kRole_Mage].armor = { true, false, false };
    context.roles[kRole_Mage].spell = { true, true, true };
    
    context.roles[kRole_Thief].name = "Thief";
    context.roles[kRole_Thief].type = kRole_Thief;
    context.roles[kRole_Thief].coreStats = { 9, 13, 9, 10 };
    context.roles[kRole_Thief].healthScalar = 10;
    context.roles[kRole_Thief].magicScalar = 2;
    context.roles[kRole_Thief].spellScalar = 1;
    context.roles[kRole_Thief].weapon = { true, true, false };
    context.roles[kRole_Thief].armor = { true, false, false };
    context.roles[kRole_Thief].spell = { true, false, false };

    context.lightWeapons.emplace_back("staff", 1);
    context.lightWeapons.emplace_back("dagger", 2);
    context.mediumWeapons.emplace_back("mace", 3);
    context.mediumWeapons.emplace_back("short sword", 4);
    context.heavyWeapons.emplace_back("long sword", 5);
    context.heavyWeapons.emplace_back("2h sword", 6);

    context.lightArmor.emplace_back("cloth", 1);
    context.lightArmor.emplace_back("leather padding", 2);
    context.mediumArmor.emplace_back("leather suit", 3);
    context.mediumArmor.emplace_back("scale", 4);
    context.heavyArmor.emplace_back("chain", 5);
    context.heavyArmor.emplace_back("plate", 6);

    //  spells by rating
    context.spells[1] = {};
    context.spells[1].emplace_back("missile", 2, 0, 2);
    context.spells[1].emplace_back("strongskin", 0, 2, 1);
    context.spells[3] = {};
    context.spells[3].emplace_back("fireball", 4, 0, 4);
    context.spells[3].emplace_back("iceball", 3, 0, 3);
    context.spells[3].emplace_back("magic shield", 0, 3, 3);
    context.spells[5] = {};
    context.spells[3].emplace_back("lightning", 8, 0, 8);
    context.spells[3].emplace_back("magic dome", 0, 5, 6);
    context.spells[3].emplace_back("swarm", 5, 3, 9);

    //  create map
    int row, col;
    for (row = 0; row < SimContext::kGridVertical; ++row)
    {
        for (col = 0; col < SimContext::kGridHorizontal; ++col)
        {
            context.grid[row][col] = ' ';
        }
    }

    context.turns = 0;
    context.nextPartyId = 1;
    context.generatingCharacters = true;
}

void drawMap(SimContext& context)
{
    std::vector<std::vector<Party*>> combats;
    combats.reserve(16);
    int row = 0;
    int col = 0;
    for (col = 0; col < SimContext::kGridHorizontal+2; ++col)
        std::cout << '-';
    std::cout << std::endl;

    for (row=0; row < SimContext::kGridVertical; ++row)
    {
        std::cout << '|';
        for (col=0; col < SimContext::kGridHorizontal; ++col)
        {
            char ch = context.grid[row][col];
            std::vector<Party*>* combat = nullptr;

            for (auto partyIt = context.parties.begin();
                 partyIt != context.parties.end();
                 ++partyIt)
            {
                Party* party = (*partyIt).get();
                if (party->x() == col && party->y() == row)
                {
                    if (party->combat())
                    {
                        if (!combat)
                        {
                            combats.push_back(std::vector<Party*>());
                            combat = &combats.back();
                            ch = '0' + combats.size() - 1;
                        }
                        combat->push_back(party);
                    }
                    else
                    {
                        ch = 'A' + party->id() - 1;
                    }
                }
            }

            std::cout << ch;
        }
        std::cout << "|" << std::endl;
    }
    for (col = 0; col < SimContext::kGridHorizontal+2; ++col)
        std::cout << '-';
    std::cout << std::endl;

   
    for (auto party: context.parties)
    {
        std::cout << "Party[" << party->id() << "]: ";
        for (size_t i = 0; i < party->count(); ++i)
        {
            std::cout << party->player(i)->name() << "    ";
        }
        std::cout << std::endl;
    } 

    for (int combatIdx = 0; combatIdx < combats.size(); ++combatIdx)
    {
        std::cout << "Combat[" << combatIdx << "]: ";
        for (auto party: combats[combatIdx])
        {
            std::cout << "Party(" << party->id() << ") ";
        } 
        std::cout << std::endl;
    }

    combats.clear();
}


int main(int argc, const char* argv[])
{
    cinekine::JobQueue jobQueue(32);
    std::srand(std::time(0));

    //  simulation context init
    SimContext context;
    initContext(context);
    
    //  add our application job to the queue
    std::unique_ptr<cinekine::Job> job(new GeneratePlayers(36));
    jobQueue.add(std::move(job));

    while (!jobQueue.empty())
    {
        jobQueue.schedule();
        while (jobQueue.dispatch(&context))
        {
            std::cout << std::flush;
        }

        //  house cleaning
        for (auto partyIt = context.parties.begin();
             partyIt != context.parties.end();)
        {
            Party* party = (*partyIt).get();
            if (party->empty())
            {
                partyIt = context.parties.erase(partyIt);
                continue;
            }
            ++partyIt;
        }
        for (auto it = context.players.begin();
             it != context.players.end();)
        {
            Player* player = (*it).get();
            if (player->health() == 0)
            {
                it = context.players.erase(it);
                continue;
            }
            ++it;
        }

        ++context.turns;

        //  print map
        drawMap(context);

        if (context.parties.size() <= 1 && !context.generatingCharacters)
        {
            std::cout << "THE GAME IS OVER (" << context.turns << " turns)."
            << std::endl;
            break;
        }

        /*char c;
        std::cout << std::flush << std::endl << "Next> ";
        std::cin >> c;
        if (c == 'q')
        {
            break;
        }
        */
    }

    return 0;
}
