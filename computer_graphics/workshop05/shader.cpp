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
	
	void use() 
	{
		glUseProgram(id);
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

	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value); 
	}

	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value); 
	}

	void setFloat(const std::string &name, float value) const
	{ 
		glUniform1f(glGetUniformLocation(id, name.c_str()), value); 
	}
	
	void setMatrix(const std::string &name, mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}
	
	void setVec3(const std::string &name, vec3 value) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
	}
};
