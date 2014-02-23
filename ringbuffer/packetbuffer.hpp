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

#ifndef CK_Sample_PacketBuffer_hpp
#define CK_Sample_PacketBuffer_hpp

#include <cstdint>
#include <vector>

struct Packet
{
    uint8_t* data;
    uint32_t size;
};

//  The Ring Buffer is best accessed by at least two threads.  The typical ring
//  buffer is designed to restrict read access during write operations to the
//  common buffer memory.
//
//  To miminize this locking behavior, a Producer should be able to write data
//  into the tail packet of the buffer, while a Consumer pulls packets from the
//  head of the buffer.
//
//
class PacketBuffer
{
public:
    PacketBuffer(uint32_t packetDataSize, uint32_t packetCapacity);

    bool empty() const;
    
    const Packet* readHead();
    bool advanceRead();
    Packet* writeHead();
    bool advanceWrite();

private:
    uint32_t nextWriteIndex() const;

    const uint32_t _packetDataSize;
    std::vector<uint8_t> _byteBuffer;
    std::vector<Packet> _packets;
    volatile uint32_t _readIndex;
    volatile uint32_t _writeIndex;
};

inline bool PacketBuffer::empty() const
{
    return _readIndex == _writeIndex;
}

inline uint32_t PacketBuffer::nextWriteIndex() const
{
    return (_writeIndex + 1) % _packets.size();
}


#endif
