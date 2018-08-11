#include <vector>
#include <ros/ros.h>
#include <nav_msgs/GetMap.h>

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
                if (abs((*it_line).top - (*it_area).p_3.y) < 5 && abs((*it_line).bottom - (*it_area).p_4.y) < 5)
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

    ROS_INFO("%d", int(areaVec.size()));

    return areaVec;
}