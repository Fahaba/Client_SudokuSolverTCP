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
            m_cols[col][row] = std::stoi(it->c_str());
            m_rows[row][col] = std::stoi(it->c_str());
            m_boxField[row][col] = std::stoi(it->c_str());
            col++;
        }

        row++;
    }
    m_boxGrid.push_back(std::make_pair(m_name, m_boxField));
    InitializeGrid();
    return true;
}

std::pair<int, int> Box::CalculateOffsetByName(std::string name)
{
    std::pair<int, int> offsetxy = std::make_pair(0, 0);

    offsetxy.first = (int)name[5] - 49;
    offsetxy.second = (int)name[4] - 65;

    return offsetxy;
}

void Box::InitializeGrid()
{
    for (auto box : m_boxNames)
    {
        if (box == m_name)
            continue;

        BoxField bf;
        for (int i = 0; i < bf.size(); i++)
        {
            for (int j = 0; j < bf[i].size(); j++)
            {
                bf[i][j] = 0;
            }
        }
        m_boxGrid.push_back(std::make_pair(box, bf));
    }
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

void Box::SetValueInGrid(std::string boxName, int x, int y, int val)
{
    std::pair<int, int> offsetxy = CalculateOffsetByName(boxName);
    m_rows[offsetxy.first + x]
        [offsetxy.second + y] = val;
    m_cols[offsetxy.second + y]
        [offsetxy.first + x] = val;

    for (int i = 0; i < m_rows.size(); i++)
    {
        for (int j = 0; j < m_rows[i].size(); j++)
            std::cout << m_rows[i][j];
        std::cout << std::endl;
    }
}

void Box::CalculatePossibleValues()
{
    std::pair<int, int> offsetxy = CalculateOffsetByName(m_name);

    

    for (int i = offsetxy.first; i < offsetxy.first + 3; i++)
    {
        for (int cell = offsetxy.second; cell < offsetxy.second + 3; cell++)
        {
            if (m_rows[i][cell] > 0)
                continue;

            uint64_t possible = 0b1111111111;
            uint64_t one = 0x0000000001;
            // search vertical
            for (int col = 0; col < m_cols[cell].size(); col++)
            {
                int val = m_cols[cell][col];
                if (val && (possible & (one << val)))
                {
                    possible &= ~(one << val);
                    //std::cout << "elim vert: " << val << std::endl;
                }
            }
            // search horizontal
            for (int row = 0; row < m_rows[i].size(); row++)
            {
                int val = m_rows[i][row];
                
                if (val && (possible & (one << val)))
                {
                    possible &= ~(one << val);
                    //std::cout << "elim hor: " << val << std::endl;
                }
            }
            // search in own box 3x3
            for (int box_i = offsetxy.first; box_i < offsetxy.first + 3; box_i++)
            {
                for (int box_j = offsetxy.second; box_j < offsetxy.second + 3; box_j++)
                {
                    int val = m_rows[box_i][box_j];
                    if (val && possible & (one << val))
                    {
                        possible &= ~(one << val);
                        //std::cout << "elim box: " << val << std::endl;
                    }
                }
            }

            for (int pos = 1; pos <= 9; pos++)
            {
                if (!(possible & (one << pos)))
                    continue;

                std::cout << "possible: " << pos << std::endl;

                bool possib = false;
                int x, y;
                    for (int otherBox = 0; otherBox < 9; otherBox += 3)
                    {
                        //if (otherBox == offsetxy.first)
                        //    continue;

                        bool isIn3x3 = false;
                        for (int box_i = otherBox; box_i < otherBox + 3; box_i++)
                        {
                            bool isInCell = false;
                            
                            for (int box_j = offsetxy.second; box_j < offsetxy.second + 3; box_j++)
                            {
                                if (box_i == i && m_rows[i][box_j] == 0)
                                    continue;

                                x = box_i;
                                y = box_j;

                                if (m_rows[box_i][box_j] == pos)
                                    isInCell = true;

                                if (m_cols[box_j][box_i] == pos)
                                    isInCell = true;


                            }
                            isIn3x3 = isInCell;
                        }
                        possib = isIn3x3;
                        std::cout << "isIn3x3?" << pos << ": " << isIn3x3 << std::endl;
                    }
                    if (possib)
                    {
                        std::cout << "found only candidate: " << pos << "  near  " << i << " " << cell << std::endl;
                        m_rows[i][cell] = pos;
                        m_cols[cell][i] = pos;
                        //trigger send msg
                    }
                    else
                    {

                    }
            }
            std::cout << "end pos" << std::endl;
        }
    }
}