#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/Point.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <nav_msgs/Odometry.h>
#include <sstream>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

// struct odom_struct{
//     geometry_msgs::Point point;
//     geometry_msgs::Quaternion ::quaternion;
// };

struct Point
{
    double x;
    double y;
};

class Go
{

  public:
    Go();

  private:
    tf::TransformListener listener;
    std::string odom_frame = "odom";
    std::string base_frame = "base_footprint";

    tf::StampedTransform get_odom();
    void adjust_pose();

};

Go::Go()
{

    std::vector<Point> point_vector;
    std::fstream file("/home/sky/catkin_ws/src/robot_2dnav/src/position.txt");

    if(!file.is_open()){
        ROS_INFO("file open failed!");
        return;
    }
    while(!file.eof()){
        Point point;
        file >> point.x;
        file >> point.y;
        point_vector.push_back(point);
    }


    ros::NodeHandle nh;
    ros::Publisher publisher = nh.advertise<geometry_msgs::Twist>("cmd_vel", 5);
    geometry_msgs::Point point;
    geometry_msgs::Twist twist;

    try
    {
        listener.waitForTransform(odom_frame, base_frame, ros::Time(0), ros::Duration(5));
    }
    catch (tf::TransformException ex)
    {
        ROS_INFO("Cannot find transform between odom and base_footprint");
    }

    ros::Rate r(20);
    tf::StampedTransform stampedTransform;


    //定点巡逻
    for (std::vector<Point>::iterator it = point_vector.begin(); it != point_vector.end(); it++)
    {

        double goal_x = (*it).x, goal_y = (*it).y;
        //运动到指定位置
        while (true)
        {
            stampedTransform = get_odom();
            float x_start = stampedTransform.getOrigin().x();
            float y_start = stampedTransform.getOrigin().y();
            float distance = sqrt(pow((goal_x - x_start), 2) + pow((goal_y - y_start), 2));
            if (distance < 0.05)
            {
                geometry_msgs::Twist t;
                publisher.publish(t);
                break;
            }
            twist.linear.x = distance;
            if (distance > 0.2)
            {
                twist.linear.x = 0.2;
            }

            double roll, pitch, yaw;
            tf::Matrix3x3(stampedTransform.getRotation()).getRPY(roll, pitch, yaw);
            double path_angular = atan2(goal_y - y_start, goal_x - x_start);
            double angular = fabs(path_angular - yaw);
            if (angular > 3.14)
            {
                angular = fabs(6.28 - angular);
            }
            double min_angular_1 = std::min(fabs(yaw + angular - path_angular), fabs(6.28 - fabs(yaw + angular - path_angular)));
            double min_angular_2 = std::min(fabs(yaw - angular - path_angular), fabs(6.28 - fabs(yaw - angular - path_angular)));
            if (min_angular_1 < min_angular_2)
            {
                twist.angular.z = 5 * angular;
            }
            else
            {
                twist.angular.z = -5 * angular;
            }
            if (twist.angular.z > 2.8)
                twist.angular.z = 2.80;
            else if (twist.angular.z < -2.8)
                twist.angular.z = -2.80;

            // ROS_INFO("%f", twist.linear.x);

            publisher.publish(twist);
            r.sleep();
        }
    }

    geometry_msgs::Twist t;
    publisher.publish(t);

    file.close();
}

// void adjust_pose(){
//     while(true){
//         stampedTransform = get_odom();
//         tf::Matrix3x3(stampedTransform.getRotation()).getRPY(roll, pitch, yaw);
//         double angular = fabs(path_angular - yaw);
//         if(angular > 3.14){
//             angular = fabs(6.28 - angular);
//         }

//         if(std::min(fabs(yaw + angular - path_angular), fabs(6.28 - fabs(yaw + angular - path_angular))) < std::min(fabs(yaw - angular - path_angular), fabs(6.28 - fabs(yaw - angular - path_angular)))){
//                 twist.angular.z = 5 * angular;
//             }
//             else{
//                 twist.angular.z = -5 * angular;
//             }
//         if(twist.angular.z > 2.8)
//             twist.angular.z = 2.80;
//         else if(twist.angular.z < -2.8)
//             twist.angular.z = -2.80;
//         ROS_INFO("%f", twist.angular.z);
//         ROS_INFO("%f, %f", path_angular, yaw);
//         if(fabs(yaw - path_angular) < 0.017){
//             ROS_INFO("break");
//             twist.angular.z = 0;
//             publisher.publish(twist);
//             break;
//         }
//         // twist.angular.z = 2.74;
//         // twist.angular.z = 2 * fabs(yaw - path_angular);
//         // ROS_INFO("%f", twist.angular.z);
//         publisher.publish(twist);
//         r.sleep();
//     }
// }

tf::StampedTransform Go::get_odom()
{

    tf::StampedTransform transform;
    try
    {
        listener.lookupTransform(odom_frame, base_frame, ros::Time(0), transform);
    }
    catch (tf::TransformException ex)
    {
        ROS_ERROR("%s", ex.what());
        ros::Duration(1.0).sleep();
    }

    return transform;
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "go");
    Go go;

    return 0;
}
