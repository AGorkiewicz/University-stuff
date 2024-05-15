#include "utils.cpp"
#include "window.cpp"
#include "drawables.cpp"
#include "tilemap.cpp"

int main(int argc, char** argv)
{
	TileMap tiles(argc - 1, argv + 1);

	if (tiles.data.empty()) return printf("No tiles loaded.\n"), 0;

	auto[NS, EW] = tiles.get_middle();
	lf height = 30.0;
	int level = 0;

	if (tiles.start_NS != INF) NS = tiles.start_NS;
	if (tiles.start_EW != INF) EW = tiles.start_EW;
	if (tiles.start_height != INF) height = tiles.start_height;
	if (tiles.start_level != INF) level = tiles.start_level - 1;

	MyWindow window(2048, 1024, "Pracownia 6");

	lf prev_timer = glfwGetTime();
	lf horizontal_angle = 0.0;
	lf vertical_angle = -PI / 2.0;
	lf FoV = 45.0;
	int control_mode = 0;
	bool auto_lod = true;

	auto switch_control = [&](){
		glfwSetInputMode(window.ptr, GLFW_CURSOR, control_mode == 1 ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(window.ptr, window.mx / 2.0, window.my / 2.0);
	};

	switch_control();

	Planet terrain(tiles, level);
	
	glClearColor(0.3, 0.4, 0.5, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	lf agg_fps = 0.0;

	while (true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lf curr_timer = glfwGetTime(), delta_time = curr_timer - prev_timer;
		if (delta_time < 0.001) continue;
		prev_timer = curr_timer;

		lf fps = 1.0 / delta_time;
		agg_fps = 0.95 * agg_fps + 0.05 * fps;

		printf("Frames per second: %.2f    Level of detail (1 = high, 8 = low): %d    Height: %.2f km    Rendered triangles per frame: %lld\n", fps, level + 1, height, terrain.rendered_triangles(level));

		if (auto_lod == true && fps < 30.0 && level < 7) level += 1;
		if (auto_lod == true && agg_fps > 55.0 && level > 0) level -= 1, agg_fps = 0.0;

		if (control_mode == 0) vertical_angle = -PI / 2.0, horizontal_angle = 0.0;

		lf alpha = DEGREE * -EW, beta = DEGREE * NS;
		vec3 spherical_position(cos(beta) * cos(alpha), sin(beta), cos(beta) * sin(alpha));
		vec3 camera_position = spherical_position * vec3(RADIUS + height);
		vec3 up(-sin(beta) * cos(alpha), cos(beta), -sin(beta) * sin(alpha));

		mat4 projection_matrix = glm::perspective(lf(glm::radians(FoV)), lf(window.mx) / lf(window.my), lf(0.1), lf(3 * RADIUS));
		mat4 view_matrix = control_mode ? glm::rotate(vertical_angle, vec3(-1, 0, 0)) * glm::rotate(horizontal_angle, vec3(0, -1, 0)) * lookAt(camera_position, camera_position + up, spherical_position) : lookAt(camera_position, vec3(0), up);
		mat4 perspective_matrix = projection_matrix * view_matrix;

		vec2 shift(0.0, 0.0);
		lf speed = delta_time * height * 0.02;

		terrain.draw(perspective_matrix, level);

		glfwPollEvents(), glfwSwapBuffers(window.ptr);

		auto pressed = [&](int key) { return glfwGetKey(window.ptr, key) == GLFW_PRESS; };

		if (pressed(GLFW_KEY_MINUS)) FoV += 0.2;
		if (pressed(GLFW_KEY_EQUAL)) FoV -= 0.2;
		if (pressed(GLFW_KEY_W) || pressed(GLFW_KEY_UP))    shift.y += 1.0;
		if (pressed(GLFW_KEY_S) || pressed(GLFW_KEY_DOWN))  shift.y -= 1.0;
		if (pressed(GLFW_KEY_A) || pressed(GLFW_KEY_LEFT))  shift.x -= 1.0;
		if (pressed(GLFW_KEY_D) || pressed(GLFW_KEY_RIGHT)) shift.x += 1.0;
		if (pressed(GLFW_KEY_0)) auto_lod = true;
		if (pressed(GLFW_KEY_1)) auto_lod = false, level = 0;
		if (pressed(GLFW_KEY_2)) auto_lod = false, level = 1;
		if (pressed(GLFW_KEY_3)) auto_lod = false, level = 2;
		if (pressed(GLFW_KEY_4)) auto_lod = false, level = 3;
		if (pressed(GLFW_KEY_5)) auto_lod = false, level = 4;
		if (pressed(GLFW_KEY_6)) auto_lod = false, level = 5;
		if (pressed(GLFW_KEY_7)) auto_lod = false, level = 6;
		if (pressed(GLFW_KEY_8)) auto_lod = false, level = 7;
		if (pressed(GLFW_KEY_Q)) height -= speed * 50.0;
		if (pressed(GLFW_KEY_E)) height += speed * 50.0;
		if (pressed(GLFW_KEY_F)) control_mode = 1, switch_control();
		if (pressed(GLFW_KEY_G)) control_mode = 0, switch_control();
		if (pressed(GLFW_KEY_ESCAPE) || glfwWindowShouldClose(window.ptr)) break;

		if (length(shift) > 0.1){
			shift = speed * vec2(glm::rotate(horizontal_angle, vec3(0, 0, 1)) * vec4(shift, 0, 0)) / length(shift);
			NS += shift.y;
			EW += shift.x;
		}

		if (control_mode == 1){
			double xpos, ypos, cx = window.mx / 2.0, cy = window.my / 2.0;
			glfwGetCursorPos(window.ptr, &xpos, &ypos);
			horizontal_angle += 0.05 * delta_time * (cx - xpos);
			vertical_angle   += 0.05 * delta_time * (cy - ypos);	
			glfwSetCursorPos(window.ptr, cx, cy);
		}

		umin(NS, +90);
		umax(NS, -90);

		if (EW > +180.0) EW -= 180.0;
		if (EW < -180.0) EW += 180.0;

		if (horizontal_angle < 0.0) horizontal_angle += 2.0 * PI;
		if (horizontal_angle > 2.0 * PI) horizontal_angle -= 2.0 * PI;

		umin(vertical_angle, +PI / 2.0f - 1e-5f);
		umax(vertical_angle, -PI / 2.0f + 1e-5f);
	}

	return 0;
}
