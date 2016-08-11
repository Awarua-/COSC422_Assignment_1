#pragma once
#include <GL/glew.h> 
#include <fstream> 

using namespace std;

class Utilities
{
public:
	static GLuint loadShader(GLenum shaderType, string filename);
	static void loadTGA(string filename);
};