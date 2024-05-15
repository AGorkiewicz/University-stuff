#pragma once

namespace debug { template <class c> struct rge { c b, e; }; template <class c> rge<c> range(c i, c j) { return rge<c>{i, j}; } template <class c> char spk(...); template <class c> auto spk(c* a) -> decltype(cerr << *a, 0); struct stream { ~stream() { cerr << endl; } template <class c> typename enable_if <sizeof spk<c>(0) != 1, stream&>::type operator<<(c i) { cerr << boolalpha << i; return *this; } template <class c> typename enable_if <sizeof spk<c>(0) == 1, stream&>::type operator<<(c i) { return *this << range(begin(i), end(i)); } template <class a, class b> stream& operator<<(pair <a,b> p) { return *this << "(" << p.first << ", " << p.second << ")"; } template <class c> stream& operator<<(rge<c> d) { *this << "["; for (auto it=d.b; it!=d.e; it++) *this << ", " + 2 * (it == d.b) << *it; return *this << "]"; } stream& _dbg(const string& s, int i, int b) { return *this; } template <class c, class ... cs> stream& _dbg(const string& s, int i, int b, c arg, cs ... args) { if (i == (int)(s.size())) return (*this << ": " << arg); b += (s[i] == '(') + (s[i] == '[') + (s[i] == '{') - (s[i] == ')') - (s[i] == ']') - (s[i] == '}'); return (s[i] == ',' && b == 0) ? (*this << ": " << arg << "     ")._dbg(s, i + 1, b, args...) : (s[i] == ' ' ? *this : *this << s[i])._dbg(s, i + 1, b, arg, args...); } };}
#define dout debug::stream()
#define dbg(...) ((dout << ">> ")._dbg(#__VA_ARGS__, 0, 0, __VA_ARGS__))

#include "game.cpp"

void draw_mask(ull mask){
	rep(y, 0, 7){
		rep(x, 0, 7){
			if ((mask >> pos(y, x)) & 1){
				printf("#");
			}else{
				printf(".");
			}
		}
		printf("\n");
	}
	printf("\n");
}
