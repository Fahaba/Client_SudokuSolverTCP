#include "Box.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

Box::Box(std::string cfgPath, std::string name)
{
    m_name = name;
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
    std::cout << "Box: " << m_name << std::endl;
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

std::vector<std::string> Box::GetBoxNames()
{
    return m_boxNames;
}

void Box::AddOtherBox(std::string boxName, std::string boxAddr)
{
    GridMember newBox;
    std::vector<std::string> tokens;
    std::istringstream tok(boxAddr);
    std::string token;
    while (std::getline(tok, token, ','))
    {
        tokens.push_back(token);
    }

    newBox.ip = tokens[0];
    newBox.port = stoi(tokens[1]);

    m_otherBoxes.push_back(std::make_pair(boxName, newBox));
}