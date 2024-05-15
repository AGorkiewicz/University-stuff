#pragma once
#include "agent.cpp"

struct Judge
{
	virtual vector <int> fitness(vector<Agent*> vec) { return {}; }
};

struct TourneyJudge
{
	vector <int> fitness(vector<Agent*> vec)
	{
		int n = ss(vec);
		
		vector <int> cnt(n, 0);

		rep(i, 0, n - 1) rep(j, 0, n - 1){
			if (i == j) continue;
			int res = match(vec[i], vec[j]);
			cnt[res == 0 ? i : j] += 1;
		}
		
		return cnt;
	}
};

struct DoubleTourneyJudge
{
	pair < vector <int> , vector <int> > fitness(vector<Agent*> vec0, vector<Agent*> vec1)
	{
		assert(ss(vec0) == ss(vec1));

		int n = ss(vec0);
		
		vector <int> cnt0(n, 0);
		vector <int> cnt1(n, 0);
		
		rep(i, 0, n - 1) rep(j, 0, n - 1){
			int res = match(vec0[i], vec1[j]);
			if (res == 0) cnt0[i] += 1;
			if (res == 1) cnt1[j] += 1;
		}
		
		return {cnt0, cnt1};
	}
};

template <class NemesisAgent, const int turns = 1, const int win_wei = 1, const int dif_wei = 0>
struct NemesisJudge
{
	vector <int> fitness(const vector<Agent*>& vec)
	{
		int n = ss(vec);
		vector <int> cnt(n, 0);
		auto* nemesis = new NemesisAgent;
		rep(i, 0, n - 1){
			srand(0);
			rep(j, 1, turns){
				auto res = match_with_diff(nemesis, vec[i]);
				if (res.f == 1) cnt[i] += win_wei;
				cnt[i] -= res.s * dif_wei;
			}
		}
		return cnt;
	}
};



