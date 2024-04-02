#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;

//���ò��ʵ�����
struct Material
{
    //���ʶԲ�ͬ����ı�����ɫ
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    //�߹ⷴ�����
    float shininess;
};
//���ù�Դ������
struct Light
{
    vec3 direction; //��Դ����
    vec3 position; //���Դ�Ĺ�Դλ��

    //��Դ�Բ�ͬ�������͵�ǿ��
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //���Դ
    float constant; //���Դ�ĳ�����
    float linear; //���Դ��һ����
    float quadratic; //���Դ�Ķ�����
    //�۹��
    float cutOff; //�й��
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