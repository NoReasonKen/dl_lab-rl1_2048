#include "board.hpp"
#include <algorithm>
#include <iomanip>
#include <iterator>
#include <random>
#include <cassert>

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
	os << "|";
	for (auto j(0); j < 4; j++)
	{
	    os << std::setw(4);
	    if (b.board_[i][j] == 0)
		os << 0;
	    else
		os << (1 << b.board_[i][j]);
	}
	os << "|\n";
    }
    os << "+----------------+\n";
    
    return os;
}

Board Board::mirror()
{
    Board b(*this);

    for (auto& i: b.board_)
	std::reverse(i.begin(), i.end());
    
    return b;
}

Board Board::transpose()
{
    Board b;

    for (size_t i(0); i < board_.size(); i++)
	for (size_t j(0); j < board_[i].size(); j++)
	    b.board_[j][i] = board_[i][j];

    return b;
}

Board Board::left(unsigned& s)
{
    Board b(*this);

    for (size_t i(0); i < b.board_.size(); i++)
	s += combine(b.board_[i]);

    return b;
}

Board Board::right(unsigned& s)
{
    return this->mirror().left(s).mirror();
}

Board Board::up(unsigned& s)
{
    return this->transpose().left(s).transpose();
}

Board Board::down(unsigned& s)
{
    return this->transpose().right(s).transpose();
}

bool Board::popup()
{
    unsigned count(0);

    for (auto& i: board_)
	for (auto j: i)
	    if (j == 0)
		count++;

    if (count == 0)
	return false;

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
			j = 2;
		    else
			j = 1;
		    return true;
		}
		count--;
	    }

    static_assert(true, "popup func reach an unreachable place");
    return false;
}

Row Board::get_tuple(unsigned idx)
{
    if (idx < 4)
	return board_[idx];
    else
    {
	Row r(4, 0);
	
	for (size_t i(0); i < 4; i++)
	    r[i] = board_[i][idx - 4];
	
	return r;
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
	if (*i == *(i + 1) && *i != 0)
	{
	    score += (1 << *i) * 2;
	    *i += 1;
	    for (auto j(i + 1); j != ie; j++)
		*j = *(j + 1);
	    *(r.end() - 1) = 0;
	}
    }

    return score;
}
