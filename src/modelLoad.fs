#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
//设置材质的属性
struct Material
{
    //材质对不同反射的表现颜色
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    //高光反射的幂
    float shininess;
};
//平行光
struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
//点光源
struct PointLight
{
    vec3 position;
    
    float constant; //点光源的常数项
    float linear; //点光源的一次项
    float quadratic; //点光源的二次项

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[10];

uniform vec3 viewPos;

uniform int pointLightCount; //点光源个数
vec3 getDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 getPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

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
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(lightDir, normal), 0.0f);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    //vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, Texcoord));
    //vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, Texcoord));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;

    return (ambient + diffuse + specular);
}

vec3 getPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //点光源衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(lightDir, normal), 0.0f);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    //vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, Texcoord));
    //vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, Texcoord));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}
