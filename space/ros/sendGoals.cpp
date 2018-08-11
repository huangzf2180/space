#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <nav_msgs/Odometry.h>
#include <sstream>
#include <iostream>
#include <vector>
#include<ctime>
#include<cstdio>
#include "test_1.cpp"

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

float w = 1.0;

float point_array[3][2];

void doneCb(const actionlib::SimpleClientGoalState &state, const move_base_msgs::MoveBaseAction::ConstPtr &result)
{
}

void activeCb()
{
}

void feedbackCB(const move_base_msgs::MoveBaseFeedback::ConstPtr &feedback, MoveBaseClient *ac, float goal_x, float goal_y)
{
    // w = feedback->base_position.pose.orientation.w;
    float x = feedback->base_position.pose.position.x;
    float y = feedback->base_position.pose.position.y;
    // ROS_INFO("goal_x = %f, goal_y = %f", abs(x - goal_x), abs(y - goal_y));
    // ROS_INFO("current_x = %f, current_y = %f", x, y);

    if (fabs(x - goal_x) < 0.05 && fabs(y - goal_y) < 0.1)
    {
        ROS_INFO("You have arrived one goal position!");
        ac->cancelGoal();
    }
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "navigation_goals");
    MoveBaseClient ac("move_base", true);

    while (!ac.waitForServer(ros::Duration(5)))
    {
        ROS_INFO("cannot connect to server!");
    }

    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.frame_id = "map";

    std::vector<point> vec = getPointVector();
    ROS_INFO("point count = %d", int(vec.size()));

    srand((unsigned)time(NULL));

    while (true)
    {
        point p = vec.at(rand() % vec.size());
        ROS_INFO("point = %f, %f", p.x, p.y);

        // std::cout << "输入三组坐标:" << std::endl;
        goal.target_pose.header.stamp = ros::Time::now();

        // for (int i = 0; i < 3; i++)
        // {
        //     std::cin >> point_array[i][0] >> point_array[i][1];
        // }

        // for (int i = 0; i < 3; i++)
        // {
        goal.target_pose.pose.position.x = p.x;
        goal.target_pose.pose.position.y = p.y;
        goal.target_pose.pose.orientation.w = 1.0;

        ac.sendGoal(
            goal,
            MoveBaseClient::SimpleDoneCallback(),
            MoveBaseClient::SimpleActiveCallback(),
            boost::bind(&feedbackCB, _1, &ac, p.x, p.y)
        );

        ac.waitForResult();

        if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("You have arrived to the final goal position!");
        }
        else
        {
            ROS_INFO("The base failed for some reason!");
            // ROS_INFO(ac.getState().str());
        }
        // }
    }

    return 0;
}
