#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include <vector>
#include <ostream>
#include <utility>

typedef std::vector<uint8_t> Row;
typedef std::vector<Row> VecRow;

class Board
{
    /****************************************************************/
    /*property*/
    VecRow board_;
    /****************************************************************/
  public:
    /****************************************************************/
    /*constructor*/
    Board();
    Board(const Board&);
    Board(Board&&);
    /****************************************************************/
    /*operator*/
    Board& operator=(const Board&);
    Board& operator=(Board&&);
    /****************************************************************/
    /*methods*/
    friend std::ostream& operator<<(std::ostream&, const Board&);
    Board mirror();
    Board transpose();
    unsigned left();
    unsigned right();
    unsigned up();
    unsigned down();
    void popup();
};

#endif
