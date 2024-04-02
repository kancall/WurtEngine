#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;

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
//聚光灯
struct SpotLight
{
    vec3 position;
    vec3 direction;

    float cutOff; //切光角
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;   
};
uniform int pointLightCount; //点光源个数

uniform DirLight dirLight;
uniform PointLight pointLights[10]; //设置点光源数组，要设置成固定大小，因为glsl没有动态数组
uniform SpotLight spotLight;

uniform Material material;

uniform vec3 viewPos;
uniform vec3 objectColor;

vec3 getDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 getPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 getSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 res = getDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < pointLightCount; i++)
        res += getPointLight(pointLights[i], norm, FragPos, viewDir);  
    res += getSpotLight(spotLight, norm, FragPos, viewDir);   

    
    FragColor = vec4(res, 1.0);
}

vec3 getDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(lightDir, normal), 0.0f);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, Texcoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, Texcoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));

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

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, Texcoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, Texcoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

vec3 getSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(lightDir, normal), 0.0f);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, Texcoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, Texcoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, Texcoord));
    //光源衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
    //光源边缘平滑过渡
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  

    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}