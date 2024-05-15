#pragma once
#include "game.cpp"
#define popcount(x) __builtin_popcountll(x)

struct Agent
{
	virtual int get_move(State state) { return -1; }

	virtual string name() { return "VirtualAgent"; }
};

