//
// Created by bismarck on 12/8/22.
//

#ifndef MSG_SERIALIZE_WRITER_H
#define MSG_SERIALIZE_WRITER_H

#include <string>
#include <cstring>
#include "check.h"
#include "header_process.h"
#include "iostream"

template<typename T>
std::string serilize(int id, T t) {
    /*序列化方式2
    std::string serilized_str;
    uint8_t s_u8[255];
    int size_t = sizeof (t);
    std::cout<<"size: "<<size_t<<std::endl;
    memset(&s_u8[0], 0xAA,1);
    memset(&s_u8[1], (size_t + 4), 1);
    memset(&s_u8[2], id, 1);
    memcpy(&s_u8[3], &t, size_t);
    memset(&s_u8[size_t + headerSize], checkFunc(s_u8, size_t+headerSize), 1);
    memset(&s_u8[size_t + 4], 0, (255-size_t-4));
    serilized_str = (char*)s_u8;
    serilized_str.resize(headerSize + size_t + checkSize);
    return serilized_str;
     */
    std::string s;
    s.resize(headerSize + sizeof(T) + checkSize + tailSize);// 分配足够的空间，包含头部、数据和校验码
    parseToHeader((uint8_t*)s.data(), id, sizeof(T));// 将头部数据写入字符串的开头

//     static void parseToHeader(uint8_t* data, int id, int size) {
//     data[0] = 0xAA;
//     data[1] = (uint8_t)(size + headerSize + checkSize);
//     data[2] = (uint8_t)id;
// }
    // 将数据体 t 拷贝到字符串中，紧跟在头部之后
    memcpy((void*)(s.data()+headerSize), &t, sizeof(T));
    // std::cout<<sizeof(T)<<std::endl;
     // 计算校验码并将其写入字符串末尾
    s[sizeof(T)+headerSize] = checkFunc((uint8_t*)s.data(), sizeof(T)+headerSize);  
    // std::cout<<sizeof(T)+headerSize+1<<std::endl;
    s[sizeof(T)+headerSize+1] = (uint8_t)0xBB;
    return std::move(s);
}

#endif //MSG_SERIALIZE_WRITER_H
