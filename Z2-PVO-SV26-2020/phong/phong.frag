#version 330 core

struct Light{
	vec3 pos; 
	vec3 kA; 
	vec3 kD; 
	vec3 kS; 
};
struct Material{ //Materijal objekta
	float shine;
	sampler2D diffuse;
	sampler2D specular;
};

in vec3 chNor;
in vec3 chFragPos;
in vec2 chTexCoords;

out vec4 outCol;
out vec4 FragColor;


uniform Light uLight;
uniform Material uMaterial;
uniform vec3 uColor;
uniform bool hasText;
uniform vec3 uViewPos;	//Pozicija kamere (za racun spekularne komponente)

void main()
{
	vec3 resA;
	vec3 resD;
	vec3 resS;
	vec3 normal = normalize(chNor);
	vec3 lightDirection = normalize(uLight.pos - chFragPos);
	float nD = max(dot(normal, lightDirection), 0.0);

	vec3 viewDirection = normalize(uViewPos - chFragPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float s = pow(max(dot(viewDirection, reflectionDirection), 0.0), uMaterial.shine);

	if (hasText) {
		resA = uLight.kA * vec3(texture(uMaterial.diffuse, chTexCoords));
		resD = uLight.kD * nD * vec3(texture(uMaterial.diffuse, chTexCoords));
		resS = uLight.kS * s * vec3(texture(uMaterial.specular, chTexCoords)) * 100;
	} else {
		resA = uLight.kA * uColor;
		resD = uLight.kD * nD * uColor;  
		resS = uLight.kS * s * uColor;
	}

	outCol = vec4(resA + resD + resS, 1);

//	if(outCol.a < 0.1)
//        discard;
//    FragColor = outCol;


}