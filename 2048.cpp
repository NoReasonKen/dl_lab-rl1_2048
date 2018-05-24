#include "board.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <fstream>

#define NUM_FEATURE 8
#define LR 0.1

enum Move {left, right, up, down};
typedef std::vector<double> Table;

struct Record
{
    unsigned score;
    Board before, after;
};

unsigned play_one_game(std::vector<Table>&, unsigned& max);
void board_init(Board&);
bool is_gameover(const Board&);
double evaluate_move(const Board&, const std::vector<Table>&, int, bool=false);
double sum_of_possible_next_state(Board& b, const std::vector<Table>&);
unsigned decide_action(std::vector<double>&, double);
void update_table(std::vector<Record>&, std::vector<Table>&);
uint16_t take_out_index(const Row&);

int main()
{
    std::vector<Table> weight_table(NUM_FEATURE
	    , Table(std::numeric_limits<uint16_t>::max(), 0.0));
    unsigned high_score(0), score(0), max(0);
    std::vector<unsigned> log(30);
    for (auto i(1); true; i++)
    {
	score = play_one_game(weight_table, max);
	log.erase(log.begin());
	log.emplace_back(score);
	if (score > high_score)
	{
	    high_score = score;
	    std::ofstream ofs("weight_table.txt");
	    for (auto& j: weight_table)
	    {
		for (size_t k(0); k < j.size(); k++)
		{
		    if ((k + 1) % 16 == 0)
			ofs << "\n";
		    else
			ofs << j[k] << " ";
		}
		ofs << "\n";
	    }
	    ofs.close();
	}
	std::cout << i << "\t:" << score << "\t" << "high:" << high_score << "\t";
	std::cout << "mean(30):" << std::accumulate(log.begin(), log.end(), 0) / log.size() << "\t";
	std::cout << "max:" << (1 << max) << "\n";
    }
}

unsigned play_one_game(std::vector<Table>& w_t, unsigned& max)
{
    std::vector<Record> records;
    Board state;
    unsigned total_score(0);
    Move action;

    board_init(state);

    //std::cout << state << "\n" << total_score << "\n";

    for (auto iter(1); !is_gameover(state); iter++)
    {
	std::vector<double> estimate_val(4);

	for (size_t i(left); i < down; i++)
	    estimate_val[i] = evaluate_move(state, w_t, i);

	//double max_tmp = *(std::max_element(estimate_val.begin()
	//				    , estimate_val.end()));

	action = (Move)std::distance(estimate_val.begin(), 
		std::max_element(estimate_val.begin(), estimate_val.end()));

	//action = (Move)decide_action(estimate_val, max_tmp);

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
	
	//std::cout << state << iter << ": " << action << "\tscore: " << total_score << "\n";
    }
    max = state.max_token();
    update_table(records, w_t);
    return total_score;
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
   
    if (is_after_state)
    {

    }
    else
    {
	switch(move)
	{
	    case left:
		if (tmp == tmp.left(score))
		    return 0;
		else
		    tmp = tmp.left(score);
		break;
	    case right:
		if (tmp == tmp.right(score))
		    return 0;
		else
		tmp = tmp.right(score);
		break;
	    case up:
		if (tmp == tmp.up(score))
		    return 0;
		else
		tmp = tmp.up(score);
		break;
	    case down:
		if (tmp == tmp.down(score))
		    return 0;
		else
		tmp = tmp.down(score);
		break;
	}
    }

    return (float)score + sum_of_possible_next_state(tmp, e);
}

double sum_of_possible_next_state(Board& b
				, const std::vector<Table>& e)
{
    double sum(0.0);

    for (auto i(0); i < NUM_FEATURE; i++)
    {
	std::vector<size_t> empty_idx;
	uint16_t idx(0);

	Row r(b.get_tuple(i));
	for (size_t j(0); j < r.size(); j++)
	{
	    if (r[j] == 0)
		empty_idx.emplace_back(r.size() - 1 - j);
	    else
		idx += (r[j] & 0xf) << (r.size() - 1 - j) * 4;
	}
	for (size_t j(0); j < empty_idx.size(); j++)
	{
	    sum += e[i][idx + (1 << empty_idx[j]) * 4] * 0.9 / empty_idx.size()
		+ e[i][idx + (2 << empty_idx[j]) * 4] * 0.1 / empty_idx.size();
	}
    }

    return sum;
}

unsigned decide_action(std::vector<double>& e, double largest_value)
{
    unsigned largest_count(0);

    for (auto i(e.begin()); i != e.end(); i++)
    {
	if (*i >= largest_value)
	    largest_count++;
	else
	    *i = -1;
    }

    std::random_device rd;
    std::uniform_int_distribution<int> uid(0, largest_count - 1);
    for (size_t i(0), counter(uid(rd)); i < e.size(); i++, counter--)
    {
	if (e[i] < 0)
	    counter++;
	if (counter <= 0)
	    return i;
    }
    return 0;
}

void update_table(std::vector<Record>& records
		, std::vector<Table>& w_t)
{
    uint16_t idx_t, idx_t1;

    for (size_t i(0); i < NUM_FEATURE; i++)
    {
	Row r(records.back().after.get_tuple(i));
	idx_t1 = take_out_index(r);
	w_t[i][idx_t1] = 0;
    }

    for (int i(records.size() - 1); i >= 0; i--) 
    {
	double sum_t(0), sum_t1(0), modify;

	for (size_t j(0); j < NUM_FEATURE; j++)
	{
	    Row r(records[i].before.get_tuple(j)),
		r1(records[i].after.get_tuple(j));

	    sum_t += w_t[j][take_out_index(r)];
	    sum_t1 += w_t[j][take_out_index(r1)];
	}

	modify = ((float)LR / NUM_FEATURE) * ((float)records[i].score + sum_t1 - sum_t);

	for (size_t j(0); j < NUM_FEATURE; j++)
	{
	    Row r(records[i].before.get_tuple(j));
	    
	    idx_t = take_out_index(r);

	    w_t[j][idx_t] = w_t[j][idx_t] + modify; 
	    if (w_t[j][idx_t] < 0)
		w_t[j][idx_t] = 0;
	    
	    /*
	    w_t[j][idx_t] = w_t[j][idx_t] + ((float)LR / NUM_FEATURE) 
			* (w_t[j][idx_t1] - w_t[j][idx_t] + (float)records[i].score);
	    */
	}
    }
}

uint16_t take_out_index(const Row& r)
{
    uint16_t idx(0);

    for (size_t j(0); j < r.size(); j++)
	idx += (r[j] & 0xf) << (r.size() - 1 - j) * 4;

    return idx;
}
