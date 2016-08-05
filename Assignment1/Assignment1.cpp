//  ========================================================================
//  COSC422: Advanced Computer Graphics;  University of Canterbury.
//
//  FILE NAME: Assignment1.cpp
//
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Assignment1.h"
using namespace std;

GLuint vaoID;
GLuint theProgram;
GLuint matrixLoc;
float angle = 0.0;
glm::mat4 projView;

// Force nvidia dedicated graphics
// comment out if causing problems
#ifdef _WIN32 
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}


void initialise()
{
	glm::mat4 proj, view;
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "Assignment1.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "Assignment1.frag");
	GLuint shadertc = loadShader(GL_TESS_CONTROL_SHADER, "Assignment1.tcs");
	GLuint shaderte = loadShader(GL_TESS_EVALUATION_SHADER, "Assignment1.tes");

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shadertc);
	glAttachShader(program, shaderte);
	glAttachShader(program, shaderf);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program);

	proj = glm::perspective(float(20 * 3.1415 / 180), 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
	view = glm::lookAt(glm::vec3(0.0, 5.0, 12.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix
	projView = proj*view;  //Product matrix

	GLuint vboID[4];

	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenVertexArrays(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);

	for (auto j = 0; j < 128; j++)
	{	
		for (auto i = 0; i < 128; i++)
		{
			verts[(j * 3 * 128) + i * 3] = i;
			verts[(j * 3 * 128) + (i * 3 + 1)] = 0;
			verts[(j * 3 * 128) + (i * 3 + 2)] = -j;
			elems[(j * 128) + i] = (j * 128) + i;
		}
	}

	for (auto k = 0; k < sizeof(elems) / 4; k++)
	{
		elems[k * 4] = k;
		elems[k * 4 + 1] = k + 1;
		elems[k * 4 + 2] = 128 + k + 1;
		elems[k * 4 + 3] = 128 + k;
	}

//	GLfloat outLevel[4] = { 6, 6, 6, 6 };
//	GLfloat inLevel[2] = { 6, 6 };

	glPatchParameteri(GL_PATCH_VERTICES, 4);
//	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outLevel);
//	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, inLevel);


    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void update(int value)
{
	angle++;
	glutTimerFunc(50, update, 0);
	glutPostRedisplay();
}

void display()
{
	glm::mat4 matrix = glm::mat4(1.0);
	matrix = glm::rotate(matrix, float(angle * 3.15159 / 180), glm::vec3(0.0, 1.0, 0.0));  //rotation matrix
	glm::mat4 prodMatrix = projView * matrix;        //Model-view-proj matrix

	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_PATCHES, 128 * 128, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Assignment1 Dion Woolley");
	glutInitContextVersion (4, 2);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutTimerFunc(50, update, 0); 
	glutMainLoop();
}

