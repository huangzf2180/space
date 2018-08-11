#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_broadcaster.h>
#include <sstream>
#include <nav_msgs/GetMap.h>
#include <ctime>
#include<iostream>
// #include<boustrophedon.cpp>

#define MAP_PIXEL_WIDTH 384
#define MAP_PIXEL_HEIGHT 384

using std::vector;

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

// struct pixel_point
// {
//     int x;
//     int y;
// };

// struct point
// {
//     double x;
//     double y;
// };

// vector<point> get_mapdata_array()
// {

//     ros::NodeHandle nh;
//     ros::ServiceClient sc = nh.serviceClient<nav_msgs::GetMap>("static_map", true);
//     // while(!sc.waitForService(ros::Duration(5))){
//     //     ROS_INFO("can not connect to server");
//     //     return;
//     // }

//     nav_msgs::GetMap srv;
//     if (!sc.call(srv))
//     {
//         ROS_INFO("call service -- static_map failed");
//         // return nullptr;
//     }

//     int pixel_point_array[MAP_PIXEL_HEIGHT][MAP_PIXEL_WIDTH];
//     nav_msgs::OccupancyGrid map_data = srv.response.map;

//     for (int i = 0; i < MAP_PIXEL_HEIGHT; i++)
//     {
//         for (int j = 0; j < MAP_PIXEL_HEIGHT; j++)
//         {
//             pixel_point_array[i][j] = map_data.data[i * MAP_PIXEL_WIDTH + j] + 0;
//             // if (pixel_point_array[i][j] == 0)
//             // {
//             //     // std::cout << 0;
//             // }
//         }
//         // std::cout << std::endl;
//     }

//     // int **p = pixel_point_array;

//     for (int i = 0; i < MAP_PIXEL_HEIGHT; i++)
//     {
//         for (int j = 0; j < MAP_PIXEL_HEIGHT; j++)
//         {
//             if (pixel_point_array[i][j] == 100)
//             {

//                 for (int x = (i - 4) > 0 ? i - 4 : 0; x <= ((i + 4) >= MAP_PIXEL_HEIGHT ? MAP_PIXEL_HEIGHT - 1 : i + 4); x++)
//                 {

//                     for (int y = (j - 4) > 0 ? j - 4 : 0; y <= ((j + 4) >= MAP_PIXEL_WIDTH ? MAP_PIXEL_WIDTH - 1 : j + 4); y++)
//                     {
//                         if(pixel_point_array == 0)
//                             pixel_point_array[x][y] = -2;
//                     }
//                 }
//             }
//         }
//     }
//     ROS_INFO("6");

//     vector<point> vec;
//     for (int i = 0; i < MAP_PIXEL_HEIGHT; i++)
//     {
//         for (int j = 0; j < MAP_PIXEL_HEIGHT; j++)
//         {
//             if (pixel_point_array[i][j] == 0)
//             {
//                 point p;
//                 p.x = (j - 200) * 0.05;
//                 p.y = (184 - i) * 0.05;
//                 vec.push_back(p);
//             }
//         }
//     }

//     return vec;
// }


using std::vector;

#define width_pixel 384
#define height_pixel 384

struct Point
{
    double x;
    double y;
};

struct Line
{
    int top;
    int bottom;
    bool isFree;
};

class Area
{

  public:
    int area_id;
    bool isComplete;
    Point p_1, p_2, p_3, p_4;

    Area(int);

  private:
};

Area::Area(int id)
{
    area_id = id;
    isComplete = false;
}

vector<Area> get_areaVec()
{
    ros::NodeHandle nh;
    ros::ServiceClient client = nh.serviceClient<nav_msgs::GetMap>("static_map");
    nav_msgs::GetMap map_srv;
    if (!client.call(map_srv))
    {
        ROS_ERROR("Failed to call service");
    }

    nav_msgs::OccupancyGrid map = map_srv.response.map;

    //地图边缘点
    double left = width_pixel, top = height_pixel, right = -1, bottom = -1;
    int line_end[width_pixel] = {0};
    int line_begin[width_pixel] = {height_pixel};

    int map_array[height_pixel][width_pixel];
    for (int i = 0; i < map.data.size(); i++)
    {
        map_array[i / width_pixel][i % width_pixel] = map.data[i] + 0;
        if (map_array[i / width_pixel][i % width_pixel] == 0)
        {
            if (left > i % width_pixel)
                left = i % width_pixel;
            if (top > i / width_pixel)
                top = i / width_pixel;
            if (right < i % width_pixel)
                right = i % width_pixel;
            if (bottom < i / width_pixel)
                bottom = i / width_pixel;
            if (line_begin[i % width_pixel] > width_pixel)
                line_begin[i % width_pixel] = width_pixel;
            if (line_end[i % width_pixel] < width_pixel)
                line_end[i % width_pixel] = width_pixel;
        }
    }

    std::cout<<left<<" "<<top<<" "<<right<<" "<<bottom<<std::endl;

    int area_count = 0;
    vector<Area> areaVec;
    vector<Line> lineVec;
    for (int j = left; j <= right; j++)
    {
        lineVec.clear();
        int begin = line_begin[j];
        int end = line_end[j];

        //获取单列的连通线段
        int line_begin = begin;
        bool recordLine = true;
        for (int i = begin; i < end; i++)
        {
            if (map_array[i][j] != 0)
            {
                Line line;
                line.isFree = true;
                line.top = line_begin;
                line.bottom = i - 1;
                recordLine = false;
                lineVec.push_back(line);
            }
            else if (!recordLine)
            {
                recordLine = true;
                line_begin = i;
            }
        }

        //逐步构建区域并加入新的区域
        for (vector<Line>::iterator it_line = lineVec.begin(); it_line != lineVec.end(); it_line++)
        {
            for (vector<Area>::iterator it_area = areaVec.begin(); it_area != areaVec.end(); it_area++)
            {
                if ((*it_area).isComplete)
                    continue;
                if (abs((*it_line).top - (*it_area).p_3.y) < 10 && abs((*it_line).bottom - (*it_area).p_4.y) < 10)
                {
                    (*it_area).p_3.x = (*it_line).top;
                    (*it_area).p_3.y += j;
                    (*it_area).p_4.x = (*it_line).bottom;
                    (*it_area).p_4.y += j;
                    (*it_line).isFree = false;
                    break;
                }
            }
            if ((*it_line).isFree)
            {
                area_count++;
                Area area(area_count);
                area.p_1.x = area.p_3.x = (*it_line).top;
                area.p_1.y = area.p_3.y = j;
                area.p_2.x = area.p_4.x = (*it_line).bottom;
                area.p_2.y = area.p_4.y = j;
                areaVec.push_back(area);
            }
        }

        for (vector<Area>::iterator it_area = areaVec.begin(); it_area != areaVec.end(); it_area++)
        {
            if ((*it_area).p_3.y < j)
                (*it_area).isComplete = true;
        }
    }

    // ROS_INFO("%d", int(areaVec.size()));
    std::cout << areaVec.size() << std::endl;

    return areaVec;
}

int main(int argc, char **argv)
{

    ros::init(argc, argv, "navigation_goals");
    MoveBaseClient ac("move_base", true);

    // while (!ac.waitForServer(ros::Duration(5.0)))
    // {
    //     ROS_INFO("Waiting for the move_base action server");
    // }

    vector<Area> v = get_areaVec();

    return 0;

    // // vector<point> vec = get_mapdata_array();

    // // move_base_msgs::MoveBaseGoal goal;
    // // goal.target_pose.header.frame_id = "map";

    // // while (true)
    // // {
    // //     srand((unsigned)time(NULL));
    // //     point p = vec.at(rand() % vec.size());
    // //     goal.target_pose.header.stamp = ros::Time::now();

    // //     goal.target_pose.pose.position.x = p.x;
    // //     goal.target_pose.pose.position.y = p.y;
    // //     goal.target_pose.pose.orientation.w = 1.0;

    // //     ROS_INFO("Sending goal");
    // //     ac.sendGoal(goal);

    // //     ac.waitForResult();

    // //     if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    // //     {
    // //         ROS_INFO("You have arrived to the goal position!");
    // //     }
    // //     else
    // //     {
    // //         ROS_INFO("The base failed for some reason");
    // //     }
    // // }

    // return 0;
}