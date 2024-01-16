#version 330 core
out vec4 FragColor;

in vec3 chNormal;  
in vec3 chFragPos;  
in vec2 chUV;
  
uniform vec3 uLightPos; 
uniform vec3 uViewPos; 
uniform vec3 uLightColor;
uniform sampler2D uDiffMap1;
uniform vec3 darkColor;

uniform float uTime;

out vec4 finalColor;

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

void main()
{    
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * uLightColor;
  	
    // Point light properties
    PointLight pointLight;
    pointLight.position = uLightPos;
    pointLight.color = uLightColor;
    pointLight.constant = 1.0;
    pointLight.linear = 0.09;
    pointLight.quadratic = 0.032;

    // Calculate light attenuation
    float distance = length(chFragPos - pointLight.position);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    // diffuse 
    vec3 norm = normalize(chNormal);
    vec3 lightDir = normalize(pointLight.position - chFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * pointLight.color * attenuation;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - chFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * pointLight.color * attenuation;

    vec3 darkRed = vec3(0.5, 0.0, 0.0);
    vec3 white = vec3(1.0, 1.0, 1.0);
    float frequency = 3.0;
    vec3 dynamicColor = mix(darkColor, white, 0.5 + 0.5 * sin(uTime * frequency)); 

    FragColor = texture(uDiffMap1, chUV) * vec4(ambient + diffuse + specular + dynamicColor, 1.0);
}
