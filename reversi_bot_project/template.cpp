#pragma once
#pragma GCC optimize("O3")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
#include <immintrin.h>
using namespace std;
#define f first
#define s second
#define sc scanf
#define pr printf
#define mp make_pair
#define pb push_back
#define all(x) x.begin(),x.end()
#define ss(x) ((int)((x).size()))
#define rep(i,a,b) for(int i=(a);i<=(b);i++)
#define per(i,a,b) for(int i=(b);i>=(a);i--)
#define upgrade cin.tie(0)->sync_with_stdio(0);
#define lowb(v,x) (lower_bound(all(v),(x))-v.begin())
#define uppb(v,x) (upper_bound(all(v),(x))-v.begin())
#define erase_duplicates(x) {sort(all(x));x.resize(unique(all(x))-x.begin());}
template <class p, class q> pair<p,q> operator-(pair<p,q> a, pair<p,q> b) { return mp(a.f - b.f, a.s - b.s); }
template <class p, class q> pair<p,q> operator+(pair<p,q> a, pair<p,q> b) { return mp(a.f + b.f, a.s + b.s); }
template <class p, class q> void umin(p &a, const q &b) { if (a > b) a = b; }
template <class p, class q> void umax(p &a, const q &b) { if (a < b) a = b; }
using lf=long double;
using ll=long long;
using ull=unsigned long long;
using cull=const ull;
using cll=const ll;
using cint=const int;
using pll=pair<ll,ll>;
using pii=pair<int,int>;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
