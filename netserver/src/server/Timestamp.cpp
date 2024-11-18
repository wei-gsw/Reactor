#include "Timestamp.h"

// zh: 默认构造函数，生成当前时间戳
Timestamp::Timestamp()
{
    secsinceepoch_ = time(0);
}
// zh: 构造函数，根据秒数生成时间戳
Timestamp::Timestamp(uint64_t secsinceepoch) : secsinceepoch_(secsinceepoch)
{
}
// zh: 字符串形式的当前时间
std::string Timestamp::toString() const
{
    char timestr[64] = {0};
    tm *timeinfo = localtime(&secsinceepoch_);
    snprintf(timestr, sizeof(timestr), "%04d-%02d-%02d %02d:%02d:%02d",
             timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
             timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return std::string(timestr);
}

// int形式的当前时间
time_t Timestamp::toInt() const
{
    return secsinceepoch_;
}

// zh: 静态函数，返回当前时间戳
Timestamp Timestamp ::now()
{
    return Timestamp();
}