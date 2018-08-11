#include <vector>
#include <ros/ros.h>
#include <nav_msgs/GetMap>
using std::vector;

#define width_pixel 384
#define height_pixel 384

struct Point
{
    double x;
    double y;
};

class Area
{

  public:
  private:
    int area_id;
    Point p_1, p_2, p_3, p_4;
};

vector<area> get_areaVec()
{
    ros::NodeHandle nh;
    ros::ServiceClient client = nh.serviceClient<nav_msgs::GetMap>("static_map");
    nav_msgs::GetMap map;
    if (!client.call(map))
    {
        ROS_ERROR("Failed to call service");
    }

    //地图边缘点
    double left = width_pixel, top = height_pixel, right = -1, bottom = -1;
    int line_end[width_pixel] = {0};
    int line_begin[width_pixel] = {height_pixel};

    int j = 0;
    int map_array[height_pixel][width_pixel];
    for (int i = 0; i < map.response.map.data.size(); i++)
    {
        if (i % width_pixel == 0 && i > 1)
            j += 1;
        map_array[j][i % width_pixel] = map.response.map.data[i] + 0;
        if (map_array[j][i % width_pixel] == 0)
        {
            if (left > i % width_pixel)
                left = i % width_pixel;
            if (top > j)
                top = j;
            if (right < i % width_pixel)
                right = i % width_pixel;
            if (bottom < j)
                bottom = j;
            if (line_begin[i % width_pixel] > j)
                line_begin[i % width_pixel] = j;
            if (line_end[i % width_pixel] < j)
                line_end[i % width_pixel] = j;
        }
    }

    double Point p1, p2, p3, p4;
    //切割线
    int line = left;
    //判断当前列空间是否已全部包括在已划分区域内
    bool isLineAvailable[width_pixel] = {true};
    int area_count = 0;

    while (true)
    {
        for (int j = left; j <= right; j++)
        {
            if (!isLineAvailable[j])
                continue;

            int begin = line_begin[j];
            int end = line_end[j];
            Area area;
            //设置区域左侧顶点坐标
            area.p_1.x = i;
            area.p_1.y = j;
            for (int i = begin; i < end; i++)
            {
                if(map_array[i][j] == 0){
                    map_array[i][j] = 1;
                }
                else if (map_array[i][j] == 100)
                {
                    area.p_2.x = i;
                    area.p_2.y = j;
                    for (int k = i; k <= end; k++)
                    {
                        if (map_array[k][j] == 0)
                        {
                            line_begin[j] = k;
                            break;
                        }
                    }
                    break;
                }
            }
            if (i == end)
                line_begin[j] = i;
            if (line_begin[j] == line_end[j])
                isLineAvailable[j] = false;

            //区域前一列顶点坐标
            Point pline_top;
            Point pline_bottom;
            pline_top.x = area.p_1.x;
            pline_top.y = area.p_1.y;
            pline_bottom.x = area.p_2.x;
            pline_bottom.y = area.p_2.y;

            while (++j <= right)
            {
                if (!isLineAvailable[j])
                    continue;

                int base_x = pline_top.x;

                bool isEnd_top = true;
                bool isEnd_bottom = true;
                //判断上边界是否超出区域
                if(map_array[base_x][j] == 0){
                    for(int t = base_x; t < base_x - 4 > 0 ? base_x - 4: 0; t--){
                        if(map_array[t] != 0){
                            pline_top.x = t+1;
                            pline_top.y = j;
                            isEnd_top = false;
                            break;
                        }
                    }

                }
                else{
                    for(int t = base_x; t < base_x + 4 < height_pixel ? base_x + 4: height_pixel; t++){
                        if(map_array[t] == 0){
                            pline_top.x = t;
                            pline_top.y = j;
                            isEnd_top = false;
                            break;
                        }
                    }
                }

                //判断下边界是否超出区域
                base_x = pline_bottom.x;
                if(map_array[base_x][j] != 0){
                    for(int t = base_x; t < base_x - 4 > 0 ? base_x - 4: 0; t--){
                        if(map_array[t] == 0){
                            pline_bottom.x = t;
                            pline_bottom.y = j;
                            isEnd_bottom = false;
                            break;
                        }
                    }

                }
                else{
                    for(int t = base_x; t < base_x + 4 < height_pixel ? base_x + 4: height_pixel; t++){
                        if(map_array[t] != 0){
                            pline_top.x = t - 1;
                            pline_top.y = j;
                            isEnd_bottom = false;
                            break;
                        }
                    }
                }

                bool isEnd = isEnd_top || isEnd_bottom;
                if(isEnd){
                    area.p_3.x = pline_top.x;
                    area.p_3.y = pline_top.y;
                    area.p_4.x = pline_bottom.x;
                    area.p_4.y = pline_bottom.y;
                    break;
                }

                // int begin = line_begin[j];
                // int end = line_end[j];

                // for (int i = begin; i < end; i++)
                // {
                //     if (map_array[i][j] == 100)
                //     {
                //         area.p_2.x = i;
                //         area.p_2.y = j;
                //         for (int k = i; k <= end; k++)
                //         {
                //             if (map_array[k][j] == 0)
                //             {
                //                 line_begin[j] = k break;
                //             }
                //         }
                //         break;
                //     }
                // }
            }
        }
    }
}