#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
//ƽ�й�
struct DirLight
{
    vec3 direction;

    vec3 ambient;
};
//���Դ
struct PointLight
{
    vec3 position;
    
    float constant; //���Դ�ĳ�����
    float linear; //���Դ��һ����
    float quadratic; //���Դ�Ķ�����

    vec3 ambient;
};
uniform DirLight dirLight;
uniform PointLight pointLights[10];

uniform vec3 viewPos;

uniform int pointLightCount; //���Դ����
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
    //���Դ˥��
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

    vec3 ambient = light.ambient;// * vec3(texture(texture_diffuse1, TexCoords));

    ambient  *= attenuation;
    
    return (ambient);
}
