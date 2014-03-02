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

#include "gameobjects.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>

const uint8_t kMaxCoreStatValue = 18;

inline bool rollIncrementCoreStat(int16_t& coreStat,
                                  int& flexPoints,
                                  int16_t roleCoreStat)
{
    uint8_t dieRoll = std::rand() % kMaxCoreStatValue;
    if (dieRoll < roleCoreStat)
    {
        ++coreStat;
        --flexPoints;
    }
    return flexPoints > 0;
}


Player::Player(const char* name, const PlayerRole& playerRole,
               Region region,
               int flexPoints) :
    _role(playerRole),
    _name(name),
    _region(region),
    _level(0),
    _weapon(nullptr),
    _armor(nullptr),
    _spellRating(0)
{
    _coreStats = playerRole.coreStats;

    //  distrubute flex points among core stats
    while (flexPoints)
    {
        if (!rollIncrementCoreStat(_coreStats.strength, flexPoints,
                                   _role.coreStats.strength))
            break;
        if (!rollIncrementCoreStat(_coreStats.dexterity, flexPoints,
                                   _role.coreStats.dexterity))
            break;
        if (!rollIncrementCoreStat(_coreStats.intelligence, flexPoints,
                                   _role.coreStats.intelligence))
            break;
        if (!rollIncrementCoreStat(_coreStats.endurance, flexPoints,
                                   _role.coreStats.endurance))
            break;
    }

    _healthPointsMax = 0;
    _magicPointsMax = 0;
    
    advanceLevel();
}
 
void Player::adjustHealth(int32_t adj)
{
    _healthPoints = std::min(_healthPoints + adj, _healthPointsMax);
}

void Player::adjustMagic(int32_t adj)
{
    _magicPoints = std::min(_magicPoints + adj, _magicPointsMax);
}

void Player::advanceLevel()
{
    _healthPointsMax += (std::rand() % _role.healthScalar) + 1;
    if (_coreStats.endurance >= 12)
        ++_healthPointsMax;
    if (_coreStats.endurance >= 14)
        ++_healthPointsMax;

    _healthPoints = _healthPointsMax;

    if (_role.magicScalar)
    {
        _magicPointsMax += (std::rand() % _role.magicScalar) + 1;
        if (_magicPointsMax)
        {
            if (_coreStats.intelligence >= 12)
                ++_magicPointsMax;
            if (_coreStats.intelligence >= 14)
                ++_magicPointsMax;
        }
    }

    _spellRating += _role.spellScalar;
    _magicPoints = _magicPointsMax;
    ++_level;
}

///////////////////////////////////////////////////////////////////////////////

bool Party::canJoin() const
{
    size_t i = (std::rand() % kMaxPlayers);
    return i > _players.size();
}
void Party::fight(Party& other)
{
    //  start of combat
    if (!_combat)
    {
        std::cout << "Party[" << id() << "] attacks Party[" << other.id() << "]"
                  << std::endl;
        _combat = true;
        other._combat = true;
        return;
    }

    //  combat round
    //
    //
    int flightRoll = std::rand() % 5;
    if (!flightRoll)
    {
        std::cout << "Party[" << id() << "] flees combat!" << std::endl;
        _combat = false;
        return;
    }

    for (auto it = _players.begin(); it != _players.end(); )
    {
        auto player = *it;
        int dmg = (std::rand() % 3) + 1;
        std::cout << player->name() << " takes " << dmg << "points of damage." << std::endl;
        player->adjustHealth(-dmg);
        if (player->health() <= 0)
        {
            std::cout << player->name() << " is killed!" << std::endl;
            it = _players.erase(it);
        }
        else
        {
            ++it;
        }
    }
}