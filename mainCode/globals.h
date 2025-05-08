#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
//pablo
#include <vector>
#include <fstream>
#include <iostream>
#include "FindRec.h"

using namespace glm;
using namespace std;

const float WALK_SPEED = 15;
int stop = 1; //stop ticks 0-stop 1-move
bool mouseButtonPressed = false;
float en_angle = 0;
float back = 1; //recoil recover
float shot = 0; //recoil
int shoot = 0; //shooting
vec3 rayS;
vec3 rayE;
vec3 bronS;
float ystart = 0; //jump start position
int onground = 0; //being on ground
float walks = 0; //forwards and back walking speed
float ws2 = 0; //horizontal walking speed
float wspeed = WALK_SPEED;  //walking speed parameter
int stan = 0; //interrupt sneaking
float dif = 0; //sneaking parameter
float speed2 = 60; //speed of limbs
float pod = 0; //current groud
int kuc = 0; //sneaking
int jump = 0;//jumping
float fall_time = 0;
float jump_time = 0;
float enemy_walk = 0;
int en_way = 1;
float aspectRatio = 1;//ration of window hight and width
float angle_x = 0; //x camera rotation
float angle_y = 0; //y camera rotation
float angle_z = 0; //z camera rotation
bool dead = 0; //dead
float momen = 0; //ticks
float domen = 0; //ticks
GLuint tex[10]; //texture table

//model structure
struct parametry {
	float x;
	float y;
	float z;
	float size_x;
	float size_y;
	float size_z;
	string type;
};

struct AABB //AABB - Axis-Aligned Bounding Box)
{
	glm::vec3 min;
	glm::vec3 max;
	int health;
	string path;
	float shoot_time = 0;
	int en_tex=5;
};
std::vector<AABB> targets;

std::vector<parametry> par;

struct MainCharacter {
	float high = 1; //mc elevation
	float height = 1.6; //mc height
	vec3 pos = vec3(-120, 2.7, 121); //mc position
	vec3 dir = vec3(0, 0, 1); //looking direction
	vec3 xdir = vec3(0, 0, 1); //xdir
	float size = 1.0f; //mc size x,z
};
MainCharacter protag;

float lastX = 400, lastY = 300; // mouse starting position
bool firstMouse = true; //mouse first on screen

bool fullscreen = false; //fullscreen
GLFWwindow* window; //window
GLFWmonitor* monitor; //monitor
int windowedWidth = 800, windowedHeight = 600; //window width and height
int windowPosX, windowPosY; //position X and Y

ShaderProgram* sp;
string path = "imgpixel.txt";

float myCubeNormalsZm[] = {
	//Œciana 1
	0.0f, 0.0f,-1.0f,0.0f,
	0.0f, 0.0f,-1.0f,0.0f,
	0.0f, 0.0f,-1.0f,0.0f,

	0.0f, 0.0f,-1.0f,0.0f,
	0.0f, 0.0f,-1.0f,0.0f,
	0.0f, 0.0f,-1.0f,0.0f,
};
float myCubeNormalsZp[] = {
	//Œciana 2
	0.0f, 0.0f, 1.0f,0.0f,
	0.0f, 0.0f, 1.0f,0.0f,
	0.0f, 0.0f, 1.0f,0.0f,

	0.0f, 0.0f, 1.0f,0.0f,
	0.0f, 0.0f, 1.0f,0.0f,
	0.0f, 0.0f, 1.0f,0.0f,
};
float myCubeNormalsYm[] = {
	//Œciana 3
	0.0f,-1.0f, 0.0f,0.0f,
	0.0f,-1.0f, 0.0f,0.0f,
	0.0f,-1.0f, 0.0f,0.0f,

	0.0f,-1.0f, 0.0f,0.0f,
	0.0f,-1.0f, 0.0f,0.0f,
	0.0f,-1.0f, 0.0f,0.0f,
};
float myCubeNormalsYp[] = {
	//Œciana 4
	0.0f, 1.0f, 0.0f,0.0f,
	0.0f, 1.0f, 0.0f,0.0f,
	0.0f, 1.0f, 0.0f,0.0f,

	0.0f, 1.0f, 0.0f,0.0f,
	0.0f, 1.0f, 0.0f,0.0f,
	0.0f, 1.0f, 0.0f,0.0f,
};
float myCubeNormalsXm[] = {
	//Œciana 5
	-1.0f, 0.0f, 0.0f,0.0f,
	-1.0f, 0.0f, 0.0f,0.0f,
	-1.0f, 0.0f, 0.0f,0.0f,

	-1.0f, 0.0f, 0.0f,0.0f,
	-1.0f, 0.0f, 0.0f,0.0f,
	-1.0f, 0.0f, 0.0f,0.0f,
};
float myCubeNormalsXp[] = {
	//Œciana 6
	1.0f, 0.0f, 0.0f,0.0f,
	1.0f, 0.0f, 0.0f,0.0f,
	1.0f, 0.0f, 0.0f,0.0f,

	1.0f, 0.0f, 0.0f,0.0f,
	1.0f, 0.0f, 0.0f,0.0f,
	1.0f, 0.0f, 0.0f,0.0f,
};
float color[] = {
1.0f,1.0f,0.0f,1.0f,
1.0f,1.0f,0.0f,1.0f,
1.0f,1.0f,0.0f,1.0f,

1.0f,1.0f,0.0f,1.0f,
1.0f,1.0f,0.0f,1.0f,
1.0f,1.0f,0.0f,1.0f,
};


struct location
{
	float x;
	float y;
	float z;
	string path;
};

std::vector<location> en_location;