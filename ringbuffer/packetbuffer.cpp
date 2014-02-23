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

#include "packetbuffer.hpp"

#include <cstdlib>

PacketBuffer::PacketBuffer(uint32_t packetDataSize,
                           uint32_t packetCapacity) :
    _packetDataSize(packetDataSize),
    _byteBuffer(packetDataSize*packetCapacity),
    _packets(packetCapacity),
    _readIndex(0),
    _writeIndex(0)
{
    uint8_t* packetData = _byteBuffer.data();
    for(auto& packet : _packets)
    {
        packet.data = packetData;
        packet.size = 0;
        packetData += packetDataSize;
    }
}

const Packet* PacketBuffer::readHead()
{
    if (empty())
        return nullptr;
    return &_packets[_readIndex];
}

bool PacketBuffer::advanceRead()
{
    if (_readIndex == _writeIndex)
        return false;
    _readIndex = (_readIndex + 1) % _packets.size();
    return true;
}

Packet* PacketBuffer::writeHead()
{
    _packets[_writeIndex].size = _packetDataSize;
    return &_packets[_writeIndex];
}

bool PacketBuffer::advanceWrite()
{
    if (nextWriteIndex() == _readIndex)
        return false;
    _writeIndex = nextWriteIndex();
    return true;
}
