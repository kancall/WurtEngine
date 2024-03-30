#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;

//���ò��ʵ�����
struct Material
{
    //���ʶԲ�ͬ����ı���
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //�߹ⷴ�����
    float shininess;
};
//���ù�Դ������
struct Light
{
    vec3 position;
    //��Դ�Բ�ͬ�������͵�ǿ��
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

uniform vec3 viewPos;
uniform vec3 objectColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = material.ambient * light.ambient;
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * light.diffuse;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 res = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(res, 1.0f);
}