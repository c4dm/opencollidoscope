#pragma once

#include "cinder/audio/dsp/RingBuffer.h"


/* Packs together a RingBuffer and the erlated array used to exchange data (read/write) with the ring buffer 
*/
template <typename T>
class RingBufferPack {

public:

    RingBufferPack( size_t size ) :
        mSize( size ),
        mBuffer( size )
    {
        mArray = new T[size];
    }

    ~RingBufferPack()
    {
        delete[]  mArray;
    }

    // no copy
    RingBufferPack( const RingBufferPack &copy ) = delete;
    RingBufferPack & operator=(const RingBufferPack &copy) = delete;

    ci::audio::dsp::RingBufferT<T> & getBuffer() { return mBuffer; }

    T* getExchangeArray() { return mArray; }

    std::size_t getSize() { return mSize;  }

private:
    size_t mSize;

    ci::audio::dsp::RingBufferT<T> mBuffer;

    T* mArray;


};