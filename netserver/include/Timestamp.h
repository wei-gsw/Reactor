#pragma once
#include <iostream>
#include <string>

class Timestamp
{
private:
    // zh: 时间戳的秒数
    time_t secsinceepoch_;

public:
    // zh: 默认构造函数，生成当前时间戳
    Timestamp();
    // zh: 构造函数，根据秒数生成时间戳
    Timestamp(uint64_t secsinceepoch);
    // zh: 字符串形式的当前时间
    std::string toString() const;

    // int形式的当前时间
    time_t toInt() const;

    // zh: 静态函数，返回当前时间戳
    static Timestamp now();
};
