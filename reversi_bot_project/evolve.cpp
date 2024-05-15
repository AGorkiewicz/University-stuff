#pragma once
#include "agent.cpp"
#include "match.cpp"
#include "judge.cpp"

template <class AgentType>
void relax(AgentType* agent){
	static int best = 0;
	static NemesisJudge < RandomAgent, 1000 > judge;
	int res = judge.fitness({agent})[0];
	if (res > best){
		agent->save(100000);
		best = res;
	}
	printf("best: %d\n", best);
}

template <class AgentType>
vector <AgentType*> select(const vector <AgentType*>& vec, const vector <int>& fitness, int fit)
{
	int n = ss(vec);
	
	vector <int> ord, pos(n);

	rep(i, 0, n - 1) ord.pb(i);

	sort(all(ord), [&] (int a, int b) { return fitness[a] > fitness[b]; });

	rep(i, 0, n - 1) pos[ord[i]] = i;
	
	vector <AgentType*> res;

	rep(i, 0, n - 1) if (pos[i] < fit) res.pb(vec[i]);

	assert(ss(res) == fit);

//	relax(vec[ord[0]]);

	int score = fitness[ord[0]];
	
	printf("score: %d\n", score);

	return res;
}

template <class AgentType>
void repopulate(vector <AgentType*>& vec, int n)
{
	srand(time(NULL));
	int fit = ss(vec);
	while (ss(vec) < n){
		int a = rand() % fit;
		int b = rand() % fit;
		if (a == b) continue;
		AgentType* agent = new AgentType;
		agent->cross(vec[a], vec[b]);
		//agent->mutate();
		vec.pb(agent);
	}
}

template <class AgentType, class JudgeType>
vector <AgentType*> evolve(int n, int fit, int eliminations, vector <AgentType*> vec = {})
{
	while (ss(vec) < n){
		AgentType* agent = new AgentType;
		agent->randomize();
		vec.pb(agent);
	}

	assert(ss(vec) == n);
	
	rep(iter, 1, eliminations){
		dbg(iter);
		auto fitness = JudgeType().fitness(vector<Agent*>(all(vec)));
		vec = select(vec, fitness, fit);
		repopulate(vec, n);
	}
	
	return vec;
}

template <class AgentType, class JudgeType>
pair < vector <AgentType*>, vector <AgentType*> > double_evolve(int n, int fit, int eliminations, vector <AgentType*> vec0 = {}, vector <AgentType*> vec1 = {})
{
	while (ss(vec0) < n){
		AgentType* agent = new AgentType;
		agent->randomize();
		vec0.pb(agent);
	}
	
	while (ss(vec1) < n){
		AgentType* agent = new AgentType;
		agent->randomize();
		vec1.pb(agent);
	}
	
	assert(ss(vec0) == n);
	assert(ss(vec1) == n);

	rep(iter, 1, eliminations){
		dbg(iter);
		auto [fitness0, fitness1] = JudgeType().fitness(vector<Agent*>(all(vec0)), vector<Agent*>(all(vec1)));
		vec0 = select(vec0, fitness0, fit);
		vec1 = select(vec1, fitness1, fit);
		repopulate(vec0, n);
		repopulate(vec1, n);
	}
	
	return {vec0, vec1};
}
