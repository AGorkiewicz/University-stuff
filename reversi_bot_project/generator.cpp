#include "template.cpp"
#include "debug.cpp"
#include "game.cpp"
#include "agent.cpp"

void print_counter()
{
	printf("{");
	rep(i, 0, 63){
		printf("{");
		rep(j, 0, 63){
			printf("{");
			rep(k, 0, 1){
				printf("%d", counter[i][j][k]);
				if (k < 1) printf(",");
			}
			printf("}");
			if (j < 63) printf(",");
		}
		printf("}");
		if (i < 63) printf(",");
	}
	printf("};");
}

void update_counter()
{
	State state;
	function<int(int)> simulate = [&] (int turn) -> int {
		int move = state.random_move();
		if (move == -1) return state.winner();
		state.make_move(move);
		int res = simulate(turn + 1);
		counter[turn][move][res] += 1;
		return res;
	};
	simulate(0); 
}

int main(int argc, char ** argv)
{
	srand(argc == 1 ? 0 : atoi(argv[1]));
	
	return 0;
}
