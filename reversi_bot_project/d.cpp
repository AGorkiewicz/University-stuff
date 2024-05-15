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
	
	using GWA = GeneticWarmthAgent;
	
	vector <GWA*> vec0, vec1;

	const int n = 400;
	
	rep(i, 100000, 100000 + n - 1){
		GWA* agent = new GWA;
		agent->load(i);
		vec0.pb(agent);
	}
	
	rep(i, 200000, 200000 + n - 1){
		GWA* agent = new GWA;
		agent->load(i);
		vec1.pb(agent);
	}

	int score[2] = {0, 0};
	
	GWA* C = new GWA;
	
	RandomAgent* R = new RandomAgent;

	C->load(985);

	rep(i, 0, n - 1){
		//score[match(R, C)] += 1;
		score[match(vec0[i], C)] += 1;
		//score[match(vec0[i], R)] += 1;
		dbg(score[0], score[1]);
	}

	return 0;
}
