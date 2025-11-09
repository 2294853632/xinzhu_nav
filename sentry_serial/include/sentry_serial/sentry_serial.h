#ifndef SENTRY_SEND_H
#define SENTRY_SEND_H
#include"serial/serial.h"
#include"iostream"
#include"std_msgs/Float32.h"

#define RECIEVE_FRAME_SIZE 6
#define RECIEVE_FRAME_HEADER 0x55
#define RECIEVE_FRAME_TAIL 0x44

std::string cmd_vel_topic;
std::string dev_name;

serial::Serial sentry_serial;

//header:0xAA int(170)  tail:0xBB int(187)

union SendFrame
{
    uint8_t send_frame_uint_array[18]={/*header*/0xAA,
                         /*linear_x*/0x00,0x00,0x00,0x00,
                         /*linear_y*/0x00,0x00,0x00,0x00,
                         /*angular*/0x00,0x00,0x00,0x00,
                         /*cmd_force_scanning*/0x00,0x00,0x00,0x00,
                         /*tail*/0xBB};
    struct
    {
        uint8_t header;
        float linear_x;
        float linear_y;
        float angular_z;
        int cmd_force_scanning;
        uint8_t tail;
    };
};

// union RecieveFrame
// {
//     uint8_t recieve_frame_uint_array[RECIEVE_FRAME_SIZE]={0};
//     struct
//     {
//         uint8_t header;
//         float self_hp;
//         uint8_t tail;
//     };
// }recieve_frame;

#endif