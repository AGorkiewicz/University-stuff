#pragma once
#include "utils.cpp"
#include "shader.cpp"
#include "tilemap.cpp"
using namespace std;

vec3 spherical(lf Y, lf X)
{
	lf alpha = DEGREE * -X;
	lf beta = DEGREE * Y;
	lf x = cos(beta) * cos(alpha);
	lf y = sin(beta);
	lf z = cos(beta) * sin(alpha);
	return vec3(x, y, z);
};

struct Planet
{
	static constexpr int M = 1200;
	static constexpr int max_level = 7;
	static constexpr int jumps[max_level + 1] = {1, 2, 4, 8, 16, 48, 240, 1200};
	int min_level, min_jump, m;

	Shader terrain_shader, filling_line_shader, filling_tile_shader;
	uint terrain_VBO, terrain_EBO[8], terrain_VAO;
	uint filling_VBO, filling_line_EBO, filling_tile_EBO, filling_VAO;
	vector <vec4> terrain_vertices;
	vector <vec3> filling_vertices;
	vector <uint> terrain_indices[8];
	vector <uint> filling_line_indices;
	vector <uint> filling_tile_indices;

	Planet(TileMap& tilemap, int min_level = 0) : min_level(min_level), min_jump(jumps[min_level]), m(M / min_jump), terrain_shader("shaders/terrain"), filling_line_shader("shaders/filling_lines"), filling_tile_shader("shaders/filling_tiles")
	{
		assert(min_level >= 0 && min_level <= 7);

		auto& tiles = tilemap.data;

		rep(Y, -90, 90) rep(X, -180, 180 - 1){
			if (tiles.find({Y, X}) != tiles.end()){
				assert(Y < 90);
				rep(level, min_level, max_level){
					auto& indices = terrain_indices[level];
					assert(jumps[level] % min_jump == 0);
					int jump = jumps[level] / min_jump;
					assert(m % jump == 0);
					for (int y = 0; y < m; y += jump) for (int x = 0; x < m; x += jump){
						int i = ss(terrain_vertices) + y * (m + 1) + x;
						if ((y + x) % 2 == 0){
							indices.pb(i);
							indices.pb(i + jump * (m + 1));
							indices.pb(i + jump);
							indices.pb(i + jump);
							indices.pb(i + jump * (m + 1));
							indices.pb(i + jump * (m + 2));
						}else{
							indices.pb(i);
							indices.pb(i + jump * (m + 2));
							indices.pb(i + jump);
							indices.pb(i);
							indices.pb(i + jump * (m + 1));
							indices.pb(i + jump * (m + 2));
						}
					}
				}
				auto& tile = tilemap.data[{Y, X}];
				for (int y = 0; y <= M; y += min_jump) for (int x = 0; x <= M; x += min_jump){
					int i = ss(terrain_vertices);
					lf sy = (Y + 1.0) - lf(y) / lf(M);
					lf sx = X + lf(x) / lf(M);
					lf h = tile.data[y][x];
					vec3 pos = spherical(sy, sx);
					pos *= vec3(RADIUS + (h / 1000.0));
					terrain_vertices.pb(vec4(pos, h));
				}
			}

			int i = ss(filling_vertices);
			filling_vertices.pb(spherical(Y, X) * vec3(RADIUS));

			if (Y < 90){
				int j = i + 1 - 360 * (X + 1 == 180);
				if (tiles.find({Y, X}) == tiles.end() || tiles.find({Y, X - 1 + 360 * (X == -180)}) == tiles.end()){
					filling_line_indices.pb(i);
					filling_line_indices.pb(i + 360);
				}
				if (tiles.find({Y, X}) == tiles.end() || tiles.find({Y - 1, X}) == tiles.end()){
					filling_line_indices.pb(i);
					filling_line_indices.pb(j);
				}
				if (tiles.find({Y, X}) == tiles.end()){
					if ((Y + X) % 2 == 0){
						filling_tile_indices.pb(i);
						filling_tile_indices.pb(j);
						filling_tile_indices.pb(i + 360);
						filling_tile_indices.pb(j);
						filling_tile_indices.pb(j + 360);
						filling_tile_indices.pb(i + 360);
					}else{
						filling_tile_indices.pb(i);
						filling_tile_indices.pb(j);
						filling_tile_indices.pb(j + 360);
						filling_tile_indices.pb(i);
						filling_tile_indices.pb(j + 360);
						filling_tile_indices.pb(i + 360);
					}
				}
			}
		}

		glGenVertexArrays(1, &terrain_VAO), glBindVertexArray(terrain_VAO);

		glGenBuffers(1, &terrain_VBO), glBindBuffer(GL_ARRAY_BUFFER, terrain_VBO);
		glBufferData(GL_ARRAY_BUFFER, ss(terrain_vertices) * sizeof(vec4), terrain_vertices.data(), GL_STATIC_DRAW);

		rep(level, min_level, max_level){
			glGenBuffers(1, &terrain_EBO[level]), glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_EBO[level]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ss(terrain_indices[level]) * sizeof(uint), terrain_indices[level].data(), GL_STATIC_DRAW);
		}

		glEnableVertexAttribArray(0), glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)(0));

		glGenVertexArrays(1, &filling_VAO), glBindVertexArray(filling_VAO);

		glGenBuffers(1, &filling_VBO), glBindBuffer(GL_ARRAY_BUFFER, filling_VBO);
		glBufferData(GL_ARRAY_BUFFER, ss(filling_vertices) * sizeof(vec3), filling_vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &filling_line_EBO), glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, filling_line_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ss(filling_line_indices) * sizeof(uint), filling_line_indices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &filling_tile_EBO), glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, filling_tile_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ss(filling_tile_indices) * sizeof(uint), filling_tile_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0), glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	}

	~Planet()
	{
		glDeleteVertexArrays(1, &terrain_VAO);
		glDeleteVertexArrays(1, &filling_VAO);
		glDeleteBuffers(1, &terrain_VBO);
		rep(level, min_level, max_level) glDeleteBuffers(1, &terrain_EBO[level]);
		glDeleteBuffers(1, &filling_VBO);
		glDeleteBuffers(1, &filling_line_EBO);
		glDeleteBuffers(1, &filling_tile_EBO);
		if (terrain_shader.id) glDeleteProgram(terrain_shader.id);
		if (filling_line_shader.id) glDeleteProgram(filling_line_shader.id);
		if (filling_tile_shader.id) glDeleteProgram(filling_tile_shader.id);
	}

	void draw(mat4 perspective, int level)
	{
		umax(level, min_level);

		glBindVertexArray(terrain_VAO);

		terrain_shader.use().setMatrix("perspective", perspective);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_EBO[level]);
		glDrawElements(GL_TRIANGLES, ss(terrain_indices[level]), GL_UNSIGNED_INT, 0);

		glBindVertexArray(filling_VAO);

		filling_line_shader.use().setMatrix("perspective", perspective);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, filling_line_EBO);
		glDrawElements(GL_LINES, ss(filling_line_indices), GL_UNSIGNED_INT, 0);

		filling_tile_shader.use().setMatrix("perspective", perspective);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, filling_tile_EBO);
		glDrawElements(GL_TRIANGLES, ss(filling_tile_indices), GL_UNSIGNED_INT, 0);
	}

	ll rendered_triangles(int level)
	{
		umax(level, min_level);
		assert(ss(terrain_indices[level]) % 3 == 0);
		return ss(terrain_indices[level]) / 3;
	}
};
