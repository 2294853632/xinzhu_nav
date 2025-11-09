#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include"std_msgs/Float32.h"

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

void callback(const std_msgs::Float32::ConstPtr self_hp)
{
    MoveBaseClient ac("move_base", true);
    if(self_hp->data<400)
    {
        try
        {
            ROS_INFO("hp is not enough\n");
            ROS_INFO("recieve hp:%f",self_hp->data);
            move_base_msgs::MoveBaseGoal hp_up_point;
            hp_up_point.target_pose.header.frame_id="map";
            hp_up_point.target_pose.pose.position.x=-2.1;
            hp_up_point.target_pose.pose.position.y=3.5;
            hp_up_point.target_pose.header.stamp=ros::Time::now();
            hp_up_point.target_pose.pose.orientation=tf::createQuaternionMsgFromYaw(0.0);
            ac.waitForServer(ros::Duration(60));
            ROS_INFO("Connected to move base server");

            ac.sendGoal(hp_up_point);
            ac.waitForResult();

            if(ac.getState()==actionlib::SimpleClientGoalState::SUCCEEDED)
            {
                ROS_INFO("You have reached the goal!");
                ros::Duration((600.0-self_hp->data)/100).sleep();
                ROS_INFO("Now sentry hp is %f",self_hp->data);
            }
            else
            {
                ROS_INFO("Reach failed for some reason");
            }
        }
        catch (tf::TransformException &ex)
        {
            ROS_ERROR("%s", ex.what());
        }
        
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "send_goals_node");
    ros::NodeHandle nh;
    tf::TransformListener listener;
    MoveBaseClient ac("move_base", true);

    ros::Subscriber sub = nh.subscribe<std_msgs::Float32>("self_hp",10,callback);

    // tf::TransformListener listener;
    // listener.transformPoint("map", base_point, map_point);

    ROS_INFO("Waiting for the move_base action server");
    ac.waitForServer(ros::Duration(60));
    ROS_INFO("Connected to move base server");


    std::vector<geometry_msgs::Point> targetPoints(5);

    nh.getParam("targetPoints0_x",targetPoints[0].x);
    nh.getParam("targetPoints0_y",targetPoints[0].y);

    targetPoints[0].x=0.;
    targetPoints[0].y=0.;

    targetPoints[1].x=3;
    targetPoints[1].y=-5;

    targetPoints[2].x=10;
    targetPoints[2].y=-10;

    targetPoints[3].x=10;
    targetPoints[3].y=-5;

    targetPoints[4].x=0;
    targetPoints[4].y=0;


    for (size_t i = 0; i < targetPoints.size(); ++i)
    {
        try
        {
            move_base_msgs::MoveBaseGoal goal;
            goal.target_pose.header.frame_id = "map";
            goal.target_pose.header.stamp = ros::Time::now();
            goal.target_pose.pose.position = targetPoints[i];
            goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(0.0);

            ROS_INFO("targetPoints[%d]: [x]=%.2f,[y]=%.2f", i,targetPoints[i].x, targetPoints[i].y);

            ROS_INFO("Sending goal");
            ac.sendGoal(goal);
            ac.waitForResult();

            if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            {
                ROS_INFO("You have reached the goal!");
                ros::Duration(10.0).sleep();
            }
            else
            {
                ROS_INFO("Reach failed for some reason");
            }
        }
        catch (tf::TransformException &ex)
        {
            ROS_ERROR("%s", ex.what());
        }
    }

    ros::spin();
}
