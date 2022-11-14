#include"Buffer.h"
#include<assert.h>
// #include<copy>
using namespace std;

Buffer::Buffer()
: _buf(BUFFER_INIT_SIZE), _readIdx(0), _start(0), _end(0)
{

}

Buffer::Buffer(int size)
: _buf(size), _readIdx(0), _start(0), _end(0)
{

}

void Buffer::append(const string& msg)
{
    if(_readIdx+msg.size()>_buf.size()){
        _buf.resize(_buf.size()*2);
    }
    std::copy(&msg[0], &msg[msg.size()], &_buf[_end]);
}

string Buffer::retrieveNBytes(int size)
{
    assert(_readIdx+size<static_cast<int>(_buf.size()));
    string temp = string(peek(), size);
    _readIdx +=size;
    
    if(size==readableBytes()){
        _start = 0;
        _end = 0;
        _readIdx = 0;
    }
    
    return temp;
}

string Buffer::retrieveAll()
{
    string temp = string(peek(), readableBytes());
    _start = 0;
    _end = 0;
    _readIdx = 0;
    return temp;
}

char* Buffer::peek()
{
    return &_buf[_readIdx];
}

int Buffer::readableBytes()
{
    int n = _end - _readIdx;
    assert(n>=0);
    return _end - _readIdx;
}





Buffer::~Buffer()
{

}

