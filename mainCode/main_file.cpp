#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "globals.h"
#include "drawing.h"
#include "myCube.h"
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

#include <vector>
#include <fstream>
#include <iostream>

using namespace glm;
using namespace std;

vector<vector<uint8_t>> mapping(string path) {
	ifstream file;
	file.open("imgpixel.txt");
	if (!file) {
		cout << "There was a problem opening file for reading" << endl;
		exit(0);
	}

	vector<uint8_t> v;
	vector<vector<uint8_t>> map;
	int count = 0;
	string line;

	while (getline(file, line)) {
		try {
			int value = stoi(line);
			if (value < 0 || value > 255) {
				throw out_of_range("Value out of range");
			}
			uint8_t i = static_cast<uint8_t>(value);
			v.push_back(i);
			count++;
		}
		catch (const invalid_argument& e) {
			cout << "Invalid integer in file: " << line << endl;
			continue;
		}
		catch (const out_of_range& e) {
			cout << "Integer out of range in file: " << line << endl;
			continue;
		}

		if (count == 256) {
			map.push_back(v);
			v.clear();
			count = 0;
		}
	}

	// Push the last row if it was not pushed yet
	if (!v.empty()) {
		map.push_back(v);
	}

	file.close();
	/*
	// Print the map
	for (size_t x = 0; x < map.size(); x++) {
		for (size_t y = 0; y < map[x].size(); y++) {
			cout << static_cast<int>(map[x][y]) << " ";
		}
		cout << endl;
	}
	*/
	return map;
}

vector<vector<uint8_t>> map = mapping(path);
vector<Rectangle> rectangles = detectRectangles(map);

void colisionCoordinates(const std::vector<Rectangle>& rectangles){
	par.clear(); // Clear the vector to ensure it starts empty
	par.reserve(rectangles.size()); // Reserve space to avoid multiple allocations


	for (const auto& rect : rectangles) {
		float centerX = ((rect.x2+1 + rect.x1) / 2) -128;
		float centerY = ((rect.y2+1 + rect.y1) / 2) -128;
		float distanceX = std::abs(centerX - (rect.x1-128)) + 0.001;
		float distanceY = std::abs(centerY - (rect.y1-128)) + 0.001;


		if (rect.value == 59) { //wall
			par.push_back({ centerX, 50, centerY, distanceX, 50, distanceY, "wall"});
		}
		else if (rect.value == 225 || rect.value == 0) { //background
			par.push_back({ centerX, 1, centerY, distanceX, 0.1, distanceY, "background"});
		}
		else if (rect.value == 42) { //floor2
			par.push_back({ centerX, 8, centerY, distanceX, 0.1, distanceY, "floor2"});
		}
		else if (rect.value == 108) { //stair1 E
			par.push_back({ centerX, 3.5, centerY, (distanceX - 0.7f), 5, (distanceY - 0.7f) , "stair1" });
		}
		else if (rect.value == 147) { //stair2 N
			par.push_back({ centerX, 3.5, centerY, (distanceX - 0.7f), 5, (distanceY - 0.7f) , "stair2" });
		}
		else if (rect.value == 27) { //stair3 W
			par.push_back({ centerX, 3.5, centerY, (distanceX - 0.7f), 5, (distanceY - 0.7f) , "stair3" });
		}
		else if (rect.value == 160) { //stair4 S
			par.push_back({ centerX, 3.5, centerY, (distanceX - 0.7f), 5, (distanceY - 0.7f) , "stair4" });
		}
	}
}

//fullscreen
void toggleFullscreen(GLFWwindow* window){
	fullscreen = !fullscreen;

	if (fullscreen)
	{
		// Zapamiętaj pozycję i rozmiar okna przed przełączeniem do trybu pełnoekranowego
		glfwGetWindowPos(window, &windowPosX, &windowPosY);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

		// Przełącz do trybu pełnoekranowego
		monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		// Przełącz do trybu okienkowego
		glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowedWidth, windowedHeight, 0);
	}
}
//mouse movement
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (dead) return;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Odwrócone, ponieważ y-coordinates rosną od dołu ekranu
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // Czułość myszki
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	angle_y += -xoffset;
	angle_x += -yoffset;

	// Ograniczenie pitch
	if (angle_x > 89.0f)
		angle_x = 89.0f;
	if (angle_x < -89.0f)
		angle_x = -89.0f;
}
//reading texture duh
GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	return tex;
}
//mouse buttons
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		shoot = 1;
		if (action == GLFW_PRESS && !mouseButtonPressed) {
			mouseButtonPressed = true;
			shooting();
			std::cout << protag.pos.x << " " << protag.pos.y << " " << protag.pos.z << " " << endl;
		}
		else if (action == GLFW_RELEASE) {
			mouseButtonPressed = false;
		}
	}
}
//keyboard buttons
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_A) ws2 = wspeed;
		if (key == GLFW_KEY_D) ws2 = -wspeed;
		if (key == GLFW_KEY_W) walks = wspeed;
		if (key == GLFW_KEY_S) walks = -wspeed;
		if (key == GLFW_KEY_LEFT_SHIFT) {
			stan = 1;
			if (kuc == 0) kuc = 1;
			else kuc = 0;
		}
		if (key == GLFW_KEY_SPACE) {
			if (protag.high < pod + 0.2) {
				ystart = protag.high;
				jump = 1;
				kuc = 0;
				stan = 1;
			}
		}
		if (key == GLFW_KEY_R) { //reload
			en_location.clear(); // Clear the vector to ensure it starts empty
			targets.clear(); // Clear the vector to ensure it starts empty
			enemylocation();
			protag.pos = vec3(-62, 2.7, 121);
			protag.dir = vec3(0, 0, 1);
			dead = 0;
		}
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, true); // Ustawienie flagi zamknięcia okna
		}
		if (key == GLFW_KEY_T) toggleFullscreen(window);
		if (key == GLFW_KEY_TAB) {
			if (stop == 0) {
				stop = 1;
			}else {
				stop = 0;
			}
		}

	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) ws2 = 0;
		if (key == GLFW_KEY_D) ws2 = 0;

		if (key == GLFW_KEY_W) walks = 0;
		if (key == GLFW_KEY_S) walks = 0;
	}
}
//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}
//resizing the window
void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}
//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.1, 0.1, 0.1, 1); //Ustaw kolor czyszczenia bufora kolorów
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex[0] = readTexture("bricks.png");
	tex[1] = readTexture("stone-wall.png");
	tex[2] = readTexture("zielen.png");
	tex[4] = readTexture("white.png");
	tex[5] = readTexture("angel_block.png");
	tex[6] = readTexture("Death screen.png");
	tex[7] = readTexture("TCom_Rock_CliffLayered_1.5x1.5_1K_albedo.png");
	tex[8] = readTexture("red.png");
	tex[9] = readTexture("skin.png");
	colisionCoordinates(rectangles);
	enemylocation();
}
//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	for (int i = 0; i < 10; i++) {
		glDeleteTextures(1, &tex[i]);
	}
}

void floor1(glm::mat4 P, glm::mat4 V, glm::mat4 M, float f1fx, float f1fz, float f1lx, float f1lz, uint8_t type) {
	f1lx += 1;
	f1lz += 1;
	int m1 = abs(f1fx - f1lx);
	int m2 = abs(f1fz - f1lz);
	int mx = m1;
	int mz = m2;

	float vertex[] = {
	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 1.0f, f1fz - 128,1.0f,

	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	};
	float myCubeTexCoords2[] = {
		m1, 0,	  0, m2,    0, 0,
		m1, 0,	  m1, m2,    0, m2,
	};
	int vertexCount = 6;
	int vertexCountHalf = 3;
	vec4 position = vec4(protag.pos, 1);


	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex);

	glEnableVertexAttribArray(sp->a("color"));
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, color);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, myCubeNormalsYp);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoords2);

	glEnableVertexAttribArray(sp->a("mcpos"));
	glVertexAttribPointer(sp->a("mcpos"), 4, GL_FLOAT, false, 0, glm::value_ptr(position));


	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[0]);

	//floor 0
	if (type == 53) {
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[7]);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	//empty
	else if (type == 255) {
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	//wall
	else if (type == 59) {
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);

		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = 99;
		float myCubeTexCoord1[] = {
		m1, 0,	  0, m2,    0, 0,
		m1, 0,	  m1, m2,    0, m2,
		};
		m2 = mz;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoord1);

		glDisableVertexAttribArray(sp->a("normal"));
		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, myCubeNormalsZm);

		float vertex1[] = {
	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 100.0f, f1fz - 128,1.0f,
	f1fx - 128, 1.0f, f1fz - 128,1.0f,

	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 100.0f, f1fz - 128,1.0f,
	f1fx - 128, 100.0f, f1fz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex1);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m1 = 99;
		float myCubeTexCoord2[] = {
		1.0f, f1lz - 128,	  100.0f, f1fz - 128,    1.0f, f1fz - 128,
		1.0f, f1lz - 128,	  100.0f, f1lz - 128,      100.0f, f1fz - 128,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoord2);

		glDisableVertexAttribArray(sp->a("normal"));
		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, myCubeNormalsXp);

		float vertex2[] = {
	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 100.0f, f1lz - 128,1.0f,
	f1lx - 128, 1.0f, f1lz - 128,1.0f,


	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 100.0f, f1fz - 128,1.0f,
	f1lx - 128, 100.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex2);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = 99;
		float myCubeTexCoord3[] = {
		m1, 0,	  0, m2,    0, 0,
		m1, 0,	  m1, m2,    0, m2,
		};
		m2 = mz;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoord3);

		glDisableVertexAttribArray(sp->a("normal"));
		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, myCubeNormalsZp);

		float vertex3[] = {
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 100.0f, f1lz - 128,1.0f,
	f1fx - 128, 1.0f, f1lz - 128,1.0f,

	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 100.0f, f1lz - 128,1.0f,
	f1fx - 128, 100.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex3);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = 99;
		float myCubeTexCoord4[] = {
		m1, 0,	  0, m2,    0, 0,
		m1, 0,	  m1, m2,    0, m2,
		};
		m2 = mz;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoord2);

		glDisableVertexAttribArray(sp->a("normal"));
		glEnableVertexAttribArray(sp->a("normal"));
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, myCubeNormalsXm);

		float vertex4[] = {
	f1fx - 128, 100.0f, f1fz - 128,1.0f,
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 100.0f, f1lz - 128,1.0f,

	f1fx - 128, 100.0f, f1fz - 128,1.0f,
	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex4);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex);

	}
	//ramp s
	else if (type == 108) {
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);

		float vertexs1[] = {
	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1fz - 128,1.0f,
		};

		glDisableVertexAttribArray(sp->a("texCoord0"));
		float myCubeTexCoordsS1[] = {
		1.0f, f1fz - 128,	  1.0f, f1lz - 128,   8.0f, f1fz - 128,
		};
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS1);

		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs1);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountHalf);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = sqrt(pow(abs(f1lz - f1fz), 2) + pow(7, 2));
		m1 = mx;
		float myCubeTexCoordsS2[] = {
		0, 0,	  m1, m2,    0, m2,
		0, 0,	  m1, 0,    m1, m2,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS2);

		float vertexs2[] = {
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
	f1fx - 128, 8.0f, f1fz - 128,1.0f,

	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs2);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		float myCubeTexCoordsS3[] = {
		1.0f, f1lz - 128,	  1.0f, f1fz - 128,   8.0f, f1fz - 128,
		};
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS3);

		float vertexs3[] = {
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs3);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountHalf);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = sqrt(pow(abs(f1lz - f1fz), 2) + pow(7, 2));
		m1 = mx;
		float myCubeTexCoordsS4[] = {
		f1fx - 128, 1.0f,	  f1lx - 128, 8.0f,    f1lx - 128, 1.0f,
		f1fx - 128, 1.0f,	  f1fx - 128, 8.0f,    f1lx - 128, 8.0f,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS4);

		float vertexs4[] = {
	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
	f1lx - 128, 1.0f, f1fz - 128,1.0f,

	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 8.0f, f1fz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs4);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex);
	}
	//floor 1
	else if (type == 42) {
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[1]);

		float vertexf2[] = {
	f1fx - 128, 8.0f, f1fz - 128,1.0f,
	f1lx - 128, 8.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,

	f1fx - 128, 8.0f, f1fz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexf2);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex);
	}
	//ramp n
	else if (type == 147) {
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);

		float vertexs1[] = {
	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
		};

		glDisableVertexAttribArray(sp->a("texCoord0"));
		float myCubeTexCoordsS1[] = {
		f1fx - 128, 1.0f,	  f1lx - 128, 1.0f,   f1lx - 128, 8.0f,
		};
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS1);

		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs1);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountHalf);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = mz;
		m1 = sqrt(pow(abs(f1lx - f1fx), 2) + pow(7, 2));;
		float myCubeTexCoordsS2[] = {
		0, m2,	  m1, 0,    0, 0,
		0, m2,	  m1, m2,   m1, 0,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS2);

		float vertexs2[] = {
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
	f1fx - 128, 1.0f, f1fz - 128,1.0f,

	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs2);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		float myCubeTexCoordsS3[] = {
		f1fx - 128, 1.0f,	  f1lx - 128, 1.0f,   f1lx - 128, 8.0f,
		};
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS3);

		float vertexs3[] = {
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs3);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountHalf);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = sqrt(pow(abs(f1lz - f1fz), 2) + pow(7, 2));
		m1 = mx;
		float myCubeTexCoordsS4[] = {
		f1fx - 128, 1.0f,	  f1lx - 128, 8.0f,    f1lx - 128, 1.0f,
		f1fx - 128, 1.0f,	  f1fx - 128, 8.0f,    f1lx - 128, 8.0f,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS4);

		float vertexs4[] = {
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
	f1lx - 128, 1.0f, f1fz - 128,1.0f,

	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1fz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs4);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex);
	}
	//ramp w
	else if (type == 27) {
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);

		float vertexs1[] = {
	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 8.0f, f1fz - 128,1.0f,
		};

		glDisableVertexAttribArray(sp->a("texCoord0"));
		float myCubeTexCoordsS1[] = {
		f1fx - 128, 1.0f,	  f1lx - 128, 1.0f,   f1fx - 128, 8.0f,
		};
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS1);

		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs1);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountHalf);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = mz;
		m1 = sqrt(pow(abs(f1lx - f1fx), 2) + pow(7, 2));;
		float myCubeTexCoordsS2[] = {
		m1, 0,	  0, m2,    0, 0,
		m1, 0,	  m1, m2,    0, m2,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS2);

		float vertexs2[] = {
	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1fz - 128,1.0f,

	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs2);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		float myCubeTexCoordsS3[] = {
		f1fx - 128, 1.0f,	  f1lx - 128, 1.0f,   f1fx - 128, 8.0f,
		};
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS3);

		float vertexs3[] = {
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs3);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountHalf);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = sqrt(pow(abs(f1lz - f1fz), 2) + pow(7, 2));
		m1 = mx;
		float myCubeTexCoordsS4[] = {
		f1fx - 128, 1.0f,	  f1lx - 128, 8.0f,    f1lx - 128, 1.0f,
		f1fx - 128, 1.0f,	  f1fx - 128, 8.0f,    f1lx - 128, 8.0f,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS4);

		float vertexs4[] = {
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1fz - 128,1.0f,
	f1fx - 128, 1.0f, f1fz - 128,1.0f,

	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1fz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs4);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex);
	}
	//ramp e
	else if (type == 160) {
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);

		float vertexs1[] = {
	f1fx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
		};

		glDisableVertexAttribArray(sp->a("texCoord0"));
		float myCubeTexCoordsS1[] = {
		1.0f, f1fz - 128,	  1.0f, f1lz - 128,   8.0f, f1fz - 128,
		};
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS1);

		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs1);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountHalf);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = sqrt(pow(abs(f1lz - f1fz), 2) + pow(7, 2));
		m1 = mx;
		float myCubeTexCoordsS2[] = {
		m1, 0,	  0, m2,    0, 0,
		m1, 0,	  m1, m2,    0, m2,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS2);

		float vertexs2[] = {
	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
	f1fx - 128, 1.0f, f1fz - 128,1.0f,

	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 8.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs2);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		float myCubeTexCoordsS3[] = {
		1.0f, f1fz - 128,	  1.0f, f1lz - 128,   8.0f, f1fz - 128,
		};
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS3);

		float vertexs3[] = {
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 1.0f, f1fz - 128,1.0f,
	f1lx - 128, 8.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs3);
		glDrawArrays(GL_TRIANGLES, 0, vertexCountHalf);

		glDisableVertexAttribArray(sp->a("texCoord0"));
		m2 = sqrt(pow(abs(f1lz - f1fz), 2) + pow(7, 2));
		m1 = mx;
		float myCubeTexCoordsS4[] = {
		f1fx - 128, 1.0f,	  f1lx - 128, 8.0f,    f1lx - 128, 1.0f,
		f1fx - 128, 1.0f,	  f1fx - 128, 8.0f,    f1lx - 128, 8.0f,
		};
		m1 = mx;
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoordsS4);

		float vertexs4[] = {
	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
	f1fx - 128, 1.0f, f1lz - 128,1.0f,

	f1lx - 128, 1.0f, f1lz - 128,1.0f,
	f1lx - 128, 8.0f, f1lz - 128,1.0f,
	f1fx - 128, 8.0f, f1lz - 128,1.0f,
		};
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertexs4);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glDisableVertexAttribArray(sp->a("vertex"));
		glEnableVertexAttribArray(sp->a("vertex"));
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertex);
	}
	//default
	else
	{
		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[2]);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float momen, float angle, float anglelegs, float angleL2, float anglearms, float angle_y,float amomen) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości
	glm::mat4 V = glm::lookAt(protag.pos, protag.pos + protag.dir, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 0.1f, 100.0f); //Wylicz macierz rzutowania

	//spLambert->use(); //Aktywuj program cieniujący
	sp->use();
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku

	glm::mat4 M = glm::mat4(1.0f);

	drawcursor(P, V);
	
	for (const auto& rect : rectangles) {
		floor1(P, V, M, rect.x1, rect.y1, rect.x2, rect.y2, rect.value);
	}

	for (int i = 0; i < en_location.size(); i++) {
		enemy(P, V, momen / 40,i);
		en_shoot(i);
		melee_dmg(i);
	}

	glm::mat4 Mc1 = glm::translate(M, protag.pos); //matrix for mc model
	Mc1 = glm::rotate(Mc1, glm::radians(90.0f), glm::vec3(0, -1, 0));
	Mc1 = glm::rotate(Mc1, glm::radians(angle_y), glm::vec3(0, 1, 0));
	Mc1 = glm::translate(Mc1, glm::vec3(-0.2, -1.8, 0));
	Mc1 = glm::scale(Mc1, glm::vec3(0.6f, 0.6f, 0.6f));

	if (dead) { //dead or alive
		Mc1 = glm::translate(Mc1, glm::vec3(0.8, 0, 0));
		Mc1 = glm::translate(Mc1, glm::vec3(0, 0.4, 0));
		death(momen, angle, Mc1, P, V);
	}
	else {
		kucanie(momen);
		gravity(momen);
		skok(momen);
		en_movements(momen, amomen);
		if (shoot) {
			drawRay(P, V, M, rayS, rayE, tex[4]);
			recoil(momen);
		}
		Mc(angle, anglelegs, angleL2, anglearms, Mc1, P, V);
	}
	//Postac(angle, anglelegs, angleL2, anglearms, Mc1);
	//drw(M, P, V);


	glfwSwapBuffers(window); //Skopiuj bufor tylny do bufora przedniego
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(700, 700, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.
	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle = 0; //Aktualny kąt obrotu obiektu
	float anglelegs = 0; //Aktualny kąt nóg
	float anglearms = 0; //Aktualny kąt rąk
	float angleL2 = 0; //Aktualny kąt przedramion i łydek??
	float momen = 0; //ticks
	float amomen = 0; //ticks
	int a = 1; //variables for going back and forth
	int b = 1;
	int c = 1;

	glfwSetCursorPosCallback(window, cursor_position_callback);

	// Ukrycie kursora i zablokowanie go w oknie
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{

		momen = 10 * glfwGetTime()*stop;
		amomen += momen*stop;

		if (!dead) { walking(); }

		anglelegs += abs(walks / 2) * a * speed2 * glfwGetTime(); //Oblicz przyrost kąta po obrocie
		if (anglelegs > 80) { a = -1; }
		if (anglelegs < 0) { a = 1; }
		angleL2 += abs(walks / 2) * b * speed2 * glfwGetTime(); //Oblicz przyrost kąta po obrocie
		if (angleL2 > 40) { b = -1; }
		if (angleL2 < 10) { b = 1; }
		anglearms += abs(walks / 2) * c * speed2 * 1.5 * glfwGetTime(); //Oblicz przyrost kąta po obrocie
		if (anglearms > 120) { c = -1; }
		if (anglearms < 0) { c = 1; }

		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, momen, angle, anglelegs, angleL2, anglearms, angle_y,amomen); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}