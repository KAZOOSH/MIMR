// =============================================================================
//
// Copyright (c) 2010-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include "ofEvents.h"
#include "Poco/Exception.h"
#include "ofx/IO/ByteBuffer.h"


namespace ofx {
namespace IO {


class BufferedSerialDevice;


class SerialBufferEventArgs: public ofEventArgs
{
public:
    SerialBufferEventArgs(const BufferedSerialDevice& device,
                          const ByteBuffer& buffer);

    const ByteBuffer& buffer() const;
    OF_DEPRECATED_MSG("Use buffer() instead", const ByteBuffer& getBuffer() const);

    const BufferedSerialDevice& device() const;

protected:
    const ByteBuffer& _buffer;
    const BufferedSerialDevice& _device;
    
};


class SerialBufferErrorEventArgs: public SerialBufferEventArgs
{
public:
    SerialBufferErrorEventArgs(const BufferedSerialDevice& device,
                               const ByteBuffer& buffer,
                               const Poco::Exception& exception);

    const Poco::Exception& exception() const;
    OF_DEPRECATED_MSG("Use exception() instead", const Poco::Exception& getException() const);


protected:
    const Poco::Exception& _exception;

};


class SerialEvents
{
public:
    ofEvent<const SerialBufferEventArgs> onSerialBuffer;
    ofEvent<const SerialBufferErrorEventArgs> onSerialError;

};


} } // namespace ofx::IO
