#ifndef BUFFER_H
#define BUFFER_H

#include<string>
#include<vector>

class Buffer
{
public:
    static const int BUFFER_INIT_SIZE=4096;
    Buffer();
    Buffer(int size);
    ~Buffer();

    char *peek();
    int readableBytes();
    void retrieve(int len);
    void append(const std::string& msg);
    std::string retrieveAll();
    std::string retrieveNBytes(int len);


private:
    std::vector<char> _buf;
    int _readIdx;
    int _start;
    int _end;
    int _size;

};

#endif




