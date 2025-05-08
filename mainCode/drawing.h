#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "Abilites.h"
#include "myCube.h"
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

void Cube(glm::mat4 P, glm::mat4 V, glm::mat4 M, GLuint tex) {

	vec4 position = vec4(protag.pos, 1);
	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, myCubeVertices);

	glEnableVertexAttribArray(sp->a("color"));
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, myCubeColors);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, myCubeNormals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, myCubeTexCoords);

	glEnableVertexAttribArray(sp->a("mcpos"));
	glVertexAttribPointer(sp->a("mcpos"), 4, GL_FLOAT, false, 0, glm::value_ptr(position));


	glUniform1i(sp->u("textureMap0"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex);
	glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount);

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("mcpos"));
}
//drawing a weapon
void bron(glm::mat4 M, glm::mat4 P, glm::mat4 V) {
	glm::mat4 Mb = translate(M, glm::vec3(1.5, 1, 0));
	Mb = glm::rotate(Mb, glm::radians(160.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Mb = glm::scale(Mb, glm::vec3(0.6f, 1.6, 0.6));
	Cube(P, V, Mb, tex[0]); //1

	Mb = glm::scale(Mb, glm::vec3(1.25, 0.5, 1.25));

	glm::mat4 Mb1 = translate(Mb, glm::vec3(-1.6, -2.5, 0));
	Mb1 = glm::rotate(Mb1, glm::radians(310.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Mb1 = glm::scale(Mb1, glm::vec3(0.75f, 2, 0.75));
	Cube(P, V, Mb1, tex[0]); //2
	Mb1 = glm::scale(Mb1, glm::vec3(1/0.75f, 1/2, 1/0.75));
	Mb1 = translate(Mb1, glm::vec3(0, 0, 1));
	vec4 Br = Mb1 * vec4(0, 0, 1, 0);
	bronS = vec3(Br);

}
//drawing mc
void Mc(float angle, float anglelegs, float angleL2, float anglearms, glm::mat4 Ms, glm::mat4 P, glm::mat4 V) { //rysowanie bohatera
	Ms = glm::rotate(Ms, angle, glm::vec3(0.0f, 1.0f, 0.0f));

	float ang = -angle_x * 0.8;

	//Tu³owie
	glm::mat4 Mp = glm::rotate(Ms, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Mp = glm::translate(Mp, glm::vec3(2.0f, 0.0f, 0.0f));
	Mp = glm::scale(Mp, glm::vec3(0.5f, 0.125f, 0.3f));
	Cube(P, V, Mp, tex[9]);
	Mp = glm::scale(Mp, glm::vec3(2.0f, 8.0f, 3.3f));

	glm::mat4 Ma1 = Mp;
	//Reka1
	Ma1 = glm::translate(Mp, glm::vec3(1.0f, 0, 0.25f));
	Ma1 = glm::rotate(Ma1, glm::radians(ang + 220 + shot), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);

	//Bron
	bron(Ma1, P, V);

	//Reka2
	Ma1 = glm::translate(Mp, glm::vec3(1.0f, 0, -0.25f));
	Ma1 = glm::rotate(Ma1, glm::radians((120 - anglearms) + 120), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(40 - angleL2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);

	//Noga1
	Ma1 = glm::translate(Mp, glm::vec3(-0.4f, 0.0f, 0.25f));
	Ma1 = glm::rotate(Ma1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(80.0f - anglelegs), glm::vec3(0.0f, 0.0f, -1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(angleL2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);

	//Noga2
	Ma1 = glm::translate(Mp, glm::vec3(-0.4f, 0.0f, -0.25f));
	Ma1 = glm::rotate(Ma1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(anglelegs), glm::vec3(0.0f, 0.0f, -1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(40.0f - angleL2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);

}
//drawing ANTEK
void Postac(float angle, float anglelegs, float angleL2, float anglearms, glm::mat4 Ms) { //rysowanie bohatera
	Ms = glm::rotate(Ms, angle, glm::vec3(0.0f, 1.0f, 0.0f));

	//Tu³owie
	glm::mat4 Mp = glm::rotate(Ms, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Mp = glm::translate(Mp, glm::vec3(2.0f, 0.0f, 0.0f));
	Mp = glm::scale(Mp, glm::vec3(0.5f, 0.125f, 0.4f));
	glUniform4f(spLambert->u("color"), 1, 1, 1, 1);
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mp));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu
	Mp = glm::scale(Mp, glm::vec3(2.0f, 8.0f, 2.5f));

	//Szyja
	glm::mat4 Ma1 = Mp;
	Ma1 = glm::translate(Ma1, glm::vec3(0.6f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.1f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu

	//G³owa
	Ma1 = glm::translate(Mp, glm::vec3(0.8f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.2f, 0.1f, 0.2f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu

	//Reka1
	Ma1 = glm::translate(Mp, glm::vec3(0.4f, 0.0f, 0.45f));
	Ma1 = glm::rotate(Ma1, glm::radians(anglearms + 120), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(angleL2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu

	//Reka2
	Ma1 = glm::translate(Mp, glm::vec3(0.4f, 0.0f, -0.45f));
	Ma1 = glm::rotate(Ma1, glm::radians((120 - anglearms) + 120), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(40 - angleL2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu

	//Noga1
	Ma1 = glm::translate(Mp, glm::vec3(-0.4f, 0.0f, 0.35f));
	Ma1 = glm::rotate(Ma1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(80.0f - anglelegs), glm::vec3(0.0f, 0.0f, -1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(angleL2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu

	//Noga2
	Ma1 = glm::translate(Mp, glm::vec3(-0.4f, 0.0f, -0.35f));
	Ma1 = glm::rotate(Ma1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(anglelegs), glm::vec3(0.0f, 0.0f, -1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(40.0f - angleL2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Ma1));  //Za³adowanie macierzy modelu do programu cieniuj¹cego
	Models::cube.drawSolid(); //Narysowanie obiektu

}

void deathscreen(glm::mat4 P, glm::mat4 V) {
	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 screen = glm::translate(M, glm::vec3(protag.pos + protag.dir));
	screen = rotate(screen, glm::radians(angle_y), vec3(0, 1, 0));
	screen = rotate(screen, glm::radians(angle_x), vec3(1, 0, 0));
	screen = rotate(screen, glm::radians(180.0f), vec3(0, 0, 1));
	screen = glm::scale(screen, glm::vec3(0.25f, 0.25f, 0.25f));
	Cube(P, V, screen, tex[6]);
}
//dying animation
void death(float momen, float angle, glm::mat4 Ms, glm::mat4 P, glm::mat4 V) {

	if (kuc) {
		stop = 1;
		kuc = 0;
		kucanie(momen);
	}
	domen += 20 * momen;
	float s1 = domen; //falling
	float s2 = 0; //moving forward
	float bump = 0; //bounce
	Ms = glm::rotate(Ms, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	if (domen > 90.0f) {
		s1 = 90.0f;
		s2 = domen - s1;
		bump = s2;
	}
	if (domen > 112.5) {
		bump = 45 - s2;
	}
	if (domen > 135.0f) {
		s1 = 90.0f;
		s2 = 45.0f;
		bump = 0;
		deathscreen(P,V);
	}
	Ms = glm::translate(Ms, glm::vec3(s2 / 45, 0.0f, 0.0f));
	Ms = glm::translate(Ms, glm::vec3(0, bump / 90, 0.0f));
	Ms = glm::rotate(Ms, glm::radians(-s1), glm::vec3(0.0f, 0.0f, 1.0f));

	//Tu³owie
	glm::mat4 Mp = glm::rotate(Ms, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Mp = glm::translate(Mp, glm::vec3(2.0f, 0.0f, 0.0f));
	Mp = glm::scale(Mp, glm::vec3(0.5f, 0.125f, 0.4f));
	Cube(P, V, Mp, tex[9]);
	Mp = glm::scale(Mp, glm::vec3(2.0f, 8.0f, 2.5f));

	//Szyja
	glm::mat4 Ma1 = Mp;
	Ma1 = glm::translate(Ma1, glm::vec3(0.6f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.1f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);

	//G³owa
	Ma1 = glm::translate(Mp, glm::vec3(0.8f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.2f, 0.1f, 0.2f));
	Cube(P, V, Ma1, tex[9]);

	//Reka1
	Ma1 = glm::translate(Mp, glm::vec3(0.4f, 0.0f, 0.45f));
	Ma1 = glm::rotate(Ma1, glm::radians(180.0f - s1 / 2 + s2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(s1 / 2 - s2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);

	//Bron
	bron(Ma1, P, V);

	//Reka2
	Ma1 = glm::translate(Mp, glm::vec3(0.4f, 0.0f, -0.45f));
	Ma1 = glm::rotate(Ma1, glm::radians((180.0f - s1 / 2 + s2)), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(s1 / 2 - s2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);

	//Noga1
	Ma1 = glm::translate(Mp, glm::vec3(-0.4f, 0.0f, 0.35f));
	Ma1 = glm::rotate(Ma1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(s1 / 2 - s2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);

	//Noga2
	Ma1 = glm::translate(Mp, glm::vec3(-0.4f, 0.0f, -0.35f));
	Ma1 = glm::rotate(Ma1, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
	Ma1 = glm::scale(Ma1, glm::vec3(4.0f, 10.0f, 12.5f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::rotate(Ma1, glm::radians(s1 / 2 - s2), glm::vec3(0.0f, 0.0f, 1.0f));
	Ma1 = glm::translate(Ma1, glm::vec3(0.2f, 0.0f, 0.0f));
	Ma1 = glm::scale(Ma1, glm::vec3(0.25f, 0.1f, 0.08f));
	Cube(P, V, Ma1, tex[9]);
}

//drawing stuff
void drw(glm::mat4 M, glm::mat4 P, glm::mat4 V) {
	glm::mat4 Floor = M;
	Floor = glm::scale(Floor, glm::vec3(100, 0.01, 100));
	Cube(P, V, Floor, tex[0]);

	for (int i = 0; i < 4; i++) {
		glm::mat4 Wall = M;
		Wall = glm::rotate(Wall, glm::radians(90.0f * i), glm::vec3(0, 1, 0));
		Wall = glm::translate(Wall, glm::vec3(0, 0, 20));
		Wall = glm::rotate(Wall, glm::radians(90.0f), glm::vec3(1, 0, 0));
		Wall = glm::scale(Wall, glm::vec3(20, 0.1f, 20));
		Cube(P, V, Wall, tex[1]);
	}

	glm::mat4 M1 = M;
	M1 = glm::translate(M1, glm::vec3(4, 2, 0));
	M1 = glm::scale(M1, glm::vec3(1, 2, 1));
	Cube(P, V, M1, tex[0]);

	glm::mat4 M5 = M;
	M5 = glm::translate(M5, glm::vec3(0, 0.9f, 0));
	Cube(P, V, M5, tex[1]);

	glm::mat4 M6 = M;
	M6 = glm::translate(M6, glm::vec3(2, 1, 4.5));
	Cube(P, V, M6, tex[0]);
}

void lamp(glm::mat4 M, glm::mat4 P, glm::mat4 V) {
	glm::mat4 Lamp = glm::scale(M, glm::vec3(0.2, 0.05, 0.2));
	Cube(P, V, Lamp, tex[1]);

	for (int i = 0; i < 4; i++) {
		glm::mat4 Lampside = glm::rotate(M, glm::radians(90.0f * i), glm::vec3(0, 1, 0));
		Lampside = translate(Lampside, glm::vec3(0, -0.2, 0.2));
		Lampside = glm::rotate(Lampside, glm::radians(70.0f), glm::vec3(1, 0, 0));
		Lampside = glm::scale(Lampside, glm::vec3(0.21, 0.01, 0.2));
		Cube(P, V, Lampside, tex[0]);
	}

}

void enemy(glm::mat4 P, glm::mat4 V, float en,int number) {

	en_angle += -(en / 3);
	float mno = 1 / 2.8f;
	float dife = -0.6;
	glm::mat4 M = glm::mat4(1.0f);
	mat4 M0 = glm::translate(M, glm::vec3(en_location[number].x, en_location[number].y, en_location[number].z));
	M0 = glm::scale(M0, vec3(0.45, 0.45, 0.45));
	Cube(P, V, M0, tex[targets[number].en_tex]);

	for (int i = 0; i < 8; i++) {
		mat4 Mp = rotate(M0, glm::radians(45.f * i), vec3(0, 1, 0));
		Mp = rotate(Mp, sin(en_angle) * mno + dife, vec3(0, 1, 0));
		for (int i = 0; i < 7 - 1; i++) {

			Mp = glm::translate(Mp, glm::vec3(1.0f, 0.0f, 0.0f));
			Mp = glm::rotate(Mp, sin(en_angle) * mno + dife, glm::vec3(0.0f, 0.0f, 1.0f));
			Mp = glm::rotate(Mp, sin(en_angle) * mno + dife, glm::vec3(1.0f, 0.0f, 0.0f));
			Mp = glm::scale(Mp, vec3(1, 0.4, 0.4));
			//cylinder(P, V, M0, tex[2]);
			Cube(P, V, Mp, tex[targets[number].en_tex]);
			Mp = glm::scale(Mp, vec3(1, 2.5, 2.5));
		}
	}
}

void enemylocation() {
	en_location.push_back({ -70.0, 2.6, 65.0, "X+"});
	en_location.push_back({ -75.4, 2.8, 92.7, "Z-"});
	en_location.push_back({ -11.5, 2.8, 57.5, "X-"});
	en_location.push_back({ -16.1, 2.8, 37.5, "Z+"});
	en_location.push_back({ -37.5, 2.7, 5.8, "X+" });
	en_location.push_back({ -37.2, 2.7, -7.0, "X+" });
	en_location.push_back({ -37.0, 2.7, -19.5, "X+" });
	en_location.push_back({ -36.8, 2.7, -29.3, "X+" });
	en_location.push_back({ 7.5, 2.7, -30.7, "X-"});
	en_location.push_back({ 7.5, 2.7, -19.0, "X-" });
	en_location.push_back({ 7.6, 2.7, -8.9,	"X-" });
	en_location.push_back({ 7.7, 2.7, 3.5, "X-" });

	float size = 1.5;
	for(int i = 0; i < en_location.size(); i++) {
		float down[3] = { en_location[i].x-size,en_location[i].y-size ,en_location[i].z-size };
		float up[3] = { en_location[i].x+size,en_location[i].y+size ,en_location[i].z+size };
		targets.push_back({ glm::vec3(down[0],down[1],down[2]), glm::vec3(up[0],up[1],up[2]),5,en_location[i].path});
	}
}

void en_movements(float momen,float amomen) {
	float scale = 0.0001;
	if(enemy_walk > 0.04) { en_way = -1; }
	if(enemy_walk < -0.04) { en_way = 1; }
	enemy_walk += en_way*scale;
	for(int i =0;i<en_location.size();i++){
		if (targets[i].path == "X+") {
			en_location[i].x += enemy_walk;
			targets[i].min[0] += enemy_walk;
			targets[i].max[0] += enemy_walk;
			continue;
		} else
		if (targets[i].path == "X-") {
			en_location[i].x += -enemy_walk;
			targets[i].min[0] += -enemy_walk;
			targets[i].max[0] += -enemy_walk;
			continue;
		} else
		if (targets[i].path == "Z+") {
			en_location[i].z += enemy_walk;
			targets[i].min[2] += enemy_walk;
			targets[i].max[2] += enemy_walk;
			continue;
		} else
		if (targets[i].path == "Z-") {
			en_location[i].z += -enemy_walk;
			targets[i].min[2] += -enemy_walk;
			targets[i].max[2] += -enemy_walk;
			continue;
		}
	}
}

void cursorDraw(glm::mat4 P, glm::mat4 V, glm::mat4 M) {

	glm::vec3 cursorHorizontalPosition = glm::vec3(protag.pos.x, 0.0f, protag.pos.z);

	// Obliczamy odleg³oœæ kursora od gracza
	float cursorDistance = 2.0f; // Ustaw odpowiedni¹ odleg³oœæ

	// Obliczamy pozycjê kursora w przestrzeni œwiata
	glm::vec3 cursorWorldPosition = cursorHorizontalPosition + glm::normalize(protag.dir) * cursorDistance;

	// Skalujemy i przesuwamy kursor do uzyskanej pozycji na œrodku ekranu
	M = glm::mat4(1.0f); // Resetujemy macierz modelu
	M = glm::translate(M, cursorWorldPosition);

	int curVerCount = 18;
	float curVer[] = {
		-3,1,4,1,
		3,-1,4,1,
		-3,-1,4,1,

		-3,1,4,1,
		3,1,4,1,
		3,-1,4,1,

		-1,-1,4,1,
		1,-3,4,1,
		-1,-3,4,1,

		-1,-1,4,1,
		1,-1,4,1,
		1,-3,4,1,

		1,1,4,1,
		-1,3,4,1,
		-1,1,4,1,

		1,1,4,1,
		1,3,4,1,
		-1,3,4,1,
	};
	float curTexCoor[] = {
		1.0f, 0.0f,    0.0f, 1.0f,    0.0f, 0.0f,
		1.0f, 0.0f,    1.0f, 1.0f,    0.0f, 1.0f,

		1.0f, 0.0f,    0.0f, 1.0f,    0.0f, 0.0f,
		1.0f, 0.0f,    1.0f, 1.0f,    0.0f, 1.0f,

		1.0f, 0.0f,    0.0f, 1.0f,    0.0f, 0.0f,
		1.0f, 0.0f,    1.0f, 1.0f,    0.0f, 1.0f,
	};
	float curColor[] = {
		1,1,1,1,
		1,1,1,1,
		1,1,1,1,

		1,1,1,1,
		1,1,1,1,
		1,1,1,1,

		1,1,1,1,
		1,1,1,1,
		1,1,1,1,

		1,1,1,1,
		1,1,1,1,
		1,1,1,1,

		1,1,1,1,
		1,1,1,1,
		1,1,1,1,

		1,1,1,1,
		1,1,1,1,
		1,1,1,1,
	};
	float curNorm[] = {
		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,

		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,

		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,

		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,

		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,

		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,
		0.0f, 0.0f,-1.0f,0.0f,
	};

	// Scale down the vertices
	for (int i = 0; i < 72; ++i) {
		if (curVer[i] != 4) {
			curVer[i] *= 5;
		}
	}

	// Apply scaling and translation to the model matrix
	//M = glm::translate(M, glm::vec3( - 79.9, 2.6, 78.7));
	//M = glm::translate(M, pos + dir);

	// Set texture
	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex[0]);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex[0]);

	// Disable previously enabled vertex attributes
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	// Use the shader program
	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	// Enable and set vertex attribute pointers
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, curVer);

	glEnableVertexAttribArray(sp->a("color"));
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, curColor);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, curNorm);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, curTexCoor);

	// Draw the cursor
	glDrawArrays(GL_TRIANGLES, 0, curVerCount);

	// Disable vertex attributes
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
}

void drawcursor(glm::mat4 P, glm::mat4 V) {
	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 cr = glm::translate(M, glm::vec3(protag.pos + protag.dir));
	cr = glm::scale(cr, glm::vec3(0.004f, 0.004f, 0.004f));
	cr = rotate(cr, glm::radians(angle_y), vec3(0, 1, 0));
	cr = rotate(cr, glm::radians(angle_x), vec3(1, 0, 0));
	Cube(P, V, cr, tex[4]);
	for (int i = 0; i < 4; i++) {
		glm::mat4 cr1 = rotate(cr, glm::radians(90.0f * i), vec3(0, 0, 1));
		cr1 = translate(cr1, glm::vec3(1.6, 0, 0));
		Cube(P, V, cr1, tex[4]);
	}
}

void drawRay(glm::mat4 P, glm::mat4 V, glm::mat4 M, glm::vec3 point1, glm::vec3 point2, GLuint tex) {
	// Aktywuj program cieniuj¹cy
	spTextured->use();

	// Za³aduj do programu cieniuj¹cego macierz rzutowania, widoku i modelu
	glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M));

	// Tablica wierzcho³ków zawieraj¹ca dwa punkty definiuj¹ce promieñ
	GLfloat rayVertices[] = {
		point1.x, point1.y, point1.z, 1.0f, // Pierwszy punkt
		point2.x, point2.y, point2.z, 1.0f  // Drugi punkt
	};

	// Tablica wspó³rzêdnych tekstury dla dwóch punktów
	GLfloat rayTexCoords[] = {
		0.0f, 0.0f, // Wspó³rzêdne tekstury dla pierwszego punktu
		1.0f, 1.0f  // Wspó³rzêdne tekstury dla drugiego punktu
	};

	// W³¹cz i przypisz wspó³rzêdne wierzcho³ków
	glEnableVertexAttribArray(spTextured->a("vertex"));
	glVertexAttribPointer(spTextured->a("vertex"), 4, GL_FLOAT, false, 0, rayVertices);

	// W³¹cz i przypisz wspó³rzêdne tekstury
	glEnableVertexAttribArray(spTextured->a("texCoord"));
	glVertexAttribPointer(spTextured->a("texCoord"), 2, GL_FLOAT, false, 0, rayTexCoords);

	// Aktywuj i zbinduj teksturê
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(spTextured->u("tex"), 0);
	// Ustaw szerokoœæ linii
	glLineWidth(5.0f);

	// Rysuj promieñ jako liniê
	glDrawArrays(GL_LINES, 0, 2);

	// Przywróæ domyœln¹ szerokoœæ linii
	glLineWidth(1.0f);

	// Wy³¹cz atrybuty wierzcho³ków i tekstury
	glDisableVertexAttribArray(spTextured->a("vertex"));
	glDisableVertexAttribArray(spTextured->a("texCoord"));
}

