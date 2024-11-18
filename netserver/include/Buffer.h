#pragma once
#include <string>
#include <vector>
#include <iostream>

class Buffer
{
private:
    std::string buf_; //用于存放数据
    const uint16_t sep_; //报文分隔符

public:
    Buffer(uint16_t sep_=1);
    ~Buffer();


    void erase(size_t pos, size_t len);// erase data from buffer
    void append(const char *data, size_t size_); // append data to buffer
    void append_head(const char *data, size_t size_); // 将报文长度加到buffer的头部
    size_t size();                               // return the size of buffer
    void clear();                                // clear the buffer
    const char *data();                          //   return the data of buffer
    //从buf拆分报文,存放在ss中,并返回是否成功
    bool pickup_msg(std::string &ss);
};