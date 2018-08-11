#include<iostream>
#include<vector>

struct point{
    int x;
    int y;
};

main(int argc, char const *argv[])
{
    std::vector<int*> v;

    for(int i = 0; i < 3; i++){
        int *p = new int[2];
        p[0] = i;
        p[1] = i + 1;
        // std::cout << &x << std::endl;
        v.push_back(p);
    }
    
    
    for(std::vector<int*>::iterator it = v.begin(); it != v.end(); it++)
    {
        std::cout << (*it)[0] << " " << (*it)[1] << std::endl;
        delete[] (*it);
        std::cout << (*it)[0] << " " << (*it)[1] << std::endl;
    }
    

    return 0;
}
