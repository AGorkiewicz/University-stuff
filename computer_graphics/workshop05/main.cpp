#include "utils.cpp"
#include "window.cpp"
#include "drawables.cpp"

lf randlf() { return lf(rand() % 1001) / 1000.0; }
lf randlf(lf b) { return randlf() * b; }
lf randlf(lf a, lf b) { return randlf(b - a) + a; }

void keepBoxed(vec3& pos, const lf& r = 0.0){
	umax(pos.x, r); umin(pos.x, Box::mx - r);
	umax(pos.y, r); umin(pos.y, Box::my - r);
	umax(pos.z, r); umin(pos.z, Box::mz - r);
}

void keepBoxed(Sphere& sphere){
	keepBoxed(sphere.position, sphere.radius);
}

struct BubbleManager : Sphere
{
	lf speed;
	lf growth;

	void reset(bool from_bottom)
	{
		lf x = randlf(Box::mx);
		lf z = randlf(Box::mz);
		lf y = from_bottom ? 0.0 : randlf(Box::my);
		if (glm::length(vec2(x, z) - vec2(Box::mx / 2.0, 0.0)) < 0.5) { reset(from_bottom); return; }
		position = vec3(x, y, z);
		speed = 0.01;
		speed = randlf(0.05, 0.2);
		growth = randlf(0.01, 0.03);
		radius = randlf(0.01, 0.1) + y * growth;
		keepBoxed(position, radius);
	}

	void update(lf deltaTime)
	{
		position.y += speed * deltaTime;
		radius += speed * deltaTime * growth;
		if (position.y + radius >= Box::my) reset(true);
		keepBoxed(position, radius);
	}
};

bool loop(MyWindow* window, int level)
{
	lf start_timer = glfwGetTime();
	lf prev_timer = prev_timer;
	lf frame_len = 0.001;
	lf horizontalAngle = 0.0;
	lf verticalAngle = 0.0;
	lf initialFoV = 45.0f;
	lf speed = 1.0f;
	lf mouseSpeed = 0.05f;
	lf rotSpeed = 1.0f;
	lf cameraDistance = 0.2;
	bool mouse_control = true;
	int camera_mode = 0;
	int control_mode = 0;
	float FoV = 45.0;

	if (mouse_control) glfwSetInputMode(window->ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	vec3 light_position(Box::mx / 2.0, Box::my - 0.01, Box::mz / 2.0);
	
	Box box;

	Player player(vec3(Box::mx / 2.0, Box::my / 2.0, 0.02 * 10), 0.02);
	
	Bubbles bubbles;

	vector <BubbleManager> managers(300 * level);
	for (auto& manager : managers) manager.reset(false);

	glClearColor(0.3, 0.4, 0.5, 0.0);

	while (true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);
		
		lf timer = glfwGetTime();
		lf deltaTime = timer - prev_timer;
		if (deltaTime < frame_len) continue;
		prev_timer = timer;
		
		for (auto& manager : managers) manager.update(deltaTime);
		
		vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
		vec3 perpendicular(cos(horizontalAngle), 0.0, -sin(horizontalAngle));
		vec3 cameraPosition = player.position - cameraDistance * direction;
		keepBoxed(cameraPosition);
		
		mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + direction, vec3(0, 1, 0));
		
		if (camera_mode == 1){
			cameraPosition = vec3(Box::my * -1.0, Box::my / 2.0, player.position.z);
			viewMatrix = lookAt(cameraPosition, cameraPosition + vec3(1, 0, 0), vec3(0, 1, 0));
		}
		
		mat4 projectionMatrix = glm::perspective(lf(glm::radians(FoV)), lf(window->mx) / lf(window->my), 0.1f, 100.0f);
		mat4 unviewMatrix = glm::transpose(viewMatrix);
		mat4 perspectiveMatrix = projectionMatrix * viewMatrix;

		vector <Sphere> bubble_instances;

		for (auto& manager : managers){
			bubble_instances.pb(Sphere(manager.position, manager.radius));
		}

		sort(all(bubble_instances), [&](Sphere A, Sphere B) {
			return dot(direction, A.position) > dot(direction, B.position);
		});

		box.draw(perspectiveMatrix, light_position, player.position, cameraPosition, camera_mode);
		bubbles.draw(bubble_instances, perspectiveMatrix, unviewMatrix, light_position, player.position, cameraPosition);
		player.draw(perspectiveMatrix, unviewMatrix, light_position, cameraPosition, control_mode);
		
		glfwPollEvents();
		glfwSwapBuffers(window->ptr);
		
		auto pressed = [&](int key) { return glfwGetKey(window->ptr, key) == GLFW_PRESS; };

		vec3 shift(0.0, 0.0, 0.0);
		
		if (pressed(GLFW_KEY_MINUS)) FoV += 0.2;
		if (pressed(GLFW_KEY_EQUAL)) FoV -= 0.2;
		if (pressed(GLFW_KEY_TAB)) camera_mode ^= 1;
		if (pressed(GLFW_KEY_C)) control_mode ^= 1;
		if (pressed(GLFW_KEY_W)) shift += control_mode == 0 ? direction : vec3(0, 1, 0);
		if (pressed(GLFW_KEY_S)) shift -= control_mode == 0 ? direction : vec3(0, 1, 0);
		if (pressed(GLFW_KEY_A)) shift += control_mode == 0 ? perpendicular : vec3(0, 0, 0);
		if (pressed(GLFW_KEY_D)) shift -= control_mode == 0 ? perpendicular : vec3(0, 0, 0);
		if (pressed(GLFW_KEY_LEFT))  shift += vec3(1, 0, 0);
		if (pressed(GLFW_KEY_RIGHT)) shift -= vec3(1, 0, 0);
		if (pressed(GLFW_KEY_UP))    shift += vec3(0, 0, 1);
		if (pressed(GLFW_KEY_DOWN))  shift -= vec3(0, 0, 1);
		if (pressed(GLFW_KEY_M)) glfwSetInputMode(window->ptr, GLFW_CURSOR, (mouse_control ^= 1) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		
		if (mouse_control){
			double xpos, ypos;
			double cx = window->mx / 2.0;
			double cy = window->my / 2.0;
			glfwGetCursorPos(window->ptr, &xpos, &ypos);
			horizontalAngle += mouseSpeed * deltaTime * (cx - xpos);
			verticalAngle   += mouseSpeed * deltaTime * (cy - ypos);	
			glfwSetCursorPos(window->ptr, cx, cy);
		}

		if (length(shift) > 0.0) player.position += shift * deltaTime * speed / length(shift);
		
		umin(verticalAngle, +PI / 2.0f - 1e-5f);
		umax(verticalAngle, -PI / 2.0f + 1e-5f);
		
		if (player.position.z + player.radius >= Box::mz){
			printf("Level %d completed in %.1lf seconds!\n", level, timer - start_timer);
			return true;
		}

		for (auto& bubble : managers){
			if (length(player.position - bubble.position) <= player.radius + bubble.radius){
				printf("You ran into a bubble. You lost!\n");
				return false;
			}
		}

		keepBoxed(player);
	
		if (pressed(GLFW_KEY_ESCAPE) || glfwWindowShouldClose(window->ptr)){
			printf("Game stopped. You Lost!\n");
			return false;
		}
	} 
}

int main(int argc, char** argv)
{
	srand(argc > 1 ? atoi(argv[1]) : 5);
	
	MyWindow window(2048, 1024, "Pracownia 5");
	
	rep(level, 1, 5) if (loop(&window, level) == false) return 0;

	printf("You won!\n");
	
	return 0;
}
