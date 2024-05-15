#include "template.cpp"
#include "debug.cpp"
#include "game.cpp"
#include "warmth_agent.cpp"
#include "random_agent.cpp"
#include "warmth.cpp"
#include "genetic_warmth_agent.cpp"
#include "evolve.cpp"

// 25 mln gier
const int turns        = 1000;
const int population   = 500;
const int fit          = 250;
const int eliminations = 50;
const int win_wei = 1000000;
const int dif_wei = 1;

int main(int argc, char ** argv)
{
	srand(0);

	using judge = NemesisJudge<RandomAgent, turns, win_wei, dif_wei>;
	using GWA = GeneticWarmthAgent;

	vector <GWA*> ini;

	rep(i, 1, 500){
		GWA* agent = new GWA;
		agent->load(i);
		ini.pb(agent);
	}

	auto vec = evolve<GeneticWarmthAgent, judge>(population, fit, eliminations, ini);

	rep(i, 0, ss(vec) - 1){
		vec[i]->save(i + 1);
	}

	return 0;
}
