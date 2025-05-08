#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 color; //kolor związany z wierzchołkiem
in vec4 normal; //wektor normalny w przestrzeni modelu
in vec2 texCoord0;
in vec4 mcpos;

//Zmienne interpolowane
out vec4 ic;
out vec4 l;
out vec4 l2;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;
out float damp;
out float damp2;


void main(void) {
    float rs = 5;
    float re = 100;
    vec4 lp = vec4(-60.8, 83.4, 88.0, 1); //przestrzeń świata punkt światła
    vec4 lp2 = vec4(0, 75, -4, 1);
    l = normalize(V * lp - V*M*vertex); //wektor do światła w przestrzeni oka
    v = normalize(mcpos - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    n = normalize(V * M * normal); //wektor normalny w przestrzeni oka

    l2 = normalize(V * lp2 - V*M*vertex); //wektor do światła w przestrzeni oka

    iTexCoord0 = texCoord0;

    float ll = length(V * lp - V*M*vertex);
    if (ll <= rs) {
        damp = 1;
    } 
    else if (rs < ll && ll < re) {
        damp = (re - ll)/(re - rs);
    }
    else if (ll > re) {
        damp = 0;
    }




    float ll2 = length(V * lp2 - V*M*vertex);
    if (ll2 <= rs) {
        damp2 = 1;
    } 
    else if (rs < ll2 && ll2 < re) {
        damp2 = (re - ll2)/(re - rs);
    }
    else if (ll2 > re) {
        damp2 = 0;
    }

    ic = color;

    gl_Position=P*V*M*vertex;
}