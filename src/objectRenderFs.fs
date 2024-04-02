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
//设置光源的属性
struct Light
{
    vec3 direction; //光源方向
    vec3 position; //点光源的光源位置

    //光源对不同反射类型的强度
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //点光源
    float constant; //点光源的常数项
    float linear; //点光源的一次项
    float quadratic; //点光源的二次项
    //聚光灯
    float cutOff; //切光角
    float outerCutOff;
};

uniform Material material;
uniform Light light;

uniform vec3 viewPos;
uniform vec3 objectColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    //vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = light.ambient * texture(material.diffuse, Texcoord).rgb;
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, Texcoord).rgb;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, Texcoord).rgb;

    vec3 emission = texture(material.emission, Texcoord).rgb;

    //float distance = length(light.position - FragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    //ambient  *= attenuation; 
    //diffuse  *= attenuation;
    //specular *= attenuation;
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  
    diffuse *= intensity;
    specular *= intensity;

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   

    vec3 res = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(res, 1.0f);
}