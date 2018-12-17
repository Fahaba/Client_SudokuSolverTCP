#pragma once
#include <array>
// move this to config or s.th.
#define dimx 3
#define dimy 3


class Box
{


public:

    Box(std::string cfgPath);
    typedef std::array<int, dimy> y;
    typedef std::array< y, dimx > BoxField; // 2d array
    BoxField m_boxField;
    void print();



    ~Box();

private:
    void Initialize(std::string cfgPath);
    bool LoadConfigFromFile(std::string path);
};

