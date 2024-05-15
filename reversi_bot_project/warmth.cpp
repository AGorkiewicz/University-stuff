#pragma once

struct Warmth
{
	int tab[32][32];

	static constexpr int MAX = 999999;
	static constexpr int MUT = 9999;

	void save(int id){
		ofstream out("genes/warmth" + to_string(id));
		rep(i, 0, 31) rep(j, 0, 31){
			out << tab[i][j] << " ";
		}
		out << "\n";
		out.close();
	}

	void load(int id){
		ifstream in("genes/warmth" + to_string(id));
		rep(i, 0, 31) rep(j, 0, 31){
			in >> tab[i][j];
		}
		in.close();
	}

	void randomize(){
		rep(i, 0, 31) rep(j, 0, 31){
			tab[i][j] = rand() % (MAX + MAX + 1) - MAX;
		}
	}

	int* operator[](cint& i) { return tab[i]; }

	void cross(Warmth& A, Warmth& B){
		rep(i, 0, 31) rep(j, 0, 31){
			tab[i][j] = (rand() % 2 == 0 ? A : B)[i][j];
			//tab[i][j] = (A[i][j] + B[i][j]) / 2;
		}
	}

	void mutate(){
		srand(time(NULL));
		rep(i, 0, 31) rep(j, 0, 31){
			if (rand() % 1000 < 10){
				tab[i][j] = rand() % (MAX + MAX + 1) - MAX;
			}
		}
	}
};



