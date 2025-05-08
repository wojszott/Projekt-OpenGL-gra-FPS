#version 330

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 ic; 
in vec4 n;
in vec4 l;
in vec4 l2;
in vec4 v;
in vec2 iTexCoord0;
in float damp;
in float damp2;


void main(void) {

    //Znormalizowane interpolowane wektory
    vec4 ml = normalize(l);
    vec4 mn = normalize(n);
    vec4 mv = normalize(v);
    vec4 ml2 = normalize(l2);
    //Wektor odbity
    vec4 mr = reflect(-ml, mn);
    vec4 mr2 = reflect(-ml2, mn);

    //Parametry powierzchni
    vec4 kd = texture(textureMap0, iTexCoord0); 
    vec4 ks = texture(textureMap0, iTexCoord0);

    //Obliczenie modelu oświetlenia
    float nl = clamp(dot(mn, ml), 0, 1);
    float nl2 = clamp(dot(mn, ml2), 0, 1);
    float rv = pow(clamp(dot(mr, mv), 0, 1),25);
    float rv2 = pow(clamp(dot(mr2, mv), 0, 1),25);
    vec4 pixelColor1 = vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);
    vec4 pixelColor2 = vec4(kd.rgb*nl2, kd.a) + vec4(ks.rgb*rv2, 0);
    pixelColor = pixelColor1 + pixelColor2;
}