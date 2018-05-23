#include "board.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <random>

#define NUM_TUPLE 8

enum Move {left, right, up, down};
typedef std::vector<double> Table;

struct Record
{
    unsigned score;
    Board before, after;
};

void board_init(Board&);
bool is_gameover(const Board&);
double evaluate_move(const Board&, const std::vector<Table>&
		    , int, bool=false);
double sum_of_possible_next_state(Board& b, const std::vector<Table>&);
unsigned decide_action(std::vector<double>&, double);

int main()
{
    std::vector<Table> estimated(8
	    , Table(std::numeric_limits<uint16_t>::max(), 0.0));
    std::vector<Record> records;
    Board state;
    unsigned total_score(0);
    Move action;

    board_init(state);

    std::cout << state << "\n" << total_score << "\n";

    for (auto iter(1); !is_gameover(state); iter++)
    {
	std::vector<double> estimate_tmp(4);

	for (size_t i(left); i < down; i++)
	    estimate_tmp[i] = evaluate_move(state, estimated, i);

	double max_tmp = *(std::max_element(estimate_tmp.begin(), estimate_tmp.end()));
	double largest_value(-1);
	if (max_tmp > largest_value)
	    largest_value = max_tmp;

	action = (Move)decide_action(estimate_tmp, largest_value);

	records.emplace_back(Record());
	records.back().before = state;

	unsigned score(0);
	switch (action)
	{
	    case left:
		state = state.left(score);
		break;
	    case right:
		state = state.right(score);
		break;
	    case up:
		state = state.up(score);
		break;
	    case down:
		state = state.down(score);
		break;
	}
	state.popup();

	total_score += score;
	records.back().after = state;
	records.back().score = score;
	
	std::cout << state << iter << ": " << action << "\tscore: " << total_score << "\n";
    }

}

void board_init(Board& b)
{
    b.popup();
    b.popup();
}

bool is_gameover(const Board& b)
{
    Board b_tmp(b);
    unsigned s(0);
    return !(b_tmp.left(s).popup() || b_tmp.right(s).popup() 
	    || b_tmp.up(s).popup() || b_tmp.down(s).popup());
}

double evaluate_move(const Board& b, const std::vector<Table>& e
		    , int move, bool is_after_state)
{
    Board tmp(b);
    unsigned score(0);
    
    switch(move)
    {
	case left:
	    tmp = tmp.left(score);
	    break;
	case right:
	    tmp = tmp.right(score);
	    break;
	case up:
	    tmp = tmp.up(score);
	    break;
	case down:
	    tmp = tmp.down(score);
	    break;
    }
    
    return score + sum_of_possible_next_state(tmp, e);
}

double sum_of_possible_next_state(Board& b
				, const std::vector<Table>& e)
{
    Row r;
    double sum(0.0);

    for (auto i(0); i < NUM_TUPLE; i++)
    {
	std::vector<size_t> empty_idx;
	unsigned idx(0);

	r = b.get_tuple(i);
	for (size_t j(0); j < r.size(); j++)
	{
	    if (r[j] == 0)
		empty_idx.emplace_back(r.size() - 1 - j);
	    else
		idx += (r[j] & 0xf) << (r.size() - 1 - j) * 4;
	}
	for (size_t j(0); j < empty_idx.size(); j++)
	{
	    sum += e[i][idx + (1 << empty_idx[j]) * 4] 
		    * 0.9 / empty_idx.size()
		+ e[i][idx + (2 << empty_idx[j]) * 4] 
		    * 0.1 / empty_idx.size();
	}
    }

    return sum;
}

unsigned decide_action(std::vector<double>& e, double largest_value)
{
    std::random_device rd;
    unsigned largest_count(0);

    for (auto i(e.begin()), ie(e.end()); i != ie; i++)
    {
	if (*i >= largest_value)
	    largest_count++;
	else
	    *i = -1;
    }

    std::uniform_int_distribution<int> uid(0, largest_count - 1);

    int counter(uid(rd));
    for (size_t i(0); i < e.size(); i++, counter--)
    {
	if (e[i] < 0)
	    counter++;
	if (counter <= 0)
	    return i;
    }
    return 0;
}
