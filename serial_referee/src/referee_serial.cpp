//
// Created by ckyf on 23-3-27.
//
#include <string>
#include <ros/ros.h>
#include "serial_referee/message_game_status.h"
#include "serial_referee/message_game_hurt.h"
#include "serial_referee/message_game_command.h"
#include "serial_referee/message_game_HP.h"
#include "serial_referee/message_game_Power.h" 
#include <iostream>
#include "../include/uart.h"

ros::Publisher game_status_publisher;
ros::Publisher game_HP_publisher;
ros::Publisher game_commmand_publisher;
ros::Publisher game_hurt_publisher;
ros::Publisher game_Power_publisher;
namespace Referee {
    typedef struct __attribute__((packed))
    {
        uint8_t game_type : 4;
        uint8_t game_progress : 4;
        uint16_t stage_remain_time;
        uint64_t SyncTimeStamp;
    } ext_game_status_t;//比赛状态信息

    typedef struct __attribute__((packed))
    {
        uint16_t red_1_robot_HP;
        uint16_t red_2_robot_HP;
        uint16_t red_3_robot_HP;
        uint16_t red_4_robot_HP;
        uint16_t red_5_robot_HP;
        uint16_t red_7_robot_HP;
        uint16_t red_outpost_HP;
        uint16_t red_base_HP;
        uint16_t blue_1_robot_HP;
        uint16_t blue_2_robot_HP;
        uint16_t blue_3_robot_HP;
        uint16_t blue_4_robot_HP;
        uint16_t blue_5_robot_HP;
        uint16_t blue_7_robot_HP;
        uint16_t blue_outpost_HP;
        uint16_t blue_base_HP;
    } ext_game_robot_HP_t;//血量信息


    typedef struct __attribute__((packed))
    {
        uint8_t armor_id : 4;
        uint8_t hurt_type : 4;
    } ext_robot_hurt_t;//收击指令
 
      typedef struct __attribute__((packed))
    {
        uint16_t chassis_voltage;
        uint16_t chassis_current ;
        float_t chassis_power;
        uint16_t buffer_energy;
        uint16_t shooter_17mm_1_barrel_heat;
        uint16_t shooter_17mm_2_barrel_heat;
        uint16_t shooter_42mm_barrel_heat;
    } ext_robot_power_t;//功率

    void get_status(uint8_t* msg){
        ext_game_status_t ext_status = *(ext_game_status_t*)msg;
        serial_referee::message_game_status status_msg;
        status_msg.game_type = ext_status.game_type,
        status_msg.game_progress = ext_status.game_progress,
        status_msg.stage_remain_time = ext_status.stage_remain_time,
        status_msg.SyncTimeStamp = ext_status.SyncTimeStamp,
        game_status_publisher.publish(status_msg);
    }
    
    void get_HP(uint8_t* msg){
        ext_game_robot_HP_t ext_HP = *(ext_game_robot_HP_t*)msg;
        serial_referee::message_game_HP HP_msg;
        HP_msg.red_1_robot_HP = ext_HP.red_1_robot_HP;
        HP_msg.red_2_robot_HP = ext_HP.red_2_robot_HP;
        HP_msg.red_3_robot_HP = ext_HP.red_3_robot_HP;
        HP_msg.red_4_robot_HP = ext_HP.red_4_robot_HP;
        HP_msg.red_5_robot_HP = ext_HP.red_5_robot_HP;
        HP_msg.red_7_robot_HP = ext_HP.red_7_robot_HP;
        HP_msg.red_outpost_HP = ext_HP.red_outpost_HP;
        HP_msg.red_base_HP = ext_HP.red_base_HP;
        HP_msg.blue_1_robot_HP = ext_HP.blue_1_robot_HP;
        HP_msg.blue_2_robot_HP = ext_HP.blue_2_robot_HP;
        HP_msg.blue_3_robot_HP = ext_HP.blue_3_robot_HP;
        HP_msg.blue_4_robot_HP = ext_HP.blue_4_robot_HP;
        HP_msg.blue_5_robot_HP = ext_HP.blue_5_robot_HP;
        HP_msg.blue_7_robot_HP = ext_HP.blue_7_robot_HP;
        HP_msg.blue_outpost_HP = ext_HP.blue_outpost_HP;
        HP_msg.blue_base_HP = ext_HP.blue_base_HP;
        game_HP_publisher.publish(HP_msg);
    }
    
    void get_hurt(uint8_t* msg){
        ext_robot_hurt_t ext_hurt = *(ext_robot_hurt_t*)msg;
        serial_referee::message_game_hurt hurt_msg;
        hurt_msg.armor_id = ext_hurt.armor_id;
        hurt_msg.hurt_type = ext_hurt.hurt_type;
        game_hurt_publisher.publish(hurt_msg);
    }

     void get_power(uint8_t* msg){
        ext_robot_power_t ext_power = *(ext_robot_power_t*)msg;
        serial_referee::message_game_Power power_msg;
        power_msg.chassis_voltage = ext_power.chassis_voltage;
        power_msg.chassis_current = ext_power.chassis_current;
        power_msg.chassis_power = ext_power.chassis_power;
        power_msg.chassis_voltage = ext_power.chassis_voltage;
        power_msg.shooter_17mm_1_barrel_heat = ext_power.shooter_17mm_1_barrel_heat;
        power_msg.shooter_17mm_2_barrel_heat = ext_power.shooter_17mm_2_barrel_heat;
        power_msg.shooter_42mm_barrel_heat = ext_power.shooter_42mm_barrel_heat;
        game_Power_publisher.publish(power_msg);
    }
}

bool scan() {//循环扫描函数
    int eraseSize = 0;

//在开始扫描之前，首先检查 uart_buff 中的可用数据长度是否足够构成一个完整的帧（包括帧头 Head 和帧尾 Tail）。如果数据长度不足，直接返回 false，表示没有找到完整帧。
// sizeof(Head) + sizeof(Tail) + 1：这是最小帧的大小，确保至少有一个帧头、帧尾以及数据部分。
    if ((int)uart_buff.size() < sizeof(Head) + sizeof(Tail) +1) {
        return false;
    }
    bool okHeadFound = false;
    bool frameFound = false;
    for (long i = 0; i < ((long)uart_buff.size() - sizeof(Head) - sizeof(Tail)); i=i+1) {
        uint8_t* p = (uint8_t*)uart_buff.data() + i;//指向当前扫描位置的指针
        int size = (int)uart_buff.size() - i;
        Head head;
        memcpy(&head, p, sizeof(Head));//将当前指针位置的内容复制到 Head 结构中，以便后续检查

        int headCheckerResult = checker_head(head);//检查帧头是否合法。如果帧头不合法，则跳过当前字节，继续扫描下一字节
        if (headCheckerResult) {
            if (!okHeadFound) {
                eraseSize = i-1;
            }
            continue;
        }
        //ROS_INFO("OK444444!!!");

        //获取帧长度并检查数据完整性
        int length = getLength(head);
        //果当前缓冲区剩余的数据长度小于帧的总长度，说明整个帧还没有完全接收。此时将 okHeadFound 设置为 1，并继续等待更多数据
        if (size < length ) {
            okHeadFound = 1;
            continue;
        }
      //检查帧尾并验证帧完整性
        Tail tail;
        memcpy(&tail, p + length - 2, sizeof(Tail));//从帧的末尾位置提取帧尾信息
        if(checker_all(head, tail, p)){//检查整个帧是否完整（包括帧头和帧尾的校验）。如果帧不合法，继续扫描下一个字节
            continue;
        }
        eraseSize = i + head.length;
        p = (uint8_t*)uart_buff.data() + i + 7;

        //ROS_INFO("current buff length = %ld", uart_buff.size());
        //ROS_INFO("referee message has detected and length = %d",head.length);
        switch (head.cmd_id) {
            case 0x0001:
                if(head.length == 11){
                    Referee::get_status(p);
                }
                break;
            case 0x0003:
                if(head.length == 32){
                    Referee::get_HP(p);
                }
                break;
            case 0x0206:
                if(head.length == 1){
                    Referee::get_hurt(p);
                }    
                break;
            case 0x0202:
                if(head.length == 16){
                    Referee::get_power(p);
                }
                break;     
        }
        eraseSize = i + size;
        frameFound = true;
    }
    uart_buff.erase(0, eraseSize);//更新缓冲区
    return frameFound;
}

int main(int argc, char** argv) {
    std::string serial_name;
    ros::init(argc, argv, "serial_referee");
    ros::NodeHandle nh("~");
    nh.param<std::string>("serial_referee_name", serial_name, "/dev/ttyACM0");//串口名

    auto uart_com = open(serial_name.data(), O_RDWR);//表示以读写模式打开串口设备
    if(uart_com == -1){
        ROS_INFO("UART OPEN FAIL!");
        return 0;
    }
    struct termios Opt;
    tcgetattr(uart_com, &Opt);//：获取当前串口的属性，并存储在 Opt 结构体中
    cfsetispeed(&Opt,B115200);//设置输入波特率B115200
    cfsetospeed(&Opt,B115200);//设置输出的波特率为 B115200
    Opt.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
//     ICANON：关闭规范模式（Canonical mode）。规范模式是指输入以行为单位处理，在关闭规范模式后，输入被立即处理（不再等待回车），这通常用于非阻塞或实时数据处理。
//     ECHO：关闭回显（Echo）。关闭后，输入的字符不会显示在终端上。
//     ECHOE：关闭回显擦除（Echo erase）。关闭后，删除字符时不会回显删除字符。
// I   SIG：关闭信号处理。关闭后，输入特殊字符如 Ctrl+C 不会触发信号传递（如 SIGINT）
    Opt.c_oflag  &= ~OPOST;   /*Output*/
// OPOST：关闭输出处理。关闭后，输出数据将直接发送，不做任何处理或转换。这通常用于原始数据传输。
    tcsetattr(uart_com,TCSANOW,&Opt);
//TCSANOW：立即生效，表示将新的配置立即应用到串口，而不等待传输完成或其他延迟。

//话题发布
    game_status_publisher = nh.advertise<serial_referee::message_game_status>("/referee/status", 5);
    game_HP_publisher = nh.advertise<serial_referee::message_game_HP>("/referee/HP", 5);
    // game_commmand_publisher = nh.advertise<serial_referee::message_game_command>("/referee/command", 5);
    game_hurt_publisher = nh.advertise<serial_referee::message_game_hurt>("/referee/hurt", 5);
    game_Power_publisher = nh.advertise<serial_referee::message_game_Power>("/referee/Power", 5);
    ROS_INFO("Loop Start!");
    while(ros::ok()){
        uint8_t frame_hex[1] = {0x00};
        //read() 函数从 uart_com（之前打开的串行端口文件描述符）中读取 1 个字节的数据并存储在 frame_hex 数组中。
        if(read(uart_com, &frame_hex, 1)){
            //std::cout<<"HEX:"<<(int)frame_hex[0]<<std::endl;
            uart_buff.push_back(frame_hex[0]);
            scan();
        }else{
            ROS_INFO("Nothing Received!");
        }
        ros::spinOnce();
    }
    return 0;
}