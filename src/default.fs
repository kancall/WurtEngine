#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
//平行光
struct DirLight
{
    vec3 direction;

    vec3 ambient;
};
//点光源
struct PointLight
{
    vec3 position;
    
    float constant; //点光源的常数项
    float linear; //点光源的一次项
    float quadratic; //点光源的二次项

    vec3 ambient;
};
uniform DirLight dirLight;
uniform PointLight pointLights[10];

uniform vec3 viewPos;

uniform int pointLightCount; //点光源个数
vec3 getDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 getPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform vec3 tempColor;

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 res;
    res = getDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < pointLightCount; i++)
        res += getPointLight(pointLights[i], norm, FragPos, viewDir); 
    FragColor = vec4(res, 1.0);
}

vec3 getDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient = light.ambient;// * vec3(texture(texture_diffuse1, TexCoords));

    return (ambient);
}

vec3 getPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //点光源衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

    vec3 ambient = light.ambient;// * vec3(texture(texture_diffuse1, TexCoords));

    ambient  *= attenuation;
    
    return (ambient);
}
