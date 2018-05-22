#include "board.hpp"
#include <algorithm>
#include <iostream>

void board_init(Board&);
bool is_gameover(const Board&);
int evaluate_moves(const Board&);

enum Move {left, right, up, down};

int main()
{
    unsigned score(0);
    Board state;

    board_init(state);

    while (is_gameover(state))
    {
	switch(evaluate_moves(state))
	{

	}
    }

}

void board_init(Board& b)
{
    b.popup();
    b.popup();
}

bool is_gameover(const Board& b)
{
    Board tmp(b);
    return !(tmp.popup());
}

int evaluate_moves(const Board& b)
{
    Board tmp(b);
    std::vector<unsigned> score(4, 0);
    
    for (auto i(0); i < 4; i++)
    {
	switch(i)
	{
	    case left:
		tmp.left(score[left]);
		break;
	    case right:
		tmp.right(score[right]);
		break;
	    case up:
		tmp.up(score[up]);
		break;
	    case down:
		tmp.down(score[down]);
		break;
	}
    }
    return std::distance(score.begin()
			, std::max_element(score.begin(), score.end()));
}
