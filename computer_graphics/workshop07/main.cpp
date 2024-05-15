#include "utils.cpp"
#include "window.cpp"
#include "drawables.cpp"

int main(int argc, char ** argv)
{
	MyWindow window(2048, 1024, "Pracownia 7");

	Mesh cube(argc, argv);

	glfwSetInputMode(window.ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	lf prev_timer = glfwGetTime();
	lf horizontal_angle = PI / 2.0;
	lf vertical_angle = 0.0;
	lf FoV = 45.0;
	lf distance = 3.0;
	lf offset = 20.0;
	glClearColor(0.3, 0.4, 0.5, 0.0);

	while (true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lf curr_timer = glfwGetTime(), delta_time = curr_timer - prev_timer;
		if (delta_time < 0.001) continue;
		prev_timer = curr_timer;

		vec3 direction(cos(vertical_angle) * sin(horizontal_angle), sin(vertical_angle), cos(vertical_angle) * cos(horizontal_angle));
		mat4 projection_matrix = glm::perspective(lf(glm::radians(FoV)), lf(window.mx) / lf(window.my), lf(0.01), lf(100.0));
		vec3 camera_position = direction * distance;
		mat4 view_matrix = lookAt(camera_position, vec3(0), vec3(0, 1, 0));
		mat4 perspective_matrix = projection_matrix * view_matrix;

		cube.draw(perspective_matrix, offset);

		glfwPollEvents(), glfwSwapBuffers(window.ptr);

		auto pressed = [&](int key) { return glfwGetKey(window.ptr, key) == GLFW_PRESS; };

		if (pressed(GLFW_KEY_MINUS)) FoV += 0.2;
		if (pressed(GLFW_KEY_EQUAL)) FoV -= 0.2;
		if (pressed(GLFW_KEY_Q)) distance *= 1.01;
		if (pressed(GLFW_KEY_E)) distance /= 1.01;
		if (pressed(GLFW_KEY_ESCAPE) || glfwWindowShouldClose(window.ptr)) break;
		if (pressed(GLFW_KEY_SPACE)) offset += 0.01;

		if (true){
			double xpos, ypos, cx = window.mx / 2.0, cy = window.my / 2.0;
			glfwGetCursorPos(window.ptr, &xpos, &ypos);
			horizontal_angle += 0.05 * delta_time * (cx - xpos);
			vertical_angle   -= 0.05 * delta_time * (cy - ypos);	
			glfwSetCursorPos(window.ptr, cx, cy);
		}

		if (horizontal_angle < 0.0) horizontal_angle += 2.0 * PI;
		if (horizontal_angle > 2.0 * PI) horizontal_angle -= 2.0 * PI;

		umin(vertical_angle, +PI / 2.0f - 1e-5f);
		umax(vertical_angle, -PI / 2.0f + 1e-5f);
	}

	return 0;
}
