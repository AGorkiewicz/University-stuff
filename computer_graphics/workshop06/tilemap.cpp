#pragma once
#include "utils.cpp"

struct Tile
{
	short data[SRTM_SIZE][SRTM_SIZE];

	Tile() {}

	Tile(const char* path)
	{
		FILE* file = fopen(path, "rb");
		if (file == NULL)
		{
			fprintf(stderr, "Error opening file %s\n", path);
			return;
		}
		printf("File %s opened successfully\n", path);
		(void)(fread((void*)(data), sizeof(short), SRTM_SIZE * SRTM_SIZE, file));
		rep(i, 0, SRTM_SIZE - 1) rep(j, 0, SRTM_SIZE - 1){
			data[i][j] = htobe16(data[i][j]);
			if (data[i][j] > 9000 || data[i][j] < -500){
				data[i][j] = 0;
			}
		}
		fclose(file);
	}
};

struct TileMap
{
	int min_NS = +INF;
	int max_NS = -INF;
	int min_EW = +INF;
	int max_EW = -INF;
	int start_NS = INF;
	int start_EW = INF;
	int start_height = INF;
	int start_level = INF;

	map < pair <int, int> , Tile > data;

	TileMap(int count, char** args)
	{	
		FILE* file;

		bool start_flag = false;

		rep(i, 1, count - 1){
			if (args[i][0] == '-'){
				start_flag = false;
				if (i == count - 1) continue;
				if (string(args[i]) == string("-start")) start_flag = true;
				if (string(args[i]) == string("-level")) start_level = atoi(args[i + 1]);
			}
			else handle_argument(args[i], start_flag);
		}

		if (min_NS == +INF) min_NS = -90;
		if (max_NS == -INF) max_NS = +90;
		if (min_EW == +INF) min_EW = -180;
		if (max_EW == -INF) max_EW = +180;
		if (min_NS == max_NS) max_NS += 1;
		if (min_EW == max_EW) max_EW += 1;

		handle_directory(args[0]);
	}

	pair <int, int> get_coordinates(const char* filename)
	{
		int NS = 10 * (filename[1] - '0') + filename[2] - '0';
		int EW = 100 * (filename[4] - '0') + 10 * (filename[5] - '0') + (filename[6] - '0');

		if (filename[0] == 'S') NS *= -1;
		if (filename[3] == 'W') EW *= -1;

		return {NS, EW};
	}

	void handle_argument(string argument, bool is_start = false)
	{
		char dir = argument[0];
		if (dir != 'N' && dir != 'S' && dir != 'E' && dir != 'W'){
			if (is_start){
				start_height = atoi(argument.c_str());
			}
			return;
		}
		reverse(all(argument)), argument.pop_back(), reverse(all(argument));
		int value = atoi(argument.c_str());
		if (dir == 'S' || dir == 'W') value *= -1;
		if (dir == 'N' || dir == 'S') { umin(min_NS, value); umax(max_NS, value); if (is_start) start_NS = value; }
		if (dir == 'E' || dir == 'W') { umin(min_EW, value); umax(max_EW, value); if (is_start) start_EW = value; }
	}

	void handle_directory(const char* path)
	{
		DIR* dir = opendir(path);
		struct dirent* sfile;
		if (dir == NULL)
		{
			fprintf(stderr, "Error opening directiory %s\n", path);
			return;
		}
		printf("Directory %s opened successfully\n", path);
		while (sfile = readdir(dir))
		{	
			const auto name = sfile->d_name;
			if (name[0] == '.') continue;
			handle_file(path, name);
		}
	}

	void handle_file(const char* path, const char* filename)
	{
		const auto coord = get_coordinates(filename);
		if (coord.f < min_NS) return;
		if (coord.s < min_EW) return;
		if (coord.f >= max_NS) return;
		if (coord.s >= max_EW) return;
		data[coord] = Tile((string(path) + "/" + string(filename)).c_str());
	}

	pair <lf, lf> get_middle()
	{
		lf min_Y = +INF, max_Y = -INF;
		lf min_X = +INF, max_X = -INF;
		for (const auto& [coords, tile] : data){
			umin(min_Y, coords.f);
			umin(min_X, coords.s);
			umax(max_Y, coords.f + 1);
			umax(max_X, coords.s + 1);
		}
		return {(min_Y + max_Y) / 2.0, (min_X + max_X) / 2.0};
	}
};
