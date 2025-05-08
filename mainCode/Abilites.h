#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "myCube.h"
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

void walking() {
	//moving forward and back
	mat4 Mc = rotate(mat4(1.0f), glm::radians(angle_y), vec3(0, 1, 0));
	Mc = rotate(Mc, glm::radians(angle_x), vec3(1, 0, 0));
	vec4 dir_ = Mc * vec4(0, 0, 1, 0);
	protag.dir = vec3(dir_);
	vec3 mdir = normalize(vec3(protag.dir.x, 0, protag.dir.z));
	protag.pos += walks*(float)glfwGetTime() * mdir;
	//moving horizontally
	mat4 d = rotate(mat4(1.0f), glm::radians(angle_y + 90), vec3(0, 1, 0));
	d = rotate(d, glm::radians(angle_x), vec3(1, 0, 0));
	vec4 d_ = d * vec4(0, 0, 1, 0);
	protag.xdir = vec3(d_);
	vec3 md = normalize(vec3(protag.xdir.x, 0, protag.xdir.z));
	protag.pos += ws2*(float)glfwGetTime() * md;
	//moving vertically
	protag.pos.y = protag.height + protag.high;
}

//collisions
void kolizje(int i) {
	if (protag.pos.x - protag.size <= par[i].x + par[i].size_x && protag.pos.x + protag.size >= par[i].x - par[i].size_x) {
		if (protag.high <= par[i].y + par[i].size_y && protag.high + protag.height >= par[i].y - par[i].size_y) {
			if (protag.pos.z - protag.size <= par[i].z + par[i].size_z && protag.pos.z + protag.size >= par[i].z - par[i].size_z) {
				float dif[6] = { 0, 0, 0, 0, 0, 0 };
				dif[0] = abs(protag.pos.x - protag.size - (par[i].x + par[i].size_x));
				dif[1] = abs(protag.pos.x + protag.size - (par[i].x - par[i].size_x));
				dif[2] = abs(protag.high - (par[i].y + par[i].size_y));
				dif[3] = abs(protag.high + protag.height - (par[i].y - par[i].size_y));
				dif[4] = abs(protag.pos.z - protag.size - (par[i].z + par[i].size_z));
				dif[5] = abs(protag.pos.z + protag.size- (par[i].z - par[i].size_z));
				int num = 0;
				for (int i = 1; i < 6; i++) {
					if (dif[i] < dif[num]) { num = i; }
				}
				switch (num) {
				case 0:
					protag.pos.x = par[i].x + par[i].size_x + protag.size;
					break;
				case 1:
					protag.pos.x = par[i].x - par[i].size_x - protag.size;
					break;
				case 2:
					protag.high = par[i].y + par[i].size_y;
					pod = par[i].y + par[i].size_y;
					onground = 1;
					break;
				case 3:
					if (protag.pos.y < par[i].y) {
						protag.high = par[i].y - par[i].size_y - protag.height;
					}
					break;
				case 4:
					protag.pos.z = par[i].z + par[i].size_z + protag.size;
					break;
				case 5:
					protag.pos.z = par[i].z - par[i].size_z - protag.size;
					break;
				}
			}
		}
	}
}
//rampa
void rampa(int i,int type) {
	float scale, hi, front;
	bool side;
	switch (type){ //0 "x-" 1 "x+" 2 "z-" 3 "z+"
		case 0:
			front = 1;
			side = true;
			break;
		case 1:
			front = -1;
			side = true;
			break;
		case 2:
			front = -1;
			side = false;
			break;
		case 3:
			front = 1;
			side = false;
			break;
	}

	if (side) {
		scale = (par[i].size_y) / par[i].size_x;
		hi = abs(protag.pos.x - protag.size*front - (par[i].x + par[i].size_x * front));
	}
	else {
		scale = (par[i].size_y) / par[i].size_z;
		hi = abs(protag.pos.z - protag.size*front - (par[i].z + par[i].size_z * front));
	}
	if (protag.pos.x - protag.size <= par[i].x + par[i].size_x && protag.pos.x + protag.size  >= par[i].x - par[i].size_x) {
		if (protag.high <= scale * hi && protag.high + protag.height >= par[i].y - par[i].size_y) {
			if (protag.pos.z - protag.size  <= par[i].z + par[i].size_z && protag.pos.z + protag.size  >= par[i].z - par[i].size_z) {
				float dif[6] = { 0, 0, 0, 0, 0, 0 };
				dif[0] = abs((protag.pos.x - protag.size) - (par[i].x + par[i].size_x));
				dif[1] = abs((protag.pos.x + protag.size) - (par[i].x - par[i].size_x));
				dif[2] = abs(protag.high - (scale * hi));
				dif[3] = abs(protag.high - (scale * hi));
				dif[4] = abs((protag.pos.z - protag.size) - (par[i].z + par[i].size_z));
				dif[5] = abs((protag.pos.z + protag.size) - (par[i].z - par[i].size_z));
				int num = 0;
				for (int i = 1; i < 6; i++) {
					if (dif[i] < dif[num]) { num = i; }
				}
				switch (num) {
				case 0:
					protag.pos.x = par[i].x + par[i].size_x + protag.size;
					break;
				case 1:
					protag.pos.x = par[i].x - par[i].size_x - protag.size;
					break;
				case 2:
					protag.high =scale*hi;
					pod = scale*hi;
					onground = 1;
					break;
				case 3:
					protag.high = scale * hi;
					pod = scale * hi;
					onground = 1;
					break;
				case 4:
					protag.pos.z = par[i].z + par[i].size_z + protag.size;
					break;
				case 5:
					protag.pos.z = par[i].z - par[i].size_z - protag.size;
					break;
				}
			}
		}
	}
}

//sneaking
void kucanie(float momen) {
	if (stan == 0) { return; }
	if (kuc == 1) { //kuca
		dif = -momen * 0.3;
		if (protag.height <= 0.8) { stan = 0; dif = 0; }
		wspeed = WALK_SPEED/3;
		protag.height += dif * stan;
	}
	else { //wstaje
		dif = momen * 0.3;
		if (protag.height >= 1.6) { stan = 0; dif = 0; }
		wspeed = WALK_SPEED;
		protag.height += dif * stan;
	}
}
//gravity
void gravity(float momen) {
	onground = 0;
	for (int i = 1; i < par.size(); i++) {
		if (abs(par[i].x - protag.pos.x > 100) && abs(par[i].z - protag.pos.z > 100)) {
			continue;
		}
		if (par[i].type == "stair1") {
			rampa(i, 3);
		} else if(par[i].type == "stair2") {
			rampa(i, 1);
		}
		else if (par[i].type == "stair3") {
			rampa(i, 0);
		}
		else if (par[i].type == "stair4") {
			rampa(i, 2);
		} else {
			kolizje(i);
		}
	}
	if (!onground) {
		pod = 1;
	}
	else { 
		fall_time = 0;
		return;
	}
	if (jump == 1) {
		return;
	}
	if (protag.high <= pod + 0.2) {
		return;
	}
	fall_time += 0.001;
	if (fall_time > 0.1) { fall_time = 0.1; }
	protag.high += -fall_time;
}
//jump
void skok(float momen) {
	if (jump == 0) return;
	if (protag.high >= ystart + protag.height + 2) { 
		jump = 0;
		jump_time = 0;
		return; 
	}
	fall_time = 0;
	jump_time += 0.004;
	protag.high += 0.172-jump_time;
}

bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const AABB& box, float& tminOut) {
	float t1 = (box.min.x - rayOrigin.x) / rayDirection.x;
	float t2 = (box.max.x - rayOrigin.x) / rayDirection.x;
	float tmin = std::min(t1, t2);
	float tmax = std::max(t1, t2);

	t1 = (box.min.y - rayOrigin.y) / rayDirection.y;
	t2 = (box.max.y - rayOrigin.y) / rayDirection.y;
	tmin = std::max(tmin, std::min(t1, t2));
	tmax = std::min(tmax, std::max(t1, t2));

	t1 = (box.min.z - rayOrigin.z) / rayDirection.z;
	t2 = (box.max.z - rayOrigin.z) / rayDirection.z;
	tmin = std::max(tmin, std::min(t1, t2));
	tmax = std::min(tmax, std::max(t1, t2));

	tminOut = tmin;
	return tmax >= tmin;
}

void recoil(float momen) {
//recoil
	if (shot >= 10) { back = -1; }
	if (shot <= 0) {
		back = 1;
		shoot = 0;
	}
	shot += momen * 10 * back;
}

//shooting
void shooting() {
	
	glm::vec3 rayOrigin = protag.pos;
	glm::vec3 rayDirection = glm::normalize(protag.dir);
	glm::vec3 rayOffset = { -0.2f, 0.0f, 0.5f };
	

	// Oblicz liniê strza³u
	float maxDistance = 100.0f; // Maksymalna odleg³oœæ strza³u
	glm::vec3 rayEnd = rayOrigin + rayDirection * maxDistance;
	rayS = rayOrigin + bronS;
	printf("%f %f %f \n", rayS.x, rayS.y, rayS.z);
	rayE = rayEnd;
	// Wykryj kolizjê
	bool hit = false;
	glm::vec3 closestHitPoint;
	float closestT = maxDistance;
	int hit_id = 0;
	for (int i = 0;i<targets.size();i++) {
		float tmin;
		if (rayIntersectsAABB(rayOrigin, rayDirection, targets[i], tmin) && tmin < closestT) {
			hit = true;
			closestT = tmin;
			closestHitPoint = rayOrigin + tmin * rayDirection;
			hit_id = i;
		}
	}

	if (hit) {
		if (targets[hit_id].en_tex == 5) {
			targets[hit_id].en_tex = 8;

		} else{
			targets[hit_id].en_tex = 5;
		}
		targets[hit_id].health += -1;
		if (targets[hit_id].health == 0) {
			en_location[hit_id] = { 0,0,0 };
			targets[hit_id].min = { 0,0,0 };
			targets[hit_id].max = { 0,0,0 };
		}
	}
}

void melee_dmg(int i) {
	if (protag.pos.x - protag.size >= targets[i].min[0] && protag.pos.x + protag.size <= targets[i].max[0]) {
		if (protag.pos.y >= targets[i].min[1] && protag.pos.y <= targets[i].max[1]) {
			if (protag.pos.z - protag.size >= targets[i].min[2] && protag.pos.z + protag.size <= targets[i].max[2]) {
				dead = 1;
				domen = 0;
				en_location[i] = { 0,0,0 };
				targets[i].min= { 0,0,0 };
				targets[i].max = { 0,0,0 };
			}
		}
	}
}

void en_shoot(int i) {
	float x = abs(protag.pos.x - en_location[i].x);
	float z = abs(protag.pos.z - en_location[i].z);
	if (x < 8 && z < 8) {
		targets[i].shoot_time += 0.1*stop;
	}
	else {
		targets[i].shoot_time = 0;
	}
	if (targets[i].shoot_time > 20) {
		dead = 1;
		domen = 0;
		en_location[i] = { 0,0,0 };
		targets[i].min = { 0,0,0 };
		targets[i].max = { 0,0,0 };
		targets[i].shoot_time = 0;
	}
}


