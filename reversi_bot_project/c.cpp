#include "template.cpp"
#include "debug.cpp"
#include "game.cpp"
#include "warmth_agent.cpp"
#include "random_agent.cpp"
#include "warmth.cpp"
#include "genetic_warmth_agent.cpp"
#include "evolve.cpp"

const int population   = 400;
const int fit          = 200;
const int eliminations = 200;

int main(int argc, char ** argv)
{
	srand(0);

	using judge = DoubleTourneyJudge;
	using GWA = GeneticWarmthAgent;

	//vector <GWA*> ini;
	//rep(i, 1, 500){
	//	GWA* agent = new GWA;
	//	agent->load(i);
	//	ini.pb(agent);
	//}

	auto [vec0, vec1] = double_evolve<GeneticWarmthAgent, judge>(population, fit, eliminations);

	rep(i, 0, ss(vec0) - 1){
		vec0[i]->save(100000 + i + 1);
	}

	rep(i, 0, ss(vec1) - 1){
		vec1[i]->save(200000 + i + 1);
	}

	return 0;
}
