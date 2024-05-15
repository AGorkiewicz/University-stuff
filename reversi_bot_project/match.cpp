#include "template.cpp"
#include "debug.cpp"
#include "game.cpp"
#include "agent.cpp"
#pragma once

int visual_match(Agent* P0, Agent* P1, State state = State())
{
	state.draw();
	cin.get();
	for (;;){
		int move = (state.player == 0 ? P0 : P1)->get_move(state);
		if (move == -1) return state.winner();
		state.make_move(move);
		state.draw();
		//dbg(P0->name());
		//dbg(P1->name());
		cin.get();
	}
}

int match(Agent* P0, Agent* P1, State state = State())
{
	for (;;){
		int move = (state.player == 0 ? P0 : P1)->get_move(state);
		if (move == -1) return state.winner();
		state.make_move(move);
	}
}

pair <int,int> match_with_diff(Agent* P0, Agent* P1, State state = State())
{
	for (;;){
		int move = (state.player == 0 ? P0 : P1)->get_move(state);
		if (move == -1) return { state.winner(), state.count_points(0) - state.count_points(1) };
		state.make_move(move);
	}
}
