//
// Created by bismarck on 12/8/22.
//

#include <iostream>
#include <utility>

#include <ros/ros.h>

#include "robot_driver//serialPort.h"

using std::cout;
using std::endl;

bool serialPort::init() {
    if (name.empty()) {// 如果 name 为空，尝试默认设备
        serial.setPort("/dev/ttyUSB0");
        serial.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);// 设置超时
        serial.setTimeout(to);
        serial.open();// 打开串口
        cout << "SYSTEM USB NOT DETECTED" << endl;

        // 如果 /dev/ttyUSB0 打开失败，尝试使用 /dev/ttyUSB1
        if (!serial.isOpen()) {
            serial.setPort("/dev/ttyUSB1");
            serial.open();
        }
    } else {
        cout << "usb name is" << name << endl;
        serial.setPort(name);
        serial.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        serial.setTimeout(to);
        serial.open();

        // 如果没有启动监听线程，启动它。循环读数
        if (!listenerThread.joinable()) {
            listenerThread = std::thread(&serialPort::loop, this);
        }else{
        }
    }

    //检测串口是否已经打开，并给出提示信息
    if (serial.isOpen()) {
        cout << "Serial Port initialized!" << endl;
        return true;
    } else {
        cout << "Unable to open port " << endl;
        return false;
    }
}

serialPort::serialPort(std::string _name) {
    cout<<"init!"<<endl;
    name = std::move(_name);
    init();//构造函数中初始化串口
    cout<<"init success!"<<endl;
}
//关闭串口
serialPort::~serialPort() {
    if (serial.isOpen()) {
        serial.close();
    }
}

//串口读取检验
void serialPort::loop() {
    std::cout << "listener thread start" << std::endl;
    while (ros::ok()) {
        uint8_t data;
        try {
            serial.read(&data, 1);
            listener.append(*(char*)&data);
        } catch (serial::IOException &e) {
            init();
            std::cout << "serial read error" << std::endl;
            continue;
        }
    }
}
