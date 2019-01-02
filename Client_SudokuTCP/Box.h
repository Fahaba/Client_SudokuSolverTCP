#pragma once
#include <array>
#include <vector>
// move this to config or s.th.
#define dimx 3
#define dimy 3

/*
BOXGRID
BOX_A1 BOX_D1 BOX_G1
BOX_A4 BOX_D4 BOX_G4
BOX_A7 BOX_D7 BOX_G7
*/


struct GridMember
{
    std::string ip;
    int port;
};

class Box
{


public:

    Box(std::string cfgPath, std::string name);
    typedef std::array<int, dimy> y;
    typedef std::array< y, dimx > BoxField; // 2d array

    typedef std::array<int, 9> Cells;
    typedef std::array<Cells, 9> Rows;
    typedef std::array<Cells, 9> Cols;


    BoxField m_boxField;
    Rows m_rows;
    Cols m_cols;

    std::string m_name;
    typedef std::vector<std::pair<std::string, BoxField>> BoxGrid;
    BoxGrid m_boxGrid;
    std::vector<std::string> m_boxNames = { "BOX_A1", "BOX_D1", "BOX_G1", "BOX_A4", "BOX_D4", "BOX_G4", "BOX_A7", "BOX_D7", "BOX_G7" };
    
    typedef std::vector<std::pair<std::string, GridMember>> OtherBoxes;
    OtherBoxes m_otherBoxes;
    
    void print();
    
    std::vector<std::string> GetBoxNames();
    void SetValueInGrid(std::string boxName, int x, int y, int val);
    void AddOtherBox(std::string boxName, std::string boxAddr);
    void CalculatePossibleValues();
    std::pair<int, int> CalculateOffsetByName(std::string name);
    ~Box();

private:
    void Initialize(std::string cfgPath);
    bool LoadConfigFromFile(std::string path);

    void InitializeGrid();
   
    
protected:
    
};

