//
// Created by bismarck on 12/11/22.
//
#include <string>
#include <iostream>
#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PoseStamped.h>
#include "robot_driver/serialPort.h"
#include "robot_driver/aim.h"
#include "robot_driver/spinning_control.h"
#include <geometry_msgs/PoseStamped.h>
// #include <move_base_msgs/MoveBaseAction.h>
// #include <actionlib/client/simple_action_client.h>
// typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
ros::Publisher odomPub;
serialPort serial_handle;
// MoveBaseClient ac("move_base", true);
message_data odometry {
    int x, y, yaw;
    int vx, vy, wz;
};

message_data cmd {
   float vx, vy, wz;
};


cmd swing_wander{
    .vx = 0,
    .vy = 0,
    .wz = 0
};

message_data spin{
    float spin ;
};

spin spin_speed{
    .spin = 1200
};
 
message_data pitch{
    float pitch;
};

pitch target_pitch{
    .pitch=0
};

message_data yaw{
    float x,y,z,w;
};

yaw target_yaw{
   .x = 0,
   .y = 0,
   .z = 0,
   .w = 0
};

void odomCallback(odometry msg) {
//    ROS_INFO("Receive a frame of odom information!");
    nav_msgs::Odometry odom;
    odom.header.stamp = ros::Time::now();
    odom.header.frame_id = "odom";
    odom.pose.pose.position.x = msg.x / 1000.;
    odom.pose.pose.position.y = msg.y / 1000.;
    odom.child_frame_id = "base_link";
    odom.twist.twist.linear.x = msg.vx / 1000.;
    odom.twist.twist.linear.y = msg.vy / 1000.;
    odom.twist.twist.angular.z = msg.wz / 1000.;
    odom.twist.covariance = {
            1e-2, 0, 0, 0, 0, 0,
            0, 1e-2, 0, 0, 0, 0,
            0, 0, 1e-2, 0, 0, 0,
            0, 0, 0, 1e-2, 0, 0,
            0, 0, 0, 0, 1e-2, 0,
            0, 0, 0, 0, 0, 1e-2
    };

//    std::cout
//        <<odom.pose.pose.position.x<<" "
//        <<odom.pose.pose.position.y<<" "
//        <<odom.twist.twist.linear.x<<" "
//        <<odom.twist.twist.linear.y<<" "
//        <<odom.twist.twist.angular.z<<" "<<std::endl;
    odomPub.publish(odom);
}

void cmdCallback(const geometry_msgs::Twist::ConstPtr &msg) {
    std::cout<<"receive a control speed!"<<std::endl;
    if
    cmd c {
        //乘以1000防止小数位数过多
        .vx = (float)(msg->linear.x*1000),
        .vy = (float)(msg->linear.y*1000),
        .wz = (float)(msg->angular.z*1000)
    };
    std::cout<< c.vx << " "<< c.vy << " "<< c.wz << std::endl;
    serial_handle.sendMsg(0x82, c);//cmd_vel消息id为0x82
}

void swingCallback(const geometry_msgs::Twist::ConstPtr &msg) {
    swing_wander.vx = 0;
    swing_wander.vy = 0;
    swing_wander.wz = 0;
}

void spinCallback(const robot_driver::spinning_control::ConstPtr &msg){
 std::cout<<"receive a targetspinning speed!"<<std::endl;
 spin c{
    .spin = (float)(msg->spinning_speed)
 };
    std::cout<< c.spin <<std::endl;
    serial_handle.sendMsg(0x81 , c);
}

void pitchCallback(const robot_driver::aim::ConstPtr &msg){
std::cout<<"receive a targetpitch !"<<std::endl;
pitch c{
    .pitch = (float)(msg->pitch)
};
    std::cout<< c.pitch <<std::endl;
    serial_handle.sendMsg(0x80, c);
}

// void goalCallback(const geometry_msgs::PoseStamped::ConstPtr &msg){
// std::cout<<"receive a targetyaw !"<<std::endl;

// yaw c{
//     .x = msg->pose.orientation.x
//     .y = msg->pose.orientation.y
//     .z = msg->pose.orientation.z
//     .w = msg->pose.orientation.w
// };
// }


int main(int argc, char** argv) {
    ros::init(argc, argv, "robot_driver");
    ros::NodeHandle nh;

    nh.param<std::string>("serial_name", serial_handle.name, "/dev/ttyUSB0");
    // ros::Subscriber goal_sub = nh.subscribe("/move_base_simple/goal", 1, goalCallback);
    ros::Subscriber cmd_sub = nh.subscribe("/cmd_vel", 1, cmdCallback);
    // ros::Subscriber pitch_sub = nh.subscribe("/robot/logic_recommend_angle", 1, pitchCallback);
    ros::Subscriber spin_sub = nh.subscribe("/robot/spnning_speed", 1 , spinCallback);
    odomPub = nh.advertise<nav_msgs::Odometry>("/robot/odom", 5);
    serial_handle.init();
    serial_handle.registerCallback<odometry>(0x12, odomCallback);
    ros::spin();
}
