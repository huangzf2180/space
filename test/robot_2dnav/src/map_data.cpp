// #include<ros/ros.h>
// #include<iostream>
// #include<nav_msgs/GetMap.h>
// #include<vector>
// // #include"map_data.h"

// using std::vector;

// #define MAP_PIXEL_WIDTH 384
// #define MAP_PIXEL_HEIGHT 384

// struct pixel_point{
//     int x;
//     int y;
// };

// int** get_mapdata_array(){

//     ros::NodeHandle nh;
//     ros::ServiceClient sc = nh.serviceClient<nav_msgs::GetMap>("static_map");
//     if(!sc.waitForServer(ros::Duration(5))){
//         ROS_INFO("can not connect to server");
//         return NULL;
//     }

//     nav_msgs::GetMap srv;
//     if(!sc.call(srv)){
//         ROS_INFO("call service -- static_map failed");
//         return NULL;
//     }

//     int pixel_point_array[][MAP_PIXEL_WIDTH];
//     nav_msgs::OccupancyGrid map_data = srv.response.map;
    
//     for(int i = 0; i < MAP_PIXEL_HEIGHT; i++)
//     {
//         for(int j = 0; j < MAP_PIXEL_HEIGHT; j++){
//             pixel_point_array[i][j] = map_data.data[i * MAP_PIXEL_WIDTH + j] + 0;
//             if(array[i][j] == 0){
//                 std::cout << 0;
//             }
//         }
//         std::cout<<std::endl;
//     }

//     return pixel_point_array;
// }