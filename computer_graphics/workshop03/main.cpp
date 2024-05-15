#include <bits/stdc++.h>
#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>
#include <drawables.cpp>
#define rep(i, a, b) for (int i = (a); i <= (b); i++)

class MyWin : public AGLWindow {
public:
    MyWin() {};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0) : AGLWindow(_wd, _ht, name, vers, fullscr) {};
    void KeyCB(int key, int scancode, int action, int mods);
	void CallbackResize(GLFWwindow* window, int cx, int cy);
		void MainLoop();
	};

void MyWin::KeyCB(int key, int scancode, int action, int mods) {
    AGLWindow::KeyCB(key,scancode, action, mods); // f-key full screen switch
    if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
       ; // do something
    }
    if (key == GLFW_KEY_HOME  && (action == GLFW_PRESS)) {
       ; // do something
    }
}


lf IW(const point& a, const point& b) { return a.x * b.y - a.y * b.x; }
lf IW(const point& a, const point& b, const point& c) { return IW(a - c, b - c); }

bool collision(segment a, segment b){
	lf iw1 = IW(a.b, b.a, a.a);
	lf iw2 = IW(a.b, b.b, a.a);
	if (iw1 * iw2 > 0.0) return false;
	lf iw3 = IW(b.b, a.a, b.a);
	lf iw4 = IW(b.b, a.b, b.a);
	if (iw3 * iw4 > 0.0) return false;
	return true;
}

int N = 10;

void MyWin::MainLoop()
{
	ViewportOne(0, 0, wd, ht);

	vector <segment> segments;
	
	rep(X, 0, N - 1){
		rep(Y, 0, N - 1){
			lf x = ((X * 2 + 1) / lf(N) - 1.0);
			lf y = ((Y * 2 + 1) / lf(N) - 1.0);
			segment seg;
			seg.set_center(point(x, y));
			lf slope = (rand() % 1000000) / 10000.0;
			seg.set_slope(slope);
			seg.set_color(0, 1.0, 1.0, 0.0);
			seg.set_color(1, 1.0, 0.0, 1.0);
			segments.pb(seg);
		}
	}

	Background background;

	lf slope = 0.0;
	segment* player = &segments[0];
	player->set_color(0, 1.0, 1.0, 1.0);
	player->set_color(1, 0.0, 1.0, 1.0);
	player->set_slope(slope);
	Segments segment_drawer;
	
	segment* target = &segments.back();
	target->set_color(0, 1.0, 0.0, 0.0);
	target->set_color(1, 0.0, 1.0, 0.0);

	lf prev_timer = 0.0;
	lf frame_len = 0.001;

	do {
		glClear(GL_COLOR_BUFFER_BIT);

		AGLErrors("main-loopbegin");
		
		lf timer = glfwGetTime();
		if (timer - prev_timer < frame_len){
			continue;
		}
		prev_timer = timer;

		segment backup = *player;
		lf backup_slope = slope;

		background.draw(timer, wd, ht);
		segment_drawer.draw(segments, wd, ht);

		AGLErrors("main-afterdraw");
		
		glfwSwapBuffers(win());
		glfwPollEvents();

		point dir = player->a - player->center;
		
		if (glfwGetKey(win(), GLFW_KEY_LEFT)  == GLFW_PRESS) slope += 0.015;
		if (glfwGetKey(win(), GLFW_KEY_RIGHT) == GLFW_PRESS) slope -= 0.015;
		if (glfwGetKey(win(), GLFW_KEY_UP)    == GLFW_PRESS) player->set_center(player->center + 0.02 * dir);
		if (glfwGetKey(win(), GLFW_KEY_DOWN)  == GLFW_PRESS) player->set_center(player->center - 0.02 * dir);
		
		player->set_slope(slope);
		
		if (collision(*player, *target)){
			printf("YOU WON!\n");
			printf("Time: %f s\n", timer);
			exit(0);
		}
		
		rep(i, 1, ss(segments)){
			if (collision(*player, segments[i])){
				*player = backup;
				slope = backup_slope;
				break;
			}
		}
	} while (glfwGetKey(win(), GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(win()) == 0);
}

int main(int argc, char *argv[]) {
	if (argc > 1){
		srand(atoi(argv[1]));
	}else{
		srand(5);
	}
	if (argc > 2){
		N = atoi(argv[2]);
	}
	scale = 1.0 / N;
	MyWin win;
	win.Init(1024, 1024, "Pracownia 3", 0, 33);
	win.MainLoop();
	return 0;
}
