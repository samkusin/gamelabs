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

#ifndef CK_Sample_Jobs_GameObjects_hpp
#define CK_Sample_Jobs_GameObjects_hpp

#include <cstdint>
#include <string>
#include <vector>
#include <array>

enum Role
{
    kRole_Fighter,
    kRole_Mage,
    kRole_Thief,
    kRole_Count
};

enum Region
{
    kRegion_North,
    kRegion_Central,
    kRegion_South,
    kRegion_Count
};

struct Weapon
{
    std::string name;
    int damage;
    Weapon(const char* n, int dmg) :
        name(n),
        damage(dmg) {}
};

struct Armor
{
    std::string name;
    int defense;
    Armor(const char* n, int defense) :
        name(n),
        defense(defense) {}
};

struct Spell
{
    std::string name;
    int damage;
    int defense;
    int points;
    Spell(const char* n, int dmg, int defense, int pts) :
        name(n),
        damage(dmg),
        defense(defense),
        points(pts) {}
};

struct PlayerCoreStats
{
    //  values from 3 - 18
    int16_t strength;
    int16_t dexterity;
    int16_t intelligence;
    int16_t endurance;
};

struct CombatRole
{
    bool light;
    bool medium;
    bool heavy;
};

struct PlayerRole
{
    std::string name;
    Role type;
    //  base values are used here to differentiate one class from another
    PlayerCoreStats coreStats;
    //  a value from 1 - 4
    uint16_t healthScalar;
    uint16_t magicScalar;
    uint16_t spellScalar;
    CombatRole weapon;
    CombatRole armor;
    CombatRole spell;
};

class Player
{
public:
    Player(const char* name, const PlayerRole& playerRole,
           Region region,
           int flexPoints);
    
    const PlayerCoreStats& coreStats() const { return _coreStats; }
    PlayerCoreStats& coreStats() { return _coreStats; }
    const PlayerRole& role() const { return _role; }
    const std::string& name() const { return _name; }
    Region region() const { return _region; }
    int32_t level() const { return _level; }
    int32_t healthMax() const { return _healthPointsMax; }
    int32_t health() const { return _healthPoints; }
    int32_t magicMax() const { return _magicPointsMax; }
    int32_t magic() const { return _magicPoints; }

    void adjustHealth(int32_t adj);
    void adjustMagic(int32_t adj);

    void advanceLevel();

    void setWeapon(Weapon* weapon) { _weapon = weapon; }
    void setArmor(Armor* armor) { _armor = armor; }

    const Weapon* weapon() const { return _weapon; }
    const Armor* armor() const { return _armor; }
    int32_t spellRating() const { return _spellRating; }

private:
    const PlayerRole& _role;
    std::string _name;
    Region _region;
    int32_t _level;
    PlayerCoreStats _coreStats;
    int32_t _healthPointsMax;
    int32_t _healthPoints;
    int32_t _magicPointsMax;
    int32_t _magicPoints;

    Weapon* _weapon;
    Armor* _armor;
    int32_t _spellRating;
};



class Party
{
public:
    Party(int id, std::shared_ptr<Player> player, int x, int y) :
        _id(id),
        _x(x),
        _y(y),
        _combat(false)
    {
        _players.push_back(player);
    }
    int id() const { return _id; }
    int x() const { return _x; }
    int y() const { return _y; }

    void setXY(int x, int y) {
        _x = x;
        _y = y;
    }

    void addPlayer(std::shared_ptr<Player> player) {
        _players.push_back(player);
    }

    std::shared_ptr<Player> popPlayer() {
        auto player = _players.back();
        _players.pop_back();
        return player;
    }
    bool empty() const { return _players.empty(); }
    size_t count() const { return _players.size(); }
    Player* player(size_t i) { return _players[i].get(); }

    void fight(Party& other);
    bool combat() const { return _combat; }
    void clearCombat() { _combat = false; }
    bool canJoin() const;

private:
    int _id;
    std::vector<std::shared_ptr<Player>> _players;
    int _x, _y;
    bool _combat;
    static const size_t kMaxPlayers = 3;
};


#endif

