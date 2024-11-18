#include <string.h>
#include "Buffer.h"

Buffer::Buffer(uint16_t sep) : sep_(sep)
{
}
Buffer::~Buffer()
{
}

// append data to buffer
void Buffer::append(const char *data, size_t size_)
{
    buf_.append(data, size_);
}

// return the size of buffer
size_t Buffer::size()
{
    return buf_.size();
}

// clear the buffer
void Buffer::clear()
{
    buf_.clear();
}

//   return the data of buffer
const char *Buffer::data()
{
    return buf_.data();
}

//   erase data from buffer
void Buffer::erase(size_t pos, size_t len)
{
    buf_.erase(pos, len);
}

// 将报文长度加到buffer的头部
void Buffer::append_head(const char *data, size_t size)
{
    if (sep_ == 0)
    {
        buf_.append(data, size);
    }
    else if (sep_ == 1)
    {
        buf_.append((char *)&size, 4);
        buf_.append(data, size);
    }else
    {
        // TODO: 其他情况
    }
    // TODO: 其他情况
}

bool Buffer::pickup_msg(std::string &ss)
{
    if (buf_.size() == 0)
    {
        return false;
    }
    if (sep_ == 0)
    {
        ss = buf_;
        buf_.clear();
    }
    else if (sep_ == 1) // 四字节包头
    {
        int len;
        memcpy(&len, buf_.data(), 4);
        if (len > buf_.size() - 4)
        {
            return false;
        }
        ss = buf_.substr(4, len);
        buf_.erase(0, len + 4);
    }
    return true;
}
