#include "Box.h"


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
	// this is no file anymore
		//std::string line = "";
    /*std::ifstream inFile;
    inFile.open(path);
    std::string line = "";
    if (!inFile)
    {
        return false;
        exit(1);
    }*/

    int row = 0;
    
        std::istringstream iss(path);
        std::istream_iterator<std::string> it(iss);
        std::istream_iterator<std::string> end;
        int col = 0;
        for (; it != end ; ++it)
        {

			int val = std::stoi(it->c_str());
            SetValueInGrid(m_name, row, col, val);
			
			if (val)
			{
				newVal toSend;
				toSend.x = row;
				toSend.y = col;
				toSend.val = val;
				SendToNeighbors(std::vector<newVal>{toSend}, false);
			}
			
            
			m_boxField[row][col] = val;
			//possibleForBox &= ~(1 << val);
			col++;
			if (col % 3 == 0)
				row++;

			col = col % 3;
        }

        //row++;

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
    std::cout << "/-----|-----|-----|-----|-----|-----|-----|-----|-----\\" << std::endl;
    
    for (auto j : m_rows)
    {
        std::cout << "|  ";
        for (auto i : j)
            std::cout << i << "  |  ";

        std::cout << std::endl;
        std::cout << "|-----|-----|-----|-----|-----|-----|-----|-----|-----|";
        std::cout << std::endl;
    }
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

	int x_intern = offsetxy.first + x;
	int y_intern = offsetxy.second + y;
	
	// if set already dont set and send it again
	if (m_rows[x_intern][y_intern] > 0)
		return;

	if (boxName == m_name)
		possibleForBox &= ~(1 << val);
	
	m_rows[x_intern]
        [y_intern] = val;
    m_cols[y_intern]
        [x_intern] = val;
	
	CalculatePossibleValues();
	print();
}


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
                possibleForCell[((boxRow - offsetxy.first) * 3) + (boxCell % 3)] = 0;
                continue;
            }

            CheckOnlyPossibleInBox(boxRow, boxCell, newValues);
            if (m_rows[boxRow][boxCell])
            {
                possibleForCell[((boxRow - offsetxy.first) * 3) + (boxCell % 3)] = 0;
                continue;
            }

            CheckOnlyPossibleInColumn(boxRow, boxCell, newValues);
            // recheck
            if (m_rows[boxRow][boxCell])
            {
                possibleForCell[((boxRow - offsetxy.first) * 3) + (boxCell % 3)] = 0;
                continue;
            }
            CheckOnlyPossibleInRow(boxRow, boxCell, newValues);
            // recheck
            if (m_rows[boxRow][boxCell])
            {
                possibleForCell[((boxRow - offsetxy.first) * 3) + (boxCell % 3)] = 0;
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
            possibleForCell[((boxRow - offsetxy.first) * 3) + (boxCell % 3)] = possibleValues;
        }
    }
	// send result (and close box)
	std::cout << possibleForBox << " possible";
	SendToNeighbors(newValues, possibleForBox == 0 ? true : false);
    return newValues;
}

void Box::CheckOnlyPossibleInColumn(int boxRow, int boxCell, std::vector<newVal> &newValues)
{
    // check only possible in col
    for (int testVal = 1; testVal <= 9; testVal++)
    {
        if (!(possibleForBox & (1 << testVal)))
            continue;

        bool possible = true;
        int count = 0;
        for (int vert = 0; vert < 9; vert++)
        {
            if (!possible)
                break;

            if (count > 1)
                break;

            if (m_rows[vert][boxCell] > 0)
            {
                // already set in column
                if (m_rows[vert][boxCell] == testVal)
                {
                    count = 0;
                    possible = false;
                    break;
                }
                continue;
            }

            int countBox = 0;
            // search in 3x3
            for (int box_i = vert - vert % 3; box_i < vert - vert % 3 + 3; box_i++)
            {
                for (int box_j = boxCell - boxCell % 3; box_j < boxCell - boxCell % 3 + 3; box_j++)
                {
                    if (m_rows[box_i][box_j] == testVal)
                        countBox++;
                }
            }

            // if none found in box -> search horizontally
            if (!countBox)
            {
                int countHor = 0;
                for (int hor = 0; hor < 9; hor++)
                {
                    if (vert == boxRow && m_rows[vert][hor] == testVal)
                    {
                        countHor++;
                        count = 0;
                        possible = false;
                    }
                    if (m_rows[vert][hor] == testVal)
                        countHor++;
                }
                // if not found horizontally -> possible in this cell
                if (!countHor)
                    count++;
            }
        }
        if (count == 1)
        {
            newVal n;
            n.val = testVal;
            n.x = boxRow % 3;
            n.y = boxCell % 3;
            SetValueInGrid(m_name, n.x, n.y, n.val);
            newValues.push_back(n);
            //possibleForBox &= ~(1 << n.val);
            break;
        }
    }
}

void Box::CheckOnlyPossibleInRow(int boxRow, int boxCell, std::vector<newVal> &newValues)
{
	// check only possible in col
    for (int testVal = 1; testVal <= 9; testVal++)
    {
        if (!(possibleForBox & (1 << testVal)))
            continue;

        bool possible = true;
        int count = 0;
        for (int hor = 0; hor < 9; hor++)
        {
            if (!possible)
                break;

            if (count > 1)
                break;

            if (m_cols[hor][boxRow] > 0)
            {
                // already set in row
                if (m_cols[hor][boxRow] == testVal)
                {
                    count = 0;
                    possible = false;
                    break;
                }
                continue;
            }

            int countBox = 0;
            // search in 3x3
            for (int box_i = hor - hor % 3; box_i < hor - hor % 3 + 3; box_i++)
            {
                for (int box_j = boxRow - boxRow % 3; box_j < boxRow - boxRow % 3 + 3; box_j++)
                {
                    if (m_cols[box_i][box_j] == testVal)
                        countBox++;
                }
            }

            // if none found in box -> search vertically
            if (!countBox)
            {
                int countVert = 0;
                for (int vert = 0; vert < 9; vert++)
                {
                    if (hor == boxCell && m_cols[boxCell][vert] == testVal)
                    {
                        countVert++;
                        count = 0;
                        possible = false;
                    }
                    if (m_cols[hor][vert] == testVal)
                        countVert++;
                }
                // if not found vertically -> possible in this cell
                if (!countVert)
                    count++;
            }
        }
        if (count == 1)
        {
            newVal n;
            n.val = testVal;
            n.x = boxRow % 3;
            n.y = boxCell % 3;
            SetValueInGrid(m_name, n.x, n.y, n.val);
            newValues.push_back(n);
            //possibleForBox &= ~(1 << n.val);
            break;
        }
    }
}

void Box::CheckOnlyPossibleInBox(int boxRow, int boxCell, std::vector<newVal> &newValues)
{
    int count = 0;
    int value = 0;
    for (int i = 1; i <= 9; i++)
    {
        if (possibleForBox & (1 << i))
        {
            count++;
            value = i;
        }
    }
    if (count == 1)
    {
        newVal n;
        n.val = value;
        n.x = boxRow % 3;
        n.y = boxCell % 3;
        SetValueInGrid(m_name, n.x, n.y, n.val);
        newValues.push_back(n);
        //possibleForBox &= ~(1 << n.val);
    }
}

void Box::AddConnection(std::string boxName, SOCKET s, sockaddr_in out, sockaddr_in local)
{
    ConnectionTCP con;
    con.socket = s;
    con.out = out;
    con.local = local;

    m_storedConnections.push_back(std::make_pair(boxName, con));
}

void Box::SendToNeighbors(std::vector<newVal> newValues, bool finished)
{
    std::stringstream ss;

	for (auto val : newValues)
	{
		ss = std::stringstream();
		ss << "/HandleAddFeed.php?message=";
		ss << m_name;
		ss << "," << val.x << "," << val.y << "," << val.val;
		std::string response;
		HttpReq("GET", "127.0.0.1", 80, ss.str().c_str(), NULL, response);
		// do s.th with response?
	}

	if (finished)
	{
		ss = std::stringstream();
		ss << "/HandleAddFeed.php?message=RESULT:";
		ss << m_name;
		ss << ",";

		std::pair<int, int> offsetxy = CalculateOffsetByName(m_name);

		for (int x = offsetxy.first; x < offsetxy.first + 3; x++)
		{
			for (int y = offsetxy.second; y < offsetxy.second + 3; y++)
				ss << m_rows[x][y];
		}

		std::string response;
		HttpReq("GET", "127.0.0.1", 80, ss.str().c_str(), NULL, response);

		// close box
		//WSACleanup();
	}
}
