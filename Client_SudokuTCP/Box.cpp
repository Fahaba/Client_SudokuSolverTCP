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
            SetValueInGrid(m_name, row, col, std::stoi(it->c_str()));
            //m_cols[col][row] = std::stoi(it->c_str());
            //m_rows[row][col] = std::stoi(it->c_str());
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
    std::cout << std::endl;
}

//struct newVal {
//    int x;
//    int y;
//    int val;
//};

std::vector<newVal> Box::CalculatePossibleValues()
{
    // get offset in lines and columns for box
    std::pair<int, int> offsetxy = CalculateOffsetByName(m_name);
    std::vector<newVal> newValues;
    // store possible values in arr
    uint64_t possibleForCell[9];
    uint64_t one = 0x0000000001;
    // for each line in box
    for (int boxRow = offsetxy.first; boxRow < offsetxy.first + 3; boxRow++)
    {
        // for each cell in line of box
        for (int boxCell = offsetxy.second; boxCell < offsetxy.second + 3; boxCell++)
        {
            // if cell is filled dont proceed
            if (m_rows[boxRow][boxCell])
            {
                possibleForCell[((boxRow-offsetxy.first)*3) + (boxCell%3)] = 0;
                continue;
            }
            // bitmap to store possible values
            uint64_t possibleValues = 0b1111111111;
            

            // search vertical direction and elim possible values
            for (int vert = 0; vert < m_rows[boxRow].size(); vert++)
            {
                int val = m_rows[boxRow][vert];

                if (val && (possibleValues & (one << val)))
                    possibleValues &= ~(one << val);
            }
            // search horizontal
            for (int hor = 0; hor < m_cols[boxCell].size(); hor++)
            {
                int val = m_cols[boxCell][hor];

                if (val && (possibleValues & (one << val)))
                    possibleValues &= ~(one << val);
            }
            // search in own 3x3 box
            for (int box_i = offsetxy.first; box_i < offsetxy.first + 3; box_i++)
            {
                for (int box_j = offsetxy.second; box_j < offsetxy.second + 3; box_j++)
                {
                    int val = m_rows[box_i][box_j];

                    if (val && (possibleValues & (one << val)))
                        possibleValues &= ~(one << val);
                }
            }
            // all impossible values for this cell eliminated
            // save in arr
            possibleForCell[((boxRow-offsetxy.first) * 3) + (boxCell%3)] = possibleValues;
        }
    }

    // locate 
    for (int i = 0; i < 9; i++)
    {
        int count = 0;
        /*if (!possibleForCell[i])
            continue;*/

        /*if (!(possibleForCell[i] & (one << (i + 1))))
            continue;*/
        int posDest = 0;
        for (int pos = 0; pos < 9; pos++)
        {
            if (possibleForCell[pos] & (one << (i + 1)))
            {
                posDest = pos;
                count++;
            }
        }

        if (count != 1 || !posDest)
            continue;

        // set it
        std::cout << "setting new value: " << i + 1 << std::endl;
        SetValueInGrid(m_name, (posDest / (int)3), (posDest % 3), i + 1);
        // send it to direct neighbors
        newVal n;
        n.val = i + 1;
        n.x = posDest / (int)3;
        n.y = posDest % 3;
        newValues.push_back(n);
    }
    SendToNeighbors(newValues);
    return newValues;
}

void Box::AddConnection(std::string boxName, SOCKET s, sockaddr_in out, sockaddr_in local)
{
    ConnectionTCP con;
    con.socket = s;
    con.out = out;
    con.local = local;

    m_storedConnections.push_back(std::make_pair(boxName, con));
}

void Box::SendToNeighbors(std::vector<newVal> newValues)
{
    std::stringstream ss;
    for (auto val : newValues)
    {
        ss = std::stringstream();
        ss << m_name;
        ss << "," << val.x << "," << val.y << "," << val.val << std::endl << "\0";
        for (auto conn : m_storedConnections)
        {
            sendto(conn.second.socket, ss.str().c_str(), strlen(ss.str().c_str()), 0, (sockaddr*)&conn.second.out, sizeof(conn.second.out));
        }
    }
}
