#pragma once
#include "agent.cpp"

struct RandomAgent : Agent
{
	int get_move(State state)
	{
        ull moves = state.valid_move_mask();
        cint pop = popcount(moves);
        if (pop == 0) return -1;
        cint i = __lg(expand(1ull << (rand() % pop), moves));
        return i;
    }

	string name() { return "RandomAgent"; }
};

