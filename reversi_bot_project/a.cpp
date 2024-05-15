#include "template.cpp"
#include "debug.cpp"
#include "game.cpp"
#include "warmth_agent.cpp"
#include "random_agent.cpp"
#include "genetic_warmth_agent.cpp"
#include "match.cpp"
#include "judge.cpp"

int main(int argc, char ** argv)
{
	srand(argc == 1 ? 0 : atoi(argv[1]));
	
	RandomAgent A;
	WarmthAgent B;
	GeneticWarmthAgent C;

	int score[2] = {0, 0};
	
//	C.load(985);
//
//	NemesisJudge < RandomAgent, 1000 > judge;
//
//	NemesisJudge < RandomAgent, 1000, 1000000, 1 > jud2;
//
//	auto vec = judge.fitness({&C});
//
//	dbg(vec);
	
	rep(i, 1, 100000){
		score[match(&A, &A)] += 1;
	}
	
	dbg(score[0], score[1]);

	return 0;
}
