#pragma once
#include "utils.cpp"
using namespace std;

struct Shader
{
	GLuint id;

	Shader() : id(0) {}

	Shader(const char* path)
	{
		compileShadersFromFile(path);
	}
	
	Shader& use() 
	{
		glUseProgram(id);
		return *this;
	}
	
	void compileShaders(GLuint v, GLuint f, GLuint g = 0)
	{
		if (g) compileLink(g, "GS");
		compileLink(v, "VS");
		compileLink(f, "FS");
		id = glCreateProgram();
		glAttachShader(id, v);
		glAttachShader(id, f);
		if (g) glAttachShader(id, g);
		compileLink(id, "Linking",  3);
		glDeleteShader(v);
		glDeleteShader(f);
		if (g) glDeleteShader(g);
	} 
	
	GLint compileLink(GLuint v, const char *which, int prog = 0)
	{
		GLint res = GL_FALSE;
		int InfoLogLength;
		if (prog){
			glLinkProgram(v);
			glGetProgramiv(v, GL_LINK_STATUS, &res);
			glGetProgramiv(v, GL_INFO_LOG_LENGTH, &InfoLogLength);
		}else{
			glCompileShader(v);
			glGetShaderiv(v, GL_COMPILE_STATUS, &res);
			glGetShaderiv(v, GL_INFO_LOG_LENGTH, &InfoLogLength);
		}
		if (InfoLogLength > 0 && !res){
			std::vector<char> message(InfoLogLength + 1);
			if (prog)
				glGetProgramInfoLog(v, InfoLogLength, NULL, &message[0]);
			else
				glGetShaderInfoLog(v, InfoLogLength, NULL, &message[0]);
			printf("%s: %s\n", which, &message[0]);
			fflush(stdout);
			exit(1);
		}
		return res;
	}

	void compileShadersFromFile(const char *vfs, const char *gs = NULL) {
		GLuint v = glCreateShader(GL_VERTEX_SHADER);
		GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
		GLuint g = gs ? glCreateShader(GL_GEOMETRY_SHADER) : 0;
		getShaderSource({v, f}, vfs);
		if (gs) getShaderSource({g}, gs);
		compileShaders(v, f, g);
	}
	
	void getShaderSource(vector <GLuint> sId, const char *file) {
		vector <string> sCode = {""};
		ifstream sStream(file, ios::in);
		if (sStream.is_open()) {
			string Line = "";
			while (getline(sStream, Line)){
				if (Line[0] == '~'){
					sCode.push_back("");
				}else{
					sCode.back() += "\n" + Line;
				}
			}
			sStream.close();
		} else {
			printf("Error opening file:  %s !\n", file);
			return;
		}
		if (sId.size() != sCode.size()){
			printf("Error opening file:  %s !\n", file);
			return;
		}
		for (int i = 0; i < sId.size(); i++){
			char const * SourcePointer = sCode[i].c_str();
			glShaderSource(sId[i], 1, &SourcePointer, NULL);
		}
	}

	uint location(const string& name)
	{
		return glGetUniformLocation(id, name.c_str());
	}

	Shader& setBool(const string& name, bool value)
	{
		return glUniform1i(location(name), (int)value), *this;
	}

	Shader& setInt(const string& name, int value)
	{
		return glUniform1i(location(name), value), *this; 
	}

	Shader& setFloat(const string& name, float value)
	{ 
		return glUniform1f(location(name), value), *this; 
	}
	
	Shader& setMatrix(const string& name, mat4 value)
	{
		return glUniformMatrix4fv(location(name), 1, GL_FALSE, &value[0][0]), *this;
	}
	
	Shader& setVec3(const string& name, vec3 value)
	{
		return glUniform3f(location(name), value.x, value.y, value.z), *this;
	}

	Shader& setTextureSampler(const string& name, int value)
	{
		return glUniform1i(location(name), value), *this;
	}
};
