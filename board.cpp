#include "board.hpp"
#include <algorithm>
#include <iomanip>
#include <iterator>
#include <random>


unsigned combine(Row& r);

/********************************************************************/
/*constructor*/
Board::Board()
{
    board_.resize(4, Row(4, 0));
}
Board::Board(const Board& b)
    : board_(b.board_)
{
}
Board::Board(Board&& b)
    : board_(std::move(b.board_))
{
}
/********************************************************************/
/*operator*/
Board& Board::operator=(const Board& b)
{
    board_ = b.board_;
    return *this;
}
Board& Board::operator=(Board&& b)
{
    board_ = std::move(b.board_);
    return *this;
}
/********************************************************************/
/*methods*/
std::ostream& operator<<(std::ostream& os, const Board& b)
{
    os << "+----------------+\n";
    for (auto i(0); i < 4; i++)
    {
	os << "+";
	for (auto j(0); j < 4; j++)
	{
	    os << std::setw(4) << int(b.board_[i][j]);
	}
	os << "+\n";
    }
    os << "+----------------+\n";
    
    return os;
}

Board Board::mirror()
{
    Board b(*this);

    for (auto i(b.board_.begin()), ie(b.board_.end()); i != ie; i++)
	std::reverse(i, ie);
    
    return b;
}

Board Board::transpose()
{
    Board b;

    for (size_t i(0); i < board_.size(); i++)
    {
	for (size_t j(0); j < board_[i].size(); j++)
	{
	    b.board_[j][i] = board_[i][j];
	}
    }

    return b;
}

unsigned Board::left()
{
    Board b(*this);
    unsigned score(0);

    for (size_t i(0); i < b.board_.size(); i++)
	score += combine(b.board_[i]);

    return score;
}

unsigned Board::right()
{
    return this->mirror().left();
}

unsigned Board::up()
{
    return this->transpose().left();
}

unsigned Board::down()
{
    return this->transpose().mirror().left();
}

void Board::popup()
{
    unsigned count(0);

    for (auto& i: board_)
	for (auto j: i)
	    if (j == 0)
		count++;

    std::random_device rd;
    std::uniform_int_distribution<unsigned> uid1(0, count - 1), uid2(0, 9);

    count = uid1(rd);

    for (auto& i: board_)
	for (auto& j: i)
	    if (j == 0)
	    {
		if (count == 0)
		{
		    if (uid2(rd) == 9)
			j = 4;
		    else
			j = 2;
		    return;
		}
		count--;
	    }
}

/********************************************************************/
/*functions*/
unsigned combine(Row& r)
{
    unsigned score(0);

    std::sort(r.begin(), r.end()
	    , [](Row::value_type a, Row::value_type b)
	    {
		return !a < !b;
	    });
    for (auto i(r.begin()), ie(r.end() - 1); i != ie; i++)
    {
	if (*i == *(i + 1))
	{
	    score += *i * 2;
	    *i *= 2;
	    for (auto j(i + 1); j != ie; j++)
		*j = *(j + 1);
	    *(r.end()) = 0;
	}
    }

    return score;
}
