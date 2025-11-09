#include"../include/sentry_serial/sentry_serial.h"
#include"ros/ros.h"
#include"geometry_msgs/Twist.h"
#include <stdio.h>

union 
{
  float gun[4];
  unsigned char gundan[16];

}send;

bool cmdVelReceived_ = false;
uint8_t send_c[18];



void callback(const geometry_msgs::Twist& cmd_vel)
{
    // receive the msg from cmd_vel
    cmdVelReceived_ = true;
    ROS_INFO("Receive a /cmd_vel msg\n");
    ROS_INFO("The linear  velocity: x=%f, y=%f, z=%f\n",cmd_vel.linear.x,cmd_vel.linear.y,cmd_vel.linear.z);
    ROS_INFO("The augular velocity: roll=%f, pitch=%f, yaw=%f\n",cmd_vel.angular.x, cmd_vel.angular.y, cmd_vel.angular.z);
    // put the data in union
    SendFrame send_frame;
    send_frame.header = 0xAA;
    send_frame.linear_x = cmd_vel.linear.x;
    send_frame.linear_y = cmd_vel.linear.y;
    send_frame.angular_z = cmd_vel.angular.z;
    send_frame.cmd_force_scanning=1;
    send_frame.tail = 0xBB;
    send.gun[0]=send_frame.linear_x;
    send.gun[1]=send_frame.linear_y;
    send.gun[2]=send_frame.angular_z;
    send.gun[3]=send_frame.cmd_force_scanning;
    send_c[0]=0xaa;
    for(int i=0;i<16;i++)
    {
        send_c[i+1]=send.gundan[i];
    }
    send_c[17]=0xbb;
    ROS_INFO("send header:%d,tail:%d\n",send_frame.header,send_frame.tail);
    ROS_INFO("send linear x float:%f\n",send_frame.linear_x);
    
    sentry_serial.flush ();
    sentry_serial.write(send_c,18);

    ROS_INFO("send raw data:%f\n",send_frame.linear_y);

    ROS_INFO("\nSend date finished!\n");
}

void Noreceive_callback()
{
    
    // not receive the msg from cmd_vel
    ROS_INFO("NOT Receive a /cmd_vel msg\n");
    // put the data in union
    SendFrame send_frame;
    send_frame.header = 0xAA;
    send_frame.linear_x = 0;
    send_frame.linear_y = 0;
    send_frame.angular_z = 0;
    send_frame.cmd_force_scanning=0;
    send_frame.tail = 0xBB;
    send.gun[0]=send_frame.linear_x;
    send.gun[1]=send_frame.linear_y;
    send.gun[2]=send_frame.angular_z;
    send.gun[3]=send_frame.cmd_force_scanning;
    send_c[0]=0xaa;
    for(int i=0;i<16;i++)
    {
        send_c[i+1]=send.gundan[i];
    }
    send_c[17]=0xbb;
    ROS_INFO("send header:%d,tail:%d\n",send_frame.header,send_frame.tail);
    ROS_INFO("send linear x float:%f\n",send_frame.linear_x);
    
    sentry_serial.flush ();
    sentry_serial.write(send_c,18);


    ROS_INFO("\nSend date finished!\n");
}

int main(int argc,char** argv)
{
    ros::init(argc, argv, "sentry_serial");
    ros::NodeHandle nh;
    nh.getParam("cmd_vel_topic",cmd_vel_topic);
    nh.getParam("dev_name",dev_name);

    // ros::Publisher self_hp_publisher=nh.advertise<std_msgs::Float32>("self_hp",10);

    try
    {
        sentry_serial.setPort(dev_name);
        sentry_serial.setBaudrate(115200);
        serial::Timeout timeout = serial::Timeout::simpleTimeout(500);        
        sentry_serial.setTimeout(timeout);
        sentry_serial.open();
    }
    catch(serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }
    
    if(sentry_serial.isOpen())
    {
        ROS_INFO_STREAM("Serial Port opened");
    }
    else
    {
        return -1;
    }
    ROS_INFO_STREAM("Sentry serial init Finished!");
    
    
    // uint8_t recieve_buffer[RECIEVE_FRAME_SIZE];
    // if(sentry_serial.read(recieve_buffer,RECIEVE_FRAME_SIZE))
    // {
    //     if( (recieve_buffer[0] == RECIEVE_FRAME_HEADER) && (recieve_buffer[RECIEVE_FRAME_SIZE] == RECIEVE_FRAME_TAIL) )
    //     {
    //         for(size_t i=0; i<RECIEVE_FRAME_SIZE; i++)
    //         {
    //             recieve_frame.recieve_frame_uint_array[i]=recieve_buffer[i];
    //         }
    //     }
    // }

    // std_msgs::Float32 self_hp;
    // self_hp.data=recieve_frame.self_hp;

    // self_hp_publisher.publish(self_hp);
    // ROS_INFO_STREAM("Sentry HP is: "<<self_hp.data);

    while (ros::ok())
    {
     bool cmdVelReceived_ = false;

     ros::Subscriber cmd_vel_sub = nh.subscribe(cmd_vel_topic, 1000, callback);

     if (cmdVelReceived_==false)
     {
        Noreceive_callback();
     }
     
     ros::spin();
    }
}
