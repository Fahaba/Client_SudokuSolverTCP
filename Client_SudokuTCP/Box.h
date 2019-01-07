#pragma once
#include <array>
#include <vector>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
// move this to config or s.th.
#define dimx 3
#define dimy 3

/*
BOXGRID
BOX_A1 BOX_D1 BOX_G1
BOX_A4 BOX_D4 BOX_G4
BOX_A7 BOX_D7 BOX_G7
*/


struct ConnectionTCP
{
    SOCKET socket;
    sockaddr_in out;
    sockaddr_in local;
};

struct GridMember
{
    std::string ip;
    int port;
};

struct newVal
{
    int x;
    int y;
    int val;
};

class Box
{
    
public:

    Box(std::string cfgPath, std::string name);
    typedef std::array<int, dimy> y;
    typedef std::array< y, dimx > BoxField; // 2d array

    
    typedef std::array<int, 9> Cells;
    typedef std::array<Cells, 9> Cols;
    typedef std::array<Cells, 9> Rows;

    BoxField m_boxField;
    Rows m_rows;
    Cols m_cols;
    uint64_t possibleForBox = 0b1111111111;
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
    std::vector<newVal> CalculatePossibleValues();
    std::pair<int, int> CalculateOffsetByName(std::string name);
    typedef std::vector<std::pair<std::string, ConnectionTCP> > StoredConnections;
    StoredConnections m_storedConnections;
    void AddConnection(std::string boxName, SOCKET s, sockaddr_in out, sockaddr_in local);
    ~Box();
    void SetSocket(SOCKET s) { m_sock = s; }
private:
    void Initialize(std::string cfgPath);
    bool LoadConfigFromFile(std::string path);
    void SendToNeighbors(std::vector<newVal> newValues);
    void InitializeGrid();
    void CheckOnlyPossibleInColumn(int boxRow, int boxCell, std::vector<newVal> &newValues);
    void CheckOnlyPossibleInRow(int boxRow, int boxCell, std::vector<newVal> &newValues);
    void CheckOnlyPossibleInBox(int boxRow, int boxCell, std::vector<newVal> &newValues);

    SOCKET m_sock;
    
protected:
    
};

