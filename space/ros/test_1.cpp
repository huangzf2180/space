#include <ros/ros.h>
#include <nav_msgs/GetMap.h>
#include <iostream>
#include <cstdio>
#include <vector>

#define width_pixel 384
#define height_pixel 384

#define extra_pixel_count 6

struct pixel_point
{
    int x;
    int y;
};

struct point
{
    double x;
    double y;
};

// void set_array(int map_array[][])
// {
// }

std::vector<point> getPointVector()
{
    // ros::init("test_1");
    ros::NodeHandle nh;

    ros::ServiceClient client = nh.serviceClient<nav_msgs::GetMap>("static_map");
    nav_msgs::GetMap map;
    if (!client.call(map))
    {
        ROS_ERROR("Failed to call service");
    }

    // uint8_t c = map.response.map.header;
    // std::cout<<map.response.map.data.size()<<std::endl;
    // ros::Duration(3).sleep();
    // int map_array[] =  map.response.map.data;
    int j = 0;
    int map_array[height_pixel][width_pixel];
    for (int i = 0; i < map.response.map.data.size(); i++)
    {
        if (i % 384 == 0 && i > 1)
            j += 1;
        map_array[j][i % 384] = map.response.map.data[i] + 0;
        if (map.response.map.data[i] + 0 == 100)
        {
            // std::cout << j << " " << i % 384 << std::endl;
        }
    }

    for (int i = 0; i < height_pixel; i++)
    {
        for (int j = 0; j < width_pixel; j++)
        {
            if(i == 183 && j > 150 && j < 210){
                std::cout << map_array[i][j] << " " << std::endl;
            }
            if (map_array[i][j] == 100)
            {
                for (int x = (i - extra_pixel_count > 0 ? i - extra_pixel_count : 0); x <= (i + extra_pixel_count >= height_pixel ? height_pixel - 1 : i + extra_pixel_count); x++)
                {
                    for (int y = (j - extra_pixel_count > 0 ? j - extra_pixel_count : 0); y <= (j + extra_pixel_count >= height_pixel ? height_pixel - 1 : j + extra_pixel_count); y++)
                    {
                        if (map_array[x][y] == 0)
                        {
                            map_array[x][y] = -2;
                        }
                    }
                }
            }
        }
    }

    std::vector<pixel_point> pixel_point_vector;

    for (int i = 0; i < height_pixel; i++)
    {
        for (int j = 0; j < width_pixel; j++)
        {
            if (map_array[i][j] == 0)
            {
                pixel_point point;
                point.x = j;
                point.y = i;
                pixel_point_vector.push_back(point);
                // std::cout<< point.x << " " << point.y << std::endl;
            }
        }
    }
    ros::Duration(3).sleep();

    std::vector<point> point_vector;

    for (std::vector<pixel_point>::iterator it = pixel_point_vector.begin(); it != pixel_point_vector.end(); it++)
    {
        int x = (*it).x;
        int y = (*it).y;
        point point;
        point.x = (x - 200) * 0.05;
        point.y = (184 - y) * 0.05;
        point_vector.push_back(point);
        if(fabs(point.x) > 2.6 || fabs(point.y) > 2.6){
            // std::cout << x << " " << y << std::endl;
            // std::cout << map_array[y][x] << std::endl;
        }
        // std::cout << point.x << " " << point.y << std::endl;
        // if(x > 200  && y <= 184){
        //     point.x = (x - 200) * 0.05;
        //     point.y = (184 - y) * 0.05;
        // }
        // if(x <= 200 && y <= 184){
        //     point.x = (x - 200) * 0.05;
        //     point.y = (184 - y) * 0.05;
        // }
        // if(x <= 200 && y > 184){
        //     point.x = ( - x) * 0.05;
        //     point.y = (y - 200) * 0.05;
        // }
    }


    ROS_INFO("%d", int(point_vector.size()));
    ROS_INFO("No problem");

    // std::cout << pixel_point_vector.size();

    // ros::Subscriber subscriber = nh.subscribe<nav_msgs::OccupancyGrid>("map");

    return point_vector;
}
