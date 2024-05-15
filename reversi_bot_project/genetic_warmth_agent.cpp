#pragma once
#include "agent.cpp"
#include "warmth.cpp"

struct GeneticWarmthAgent : Agent
{
	Warmth warmth;

	int get_move(State state)
	{
		int turn = state.turn() / 2;
        
		ull moves = state.valid_move_mask();
		cint pop = popcount(moves);

		int res = -1;

		rep(j, 0, pop - 1){
			cint i = __lg(expand(1ull << j, moves));
			cint w = warmth[turn][min(i, 63 - i)] + (rand() % 2) * 2 - 1;
			if (res == -1 || w > warmth[turn][res]){
				res = i;
			}
		}
		return res;
	}

	string name() { return "GeneticWarmthAgent"; }

	void randomize() { warmth.randomize(); }

	void cross(GeneticWarmthAgent* A, GeneticWarmthAgent* B){
		warmth.cross(A->warmth, B->warmth);
	}

	void mutate() { warmth.mutate(); }

	void save(int id) { warmth.save(id); }

	void load(int id) { warmth.load(id); }
};

