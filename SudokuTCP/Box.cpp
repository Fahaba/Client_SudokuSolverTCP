#include "Box.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

Box::Box(std::string cfgPath)
{
    Initialize(cfgPath);
}


Box::~Box()
{
}

void Box::Initialize(std::string cfgPath)
{
    LoadConfigFromFile(cfgPath);
}

bool Box::LoadConfigFromFile(std::string path) 
{
    std::ifstream inFile;
    inFile.open(path);
    std::string line = "";
    if (!inFile)
    {
        return false;
        exit(1);
    }

    int row = 0;
    while (std::getline(inFile, line))
    {
        std::istringstream iss(line);
        std::istream_iterator<std::string> it(iss);
        std::istream_iterator<std::string> end;
        int col = 0;
        for (; it != end ; ++it)
        {
            m_boxField[row][col] = std::stoi(it->c_str());
            col++;
        }

        row++;
    }
    return true;
}

void Box::print() 
{
    std::cout << "Box:" << std::endl;
    std::cout << "-------------------" << std::endl;
    
   
    for (auto j : m_boxField)
    {
        std::cout << "|  ";
        for (auto i : j)
            std::cout << i << "  |  ";

        std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}