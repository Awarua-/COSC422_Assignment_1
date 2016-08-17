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
GLuint cameraLoc;
GLuint lightloc;
GLuint wireFrameLoc;
GLuint heightMap;
GLuint texWater;
GLuint texGrass;
GLuint texRock;
GLuint texSnow;
Camera camera;
int defaultTessLevel;
int maxTessLevel;
int slopePointLookAhead;

glm::vec3 light;


// Force nvidia dedicated graphics
// comment out if causing problems
#ifdef _WIN32 
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

void loadTextures()
{
	glGenTextures(1, &heightMap);   //Generate 1 texture ID 

	glActiveTexture(GL_TEXTURE0);  //Texture unit 0 
	glBindTexture(GL_TEXTURE_2D, heightMap);
	Utilities::loadTGA("HeightMap.tga");  // Load heightMap 

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texWater);
	glActiveTexture(GL_TEXTURE1);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texWater);
	Utilities::loadTGA("water.tga"); // water

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texGrass);
	glActiveTexture(GL_TEXTURE2);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texGrass);
	Utilities::loadTGA("grass.tga"); // grass

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texRock);
	glActiveTexture(GL_TEXTURE3);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texRock);
	Utilities::loadTGA("rock.tga"); // rock

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texSnow);
	glActiveTexture(GL_TEXTURE4);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texSnow);
	Utilities::loadTGA("snow.tga"); // snow

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void initialise()
{
	// Load shaders
	GLuint shaderv = Utilities::loadShader(GL_VERTEX_SHADER, "Assignment1.vert");
	GLuint shaderf = Utilities::loadShader(GL_FRAGMENT_SHADER, "Assignment1.frag");
	GLuint shadertc = Utilities::loadShader(GL_TESS_CONTROL_SHADER, "Assignment1.tcs");
	GLuint shaderte = Utilities::loadShader(GL_TESS_EVALUATION_SHADER, "Assignment1.tes");
	GLuint shaderg = Utilities::loadShader(GL_GEOMETRY_SHADER, "Assignment1.geom");

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shadertc);
	glAttachShader(program, shaderte);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderg);
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

	// Changing variables
	matrixLoc = glGetUniformLocation(program, "mvpMatrix");
	cameraLoc = glGetUniformLocation(program, "cameraVector");
	wireFrameLoc = glGetUniformLocation(program, "wireframe");

	// Lighting
	light = glm::vec3(GRIDSIZE / 2, 1000.0, GRIDSIZE / 2);
	lightloc = glGetUniformLocation(program, "lightPos");
	glUniform3fv(lightloc, 1, &light[0]);

	// Grid size
	GLuint gridMax = glGetUniformLocation(program, "gridSizeMax");
	glUniform1f(gridMax, GRIDSIZE);

	// Height modifier
	GLuint heightLoc = glGetUniformLocation(program, "heightScale");
	glUniform1i(heightLoc, HEIGHTSCALE);

	// Tessilation levels
	defaultTessLevel = 1;
	maxTessLevel = 10;
	slopePointLookAhead = 4;

	GLuint dtl = glGetUniformLocation(program, "defaultTessLevel");
	glUniform1i(dtl, defaultTessLevel);

	GLuint mtl = glGetUniformLocation(program, "maxTessLevel");
	glUniform1i(mtl, maxTessLevel);

	GLuint slopePointLoc = glGetUniformLocation(program, "slopeDistance");
	glUniform1i(slopePointLoc, slopePointLookAhead);

	// Load textures 
	loadTextures();
	
	GLuint texLoc = glGetUniformLocation(program, "heightMap");
	glUniform1i(texLoc, 0);
	texLoc = glGetUniformLocation(program, "water");
	glUniform1i(texLoc, 1);
	texLoc = glGetUniformLocation(program, "grass");
	glUniform1i(texLoc, 2);
	texLoc = glGetUniformLocation(program, "rock");
	glUniform1i(texLoc, 3);
	texLoc = glGetUniformLocation(program, "snow");
	glUniform1i(texLoc, 4);

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


	GLuint vboID[2];

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void update(int value)
{
	glutTimerFunc(50, update, 0);
	glutPostRedisplay();
}

void display()
{
	//send mvp matrix
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &camera.apply()[0][0]);

	//send camera position
	glm::vec4 cameraPos = glm::vec4(camera.getPosition(), 0);
	glUniform4fv(cameraLoc, 1, &cameraPos[0]);

	//wireframe mode
	int wireframeInt = 0;
	if (camera.getWireFrameMode())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireframeInt = 1;
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUniform1i(wireFrameLoc, wireframeInt);

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

