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
#include "Utilities.h"
#include "Camera.h"
using namespace std;

GLuint vaoID;
GLuint theProgram;
GLuint matrixLoc;
GLuint tex[1];
Camera camera;


// Force nvidia dedicated graphics
// comment out if causing problems
#ifdef _WIN32 
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

void loadTextures()
{
	glGenTextures(1, &tex[0]);   //Generate 1 texture ID 

								 // Load heightMap 
	glActiveTexture(GL_TEXTURE0);  //Texture unit 0 
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	Utilities::loadTGA("HeightMap.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void initialise()
{
	glm::mat4 proj, view;
	GLuint shaderv = Utilities::loadShader(GL_VERTEX_SHADER, "Assignment1.vert");
	GLuint shaderf = Utilities::loadShader(GL_FRAGMENT_SHADER, "Assignment1.frag");
	GLuint shadertc = Utilities::loadShader(GL_TESS_CONTROL_SHADER, "Assignment1.tcs");
	GLuint shaderte = Utilities::loadShader(GL_TESS_EVALUATION_SHADER, "Assignment1.tes");

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

	matrixLoc = glGetUniformLocation(program, "mvpMatrix");
	GLuint gridMax = glGetUniformLocation(program, "gridSizeMax");
	glUniform1f(gridMax, GRIDSIZE);

	// Load textures 
	loadTextures();
	GLuint texLoc = glGetUniformLocation(program, "heightMap");
	glUniform1i(texLoc, 0);

	// Setup Camera 
	camera = Camera();

	glutKeyboardFunc(Camera::keyPressed);
	glutKeyboardUpFunc(Camera::keyUp);
	glutSpecialFunc(Camera::specialKeyPressed);
	glutSpecialUpFunc(Camera::specialKeyUp);
	glutMotionFunc(Camera::mouseDrag);
	glutPassiveMotionFunc(Camera::mouseMove);
	glutMouseFunc(Camera::mouseClick);
	glutMouseWheelFunc(Camera::mouseScroll);

	GLuint vboID[4];

	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenVertexArrays(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);

	generateGrid();

	glPatchParameteri(GL_PATCH_VERTICES, 4);


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
	glutTimerFunc(50, update, 0);
	glutPostRedisplay();
}

void display()
{
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &camera.apply()[0][0]);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_PATCHES, 128 * 128 * 4, GL_UNSIGNED_SHORT, NULL);
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

