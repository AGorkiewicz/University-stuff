#pragma once
#include "utils.cpp"

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 uv;
};

bool load_mesh(const char* path, vector <Vertex>& vertices, vector <uint>& indices)
{
	using triple = array<int, 3>;

	vector <vec3> positions = {vec3(0)};
	vector <vec3> normals = {vec3(0)};
	vector <vec2> uvs = {vec2(0)};
	vector <triple> triples;

	printf("Loading OBJ file %s...\n", path);

	FILE* file = fopen(path, "r");

	if (file == NULL) return printf("Error loading file %s\n", path), false;

	static char header[1005], dump[1005];

	while (true)
	{
		int res = fscanf(file, "%s", header);

		if (res == EOF) break;

		if (strcmp(header, "v") == 0)
		{
			vec3 position;
			fscanf(file, "%f %f %f\n", &position.x, &position.y, &position.z);
			positions.pb(position);
			continue;
		}

		if (strcmp(header, "vn") == 0)
		{
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.pb(normal);
			continue;
		}

		if (strcmp(header, "vt") == 0)
		{
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // invert!
			uvs.pb(uv);
			continue;
		}

		if (strcmp(header, "f") == 0)
		{
			vector < vector <int> > vec;

			while (true){
				char c = getc(file);
				if (c == EOF) break;
				if (c == '\n') break;
				if (c == ' ') vec.pb({0});
				if (c == '/') vec.back().pb(0);
				if (c >= '0' && c <= '9') (vec.back().back() *= 10) += c - '0';
			}

			vector < triple > ids;

			for (auto v : vec){
				assert(ss(v) <= 3);
				while (ss(v) < 3) v.pb(0);
				ids.pb({v[0], v[2], v[1]});
			}

			rep(i, 1, ss(ids) - 2){
				triples.pb(ids[0]);
				triples.pb(ids[i]);
				triples.pb(ids[i + 1]);
			}
			
			// triple ids[3];
			// int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &ids[0][0], &ids[0][2], &ids[0][1], &ids[1][0], &ids[1][2], &ids[1][1], &ids[2][0], &ids[2][2], &ids[2][1]);
			// if (matches != 9) return printf("File %s does not fit required format\n", path), fclose(file), false;
			// rep(i, 0, 2) triples.pb(ids[i]);
			// dbg(ids[0], ids[1], ids[2]);

			continue;
		}

		fgets(dump, 1000, file); // dump comment
	}

	map <triple, uint> get_vertex;

	for (triple tri : triples){
		if (get_vertex.find(tri) == get_vertex.end()){
			get_vertex[tri] = ss(vertices);
			vertices.pb({positions[tri[0]], normals[tri[1]], uvs[tri[2]]});
		}
		indices.pb(get_vertex[tri]);
	}

	fclose(file);

	return true;
}
