#pragma once
#include "utils.cpp"
#include "mesh.cpp"
#include "shader.cpp"
#include "texture.cpp"

struct Mesh
{
	Shader shader;
	uint VAO, VBO, EBO, texture;
	vector <Vertex> vertices;
	vector <uint> indices;

	void normalize_positions()
	{
		vec3 avg_position(0);
		for (auto& vertex : vertices) avg_position += vertex.position;
		avg_position /= lf(ss(vertices));
		for (auto& vertex : vertices) vertex.position -= avg_position;
		lf max_distance(0.01);
		for (auto& vertex : vertices) umax(max_distance, length(vertex.position));
		for (auto& vertex : vertices) vertex.position /= max_distance;
	}

	Mesh(int argc, char ** argv) : shader("shaders/mesh")
	{
		if (argc != 3){
			texture = load_texture("textures/steve.png");
			(void)(load_mesh("models/steve.obj", vertices, indices));
		}else{
			texture = load_texture(argv[2]);
			(void)(load_mesh(argv[1], vertices, indices));
		}

		normalize_positions();

		glGenVertexArrays(1, &VAO), glBindVertexArray(VAO);

		glGenBuffers(1, &VBO), glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, ss(vertices) * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &EBO), glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ss(indices) * sizeof(uint), indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0), glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
		glEnableVertexAttribArray(1),	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(2), glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, uv)));
	}

	void draw(mat4 perspective, lf offset)
	{
		glBindVertexArray(VAO);

		glActiveTexture(GL_TEXTURE0), glBindTexture(GL_TEXTURE_2D, texture);

		shader.use().setMatrix("perspective", perspective).setTextureSampler("myTextureSampler", 0).setFloat("offset", offset);

		glDrawElements(GL_TRIANGLES, ss(indices), GL_UNSIGNED_INT, 0);
	}
};
