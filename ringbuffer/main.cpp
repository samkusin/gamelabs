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

#include <fstream>
#include <iostream>

#include <pthread.h>
#include <unistd.h>

//
//  The Stream manages sync between a Reader and Writer threads, and their
//  common RingBuffer.
//
class Streamer
{
public:
    Streamer(std::streambuf& in, std::streambuf& out, uint32_t bufferCapacity);
    ~Streamer();
    
    operator bool() const
    {
        return true;
    }

    bool active() const {
        return inputActive() || outputActive(); 
    }

    bool inputActive() const {
        return _inputActive;
    }
    bool outputActive() const {
        return _outputActive;
    }

    size_t outputCount() const {
        return _outputCount;
    }

private:
    static void* writer_thread(void* arg);
    static void* reader_thread(void* arg);

private:
    std::streambuf& _infile;
    std::streambuf& _outfile;

    volatile size_t _outputCount;
    volatile bool _inputActive;
    volatile bool _outputActive;
    volatile bool _terminateStream;

    PacketBuffer _buffer;
    pthread_t _writerThread;
    pthread_t _readerThread;
    pthread_mutex_t _writerMutex;
    pthread_cond_t _writerCond;
};


//  Our writer thread reads from the input file stream and populates our
//  ring buffer's write section.  If the write buffer fills up, it will wait
//  for the reader thread to process data before writing new data into the
//  buffer from our input stream
//
void* Streamer::writer_thread(void* arg)
{
    Streamer* stream = reinterpret_cast<Streamer*>(arg);
    std::streambuf& infile = stream->_infile;
    PacketBuffer& buffer = stream->_buffer;
    intptr_t result = 0;
    
    while (!stream->_terminateStream)
    {
        Packet* writeTo = buffer.writeHead();
        if (!writeTo)
        {
            result = 1;
            break;
        }
        std::streamsize sz = infile.sgetn((char*)writeTo->data, writeTo->size);
        if (!sz)
        {
            result = 2;
            break;
        }
        writeTo->size = sz;
      
        pthread_mutex_lock(&stream->_writerMutex);
        while (!buffer.advanceWrite() && stream->outputActive())
        {
        //    std::cout << "W" << std::flush;
            pthread_cond_wait(&stream->_writerCond, &stream->_writerMutex);
        }
        //std::cout << "i" << std::flush;
        pthread_mutex_unlock(&stream->_writerMutex);
        if (!stream->outputActive())
            break;
    }

    stream->_inputActive = false;

    std::cout << "writer terminated : " << result << std::endl << std::flush;

    return (void*)result;
}

//  Our reader thread acquires data from our buffer's read section and
//  outputs that data to our output file
//
//  Note - this thread uses usleeps to simulate a "slower" serialized
//  output device that runs at approximately 48khz (i.e. a sound mixer.)
//  This delay would normally allow our writer and main threads to run
//  while the reader processes data.
//  
//  An alternative to this approach might be to use another condition
//  variable, where the writer signals the reader when it has data ready
//  for processing
//  
void* Streamer::reader_thread(void* arg)
{
    Streamer* stream = reinterpret_cast<Streamer*>(arg);
    std::streambuf& outfile = stream->_outfile;
    PacketBuffer& buffer = stream->_buffer;
    intptr_t result = 0;

    while (!stream->_terminateStream)
    {
        while (!buffer.empty())
        {       
            const Packet* readFrom = buffer.readHead();
            if (!readFrom)
            {
                result = 1;
                break;
            }
            std::streamsize sz = 0;
            uint32_t readFromAmt = 0;
            while (readFromAmt < readFrom->size)
            {
                //  emulating an output of 16 bytes per cycle
                sz = outfile.sputn((char*)readFrom->data+readFromAmt, 16);
                if (!sz)
                    break;
                usleep(20);
                readFromAmt += sz;
            }
            stream->_outputCount += readFromAmt;
            if (!sz)
            {
                result = 2;
                break;
            }
            buffer.advanceRead();
            //std::cout << "o" << std::flush;

            pthread_cond_signal(&stream->_writerCond);
        }

        if (buffer.empty() && !stream->inputActive())
        {
            if (!stream->inputActive())
            {
                //  buffer is flushed and input is done
                break;
            }
            usleep(20);
        }

    }

    if (result)
    {
        pthread_cond_signal(&stream->_writerCond);
    }
    stream->_outputActive = false;

    std::cout << "reader terminated : " << result << std::endl << std::flush;

    return (void* )result;
}


Streamer::Streamer(std::streambuf& in, std::streambuf& out,
                   uint32_t bufferCapacity) :
    _infile(in),
    _outfile(out),
    _outputCount(0),
    _inputActive(true),
    _outputActive(true),
    _terminateStream(false),
    _buffer(64*1024, bufferCapacity),
    _writerThread(0),
    _readerThread(0)
{
    //  create our sync objects
    pthread_mutex_init(&_writerMutex, NULL);
    pthread_cond_init(&_writerCond, NULL);

    //  spin up our reader and writer threads
    int res = pthread_create(&_writerThread, NULL, Streamer::writer_thread, this);
    if (res)
    {
        std::cout << "pthread_create(writer) failed: " << res << std::endl;
        return;
    }

    res = pthread_create(&_readerThread, NULL, Streamer::reader_thread, this);
    if (res)
    {
        std::cout << "pthread_create(reader) failed: " << res << std::endl;
        return;
    }
}

Streamer::~Streamer()
{
    _terminateStream = true;
    pthread_join(_writerThread, NULL);
    pthread_join(_readerThread, NULL);
}


///////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        std::cout << "ringbuffer <in filename> <out filename>" << std::endl;
        return 1;
    }
    
    std::filebuf input;
    if (!input.open(argv[1], std::ios_base::binary | std::ios_base::in))
    {
        std::cout << "input file '" << argv[1] << "' failed to open" << std::endl;
        return 1;
    }

    std::filebuf output;
    if (!output.open(argv[2], std::ios_base::binary | std::ios_base::out))
    {
        std::cout << "output file '" << argv[2] << "' failed to open" << std::endl;
        return 1;
    }

    Streamer stream(input, output, 4);

    uint32_t frame = 0;

    while (stream.active())
    {
        if (!(frame % 60))
        {
            std::cout << "Output " << stream.outputCount() << " bytes..."
                      << std::endl << std::flush;
        }
        usleep(16666);          // approximate 60hz (60 fps)
        ++frame;
    }
    
    return 0;
}