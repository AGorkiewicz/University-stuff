#pragma once
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bits/stdc++.h>
#include <fstream>
#include <dirent.h>
using namespace std;
using ll=long long;
using lf=GLfloat;
using uint=GLuint;
using mat4=glm::mat4;
using vec2=glm::vec2;
using vec3=glm::vec3;
using vec4=glm::vec4;

const lf PI = acos(-1);
const lf DEGREE = PI / 180.0;
const int INF = 1000000005;
const int SRTM_SIZE = 1201;
const lf RADIUS = 6378;

namespace debug { template <class c> struct rge { c b, e; }; template <class c> rge<c> range(c i, c j) { return rge<c>{i, j}; } template <class c> char spk(...); template <class c> auto spk(c* a) -> decltype(cerr << *a, 0); struct stream { ~stream() { cerr << endl; } template <class c> typename enable_if <sizeof spk<c>(0) != 1, stream&>::type operator<<(c i) { cerr << boolalpha << i; return *this; } template <class c> typename enable_if <sizeof spk<c>(0) == 1, stream&>::type operator<<(c i) { return *this << range(begin(i), end(i)); } template <class a, class b> stream& operator<<(pair <a,b> p) { return *this << "(" << p.first << ", " << p.second << ")"; } template <class c> stream& operator<<(rge<c> d) { *this << "["; for (auto it=d.b; it!=d.e; it++) *this << ", " + 2 * (it == d.b) << *it; return *this << "]"; } stream& _dbg(const string& s, int i, int b) { return *this; } template <class c, class ... cs> stream& _dbg(const string& s, int i, int b, c arg, cs ... args) { if (i == (int)(s.size())) return (*this << ": " << arg); b += (s[i] == '(') + (s[i] == '[') + (s[i] == '{') - (s[i] == ')') - (s[i] == ']') - (s[i] == '}'); return (s[i] == ',' && b == 0) ? (*this << ": " << arg << "     ")._dbg(s, i + 1, b, args...) : (s[i] == ' ' ? *this : *this << s[i])._dbg(s, i + 1, b, arg, args...); } };}
#define dout debug::stream()
#define dbg(...) ((dout << ">> ")._dbg(#__VA_ARGS__, 0, 0, __VA_ARGS__))
#define all(x) x.begin(),x.end()
#define pb push_back
#define ss(x) ((int)((x).size()))
#define rep(i,a,b) for(int i=(a);i<=(b);i++)
#define per(i,a,b) for(int i=(b);i>=(a);i--)
#define f first
#define s second

ostream& operator << (ostream& os, vec2 a) { return os << "(" << a.x << ", " << a.y << ")"; }
ostream& operator << (ostream& os, vec3 a) { return os << "(" << a.x << ", " << a.y << ", " << a.z << ")"; }
ostream& operator << (ostream& os, vec4 a) { return os << "(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")"; }

template <class p, class q> void umin(p &a, const q &b) { if (a > b) a = b; }
template <class p, class q> void umax(p &a, const q &b) { if (a < b) a = b; }
