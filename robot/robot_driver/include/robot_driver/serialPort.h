//
// Created by bismarck on 12/8/22.
//

#ifndef RM2022ENGINEER_SERIALPORT_H
#define RM2022ENGINEER_SERIALPORT_H

#include <thread>
#include <serial/serial.h>

#include "msg_serialize.h"
#include <iostream>
class serialPort {
private:
    serial::Serial serial;// 串口对象，用于读写串口数据
    Listener listener;// Listener 对象，用于管理回调函数
    std::thread listenerThread;// 线程对象，用于监听串口数据

public:
    std::string name;
    serialPort() = default;
    explicit serialPort(std::string _name);
    ~serialPort();
    bool init();// 初始化串口
    void loop();// 串口读取循环

    template<class T>
    void sendMsg(int id, T msg) {// 将消息序列化
        std::string s = serilize(id, msg);//id：消息的 ID，用于标识消息类型
        try {
            serial.write(s);// 发送序列化的消息 
            /*在终端打印串口输出
            auto ch_str = s.data();
            for(int i = 0; i<11; i++)
                std::cout<<std::hex<<(uint8_t)(ch_str[i])<<" ";
            std::cout<<std::endl;
             */
        } catch (serial::IOException &e) {
            init();
            std::cout << "serial write error" << std::endl;
        }
    }
    //注册回调模板函数
    template<typename T>
    bool registerCallback(int id, std::function<void(T)> userCallback) {
        return listener.registerCallback(id, userCallback);
    }
    //它将用户提供的回调函数 userCallback 注册到 listener 中，并将其与 id 相关联
};


#endif //RM2022ENGINEER_SERIALPORT_H
